#pragma once
// UildWork 各工具入口。
//
// 集成后整个工具集共用一份基础代码，每个工具既可以作为子命令运行
// （UildWork.exe statistics），也保留了旧版独立 exe 的编译目标。

#include <string>

namespace uw {

// 主菜单（原 main.cpp）
int runLauncher(const std::string& fileToOpen);
// 任务清单（原 Manifest.cpp），fileToOpen 为 .udf 清单文件路径
int runManifest(const std::string& fileToOpen);
// 图表统计（原 Statistics.cpp）
int runStatistics();
// 智能秒表 / 模式选择（原 Stopwatch.cpp）
int runStopwatch();
// 实时时钟（原 shishishizhong.cpp）
int runClock();
// 正计时（原 zhengjishi.cpp）
int runCountUp();
// 倒计时（原 daojishi.cpp）
int runCountDown();
// 小闹钟（原 naozhong.cpp）
int runAlarm();
// 番茄钟（原 fanqiezhong.cpp）
int runPomodoro();

void printHelp();
void printVersion();

}  // namespace uw
