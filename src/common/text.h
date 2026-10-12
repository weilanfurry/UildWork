#pragma once
// UildWork 文本工具：UTF-8 <-> UTF-16 转换、宽度计算、对齐与格式化。

#include <string>

namespace uw {
namespace text {

std::wstring toWide(const std::string& utf8Text);
std::string fromWide(const std::wstring& wideText);

// 终端显示宽度：中日韩全角字符按 2 列计算
int displayWidth(const std::string& utf8Text);

// 在给定宽度内居中（按显示宽度计算，不是字节数）
std::string center(const std::string& utf8Text, int width);

std::string repeat(const std::string& unit, int times);

// 左侧补 0 的两位数字，例如 7 -> "07"
std::string pad2(int value);
std::string padLeft(const std::string& utf8Text, int width);

// 秒数格式化：3725 -> "01:02:05"
std::string clockText(long long totalSeconds);
// 秒数中文描述：3725 -> "1 时 2 分 5 秒"
std::string humanText(long long totalSeconds);

// 去除首尾空白（含全角空格）
std::string trim(const std::string& utf8Text);

// 解析非负整数，成功返回 true
bool parseLong(const std::string& utf8Text, long long& value);

bool endsWith(const std::string& utf8Text, const std::string& suffix);

}  // namespace text
}  // namespace uw
