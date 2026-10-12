// UildWork 正计时（对应原 zhengjishi.cpp）
//
// 原实现只有 Sleep(1000) + seconds++，既不能暂停也不能退出。
// 现在支持：空格 暂停/继续、R 归零、Q/Esc 退出；计时基于单调时钟，长时间不漂移。

#include "apps.h"

#include "../common/console.h"
#include "../common/input.h"
#include "../common/text.h"
#include "../common/timer.h"
#include "../common/ui.h"

namespace uw {

namespace {

const int kWidth = 52;

void drawElapsed(long long elapsedMs, bool paused, bool firstFrame) {
    if (firstFrame) {
        console::clear();
        ui::banner("UildWork 正计时", kWidth);
        console::writeLine(std::string());
    } else {
        console::moveTo(4, 1);
    }

    const long long totalSeconds = timer::secondsFloor(elapsedMs);
    ui::bigTime(text::clockText(totalSeconds), console::BrightGreen);

    if (firstFrame) {
        console::writeLine(std::string());
        console::ScopedColor hint(console::Gray);
        console::writeLine("空格 暂停/继续 · R 归零 · Q 或 Esc 退出");
        console::resetColor();
    }
    console::setColor(paused ? console::BrightYellow : console::BrightCyan);
    console::writeLine(text::center(paused ? "[ 已暂停 ]" : std::string("[ 计时中 ]"), kWidth));
    console::resetColor();
}

}  // namespace

int runCountUp() {
    console::init("UildWork 正计时");
    console::clear();
    ui::banner("UildWork 正计时", kWidth);
    console::writeLine(std::string());
    console::setColor(console::BrightWhite);
    console::writeLine(text::center("按任意键开始计时", kWidth));
    console::resetColor();
    console::writeLine(std::string());
    const input::KeyEvent start = input::readKey();
    if (start.key == input::Key::Escape || start.isCharacter('q')) {
        return 0;
    }

    timer::Timer stopwatch;
    bool firstFrame = true;
    bool paused = false;
    int framesWithoutConsole = 0;

    for (;;) {
        const long long elapsed = stopwatch.elapsedMs();
        if (input::keyPressed()) {
            const input::KeyEvent event = input::readKey();
            if (event.key == input::Key::Escape || event.isCharacter('q')) {
                return 0;
            }
            if (event.key == input::Key::Space) {
                stopwatch.togglePause();
                paused = stopwatch.paused();
                firstFrame = false;
                drawElapsed(elapsed, paused, false);
                continue;
            }
            if (event.isCharacter('r')) {
                stopwatch.reset();
                paused = false;
                firstFrame = true;
                continue;
            }
        }
        drawElapsed(elapsed, paused, firstFrame);
        firstFrame = false;
        if (!input::isInteractive() && ++framesWithoutConsole >= 4) {
            return 0;
        }
        timer::sleepMs(100);
    }
}

}  // namespace uw
