#pragma once
// UildWork 界面组件：横幅、菜单、进度条、大号数字时钟、配色主题。
//
// 这些组件原本散落在 8 个 cpp 里各写一遍（菜单样式、颜色表、进度显示各不相同），
// 现在集中在这里，所有工具的外观和行为保持一致。

#include <string>
#include <vector>

namespace uw {
namespace ui {

struct MenuItem {
    std::string label;
    std::string hint;  // 可选，显示在标签右侧的浅色说明
};

// 交互式菜单：↑/↓ 或 W/S 移动，数字键直接跳选，回车确认，Esc 取消。
// 返回被选中项的下标，取消返回 -1。
int selectMenu(const std::string& utf8Title, const std::vector<MenuItem>& items,
               int initialIndex = 0, const std::string& utf8Footer = std::string());

// 全屏横幅：用于主界面标题
void banner(const std::string& utf8Title, int width = 52);
void rule(const std::string& utf8Unit = "=", int width = 52);

// 进度条，fraction 取值 [0, 1]
std::string progressBar(double fraction, int width = 28);

// 大号数字时钟（5 行 5 列的方块数字），utf8Text 形如 "01:23:45"
void bigTime(const std::string& utf8Text, int foreground);

// 配色主题
struct Theme {
    std::string name;
    int foreground;
    int background;
};

std::vector<Theme> defaultThemes();
void applyTheme(const Theme& theme);

// 让用户选择配色（保持旧版本的“表格模板 / 背景颜色”功能）
Theme chooseTheme(const std::string& utf8Title);

}  // namespace ui
}  // namespace uw
