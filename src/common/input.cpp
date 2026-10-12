#include "input.h"

#include "console.h"
#include "text.h"

#include <windows.h>

#include <conio.h>
#include <io.h>

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace uw {
namespace input {

namespace {

bool g_interactive = true;
bool g_probed = false;
bool g_endOfInput = false;

bool probeInteractive() {
    if (!g_probed) {
        g_probed = true;
        // 用 Win32 判断标准输入是否连着控制台（比 _fileno/_isatty 更可靠：
        // 后者在 -std=c++11（__STRICT_ANSI__）下不会被 MinGW 头文件声明）
        const HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode = 0;
        const bool consoleInput =
            (inputHandle != NULL && inputHandle != INVALID_HANDLE_VALUE &&
             GetConsoleMode(inputHandle, &mode) != FALSE);
        g_interactive = consoleInput && console::isConsole();
    }
    return g_interactive;
}

// 非交互环境下从标准输入读一行，返回 false 表示已到文件末尾
bool readLineFromStream(std::string& line) {
    if (!std::getline(std::cin, line)) {
        return false;
    }
    return true;
}

KeyEvent fromLine(const std::string& line) {
    const std::string cleaned = text::trim(line);
    if (cleaned.empty()) {
        return KeyEvent(Key::Enter);
    }
    std::string lower;
    for (size_t i = 0; i < cleaned.size(); ++i) {
        lower += static_cast<char>(std::tolower(static_cast<unsigned char>(cleaned[i])));
    }
    // 只识别完整单词，不做 w/a/s/d/h/j/k/l 这类单键别名：
    // 各工具把 a/d/s/l 等用作功能快捷键（新增/删除/保存/另存为），
    // 单键别名会把这些按键抢走，脚本化输入也会变得含糊。
    if (lower == "up") return KeyEvent(Key::Up);
    if (lower == "down") return KeyEvent(Key::Down);
    if (lower == "left") return KeyEvent(Key::Left);
    if (lower == "right") return KeyEvent(Key::Right);
    if (lower == "enter" || lower == "ok") return KeyEvent(Key::Enter);
    if (lower == "esc" || lower == "escape" || lower == "q") return KeyEvent(Key::Escape);
    if (lower == "space") return KeyEvent(Key::Space);
    return KeyEvent(Key::Character, cleaned[0]);
}

}  // namespace

bool isInteractive() {
    return probeInteractive();
}

void clearBuffer() {
    if (!probeInteractive()) {
        return;
    }
    while (_kbhit()) {
        _getch();
    }
}

bool keyPressed() {
    if (!probeInteractive()) {
        return false;
    }
    return _kbhit() != 0;
}

KeyEvent readKey() {
    if (!probeInteractive()) {
        std::string line;
        if (!readLineFromStream(line)) {
            // 输入结束：恒返回 Esc，保证调用方的循环可以退出
            g_endOfInput = true;
            return KeyEvent(Key::Escape);
        }
        g_endOfInput = false;
        return fromLine(line);
    }
    g_endOfInput = false;
    const int first = _getch();
    if (first == 0 || first == 224) {
        switch (_getch()) {
            case 72: return KeyEvent(Key::Up);
            case 80: return KeyEvent(Key::Down);
            case 75: return KeyEvent(Key::Left);
            case 77: return KeyEvent(Key::Right);
            case 71: return KeyEvent(Key::Home);
            case 79: return KeyEvent(Key::End);
            case 82: return KeyEvent(Key::Insert);
            case 83: return KeyEvent(Key::Delete);
            default: return KeyEvent(Key::None);
        }
    }
    switch (first) {
        case 13: return KeyEvent(Key::Enter);
        case 27: return KeyEvent(Key::Escape);
        case 32: return KeyEvent(Key::Space);
        case 9: return KeyEvent(Key::Tab);
        case 8: return KeyEvent(Key::Backspace);
        case 3: return KeyEvent(Key::Escape);  // Ctrl+C 等价于放弃
        default: break;
    }
    return KeyEvent(Key::Character, static_cast<char>(first));
}

bool atEndOfInput() {
    return g_endOfInput;
}

char readChar(const std::string& allowed, bool echo) {
    for (;;) {
        const KeyEvent event = readKey();
        if (event.key == Key::Escape) {
            return 27;
        }
        if (event.key == Key::Enter && allowed.find('\r') != std::string::npos) {
            return '\r';
        }
        if (event.key == Key::Character) {
            for (size_t i = 0; i < allowed.size(); ++i) {
                const char candidate = allowed[i];
                const char lowerEvent = static_cast<char>(std::tolower(static_cast<unsigned char>(event.character)));
                const char lowerCandidate = static_cast<char>(std::tolower(static_cast<unsigned char>(candidate)));
                if (lowerEvent == lowerCandidate) {
                    if (echo) {
                        console::writeLine(std::string(1, event.character));
                    }
                    return event.character;
                }
            }
        }
    }
}

int readInt(const std::string& utf8Prompt, int lowest, int highest) {
    for (;;) {
        console::write(utf8Prompt);
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            return lowest;
        }
        long long value = 0;
        if (text::parseLong(line, value) && value >= lowest && value <= highest) {
            return static_cast<int>(value);
        }
        console::ScopedColor warning(console::BrightRed);
        console::writeLine("输入无效，请输入 " + std::to_string(lowest) + " 到 " +
                           std::to_string(highest) + " 之间的整数。");
    }
}

double readDouble(const std::string& utf8Prompt, double lowest, double highest) {
    for (;;) {
        console::write(utf8Prompt);
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            return lowest;
        }
        const std::string cleaned = text::trim(line);
        bool ok = !cleaned.empty();
        double value = 0.0;
        if (ok) {
            char* end = NULL;
            value = std::strtod(cleaned.c_str(), &end);
            ok = (end != NULL && *end == '\0');
        }
        if (ok && value >= lowest && value <= highest) {
            return value;
        }
        console::ScopedColor warning(console::BrightRed);
        console::writeLine("输入无效，请输入一个数值。");
    }
}

