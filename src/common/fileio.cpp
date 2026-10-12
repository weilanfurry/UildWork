#include "fileio.h"

#include "text.h"

#include <cstdio>

namespace uw {
namespace fileio {

namespace {

FILE* openWide(const std::string& utf8Path, const wchar_t* mode) {
    return _wfopen(text::toWide(utf8Path).c_str(), mode);
}

}  // namespace

bool writeText(const std::string& utf8Path, const std::string& utf8Content, std::string& error) {
    FILE* file = openWide(utf8Path, L"wb");
    if (file == NULL) {
        error = "无法写入文件：" + utf8Path;
        return false;
    }
    size_t written = 0;
    if (!utf8Content.empty()) {
        written = std::fwrite(utf8Content.data(), 1, utf8Content.size(), file);
    }
    const bool complete = (written == utf8Content.size());
    std::fclose(file);
    if (!complete) {
        error = "写入过程中出错：" + utf8Path;
        return false;
    }
    return true;
}

bool readLines(const std::string& utf8Path, std::vector<std::string>& lines, std::string& error) {
    FILE* file = openWide(utf8Path, L"rb");
    if (file == NULL) {
        error = "无法读取文件：" + utf8Path;
        return false;
    }
    std::string content;
    char buffer[4096];
    size_t read = 0;
    while ((read = std::fread(buffer, 1, sizeof(buffer), file)) > 0) {
        content.append(buffer, read);
    }
    std::fclose(file);

    if (content.size() >= 3 && static_cast<unsigned char>(content[0]) == 0xEF &&
        static_cast<unsigned char>(content[1]) == 0xBB &&
        static_cast<unsigned char>(content[2]) == 0xBF) {
        content.erase(0, 3);
    }

    lines.clear();
    size_t start = 0;
    while (start < content.size()) {
        const size_t newline = content.find('\n', start);
        std::string line = (newline == std::string::npos)
                               ? content.substr(start)
                               : content.substr(start, newline - start);
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }
        lines.push_back(line);
        if (newline == std::string::npos) {
            break;
        }
        start = newline + 1;
    }
    return true;
}

}  // namespace fileio
}  // namespace uw
