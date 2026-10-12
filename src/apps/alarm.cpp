// UildWork 小闹钟（对应原 naozhong.cpp）
//
// 修复的问题：
//   1. 原来要求 时、分、秒 三个数字与系统时间完全相等才响铃，如果设定的时刻比当前
//      时间早（例如下午设 07:30），程序会永远等下去 -> 现在自动顺延到次日并明确提示。
//   2. 定时器每秒 system("cls") 一次 -> 改为按需重绘。
//   3. 响铃后无法停止，且用 taskkill 杀 wscript.exe 会影响系统上所有脚本
//      -> 复用 audio 模块，按任意键停止。

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

long long secondsOfDay(const app::LocalTime& time) {
    return static_cast<long long>(time.hour) * 3600 + static_cast<long long>(time.minute) * 60 +
           time.second;
}

void drawAlarm(const app::LocalTime& now, int hour, int minute, int second, bool nextDay,
               long long remainingSeconds, bool firstFrame) {
    if (firstFrame) {
        console::clear();
        ui::banner("UildWork 小闹钟", kWidth);
        console::writeLine(std::string());
    } else {
        console::moveTo(4, 1);
    }

    const std::string targetLine = std::string("闹钟时间 ") + app::formatDate(now) + " " +
                                   text::pad2(hour) + ":" + text::pad2(minute) + ":" +
                                   text::pad2(second) + (nextDay ? "（次日）" : "");
    console::ScopedColor target(console::BrightCyan);
    console::writeLine(text::center(targetLine, kWidth));
    console::resetColor();

    ui::bigTime(text::clockText(remainingSeconds), console::BrightGreen);

    console::ScopedColor current(console::BrightWhite);
    console::writeLine(text::center("当前时间 " + app::formatClock(now), kWidth));
    console::resetColor();

    if (firstFrame) {
        console::writeLine(std::string());
        console::ScopedColor hint(console::Gray);
        console::writeLine("Q 或 Esc 取消闹钟");
        console::resetColor();
    }
}

}  // namespace

int runAlarm() {
    console::init("UildWork 小闹钟");
    console::clear();
    ui::banner("UildWork 小闹钟", kWidth);
    console::writeLine(std::string());
    console::ScopedColor hint(console::Gray);
    console::writeLine("按 时:分:秒 输入闹钟时间（例如 07:30:00），输入 q 取消。");
    console::resetColor();

    int hour = 0;
    int minute = 0;
    int second = 0;
    if (!input::readTimeOfDay("请输入闹钟时间：", hour, minute, second)) {
        return 0;
    }
    const int ringtone = audio::chooseRingtone("选择闹钟铃声");

    const app::LocalTime start = app::localNow();
    const long long targetOfDay = static_cast<long long>(hour) * 3600 + minute * 60 + second;
    const bool nextDay = targetOfDay <= secondsOfDay(start);

    bool firstFrame = true;
    long long previousRemaining = -1;
    int framesWithoutConsole = 0;

    for (;;) {
        const app::LocalTime now = app::localNow();
        long long remaining = targetOfDay - secondsOfDay(now);
        if (remaining <= 0) {
            remaining += 86400;
        }
        // 跨过设定时刻：上一帧还剩不到 1 秒，这一帧回绕成接近一天
        if (previousRemaining >= 0 && previousRemaining <= 1 && remaining > 86390) {
            drawAlarm(now, hour, minute, second, nextDay, 0, firstFrame);
            audio::ringUntilDismissed(ringtone, "闹钟时间到！");
            return 0;
        }
        previousRemaining = remaining;

        drawAlarm(now, hour, minute, second, nextDay, remaining, firstFrame);
        firstFrame = false;

        if (input::keyPressed()) {
            const input::KeyEvent event = input::readKey();
            if (event.key == input::Key::Escape || event.isCharacter('q')) {
                return 0;
            }
        }
        if (!input::isInteractive() && ++framesWithoutConsole >= 4) {
            return 0;
        }
        timer::sleepMs(200);
    }
}

}  // namespace uw
