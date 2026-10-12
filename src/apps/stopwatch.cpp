// UildWork 智能秒表（对应原 Stopwatch.cpp）
//
// 原实现是 while(true) 里 getch() 后 system("start X.exe")，
// 既不能退出（Esc 无效），时间显示也不会刷新。
// 现在改为菜单驱动，Esc 返回主菜单，并且复用统一的时间/问候语显示。

#include "apps.h"

#include "../common/appinfo.h"
#include "../common/console.h"
#include "../common/input.h"
#include "../common/paths.h"
#include "../common/text.h"
#include "../common/timer.h"
#include "../common/ui.h"

#include <vector>

namespace uw {

namespace {

const int kWidth = 52;

void showHeader() {
    ui::banner("UildWork 智能秒表", kWidth);
    const app::LocalTime now = app::localNow();
    console::setColor(console::BrightCyan);
    console::write("您的时间：");
    console::setColor(console::BrightWhite);
    console::writeLine(app::formatDate(now) + " " + app::weekdayName(now.weekday) + " " +
                       text::pad2(now.hour) + ":" + text::pad2(now.minute));
    console::resetColor();
    console::writeLine(std::string());
}

}  // namespace

int runStopwatch() {
    console::init("UildWork 智能秒表");
    // 问候框是模态对话框，只在真有控制台时弹出：
    // 被脚本/管道调用时它会一直阻塞，也没人点“确定”。
    if (console::isConsole() && input::isInteractive()) {
        app::showGreetingDialog();
    }

    const std::vector<ui::MenuItem> items = {
        {"实时时钟", "当前日期、时间与大号数字显示"},
        {"正计时", "从 0 开始累计，可暂停"},
        {"倒计时", "1 小时 / 30 分 / 10 分 / 1 分 / 自定义"},
        {"小闹钟", "指定时刻提醒，可循环播放铃声"},
        {"番茄钟", "25 分钟工作 + 5 分钟休息"},
        {"返回主菜单", ""}};

    for (;;) {
        console::clear();
        showHeader();
        const int choice = ui::selectMenu("请选择模式", items, 0);
        if (choice < 0 || choice == 5) {
            return 0;
        }
        static const char* const kLegacy[] = {"shishishizhong.exe", "zhengjishi.exe", "daojishi.exe",
                                             "naozhong.exe", "fanqiezhong.exe"};
        static const char* const kCommands[] = {"clock", "countup", "countdown", "alarm", "pomodoro"};
        console::ScopedColor color(console::BrightGreen);
        console::writeLine("启动" + items[static_cast<size_t>(choice)].label + "...");
        if (!paths::launchTool(kLegacy[choice], kCommands[choice])) {
            console::ScopedColor warning(console::BrightRed);
            console::writeLine("启动失败：找不到可执行文件。");
            input::pause("按任意键继续...");
        }
        timer::sleepMs(300);
    }
}

}  // namespace uw
