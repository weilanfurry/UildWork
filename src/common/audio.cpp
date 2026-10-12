#include "audio.h"

#include "console.h"
#include "input.h"
#include "paths.h"
#include "text.h"
#include "ui.h"

#include <windows.h>
#include <mmsystem.h>

#include <process.h>

namespace uw {
namespace audio {

namespace {

const wchar_t* kMciAlias = L"uildwork_ringtone";

std::wstring g_openMedia;
volatile long g_beepStop = 0;
HANDLE g_beepThread = NULL;

void mciSend(const std::wstring& command) {
    mciSendStringW(command.c_str(), NULL, 0, NULL);
}

void closeMedia() {
    if (!g_openMedia.empty()) {
        mciSend(L"stop " + std::wstring(kMciAlias));
        mciSend(L"close " + std::wstring(kMciAlias));
        g_openMedia.clear();
    }
}

// 用 MCI 打开媒体文件，成功返回 true
bool openMedia(const std::string& path) {
    const std::wstring widePath = text::toWide(path);
    if (widePath == g_openMedia) {
        return true;
    }
    closeMedia();
    const std::wstring command =
        L"open \"" + widePath + L"\" type mpegvideo alias " + std::wstring(kMciAlias);
    const MCIERROR result = mciSendStringW(command.c_str(), NULL, 0, NULL);
    if (result != 0) {
        return false;
    }
    g_openMedia = widePath;
    return true;
}

unsigned __stdcall beepThreadProc(void* parameter) {
    const int frequency = (parameter == NULL) ? 880 : *static_cast<int*>(parameter);
    while (InterlockedCompareExchange(&g_beepStop, 0, 0) == 0) {
        Beep(static_cast<DWORD>(frequency), 220);
        Sleep(120);
    }
    return 0;
}

int g_beepFrequency = 880;

void startBeepLoop(int frequency) {
    if (g_beepThread != NULL) {
        return;
    }
    InterlockedExchange(&g_beepStop, 0);
    g_beepFrequency = frequency;
    g_beepThread = reinterpret_cast<HANDLE>(
        _beginthreadex(NULL, 0, beepThreadProc, &g_beepFrequency, 0, NULL));
}

void stopBeepLoop() {
    if (g_beepThread == NULL) {
        return;
    }
    InterlockedExchange(&g_beepStop, 1);
    WaitForSingleObject(g_beepThread, 2000);
    CloseHandle(g_beepThread);
    g_beepThread = NULL;
}

}  // namespace

std::vector<Ringtone> availableRingtones() {
    std::vector<Ringtone> result;

    Ringtone beep;
    beep.id = RingtoneBeep;
    beep.label = "系统蜂鸣";
    result.push_back(beep);

    Ringtone first;
    first.id = RingtoneWalkingTheWire;
    first.label = "Walking The Wire";
    first.fileName = "Walking The Wire.mp3";
    first.path = paths::findResource(first.fileName);
    result.push_back(first);

    Ringtone second;
    second.id = RingtoneBattleSymphony;
    second.label = "Battle Symphony";
    second.fileName = "Battle Symphony.mp3";
    second.path = paths::findResource(second.fileName);
    result.push_back(second);

    return result;
}

Ringtone ringtoneById(int id) {
    const std::vector<Ringtone> tones = availableRingtones();
    for (size_t i = 0; i < tones.size(); ++i) {
        if (tones[i].id == id) {
            return tones[i];
        }
    }
    return tones[0];
}

int chooseRingtone(const std::string& utf8Title) {
    const std::vector<Ringtone> tones = availableRingtones();
    std::vector<ui::MenuItem> items;
    for (size_t i = 0; i < tones.size(); ++i) {
        ui::MenuItem item;
        item.label = tones[i].label;
        if (!tones[i].fileName.empty() && tones[i].path.empty()) {
            item.hint = "音乐文件缺失，将使用系统蜂鸣";
        }
        items.push_back(item);
    }
    const int index = ui::selectMenu(utf8Title, items, 0);
    if (index < 0) {
        return RingtoneBeep;
    }
    return tones[static_cast<size_t>(index)].id;
}

void startLoop(int ringtoneId) {
    stopLoop();
    const Ringtone tone = ringtoneById(ringtoneId);
    if (!tone.path.empty() && openMedia(tone.path)) {
        mciSend(L"play " + std::wstring(kMciAlias) + L" repeat");
        return;
    }
    startBeepLoop(880);
}

void stopLoop() {
    stopBeepLoop();
    if (!g_openMedia.empty()) {
        mciSend(L"stop " + std::wstring(kMciAlias));
        closeMedia();
    }
}

void chime(int ringtoneId, int durationMs) {
    if (durationMs <= 0) {
        durationMs = 1200;
    }
    stopLoop();
    const Ringtone tone = ringtoneById(ringtoneId);
    if (!tone.path.empty() && openMedia(tone.path)) {
        mciSend(L"play " + std::wstring(kMciAlias));
        Sleep(static_cast<DWORD>(durationMs));
        mciSend(L"stop " + std::wstring(kMciAlias));
        closeMedia();
        return;
    }
    for (int i = 0; i < 3; ++i) {
        Beep(880, 160);
        Sleep(120);
    }
}

void ringUntilDismissed(int ringtoneId, const std::string& utf8Message) {
    startLoop(ringtoneId);

    console::setColor(console::BrightYellow);
    console::writeLine(std::string());
    console::writeLine(utf8Message);
    console::writeLine("按任意键停止铃声...");
    console::resetColor();

    if (!input::isInteractive()) {
        // 无控制台（例如被脚本调用）时不要无限响铃
        Sleep(1500);
    } else {
        input::clearBuffer();
        while (!input::keyPressed()) {
            Sleep(50);
        }
        input::readKey();
    }
    stopLoop();
}

}  // namespace audio
}  // namespace uw
