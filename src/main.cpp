// UildWork 主程序入口
//
// 一份可执行文件承载整套工具，通过子命令分发：
//   UildWork.exe                 -> 主菜单
//   UildWork.exe statistics      -> 图表统计
//   UildWork.exe manifest [文件] -> 任务清单（可用 .udf 清单文件路径）
//   UildWork.exe stopwatch       -> 智能秒表（模式选择）
//   UildWork.exe clock           -> 实时时钟
//   UildWork.exe countup         -> 正计时
//   UildWork.exe countdown       -> 倒计时
//   UildWork.exe alarm           -> 小闹钟
//   UildWork.exe pomodoro        -> 番茄钟
//
// 顺带修好了一个存在已久的 bug：安装包把 .udf 关联到
// "{app}\UildWork.exe" "%1"，但旧版 main() 完全忽略命令行参数，
// 双击 .udf 只会打开主菜单。现在会直接打开对应的清单文件。

#include "apps/apps.h"

#include "common/appinfo.h"
#include "common/console.h"
#include "common/paths.h"
#include "common/text.h"

#include <string>

namespace {

std::string toLower(const std::string& value) {
    std::string result = value;
    for (size_t i = 0; i < result.size(); ++i) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = static_cast<char>(result[i] - 'A' + 'a');
        }
    }
    return result;
}

}  // namespace

int main() {
    const std::vector<std::string> args = uw::paths::arguments();
    if (args.size() <= 1) {
        return uw::runLauncher(std::string());
    }

    const std::string command = toLower(args[1]);
    if (command == "statistics" || command == "stats") {
        return uw::runStatistics();
    }
    if (command == "manifest" || command == "tasklist" || command == "tasks") {
        const std::string file = (args.size() > 2) ? args[2] : std::string();
        return uw::runManifest(file.empty() ? file : uw::paths::absoluteFrom(file));
    }
    if (command == "stopwatch") {
        return uw::runStopwatch();
    }
    if (command == "clock") {
        return uw::runClock();
    }
    if (command == "countup") {
        return uw::runCountUp();
    }
    if (command == "countdown") {
        return uw::runCountDown();
    }
    if (command == "alarm") {
        return uw::runAlarm();
    }
    if (command == "pomodoro") {
        return uw::runPomodoro();
    }
    if (command == "help" || command == "-h" || command == "--help" || command == "/?") {
        uw::printHelp();
        return 0;
    }
    if (command == "version" || command == "-v" || command == "--version") {
        uw::printVersion();
        return 0;
    }

    // 文件关联：UildWork.exe "D:\...\待办.udf"
    // 即使文件不存在也交给任务清单处理，它会给出"无法读取文件"的明确提示，
    // 而不是让用户看到一个莫名其妙的"未知命令"。
    const std::string asPath = uw::paths::absoluteFrom(args[1]);
    if (uw::text::endsWith(toLower(asPath), ".udf")) {
        return uw::runManifest(asPath);
    }

    uw::console::init();
    uw::console::setColor(uw::console::BrightRed);
    uw::console::writeLine(std::string("未知命令: ") + args[1]);
    uw::console::resetColor();
    uw::console::writeLine(std::string());
    uw::printHelp();
    return 1;
}
