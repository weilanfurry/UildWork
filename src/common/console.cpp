#include "console.h"

#include "text.h"

#include <windows.h>

#include <cstdio>

namespace uw {
namespace console {

namespace {

HANDLE g_output = INVALID_HANDLE_VALUE;
bool g_hasConsole = false;
int g_foreground = White;
int g_background = Black;

HANDLE outputHandle() {
    if (g_output == INVALID_HANDLE_VALUE) {
        g_output = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    return g_output;
}

// 输出一段 UTF-8 文本。连着控制台时转成 UTF-16 交给 WriteConsoleW，
// 被重定向时原样写出 UTF-8 字节，保证两种场景都不乱码。
void emit(const std::string& utf8Text) {
    if (utf8Text.empty()) {
        return;
    }
    if (g_hasConsole) {
        const std::wstring wideText = text::toWide(utf8Text);
        if (!wideText.empty()) {
            const wchar_t* cursor = wideText.c_str();
            size_t remaining = wideText.size();
            while (remaining > 0) {
                const DWORD chunk = static_cast<DWORD>(remaining > 4096 ? 4096 : remaining);
                DWORD written = 0;
                if (!WriteConsoleW(outputHandle(), cursor, chunk, &written, NULL) || written == 0) {
                    break;
                }
                cursor += written;
                remaining -= written;
            }
        }
    } else {
        std::fwrite(utf8Text.data(), 1, utf8Text.size(), stdout);
        std::fflush(stdout);
    }
}

}  // namespace

void init(const std::string& utf8Title) {
    g_output = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    g_hasConsole = (GetConsoleMode(g_output, &mode) != FALSE);
    if (g_hasConsole) {
        // 使用 Unicode 通道输出，代码页只影响其它程序读取标准输出的场景
        SetConsoleOutputCP(CP_UTF8);
        resetColor();
        setCursorVisible(true);
    }
    if (!utf8Title.empty()) {
        setTitle(utf8Title);
    }
}

bool isConsole() {
    return g_hasConsole;
}

void setTitle(const std::string& utf8Title) {
    SetConsoleTitleW(text::toWide(utf8Title).c_str());
}

void setColor(int foreground, int background) {
    g_foreground = foreground;
    g_background = background;
    if (!g_hasConsole) {
        return;
    }
    const WORD attribute = static_cast<WORD>((background & 0x0F) << 4 | (foreground & 0x0F));
    SetConsoleTextAttribute(outputHandle(), attribute);
}

void resetColor() {
    setColor(White, Black);
}

int currentForeground() {
    return g_foreground;
}

int currentBackground() {
    return g_background;
}

ScopedColor::ScopedColor(int foreground, int background)
    : previousForeground_(g_foreground), previousBackground_(g_background) {
    setColor(foreground, background);
}

ScopedColor::~ScopedColor() {
    setColor(previousForeground_, previousBackground_);
}

void clear() {
    if (!g_hasConsole) {
        // 被重定向时不做任何事，避免把控制台控制序列写进日志文件
        return;
    }
    const HANDLE handle = outputHandle();
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!GetConsoleScreenBufferInfo(handle, &info)) {
        return;
    }
    const DWORD cellCount = static_cast<DWORD>(info.dwSize.X) * static_cast<DWORD>(info.dwSize.Y);
    const COORD home = {0, 0};
    DWORD written = 0;
    FillConsoleOutputCharacterW(handle, L' ', cellCount, home, &written);
    FillConsoleOutputAttribute(
        handle, static_cast<WORD>((g_background & 0x0F) << 4 | (g_foreground & 0x0F)), cellCount, home,
        &written);
    SetConsoleCursorPosition(handle, home);
}

void setCursorVisible(bool visible) {
    if (!g_hasConsole) {
        return;
    }
    const HANDLE handle = outputHandle();
    CONSOLE_CURSOR_INFO info;
    if (!GetConsoleCursorInfo(handle, &info)) {
        return;
    }
    info.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(handle, &info);
}

void moveTo(int row, int column) {
    if (!g_hasConsole) {
        return;
    }
    COORD position;
    position.X = static_cast<SHORT>(column > 0 ? column - 1 : 0);
    position.Y = static_cast<SHORT>(row > 0 ? row - 1 : 0);
    SetConsoleCursorPosition(outputHandle(), position);
}

void write(const std::string& utf8Text) {
    emit(utf8Text);
}

void writeLine(const std::string& utf8Text) {
    emit(utf8Text + "\n");
}

void writeAt(int row, int column, const std::string& utf8Text) {
    moveTo(row, column);
    emit(utf8Text);
}

void infoDialog(const std::string& utf8Title, const std::string& utf8Text) {
    MessageBoxW(NULL, text::toWide(utf8Text).c_str(), text::toWide(utf8Title).c_str(),
                MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND);
}

}  // namespace console
}  // namespace uw
