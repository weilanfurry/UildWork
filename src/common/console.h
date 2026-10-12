#pragma once
// UildWork 控制台基础层
//
// 统一处理：控制台颜色、清屏、光标、窗口标题、UTF-8 输出、对话框。
// 设计要点：
//   * 所有输出走 WriteConsoleW（UTF-8 -> UTF-16），因此不依赖控制台代码页，
//     中文在把 stdout 重定向到文件时也不会变成乱码。
//   * 清屏使用 Win32 API，不再 system("cls")（避免每次清屏都创建 cmd.exe 进程）。
//   * 颜色统一使用 SetConsoleTextAttribute，废弃原实现里混杂的 system("color XX")。

#include <string>

namespace uw {
namespace console {

// 标准 16 色，对应 SetConsoleTextAttribute 的低 4 位
enum Color {
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Yellow = 6,
    White = 7,
    Gray = 8,
    BrightBlue = 9,
    BrightGreen = 10,
    BrightCyan = 11,
    BrightRed = 12,
    BrightMagenta = 13,
    BrightYellow = 14,
    BrightWhite = 15
};

// 进程启动时调用一次：探测是否真的连着控制台，并设置标题
void init(const std::string& utf8Title = std::string());

// 是否连着真实控制台窗口（被重定向时为 false）
bool isConsole();

void setTitle(const std::string& utf8Title);

void setColor(int foreground, int background = Black);
void resetColor();
int currentForeground();
int currentBackground();

// 作用域内使用指定颜色的 RAII 小工具
class ScopedColor {
public:
    ScopedColor(int foreground, int background = Black);
    ~ScopedColor();

private:
    int previousForeground_;
    int previousBackground_;
    ScopedColor(const ScopedColor&);
    ScopedColor& operator=(const ScopedColor&);
};

void clear();
void setCursorVisible(bool visible);
void moveTo(int row, int column);  // 1 基坐标

void write(const std::string& utf8Text);
void writeLine(const std::string& utf8Text = std::string());
void writeAt(int row, int column, const std::string& utf8Text);

// 弹出模态提示框（MessageBoxW，自动做 UTF-8 -> UTF-16 转换）
void infoDialog(const std::string& utf8Title, const std::string& utf8Text);

}  // namespace console
}  // namespace uw
