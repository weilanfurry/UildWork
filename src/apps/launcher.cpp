// UildWork 主菜单（对应原 main.cpp）
//
// 主要变化：
//   * system("title ...") / system("cls") -> 控制台 API，界面不再闪烁。
//   * system("start X.exe") -> CreateProcess + CREATE_NEW_CONSOLE，
//     程序目录被改变时不再静默失败，缺文件会明确提示。
//   * 菜单支持方向键，输入非法选项不再需要空等 1 秒。
//   * 时间/问候语判断从两份合并为一份（app::greetingForHour）。

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

void showTitle() {
    ui::banner(std::string("UILDWORK 生产力工具集"), kWidth);
}

void showLoading() {
    console::ScopedColor color(console::BrightGreen);
    console::write("加载中");
    for (int i = 0; i < 5; ++i) {
        console::write(".");
        timer::sleepMs(160);
    }
    console::writeLine(" 完成!");
}

void showClockAndGreeting() {
    const app::LocalTime now = app::localNow();
    console::setColor(console::BrightMagenta);
    console::write("当前时间: ");
    console::setColor(console::BrightWhite);
    console::write(app::formatDateTime(now) + " " + app::weekdayName(now.weekday));
    console::setColor(console::BrightCyan);
    console::writeLine(" - " + app::greetingForHour(now.hour));
    console::resetColor();
    console::writeLine(std::string());
}

void showAbout() {
    console::clear();
    ui::banner("关于 UildWork", kWidth);
    console::setColor(console::BrightWhite);
    console::writeLine("  UildWork 是一款多功能生产力工具集，包含:");
    console::writeLine(std::string());
    console::setColor(console::BrightGreen);
    console::writeLine("  - 图表统计 - 数据录入、求和/平均、条形图与导出");
    console::setColor(console::BrightCyan);
    console::writeLine("  - 任务清单 - 任务管理与进度跟踪，可存为 .udf 清单");
    console::setColor(console::BrightRed);
    console::writeLine("  - 智能秒表 - 时钟 / 正计时 / 倒计时 / 闹钟 / 番茄钟");
    console::setColor(console::BrightMagenta);
    console::writeLine("  - 铃声提醒 - 内置蜂鸣与两首背景音乐，可随时停止");
    console::writeLine(std::string());
    console::setColor(console::BrightWhite);
    console::writeLine(std::string("  版本: ") + app::kVersion);
    console::writeLine(std::string("  官网: ") + app::kHomepage);
    console::writeLine(std::string("  开发者: ") + app::kAuthors);
    console::resetColor();
    ui::rule("=", kWidth);
    console::writeLine(std::string());
    input::pause("按任意键返回...");
}

void showExitAnimation() {
    for (int i = 0; i < 3; ++i) {
        console::clear();
        console::setColor(console::BrightRed);
        ui::rule("*", kWidth);
        console::writeLine(text::center("感谢使用 UildWork!", kWidth));
        ui::rule("*", kWidth);
        console::setColor(console::BrightYellow);
        console::writeLine(text::center("期待再次相见! 再见!", kWidth));
        console::setColor(console::BrightWhite);
        console::writeLine(text::center("程序即将退出...", kWidth));
        timer::sleepMs(320);
    }
}

// 启动失败时的统一提示（旧版 start 命令失败时只是在控制台里留一行英文报错）
void launchOrWarn(const std::string& legacyName, const std::string& subCommand,
                  const std::string& utf8Label) {
    console::ScopedColor color(console::BrightGreen);
    console::writeLine("启动" + utf8Label + "...");
    if (paths::launchTool(legacyName, subCommand)) {
        timer::sleepMs(300);
        return;
    }
    console::ScopedColor warning(console::BrightRed);
    console::writeLine("启动失败：找不到可执行文件，请检查安装是否完整。");
    input::pause("按任意键继续...");
}

void runAdvancedMenu() {
    const std::vector<ui::MenuItem> items = {
        {"退回 1.3 版本", "启动旧版 UW13.exe"},
        {"关于 UildWork", ""},
        {"卸载 UildWork", "将运行卸载程序"},
        {"返回主菜单", ""}};
    for (;;) {
        const int choice = ui::selectMenu("高级选项", items, 0);
        if (choice < 0 || choice == 3) {
            return;
        }
        if (choice == 0) {
            if (!paths::fileExists(paths::executableDirectory() + "\\UW13.exe")) {
                console::clear();
                ui::banner("高级选项", kWidth);
                console::ScopedColor warning(console::BrightRed);
                console::writeLine("未找到 UW13.exe（1.3 版本程序缺失）。");
                input::pause("按任意键返回...");
                continue;
            }
            launchOrWarn("UW13.exe", std::string(), "1.3 版本程序");
        } else if (choice == 1) {
            showAbout();
        } else if (choice == 2) {
            console::clear();
            ui::banner("卸载 UildWork", kWidth);
            console::setColor(console::BrightYellow);
            console::writeLine("卸载会删除程序文件，确定继续吗? (Y/N)");
            const char answer = input::readChar("yn", true);
            if (answer != 'y' && answer != 'Y') {
                continue;
            }
            if (!paths::fileExists(paths::executableDirectory() + "\\unins000.exe")) {
                console::clear();
                ui::banner("卸载 UildWork", kWidth);
                console::ScopedColor error(console::BrightRed);
                console::writeLine("未找到 unins000.exe，无法卸载。");
                input::pause("按任意键返回...");
                continue;
            }
            launchOrWarn("unins000.exe", std::string(), "卸载程序");
        }
    }
}

}  // namespace

int runLauncher(const std::string& fileToOpen) {
    (void)fileToOpen;
    console::init("UildWork - 多功能生产力工具集");
    console::clear();
    showTitle();
    showLoading();
    timer::sleepMs(500);

    const std::vector<ui::MenuItem> items = {
        {"图表统计", "数据录入 · 统计计算 · 导出"},
        {"任务清单", "任务管理 · .udf 清单存档"},
        {"智能秒表", "时钟 / 计时 / 闹钟 / 番茄钟"},
        {"高级选项", "版本回退 · 关于 · 卸载"},
        {"退出软件", ""}};

    bool keepRunning = true;
    while (keepRunning) {
        console::clear();
        showTitle();
        showClockAndGreeting();

        const int choice = ui::selectMenu("功能菜单", items, 0);
        if (choice < 0) {
            if (input::atEndOfInput()) {
                return 0;  // 输入流已结束（脚本/管道运行），不要空转
            }
            continue;  // 真实控制台上按 Esc：留在主菜单
        }
        switch (choice) {
            case 0:
                launchOrWarn("Statistics.exe", "statistics", "图表统计");
                break;
            case 1:
                launchOrWarn("Manifest.exe", "manifest", "任务清单");
                break;
            case 2:
                launchOrWarn("Stopwatch.exe", "stopwatch", "智能秒表");
                break;
            case 3:
                runAdvancedMenu();
                break;
            case 4: {
                console::clear();
                showTitle();
                console::setColor(console::BrightYellow);
                console::writeLine("确定要退出吗? (Y/N)");
                const char answer = input::readChar("yn", true);
                if (answer == 'y' || answer == 'Y') {
                    showExitAnimation();
                    timer::sleepMs(600);
                    keepRunning = false;
                }
                break;
            }
            default:
                break;
        }
    }
    return 0;
}

}  // namespace uw
