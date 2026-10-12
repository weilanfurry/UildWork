// 命令行帮助与版本信息（printHelp / printVersion）

#include "apps.h"

#include "../common/appinfo.h"
#include "../common/console.h"

#include <string>

namespace uw {

void printVersion() {
    console::init();
    console::writeLine(std::string(app::kName) + " " + app::kVersion);
    console::writeLine(std::string("官网: ") + app::kHomepage);
    console::writeLine(std::string("开发者: ") + app::kAuthors);
}

void printHelp() {
    console::init();
    console::writeLine(std::string(app::kName) + " " + app::kVersion +
                       " - 多功能生产力工具集");
    console::writeLine(std::string());
    console::writeLine("用法: UildWork.exe [命令] [参数]");
    console::writeLine(std::string());
    console::writeLine("命令:");
    console::writeLine("  statistics        图表统计（数据录入、统计计算、导出）");
    console::writeLine("  manifest [文件]   任务清单，可指定 .udf 清单文件");
    console::writeLine("  stopwatch         智能秒表（时钟 / 计时 / 闹钟 / 番茄钟）");
    console::writeLine("  clock             实时时钟");
    console::writeLine("  countup           正计时");
    console::writeLine("  countdown         倒计时");
    console::writeLine("  alarm             小闹钟");
    console::writeLine("  pomodoro          番茄钟");
    console::writeLine("  help              显示本帮助");
    console::writeLine("  version           显示版本信息");
    console::writeLine(std::string());
    console::writeLine("不带任何命令时启动主菜单；双击 .udf 清单文件会直接打开任务清单。");
}

}  // namespace uw
