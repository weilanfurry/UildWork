#include "ui.h"

#include "console.h"
#include "input.h"
#include "text.h"

#include <vector>

namespace uw {
namespace ui {

namespace {

const char* const kBlockGlyphs[10][5] = {
    {"█████", "█   █", "█   █", "█   █", "█████"},  // 0
    {"   ██", "    █", "    █", "    █", "    █"},  // 1
    {"█████", "    █", "█████", "█    ", "█████"},  // 2
    {"█████", "    █", "█████", "    █", "█████"},  // 3
    {"█   █", "█   █", "█████", "    █", "    █"},  // 4
    {"█████", "█    ", "█████", "    █", "█████"},  // 5
    {"█████", "█    ", "█████", "█   █", "█████"},  // 6
    {"█████", "    █", "    █", "    █", "    █"},  // 7
    {"█████", "█   █", "█████", "█   █", "█████"},  // 8
    {"█████", "█   █", "█████", "    █", "█████"}   // 9
};

const char* const kColonGlyph[5] = {"     ", "  █  ", "     ", "  █  ", "     "};

int digitIndex(char ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    return -1;
}

}  // namespace

void rule(const std::string& utf8Unit, int width) {
    console::writeLine(text::repeat(utf8Unit, width));
}

void banner(const std::string& utf8Title, int width) {
    console::ScopedColor frame(console::BrightCyan, console::Black);
    console::writeLine(text::repeat("=", width));
    console::setColor(console::BrightWhite, console::Black);
    console::writeLine(text::center(utf8Title, width));
    console::ScopedColor bottom(console::BrightCyan, console::Black);
    console::writeLine(text::repeat("=", width));
}

std::string progressBar(double fraction, int width) {
    if (width < 4) {
        width = 4;
    }
    if (fraction < 0.0) {
        fraction = 0.0;
    }
    if (fraction > 1.0) {
        fraction = 1.0;
    }
    const int filled = static_cast<int>(fraction * (width - 2) + 0.5);
    std::string bar = "[";
    bar += text::repeat("█", filled);
    bar += text::repeat(" ", width - 2 - filled);
    bar += "]";
    const int percent = static_cast<int>(fraction * 100.0 + 0.5);
    bar += " " + text::pad2(percent / 10) + std::to_string(percent % 10) + "%";
    return bar;
}

void bigTime(const std::string& utf8Text, int foreground) {
    std::vector<std::string> rows(5);
    for (size_t index = 0; index < utf8Text.size(); ++index) {
        const char ch = utf8Text[index];
        const int digit = digitIndex(ch);
        for (int row = 0; row < 5; ++row) {
            std::string glyph;
            if (digit >= 0) {
                glyph = kBlockGlyphs[digit][row];
            } else if (ch == ':') {
                glyph = kColonGlyph[row];
            } else {
                continue;
            }
            if (!rows[row].empty()) {
                rows[row] += " ";
            }
            rows[row] += glyph;
        }
    }
    console::ScopedColor color(foreground);
    for (int row = 0; row < 5; ++row) {
        console::writeLine(text::center(rows[row], 52));
    }
}

std::vector<Theme> defaultThemes() {
    std::vector<Theme> themes;

    Theme blank;
    blank.name = "空白文档（默认黑白）";
    blank.foreground = console::White;
    blank.background = console::Black;
    themes.push_back(blank);

    Theme classic;
    classic.name = "蓝底白字";
    classic.foreground = console::BrightWhite;
    classic.background = console::Blue;
    themes.push_back(classic);

    Theme matrix;
    matrix.name = "黑底绿字";
    matrix.foreground = console::BrightGreen;
    matrix.background = console::Black;
    themes.push_back(matrix);

    Theme bright;
    bright.name = "绚丽背景（黑底亮黄字）";
    bright.foreground = console::BrightYellow;
    bright.background = console::Black;
    themes.push_back(bright);

    Theme colorful;
    colorful.name = "多彩字体（黑底亮青字）";
    colorful.foreground = console::BrightCyan;
    colorful.background = console::Black;
    themes.push_back(colorful);

    return themes;
}

void applyTheme(const Theme& theme) {
    console::setColor(theme.foreground, theme.background);
    console::clear();
}

Theme chooseTheme(const std::string& utf8Title) {
    const std::vector<Theme> themes = defaultThemes();
    std::vector<MenuItem> items;
    for (size_t i = 0; i < themes.size(); ++i) {
        MenuItem item;
        item.label = themes[i].name;
        items.push_back(item);
    }
    console::clear();
    const int index = selectMenu(utf8Title + "（回车使用默认配色）", items, 0);
    if (index < 0) {
        return themes[0];
    }
    return themes[static_cast<size_t>(index)];
}

int selectMenu(const std::string& utf8Title, const std::vector<MenuItem>& items, int initialIndex,
               const std::string& utf8Footer) {
    if (items.empty()) {
        return -1;
    }
    int selected = initialIndex;
    if (selected < 0 || selected >= static_cast<int>(items.size())) {
        selected = 0;
    }
    console::setCursorVisible(false);
    int result = -1;
    for (;;) {
        console::clear();
        banner(utf8Title);
        console::writeLine(std::string());
        for (size_t i = 0; i < items.size(); ++i) {
            const bool active = (static_cast<int>(i) == selected);
            if (active) {
                console::setColor(console::BrightWhite, console::Blue);
                console::write("  ▸ ");
            } else {
                console::setColor(console::White, console::Black);
                console::write("    ");
            }
            std::string line = "【" + std::to_string(i + 1) + "】 " + items[i].label;
            if (!items[i].hint.empty()) {
                line += "   (" + items[i].hint + ")";
            }
            console::writeLine(line);
            console::resetColor();
        }
        console::writeLine(std::string());
        if (!utf8Footer.empty()) {
            console::ScopedColor hint(console::Gray);
            console::writeLine(utf8Footer);
        }
        console::ScopedColor hint(console::Gray);
        console::writeLine("数字键直接选择 · ↑/↓ 移动 · Enter 确认 · Esc 返回");
        console::resetColor();

        const input::KeyEvent event = input::readKey();
        if (event.key == input::Key::Escape) {
            result = -1;
            break;
        }
        if (event.key == input::Key::Up) {
            selected = (selected + static_cast<int>(items.size()) - 1) % static_cast<int>(items.size());
            continue;
        }
        if (event.key == input::Key::Down) {
            selected = (selected + 1) % static_cast<int>(items.size());
            continue;
        }
        if (event.key == input::Key::Home) {
            selected = 0;
            continue;
        }
        if (event.key == input::Key::End) {
            selected = static_cast<int>(items.size()) - 1;
            continue;
        }
        if (event.key == input::Key::Enter || event.key == input::Key::Space) {
            result = selected;
            break;
        }
        if (event.key == input::Key::Character) {
            const int digit = digitIndex(event.character);
            if (digit > 0 && digit <= static_cast<int>(items.size())) {
                // 数字键直接选中并确认，保持与旧版“单键选择”一致的操作习惯
                selected = digit - 1;
                result = selected;
                break;
            }
            if (event.character == 'w' || event.character == 'W' || event.character == 'k') {
                selected = (selected + static_cast<int>(items.size()) - 1) % static_cast<int>(items.size());
                continue;
            }
            if (event.character == 's' || event.character == 'S' || event.character == 'j') {
                selected = (selected + 1) % static_cast<int>(items.size());
                continue;
            }
        }
    }
    console::setCursorVisible(true);
    console::resetColor();
    return result;
}

}  // namespace ui
}  // namespace uw