std::string readLine(const std::string& utf8Prompt, bool allowEmpty) {
    for (;;) {
        console::write(utf8Prompt);
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            return std::string();
        }
        line = text::trim(line);
        if (!line.empty() || allowEmpty) {
            return line;
        }
        console::ScopedColor warning(console::BrightRed);
        console::writeLine("内容不能为空，请重新输入。");
    }
}

bool readTimeOfDay(const std::string& utf8Prompt, int& hour, int& minute, int& second) {
    for (;;) {
        console::write(utf8Prompt);
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            return false;
        }
        line = text::trim(line);
        if (line.empty()) {
            return false;
        }
        if (line == "q" || line == "Q" || line == "quit") {
            return false;
        }
        // 允许使用全角冒号（：的 UTF-8 编码为 EF BC 9A），统一替换成半角
        std::string normalized;
        for (size_t i = 0; i < line.size(); ++i) {
            if (i + 2 < line.size() && static_cast<unsigned char>(line[i]) == 0xEF &&
                static_cast<unsigned char>(line[i + 1]) == 0xBC &&
                static_cast<unsigned char>(line[i + 2]) == 0x9A) {
                normalized += ':';
                i += 2;
            } else {
                normalized += line[i];
            }
        }
        int values[3] = {0, 0, 0};
        int count = 0;
        std::string current;
        for (size_t i = 0; i <= normalized.size(); ++i) {
            const char ch = (i < normalized.size()) ? normalized[i] : ':';
            if (ch == ':') {
                if (!current.empty() && count < 3) {
                    long long value = 0;
                    if (!text::parseLong(current, value)) {
                        count = -1;
                        break;
                    }
                    values[count++] = static_cast<int>(value);
                    current.clear();
                }
            } else if (ch != ' ') {
                current += ch;
            }
        }
        if (count == 3 && values[0] >= 0 && values[0] <= 23 && values[1] >= 0 && values[1] <= 59 &&
            values[2] >= 0 && values[2] <= 59) {
            hour = values[0];
            minute = values[1];
            second = values[2];
            return true;
        }
        console::ScopedColor warning(console::BrightRed);
        console::writeLine("时间格式无效，请按 时:分:秒 输入（例如 07:30:00），或输入 q 取消。");
    }
}

void pause(const std::string& utf8Prompt) {
    if (!utf8Prompt.empty()) {
        console::write(utf8Prompt);
    }
    readKey();
}

}  // namespace input
}  // namespace uw
