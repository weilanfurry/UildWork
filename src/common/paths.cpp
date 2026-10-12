#include "paths.h"

#include "text.h"

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>

#include <vector>

// 部分 MinGW 头文件（较旧的 _WIN32_IE 配置）不定义这个常量，它的值固定为 0
#ifndef SHGFP_TYPE_CURRENT
#define SHGFP_TYPE_CURRENT 0
#endif

namespace uw {
namespace paths {

namespace {

std::string g_executableDirectory;
std::string g_executablePath;
bool g_resolved = false;

void resolveSelf() {
    if (g_resolved) {
        return;
    }
    g_resolved = true;
    std::vector<wchar_t> buffer(MAX_PATH);
    for (;;) {
        const DWORD length =
            GetModuleFileNameW(NULL, &buffer[0], static_cast<DWORD>(buffer.size()));
        if (length == 0) {
            g_executablePath = "UildWork.exe";
            g_executableDirectory = ".";
            return;
        }
        if (length < buffer.size() - 1) {
            buffer.resize(static_cast<size_t>(length) + 1);
            GetModuleFileNameW(NULL, &buffer[0], static_cast<DWORD>(buffer.size()));
            break;
        }
        buffer.resize(buffer.size() * 2);
    }
    g_executablePath = text::fromWide(&buffer[0]);
    const size_t slash = g_executablePath.find_last_of("\\/");
    g_executableDirectory =
        (slash == std::string::npos) ? std::string(".") : g_executablePath.substr(0, slash);
}

bool isRegularFile(const std::string& path) {
    const DWORD attributes = GetFileAttributesW(text::toWide(path).c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

std::string parentDirectory(const std::string& path) {
    const size_t slash = path.find_last_of("\\/");
    if (slash == std::string::npos) {
        return std::string();
    }
    return path.substr(0, slash);
}

}  // namespace

const std::string& executableDirectory() {
    resolveSelf();
    return g_executableDirectory;
}

std::string executablePath() {
    resolveSelf();
    return g_executablePath;
}

std::string executableName() {
    resolveSelf();
    const size_t slash = g_executablePath.find_last_of("\\/");
    if (slash == std::string::npos) {
        return g_executablePath;
    }
    return g_executablePath.substr(slash + 1);
}

std::string findResource(const std::string& fileName) {
    if (fileName.empty()) {
        return std::string();
    }
    // 已经是绝对路径就直接判断
    if (fileName.size() > 1 && fileName[1] == ':') {
        return isRegularFile(fileName) ? fileName : std::string();
    }

    std::vector<std::string> candidates;
    std::string directory = executableDirectory();
    for (int depth = 0; depth < 3 && !directory.empty(); ++depth) {
        candidates.push_back(directory + "\\assets\\" + fileName);
        candidates.push_back(directory + "\\" + fileName);
        directory = parentDirectory(directory);
    }
    for (size_t i = 0; i < candidates.size(); ++i) {
        if (isRegularFile(candidates[i])) {
            return candidates[i];
        }
    }
    return std::string();
}

bool fileExists(const std::string& path) {
    return isRegularFile(absoluteFrom(path));
}

std::string absoluteFrom(const std::string& path) {
    if (path.empty()) {
        return path;
    }
    if (path.size() > 1 && path[1] == ':') {
        return path;
    }
    if (!path.empty() && (path[0] == '\\' || path[0] == '/')) {
        return path;
    }
    return executableDirectory() + "\\" + path;
}

std::string documentsDirectory() {
    wchar_t buffer[MAX_PATH] = {0};
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, buffer))) {
        const std::string path = text::fromWide(buffer);
        if (!path.empty()) {
            return path;
        }
    }
    const DWORD length = GetEnvironmentVariableW(L"USERPROFILE", buffer, MAX_PATH);
    if (length > 0 && length < MAX_PATH) {
        const std::string profile = text::fromWide(buffer);
        if (!profile.empty()) {
            return profile + "\\Documents";
        }
    }
    return executableDirectory();
}

bool ensureDirectory(const std::string& path) {
    if (path.empty()) {
        return false;
    }
    const DWORD attributes = GetFileAttributesW(text::toWide(path).c_str());
    if (attributes != INVALID_FILE_ATTRIBUTES) {
        return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
    return CreateDirectoryW(text::toWide(path).c_str(), NULL) != FALSE;
}

bool launchDetached(const std::string& executablePathToRun, const std::string& arguments) {
    const std::string fullPath = absoluteFrom(executablePathToRun);
    if (!isRegularFile(fullPath)) {
        return false;
    }
    std::wstring commandLine = L"\"" + text::toWide(fullPath) + L"\"";
    if (!arguments.empty()) {
        commandLine += L" " + text::toWide(arguments);
    }
    std::vector<wchar_t> mutableCommand(commandLine.begin(), commandLine.end());
    mutableCommand.push_back(L'\0');

    STARTUPINFOW startup;
    ZeroMemory(&startup, sizeof(startup));
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process;
    ZeroMemory(&process, sizeof(process));

    const std::wstring workingDirectory = text::toWide(executableDirectory());
    const BOOL created =
        CreateProcessW(NULL, &mutableCommand[0], NULL, NULL, FALSE,
                       CREATE_NEW_CONSOLE, NULL, workingDirectory.c_str(), &startup, &process);
    if (!created) {
        return false;
    }
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    return true;
}

bool launchTool(const std::string& legacyExecutableName, const std::string& subCommand) {
    if (!legacyExecutableName.empty()) {
        const std::string sibling = executableDirectory() + "\\" + legacyExecutableName;
        if (isRegularFile(sibling) && launchDetached(sibling)) {
            return true;
        }
    }
    return launchDetached(executablePath(), subCommand);
}

std::vector<std::string> arguments() {
    std::vector<std::string> result;
    int count = 0;
    LPWSTR* wideArguments = CommandLineToArgvW(GetCommandLineW(), &count);
    if (wideArguments == NULL) {
        result.push_back(executablePath());
        return result;
    }
    for (int i = 0; i < count; ++i) {
        result.push_back(text::fromWide(wideArguments[i]));
    }
    LocalFree(wideArguments);
    return result;
}

}  // namespace paths
}  // namespace uw
