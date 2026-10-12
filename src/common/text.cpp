#include "text.h"

#include <windows.h>

#include <cstdlib>
#include <cstring>

namespace uw {
namespace text {

namespace {

// 把一个 UTF-8 字符解码为码点，返回消耗的字节数（非法字节按 1 字节跳过）
int decodeUtf8(const std::string& utf8Text, size_t index, unsigned int& codePoint) {
    const unsigned char first = static_cast<unsigned char>(utf8Text[index]);
    if (first < 0x80) {
        codePoint = first;
        return 1;
    }
    int extra = 0;
    if ((first & 0xE0) == 0xC0) {
        codePoint = first & 0x1F;
        extra = 1;
    } else if ((first & 0xF0) == 0xE0) {
        codePoint = first & 0x0F;
        extra = 2;
    } else if ((first & 0xF8) == 0xF0) {
        codePoint = first & 0x07;
        extra = 3;
    } else {
        codePoint = first;
        return 1;
    }
    if (index + extra >= utf8Text.size()) {
        return 1;
    }
    for (int i = 1; i <= extra; ++i) {
        const unsigned char next = static_cast<unsigned char>(utf8Text[index + i]);
        if ((next & 0xC0) != 0x80) {
            return 1;
        }
        codePoint = (codePoint << 6) | (next & 0x3F);
    }
    return extra + 1;
}

// 判断码点是否占两个终端列（CJK 及全角符号）
bool isWideCodePoint(unsigned int codePoint) {
    if (codePoint >= 0x1100 && codePoint <= 0x115F) return true;  // 韩文字母
    if (codePoint >= 0x2E80 && codePoint <= 0x303E) return true;  // 部首、汉字扩展
    if (codePoint >= 0x3041 && codePoint <= 0x33FF) return true;  // 假名、注音、符号
    if (codePoint >= 0x3400 && codePoint <= 0x4DBF) return true;  // 汉字扩展 A
    if (codePoint >= 0x4E00 && codePoint <= 0x9FFF) return true;  // 基本汉字
    if (codePoint >= 0xA000 && codePoint <= 0xA4CF) return true;  // 彝文
    if (codePoint >= 0xAC00 && codePoint <= 0xD7A3) return true;  // 韩文音节
    if (codePoint >= 0xF900 && codePoint <= 0xFAFF) return true;  // 兼容汉字
    if (codePoint >= 0xFE30 && codePoint <= 0xFE6F) return true;  // 兼容形式
    if (codePoint >= 0xFF00 && codePoint <= 0xFF60) return true;  // 全角 ASCII
    if (codePoint >= 0xFFE0 && codePoint <= 0xFFE6) return true;  // 全角符号
    if (codePoint >= 0x20000 && codePoint <= 0x3FFFD) return true;  // 汉字扩展 B 以上
    return false;
}

}  // namespace

std::wstring toWide(const std::string& utf8Text) {
    if (utf8Text.empty()) {
        return std::wstring();
    }
    const int size = MultiByteToWideChar(CP_UTF8, 0, utf8Text.c_str(),
                                        static_cast<int>(utf8Text.size()), NULL, 0);
    if (size <= 0) {
        return std::wstring();
    }
    std::wstring result(static_cast<size_t>(size), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8Text.c_str(),
                        static_cast<int>(utf8Text.size()), &result[0], size);
    return result;
}

std::string fromWide(const std::wstring& wideText) {
    if (wideText.empty()) {
        return std::string();
    }
    const int size = WideCharToMultiByte(CP_UTF8, 0, wideText.c_str(),
                                        static_cast<int>(wideText.size()), NULL, 0, NULL, NULL);
    if (size <= 0) {
        return std::string();
    }
    std::string result(static_cast<size_t>(size), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wideText.c_str(), static_cast<int>(wideText.size()),
                        &result[0], size, NULL, NULL);
    return result;
}

int displayWidth(const std::string& utf8Text) {
    int width = 0;
    size_t index = 0;
    while (index < utf8Text.size()) {
        unsigned int codePoint = 0;
        const int consumed = decodeUtf8(utf8Text, index, codePoint);
        index += static_cast<size_t>(consumed);
        if (codePoint == '\t') {
            width += 4;
        } else if (codePoint >= 0x20) {
            width += isWideCodePoint(codePoint) ? 2 : 1;
        }
    }
    return width;
}

std::string center(const std::string& utf8Text, int width) {
    const int textWidth = displayWidth(utf8Text);
    if (textWidth >= width) {
        return utf8Text;
    }
    const int left = (width - textWidth) / 2;
    return std::string(static_cast<size_t>(left), ' ') + utf8Text;
}

std::string repeat(const std::string& unit, int times) {
    std::string result;
    if (times <= 0 || unit.empty()) {
        return result;
    }
    result.reserve(unit.size() * static_cast<size_t>(times));
    for (int i = 0; i < times; ++i) {
        result += unit;
    }
    return result;
}

std::string pad2(int value) {
    if (value < 0) {
        value = 0;
    }
    std::string result;
    if (value < 10) {
        result += '0';
    }
    result += std::to_string(value);
    return result;
}

std::string padLeft(const std::string& utf8Text, int width) {
    const int textWidth = displayWidth(utf8Text);
    if (textWidth >= width) {
        return utf8Text;
    }
    return std::string(static_cast<size_t>(width - textWidth), ' ') + utf8Text;
}

std::string clockText(long long totalSeconds) {
    if (totalSeconds < 0) {
        totalSeconds = 0;
    }
    const long long hours = totalSeconds / 3600;
    const long long minutes = (totalSeconds % 3600) / 60;
    const long long seconds = totalSeconds % 60;
    std::string result;
    if (hours < 10) {
        result += '0';
    }
    result += std::to_string(hours);
    result += ':';
    result += pad2(static_cast<int>(minutes));
    result += ':';
    result += pad2(static_cast<int>(seconds));
    return result;
}

std::string humanText(long long totalSeconds) {
    if (totalSeconds < 0) {
        totalSeconds = 0;
    }
    const long long hours = totalSeconds / 3600;
    const long long minutes = (totalSeconds % 3600) / 60;
    const long long seconds = totalSeconds % 60;
    std::string result;
    if (hours > 0) {
        result += std::to_string(hours) + " 时 ";
    }
    if (hours > 0 || minutes > 0) {
        result += std::to_string(minutes) + " 分 ";
    }
    result += std::to_string(seconds) + " 秒";
    return result;
}

std::string trim(const std::string& utf8Text) {
    std::string result = utf8Text;
    while (!result.empty()) {
        const unsigned char front = static_cast<unsigned char>(result[0]);
        const unsigned char back = static_cast<unsigned char>(result[result.size() - 1]);
        const bool frontSpace = (front == ' ' || front == '\t' || front == '\r' || front == '\n');
        const bool backSpace = (back == ' ' || back == '\t' || back == '\r' || back == '\n');
        if (frontSpace) {
            result.erase(0, 1);
            continue;
        }
        if (backSpace) {
            result.erase(result.size() - 1, 1);
            continue;
        }
        // 全角空格 U+3000 = EF BC ... 实际为 E3 80 80
        if (result.size() >= 3 && static_cast<unsigned char>(result[0]) == 0xE3 &&
            static_cast<unsigned char>(result[1]) == 0x80 &&
            static_cast<unsigned char>(result[2]) == 0x80) {
            result.erase(0, 3);
            continue;
        }
        if (result.size() >= 3) {
            const size_t tail = result.size() - 3;
            if (static_cast<unsigned char>(result[tail]) == 0xE3 &&
                static_cast<unsigned char>(result[tail + 1]) == 0x80 &&
                static_cast<unsigned char>(result[tail + 2]) == 0x80) {
                result.erase(tail, 3);
                continue;
            }
        }
        break;
    }
    return result;
}

bool parseLong(const std::string& utf8Text, long long& value) {
    const std::string cleaned = trim(utf8Text);
    if (cleaned.empty()) {
        return false;
    }
    size_t index = 0;
    bool negative = false;
    if (cleaned[0] == '+' || cleaned[0] == '-') {
        negative = (cleaned[0] == '-');
        index = 1;
    }
    if (index >= cleaned.size()) {
        return false;
    }
    long long result = 0;
    for (; index < cleaned.size(); ++index) {
        const char ch = cleaned[index];
        if (ch < '0' || ch > '9') {
            return false;
        }
        result = result * 10 + (ch - '0');
        if (result > 1000000000000LL) {
            // 明显超出合理范围，视为非法输入
            return false;
        }
    }
    value = negative ? -result : result;
    return true;
}

bool endsWith(const std::string& utf8Text, const std::string& suffix) {
    if (suffix.size() > utf8Text.size()) {
        return false;
    }
    return utf8Text.compare(utf8Text.size() - suffix.size(), suffix.size(), suffix) == 0;
}

}  // namespace text
}  // namespace uw
