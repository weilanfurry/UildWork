#pragma once
// UildWork 路径与进程层
//
// 解决的问题：
//   1. 原代码依赖当前工作目录（system("start Statistics.exe")、导出文件写到 CWD）。
//      安装到 Program Files 后，快捷方式启动时 CWD 不是程序目录，会静默失败。
//      这里统一以“可执行文件所在目录”为基准解析资源。
//   2. 用 CreateProcess + CREATE_NEW_CONSOLE 替换 system("start ...")，
//      不经过 cmd.exe，路径带空格也不会出问题，并且能拿到失败原因。

#include <string>
#include <vector>

namespace uw {
namespace paths {

// 可执行文件所在目录（不带结尾反斜杠），例如 D:\Uild\UildWork\ZERO\app
const std::string& executableDirectory();

// 当前可执行文件完整路径
std::string executablePath();

// 当前可执行文件名，例如 UildWork.exe
std::string executableName();

// 在 程序目录\、程序目录\assets\、上级目录及其 assets\ 中查找资源，
// 找到返回绝对路径，否则返回空字符串
std::string findResource(const std::string& fileName);

bool fileExists(const std::string& path);

// 让 path 成为绝对路径（相对路径按程序目录解析）
std::string absoluteFrom(const std::string& path);

// 文档目录（用于导出等写操作），取不到时退化为程序目录
std::string documentsDirectory();

// 确保目录存在，返回是否可写
bool ensureDirectory(const std::string& path);

// 启动一个独立进程（新开一个控制台窗口），成功返回 true
bool launchDetached(const std::string& executablePath, const std::string& arguments = std::string());

// 命令行参数（UTF-8），arguments()[0] 是程序自身路径。
// 不用 main(char** argv)：那套参数经过 ANSI 代码页转换，
// 中文路径（例如 "D:\源码整合\待办.udf"）会变成 GBK 字节而无法使用。
std::vector<std::string> arguments();

// 优先启动同目录下的独立小工具（保持旧版本的部署方式）；
// 找不到时退化为“用自身 + 子命令”启动，成功返回 true
bool launchTool(const std::string& legacyExecutableName, const std::string& subCommand);

}  // namespace paths
}  // namespace uw
