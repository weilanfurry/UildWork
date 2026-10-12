#pragma once
// UildWork 文件读写（UTF-8 文本）
//
// 为什么不用 std::ofstream：
//   Windows 上 MinGW 的 fstream 用 ANSI 代码页打开路径，遇到中文目录会失败，
//   例如程序位于 D:\...\源码整合\... 时写入会直接报错。
//   这里统一用 _wfopen（宽字符路径）+ UTF-8 字节，路径和内容都不受代码页影响。

#include <string>
#include <vector>

namespace uw {
namespace fileio {

// 覆盖写入 UTF-8 文本（不写 BOM）
bool writeText(const std::string& utf8Path, const std::string& utf8Content, std::string& error);

// 读取文本文件的每一行，自动去掉行尾 \r 与开头的 UTF-8 BOM
bool readLines(const std::string& utf8Path, std::vector<std::string>& lines, std::string& error);

}  // namespace fileio
}  // namespace uw
