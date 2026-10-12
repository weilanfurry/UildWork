// UildWork 任务清单（对应原 Manifest.cpp）
//
// 修复的问题：
//   1. 原代码用变长数组 string items[itemCount]（非标准 C++，数量大时爆栈，
//      输入 0 或负数直接崩溃）-> 改用 std::vector。
//   2. cin >> item 遇到空格会截断任务名 -> 改用整行读取。
//   3. completionStatus[itemIndex-1] 没有任何范围校验 -> 越界写入随意内存 -> 先校验。
//   4. 清单只存在内存里，关掉就没了；而安装包却注册了 .udf 文件关联
//      （"{app}\UildWork.exe" "%1"），旧版完全没处理这个参数。现在支持读写 .udf 清单。

#include "apps.h"

#include "../common/appinfo.h"
#include "../common/console.h"
#include "../common/fileio.h"
#include "../common/input.h"
#include "../common/paths.h"
#include "../common/text.h"
#include "../common/ui.h"

#include <string>
#include <vector>

namespace uw {

namespace {

const int kWidth = 52;
const char* const kFileHeader = "UILDWORK-MANIFEST 1";

struct Task {
    std::string title;
    bool done;

    Task() : done(false) {}
};

struct TaskList {
    std::string name;
    std::vector<Task> tasks;
};

std::string safeFileName(const std::string& rawName) {
    std::string result;
    for (size_t i = 0; i < rawName.size(); ++i) {
        const char ch = rawName[i];
        const bool illegal = (ch == '\\' || ch == '/' || ch == ':' || ch == '*' || ch == '?' ||
                              ch == '"' || ch == '<' || ch == '>' || ch == '|');
        result += illegal ? '_' : ch;
    }
    if (result.empty()) {
        result = "任务清单";
    }
    return result;
}

std::string defaultPathFor(const TaskList& list) {
    const std::string directory = paths::documentsDirectory() + "\\UildWork";
    paths::ensureDirectory(directory);
    return directory + "\\" + safeFileName(list.name) + ".udf";
}

bool saveTaskList(const std::string& path, const TaskList& list, std::string& error) {
    std::string content;
    content += std::string(kFileHeader) + "\n";
    content += "@name " + list.name + "\n";
    for (size_t i = 0; i < list.tasks.size(); ++i) {
        content += list.tasks[i].done ? "[x] " : "[ ] ";
        content += list.tasks[i].title + "\n";
    }
    return fileio::writeText(path, content, error);
}

bool loadTaskList(const std::string& path, TaskList& list, std::string& error) {
    std::vector<std::string> lines;
    if (!fileio::readLines(path, lines, error)) {
        return false;
    }
    if (lines.empty() ||
        lines[0].compare(0, std::string(kFileHeader).size(), kFileHeader) != 0) {
        error = "文件格式不是 UildWork 清单：\n" + path;
        return false;
    }

    TaskList loaded;
    for (size_t index = 1; index < lines.size(); ++index) {
        const std::string& line = lines[index];
        if (line.empty()) {
            continue;
        }
        if (line.compare(0, 6, "@name ") == 0) {
            loaded.name = text::trim(line.substr(6));
            continue;
        }
        if (line.compare(0, 4, "[x] ") == 0) {
            Task task;
            task.done = true;
            task.title = text::trim(line.substr(4));
            if (!task.title.empty()) {
                loaded.tasks.push_back(task);
            }
            continue;
        }
        if (line.compare(0, 4, "[ ] ") == 0) {
            Task task;
            task.title = text::trim(line.substr(4));
            if (!task.title.empty()) {
                loaded.tasks.push_back(task);
            }
            continue;
        }
        // 兼容手工编辑过的纯文本清单
        Task task;
        task.title = text::trim(line);
        if (!task.title.empty()) {
            loaded.tasks.push_back(task);
        }
    }
    if (loaded.name.empty()) {
        loaded.name = "我的任务清单";
    }
    list = loaded;
    return true;
}

int doneCount(const TaskList& list) {
    int count = 0;
    for (size_t i = 0; i < list.tasks.size(); ++i) {
        if (list.tasks[i].done) {
            ++count;
        }
    }
    return count;
}

void drawList(const TaskList& list, int selected, const std::string& status, bool dirty) {
    console::clear();
    ui::banner(list.name.empty() ? "任务清单" : list.name, kWidth);

    if (list.tasks.empty()) {
        console::ScopedColor empty(console::BrightYellow);
        console::writeLine("  清单还是空的，按 A 添加第一项任务。");
    }
    for (size_t i = 0; i < list.tasks.size(); ++i) {
        const bool active = (static_cast<int>(i) == selected);
        if (active) {
            console::setColor(console::BrightWhite, console::Blue);
            console::write("  ▸ ");
        } else {
            console::setColor(console::White, console::Black);
            console::write("    ");
        }
        console::write(text::pad2(static_cast<int>(i) + 1) + " ");
        console::write(list.tasks[i].done ? "[√] " : "[ ] ");
        console::writeLine(list.tasks[i].title);
        console::resetColor();
    }

    console::writeLine(std::string());
    console::setColor(console::BrightCyan);
    console::writeLine("  进度: " + std::to_string(doneCount(list)) + " / " +
                       std::to_string(list.tasks.size()) + (dirty ? "   (未保存)" : ""));
    console::resetColor();
    ui::rule("-", kWidth);

    if (!status.empty()) {
        console::ScopedColor note(console::BrightYellow);
        console::writeLine(status);
    }
    console::ScopedColor hint(console::Gray);
    console::writeLine("↑/↓ 选择 · 空格 完成/取消 · A 新增 · E 编辑 · D 删除");
    console::writeLine("S 保存 · L 另存为 · O 打开 · Enter 保存并退出 · Esc 放弃退出");
    console::resetColor();
}

void addTask(TaskList& list, int& selected) {
    console::clear();
    ui::banner("新增任务", kWidth);
    const std::string title = input::readLine("请输入任务内容：");
    if (title.empty()) {
        return;
    }
    Task task;
    task.title = title;
    list.tasks.push_back(task);
    selected = static_cast<int>(list.tasks.size()) - 1;
}

void editTask(TaskList& list, int selected) {
    if (selected < 0 || selected >= static_cast<int>(list.tasks.size())) {
        return;
    }
    console::clear();
    ui::banner("编辑任务", kWidth);
    console::ScopedColor hint(console::Gray);
    console::writeLine("当前内容： " + list.tasks[static_cast<size_t>(selected)].title);
    console::resetColor();
    const std::string title = input::readLine("请输入新的内容（直接回车保持原样）：", true);
    if (!title.empty()) {
        list.tasks[static_cast<size_t>(selected)].title = title;
    }
}

void deleteTask(TaskList& list, int& selected) {
    if (selected < 0 || selected >= static_cast<int>(list.tasks.size())) {
        return;
    }
    console::clear();
    ui::banner("删除任务", kWidth);
    console::setColor(console::BrightYellow);
    console::writeLine("确定删除 “" + list.tasks[static_cast<size_t>(selected)].title + "” 吗? (Y/N)");
    const char answer = input::readChar("yn", true);
    if (answer != 'y' && answer != 'Y') {
        return;
    }
    list.tasks.erase(list.tasks.begin() + selected);
    if (selected >= static_cast<int>(list.tasks.size())) {
        selected = static_cast<int>(list.tasks.size()) - 1;
    }
}

}  // namespace

int runManifest(const std::string& fileToOpen) {
    console::init("UildWork 任务清单");

    TaskList list;
    std::string currentPath = fileToOpen;
    std::string status;
    bool dirty = false;

    if (!currentPath.empty()) {
        std::string error;
        if (loadTaskList(currentPath, list, error)) {
            status = "已打开：" + currentPath;
        } else {
            console::clear();
            ui::banner("任务清单", kWidth);
            console::ScopedColor warning(console::BrightRed);
            console::writeLine(error);
            input::pause("按任意键返回...");
            return 1;
        }
    } else {
        const ui::Theme theme = ui::chooseTheme("任务清单 - 选择配色");
        ui::applyTheme(theme);
        list.name = input::readLine("给你的清单起个名字：");
        const int count = input::readInt("您有多少个事项？", 0, 200);
        for (int i = 0; i < count; ++i) {
            Task task;
            task.title = input::readLine("第 " + std::to_string(i + 1) + " 项: ");
            list.tasks.push_back(task);
        }
        dirty = !list.tasks.empty();
    }

    int selected = list.tasks.empty() ? -1 : 0;
    for (;;) {
        drawList(list, selected, status, dirty);
        status.clear();

        const input::KeyEvent event = input::readKey();
        if (event.key == input::Key::Escape) {
            if (dirty) {
                console::clear();
                ui::banner("任务清单", kWidth);
                console::setColor(console::BrightYellow);
                console::writeLine("还有未保存的修改，确定放弃吗? (Y/N)");
                const char answer = input::readChar("yn", true);
                if (input::atEndOfInput()) {
                    return 0;
                }
                if (answer != 'y' && answer != 'Y') {
                    continue;
                }
            }
            return 0;
        }
        if (event.key == input::Key::Enter) {
            if (list.tasks.empty() && list.name.empty()) {
                return 0;
            }
            break;
        }
        if (event.key == input::Key::Up) {
            if (!list.tasks.empty()) {
                selected = (selected <= 0) ? static_cast<int>(list.tasks.size()) - 1 : selected - 1;
            }
            continue;
        }
        if (event.key == input::Key::Down) {
            if (!list.tasks.empty()) {
                selected = (selected + 1) % static_cast<int>(list.tasks.size());
            }
            continue;
        }
        if (event.key == input::Key::Space) {
            if (selected >= 0 && selected < static_cast<int>(list.tasks.size())) {
                Task& task = list.tasks[static_cast<size_t>(selected)];
                task.done = !task.done;
                dirty = true;
            }
            continue;
        }
        if (event.key == input::Key::Delete) {
            deleteTask(list, selected);
            if (list.tasks.empty()) {
                selected = -1;
            }
            dirty = true;
            continue;
        }
        if (event.key != input::Key::Character) {
            continue;
        }
        const char ch = event.character;
        if (ch == 'a' || ch == 'A') {
            addTask(list, selected);
            dirty = true;
        } else if (ch == 'e' || ch == 'E') {
            editTask(list, selected);
            dirty = true;
        } else if (ch == 'd' || ch == 'D') {
            deleteTask(list, selected);
            dirty = true;
        } else if (ch == 's' || ch == 'S') {
            if (currentPath.empty()) {
                currentPath = defaultPathFor(list);
            }
            std::string error;
            if (saveTaskList(currentPath, list, error)) {
                dirty = false;
                status = "已保存到：" + currentPath;
            } else {
                status = "保存失败：" + error;
            }
        } else if (ch == 'l' || ch == 'L') {
            console::clear();
            ui::banner("另存为", kWidth);
            const std::string path = input::readLine("请输入保存路径（回车使用默认位置）：", true);
            currentPath = path.empty() ? defaultPathFor(list) : paths::absoluteFrom(path);
            std::string error;
            if (saveTaskList(currentPath, list, error)) {
                dirty = false;
                status = "已保存到：" + currentPath;
            } else {
                status = "保存失败：" + error;
            }
        } else if (ch == 'o' || ch == 'O') {
            console::clear();
            ui::banner("打开清单", kWidth);
            const std::string path = input::readLine("请输入 .udf 清单路径（回车取消）：", true);
            if (!path.empty()) {
                TaskList loaded;
                std::string error;
                if (loadTaskList(paths::absoluteFrom(path), loaded, error)) {
                    list = loaded;
                    currentPath = paths::absoluteFrom(path);
                    selected = list.tasks.empty() ? -1 : 0;
                    dirty = false;
                    status = "已打开：" + currentPath;
                } else {
                    status = "打开失败：" + error;
                }
            }
        }
    }

    if (currentPath.empty()) {
        currentPath = defaultPathFor(list);
    }
    console::clear();
    ui::banner("任务清单", kWidth);
    std::string error;
    if (saveTaskList(currentPath, list, error)) {
        console::ScopedColor ok(console::BrightGreen);
        console::writeLine("已保存到：" + currentPath);
    } else {
        console::ScopedColor fail(console::BrightRed);
        console::writeLine("保存失败：" + error);
        input::pause("按任意键退出...");
    }
    return 0;
}

}  // namespace uw
