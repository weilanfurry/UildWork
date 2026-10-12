// UildWork 倒计时（对应原 daojishi.cpp）
//
// 修复的问题：
//   1. Sleep 累减导致长时间倒计时误差越来越大 -> 改用截止时间计算。
//   2. 没有输入校验：可以输入负数或 0，进入后行为异常 -> 现在做范围校验。
//   3. 铃声菜单与播放逻辑和 naozhong.cpp 重复 -> 复用 audio 模块。
//   4. 到点后无法停止（Beep 死循环 / taskkill wscript.exe）-> 按任意键停止。

#include "apps.h"

#include "../common/appinfo.h"
#include "../common/audio.h"
#include "../common/console.h"
#include "../common/input.h"
#include "../common/text.h"
#include "../common/timer.h"
#include "../common/ui.h"

#include <vector>

namespace uw {

namespace {

const int kWidth = 52;

void drawRemaining(long long remainingMs, long long totalMs, bool paused, bool firstFrame,
                   const std::string& focusLine) {
    if (firstFrame) {
        console::clear();
        ui::banner("UildWork 倒计时", kWidth);
        console::writeLine(std::string());
    } else {
        console::moveTo(4, 1);
    }

    ui::bigTime(text::clockText(timer::secondsCeil(remainingMs)), console::BrightGreen);

    const double fraction = (totalMs > 0) ? static_cast<double>(remainingMs) / static_cast<double>(totalMs)
                                          : 0.0;
    console::ScopedColor bar(console::BrightCyan);
    console::writeLine(text::center(ui::progressBar(fraction), kWidth));
    console::resetColor();

    if (!focusLine.empty()) {
        console::ScopedColor focus(console::BrightMagenta);
        console::writeLine(text::center(focusLine, kWidth));
        console::resetColor();
    } else {
        console::writeLine(std::string());
    }
    console::setColor(paused ? console::BrightYellow : console::BrightWhite);
    console::writeLine(text::center(paused ? "[ 已暂停 ]" : "[ 计时中 ]", kWidth));
    console::resetColor();
}

bool chooseDuration(long long& durationMs) {
    const std::vector<ui::MenuItem> items = {
        {"1 小时", ""}, {"30 分钟", ""}, {"10 分钟", ""}, {"1 分钟", ""}, {"自定义（时:分:秒）", ""}};
    const int choice = ui::selectMenu("请选择倒计时时长", items, 0);
    if (choice < 0) {
        return false;
    }
    switch (choice) {
        case 0: durationMs = 3600LL * 1000; break;
        case 1: durationMs = 30LL * 60 * 1000; break;
        case 2: durationMs = 10LL * 60 * 1000; break;
        case 3: durationMs = 60LL * 1000; break;
        default: {
            console::clear();
            ui::banner("自定义倒计时", kWidth);
            int hour = 0;
            int minute = 0;
            int second = 0;
            if (!input::readTimeOfDay("请输入时长（时:分:秒，例如 0:25:00）：", hour, minute, second)) {
                return false;
            }
            durationMs = ((static_cast<long long>(hour) * 60 + minute) * 60 + second) * 1000;
            if (durationMs <= 0) {
                console::ScopedColor warning(console::BrightRed);
                console::writeLine("时长必须大于 0。");
                input::pause();
                return false;
            }
            break;
        }
    }
    return true;
}

}  // namespace

int runCountDown() {
    console::init("UildWork 倒计时");
    console::clear();
    ui::banner("UildWork 倒计时", kWidth);
    console::writeLine(std::string());

    long long durationMs = 0;
    if (!chooseDuration(durationMs)) {
        return 0;
    }
    const int ringtone = audio::chooseRingtone("选择提醒铃声");
    const app::LocalTime target = app::localNow();
    const long long endOfDay =
        (static_cast<long long>(target.hour) * 3600 + target.minute * 60 + target.second) +
        durationMs / 1000;
    const bool endsNextDay = (endOfDay >= 86400);
    const std::string focus = "预计结束时间 " + app::formatDate(target) +
                              (endsNextDay ? "（次日） " : " ") + text::clockText(endOfDay % 86400);

    timer::Timer countdown(durationMs);
    bool paused = false;
    bool firstFrame = true;
    int framesWithoutConsole = 0;

    for (;;) {
        if (input::keyPressed()) {
            const input::KeyEvent event = input::readKey();
            if (event.key == input::Key::Escape || event.isCharacter('q')) {
                return 0;
            }
            if (event.key == input::Key::Space) {
                countdown.togglePause();
                paused = countdown.paused();
            }
        }
        if (countdown.finished()) {
            drawRemaining(0, durationMs, false, false, focus);
            audio::ringUntilDismissed(ringtone, "倒计时结束！");
            return 0;
        }
        drawRemaining(countdown.remainingMs(), durationMs, paused, firstFrame, focus);
        firstFrame = false;
        if (!input::isInteractive() && ++framesWithoutConsole >= 4) {
            return 0;
        }
        timer::sleepMs(100);
    }
}

}  // namespace uw
