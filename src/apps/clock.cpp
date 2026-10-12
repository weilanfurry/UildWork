// UildWork 实时时钟（对应原 shishishizhong.cpp）
//
// 原实现每秒 system("cls") 一次（每次都创建 cmd.exe 进程），且只能 Ctrl+C 退出。
// 现在：只在秒数变化时重绘、支持大号数字与日期开关、Q/Esc 退出。

#include "apps.h"

#include "../common/appinfo.h"
#include "../common/console.h"
#include "../common/input.h"
#include "../common/text.h"
#include "../common/timer.h"
#include "../common/ui.h"

namespace uw {

namespace {

const int kWidth = 52;

void drawClock(bool bigDigits, bool showDate, const app::LocalTime& now, bool firstFrame) {
    if (firstFrame) {
        console::clear();
        ui::banner("UildWork 实时时钟", kWidth);
        console::writeLine(std::string());
    } else {
        // 只回到时间区域首行重绘，避免整屏闪烁
        console::moveTo(4, 1);
    }

    const std::string dateLine =
        showDate ? (app::formatDate(now) + "  " + app::weekdayName(now.weekday) + "  " +
                    app::greetingForHour(now.hour))
                 : std::string(" ");
    console::ScopedColor dateColor(console::BrightCyan);
    console::writeLine(text::center(dateLine, kWidth));
    console::resetColor();

    if (bigDigits) {
        ui::bigTime(text::pad2(now.hour) + ":" + text::pad2(now.minute) + ":" + text::pad2(now.second),
                    console::BrightGreen);
    } else {
        console::ScopedColor color(console::BrightGreen);
        console::writeLine(text::center(text::pad2(now.hour) + ":" + text::pad2(now.minute) + ":" +
                                            text::pad2(now.second),
                                        kWidth));
        console::resetColor();
    }

    if (firstFrame) {
        console::writeLine(std::string());
        console::ScopedColor hint(console::Gray);
        console::writeLine("B 切换大号/紧凑显示 · D 显示/隐藏日期 · Q 或 Esc 退出");
        console::resetColor();
    }
}

}  // namespace

int runClock() {
    console::init("UildWork 实时时钟");

    bool bigDigits = true;
    bool showDate = true;
    bool firstFrame = true;
    int lastSecond = -1;
    int framesWithoutConsole = 0;

    for (;;) {
        if (input::keyPressed()) {
            const input::KeyEvent event = input::readKey();
            if (event.key == input::Key::Escape || event.isCharacter('q')) {
                return 0;
            }
            if (event.isCharacter('b')) {
                bigDigits = !bigDigits;
                firstFrame = true;
                lastSecond = -1;
                continue;
            }
            if (event.isCharacter('d')) {
                showDate = !showDate;
                firstFrame = true;
                lastSecond = -1;
                continue;
            }
        }

        const app::LocalTime now = app::localNow();
        if (now.second != lastSecond || firstFrame) {
            drawClock(bigDigits, showDate, now, firstFrame);
            lastSecond = now.second;
            firstFrame = false;
            if (!input::isInteractive()) {
                // 没有连接控制台（例如被重定向）时不要死循环
                if (++framesWithoutConsole >= 3) {
                    return 0;
                }
            }
        }
        timer::sleepMs(50);
    }
}

}  // namespace uw
