// UildWork 番茄钟（对应原 fanqiezhong.cpp）
//
// 原实现的问题：
//   1. 每秒钟执行 system("color 07") 和 system("cls")，也就是每秒创建两个进程，
//      长时间运行会明显占用资源 -> 改为直接重绘。
//   2. 工作和休息时长写死在代码里，不能调整 -> 现在可配置。
//   3. 不能暂停、不能退出（只能关窗口）-> 支持 空格 暂停、S 跳过、Q 退出。
//   4. 只有蜂鸣提示 -> 复用 audio 模块，可选择铃声。

#include "apps.h"

#include "../common/appinfo.h"
#include "../common/audio.h"
#include "../common/console.h"
#include "../common/input.h"
#include "../common/text.h"
#include "../common/timer.h"
#include "../common/ui.h"

namespace uw {

namespace {

const int kWidth = 52;

enum Phase { PhaseWork, PhaseShortBreak, PhaseLongBreak };

struct Settings {
    long long workMs;
    long long shortBreakMs;
    long long longBreakMs;
    int cyclesBeforeLongBreak;
};

std::string phaseName(Phase phase) {
    switch (phase) {
        case PhaseWork: return "专注工作";
        case PhaseShortBreak: return "短休息";
        default: return "长休息";
    }
}

Settings askSettings() {
    Settings settings;
    console::clear();
    ui::banner("番茄钟设置", kWidth);
    console::writeLine(std::string());
    console::ScopedColor hint(console::Gray);
    console::writeLine("经典番茄工作法为 25 / 5 / 15，每 4 轮一次长休息，可自行调整。");
    console::resetColor();
    console::writeLine(std::string());

    const int work = input::readInt("工作时长（分钟，1-180，经典值 25）：", 1, 180);
    const int shortBreak = input::readInt("短休息时长（分钟，1-60，经典值 5）：", 1, 60);
    const int longBreak = input::readInt("长休息时长（分钟，1-120，经典值 15）：", 1, 120);
    const int cycles = input::readInt("几轮后进入长休息（1-12，经典值 4）：", 1, 12);

    settings.workMs = static_cast<long long>(work) * 60 * 1000;
    settings.shortBreakMs = static_cast<long long>(shortBreak) * 60 * 1000;
    settings.longBreakMs = static_cast<long long>(longBreak) * 60 * 1000;
    settings.cyclesBeforeLongBreak = cycles;
    return settings;
}

void drawPhase(Phase phase, long long remainingMs, long long totalMs, int completedCycles,
               bool paused, bool firstFrame, const std::string& extra) {
    if (firstFrame) {
        console::clear();
        ui::banner("UildWork 番茄钟", kWidth);
        console::writeLine(std::string());
    } else {
        console::moveTo(4, 1);
    }

    const int color = (phase == PhaseWork) ? console::BrightGreen : console::BrightCyan;
    console::setColor(color);
    console::writeLine(text::center("【" + phaseName(phase) + "】" + "  已完成番茄: " +
                                        std::to_string(completedCycles),
                                    kWidth));
    console::resetColor();

    ui::bigTime(text::clockText(timer::secondsCeil(remainingMs)), color);

    const double fraction =
        (totalMs > 0) ? static_cast<double>(remainingMs) / static_cast<double>(totalMs) : 0.0;
    console::ScopedColor bar(console::BrightCyan);
    console::writeLine(text::center(ui::progressBar(fraction), kWidth));
    console::resetColor();

    if (!extra.empty()) {
        console::ScopedColor note(console::BrightYellow);
        console::writeLine(text::center(extra, kWidth));
        console::resetColor();
    } else {
        console::writeLine(std::string());
    }
    console::setColor(paused ? console::BrightYellow : console::BrightWhite);
    console::writeLine(text::center(paused ? "[ 已暂停 ]" : "[ 计时中 ]", kWidth));
    console::resetColor();

    if (firstFrame) {
        console::writeLine(std::string());
        console::ScopedColor hint(console::Gray);
        console::writeLine("空格 暂停/继续 · S 跳过当前阶段 · Q 或 Esc 退出");
        console::resetColor();
    }
}

// 阶段切换提示：等待 3 秒或用户按键
bool announcePhaseChange(const std::string& utf8Message, int ringtone) {
    audio::chime(ringtone, 1500);
    console::clear();
    ui::banner("番茄钟", kWidth);
    console::writeLine(std::string());
    console::ScopedColor color(console::BrightYellow);
    console::writeLine(text::center(utf8Message, kWidth));
    console::resetColor();
    console::writeLine(std::string());
    console::ScopedColor hint(console::Gray);
    console::writeLine(text::center("3 秒后自动继续 · 按 S 立即继续 · 按 Q 退出", kWidth));
    console::resetColor();

    for (int i = 0; i < 30; ++i) {
        if (input::keyPressed()) {
            const input::KeyEvent event = input::readKey();
            if (event.key == input::Key::Escape || event.isCharacter('q')) {
                return false;
            }
            return true;
        }
        if (!input::isInteractive()) {
            return true;
        }
        timer::sleepMs(100);
    }
    return true;
}

}  // namespace

int runPomodoro() {
    console::init("UildWork 番茄钟");
    const Settings settings = askSettings();
    const int ringtone = audio::chooseRingtone("选择阶段提示铃声");

    Phase phase = PhaseWork;
    int completedCycles = 0;
    int framesWithoutConsole = 0;
    bool firstFrame = true;
    std::string banner = "开始第一个番茄，专注 25 分钟吧！";

    for (;;) {
        long long durationMs = settings.workMs;
        if (phase == PhaseShortBreak) {
            durationMs = settings.shortBreakMs;
        } else if (phase == PhaseLongBreak) {
            durationMs = settings.longBreakMs;
        }

        timer::Timer phaseTimer(durationMs);
        bool paused = false;
        bool skip = false;
        firstFrame = true;

        while (!phaseTimer.finished() && !skip) {
            if (input::keyPressed()) {
                const input::KeyEvent event = input::readKey();
                if (event.key == input::Key::Escape || event.isCharacter('q')) {
                    return 0;
                }
                if (event.key == input::Key::Space) {
                    phaseTimer.togglePause();
                    paused = phaseTimer.paused();
                } else if (event.isCharacter('s')) {
                    skip = true;
                    break;
                }
            }
            drawPhase(phase, phaseTimer.remainingMs(), durationMs, completedCycles, paused,
                      firstFrame, banner);
            firstFrame = false;
            if (!input::isInteractive() && ++framesWithoutConsole >= 4) {
                return 0;
            }
            timer::sleepMs(100);
        }
        banner.clear();

        if (phase == PhaseWork) {
            ++completedCycles;
            if (completedCycles % settings.cyclesBeforeLongBreak == 0) {
                phase = PhaseLongBreak;
                if (!announcePhaseChange("完成 " + std::to_string(completedCycles) +
                                             " 个番茄，享受长休息吧！",
                                         ringtone)) {
                    return 0;
                }
            } else {
                phase = PhaseShortBreak;
                if (!announcePhaseChange("工作时间结束，休息一下！", ringtone)) {
                    return 0;
                }
            }
        } else {
            phase = PhaseWork;
            if (!announcePhaseChange("休息结束，开始下一个番茄！", ringtone)) {
                return 0;
            }
        }
    }
}

}  // namespace uw
