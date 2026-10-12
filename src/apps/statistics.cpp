// UildWork 图表统计（对应原 Statistics.cpp）
//
// 修复的问题：
//   1. 变长数组 string objects[objectCount] / float data[obj][cat] -> std::vector。
//   2. data[row-1][column-1] 完全没校验行列，输错一个数字就会写坏内存 -> 先校验。
//   3. long long total 累加 float 数据会丢小数；平均值的除数可能为 0 -> 改用 double 并加保护。
//   4. 导出写在当前工作目录（安装到 Program Files 后通常没有写权限，且用户找不到）
//      -> 改为写到“文档\UildWork”，并显示完整路径。
//   5. 名为“图表统计”却不画图 -> 增加按行/按列的条形图。

#include "apps.h"

#include "../common/appinfo.h"
#include "../common/console.h"
#include "../common/fileio.h"
#include "../common/input.h"
#include "../common/paths.h"
#include "../common/text.h"
#include "../common/ui.h"

#include <cstdio>
#include <string>
#include <vector>

namespace uw {

namespace {

const int kWidth = 52;
const int kBarWidth = 32;

typedef std::vector<std::vector<double> > Matrix;

// 去掉小数点后多余的 0，避免出现 12.000000 这种显示
std::string formatNumber(double value) {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%.2f", value);
    std::string text(buffer);
    if (text.find('.') != std::string::npos) {
        while (!text.empty() && text[text.size() - 1] == '0') {
            text.erase(text.size() - 1);
        }
        if (!text.empty() && text[text.size() - 1] == '.') {
            text.erase(text.size() - 1);
        }
    }
    return text.empty() ? "0" : text;
}

struct Sheet {
    std::vector<std::string> objects;
    std::vector<std::string> categories;
    Matrix data;
    ui::Theme theme;
};

void drawTable(const Sheet& sheet) {
    const size_t rows = sheet.objects.size();
    const size_t columns = sheet.categories.size();
    if (rows == 0 || columns == 0) {
        return;
    }
    int labelWidth = 8;
    for (size_t i = 0; i < rows; ++i) {
        const int width = text::displayWidth(sheet.objects[i]);
        if (width > labelWidth) {
            labelWidth = width;
        }
    }
    int cellWidth = 10;
    for (size_t j = 0; j < columns; ++j) {
        const int width = text::displayWidth(sheet.categories[j]) + 4;
        if (width > cellWidth) {
            cellWidth = width;
        }
    }

    console::write("  " + text::padLeft("对象", labelWidth) + " |");
    for (size_t j = 0; j < columns; ++j) {
        console::write(" " + text::padLeft(sheet.categories[j], cellWidth));
    }
    console::writeLine(std::string());
    console::writeLine("  " + text::repeat("-", labelWidth) + "-+" +
                       text::repeat("-", (cellWidth + 1) * columns));

    for (size_t i = 0; i < rows; ++i) {
        console::write("  " + text::padLeft(sheet.objects[i], labelWidth) + " |");
        for (size_t j = 0; j < columns; ++j) {
            console::write(" " + text::padLeft(formatNumber(sheet.data[i][j]), cellWidth));
        }
        console::writeLine(std::string());
    }
    console::writeLine(std::string());
}

void drawBar(double value, double scale) {
    int units = (scale <= 0.0) ? 0 : static_cast<int>(value / scale * kBarWidth + 0.5);
    if (units < 0) {
        units = 0;
    }
    if (units > kBarWidth) {
        units = kBarWidth;
    }
    console::write("  " + text::repeat("█", units) + text::repeat("·", kBarWidth - units));
}

void computeAndShow(const Sheet& sheet) {
    const std::vector<ui::MenuItem> modes = {
        {"按行统计（每个对象在所有项目上的合计）", ""},
        {"按列统计（每个项目在所有对象上的合计）", ""}};
    const int mode = ui::selectMenu("统计计算", modes, 0);
    if (mode < 0) {
        return;
    }

    std::vector<double> totals;
    std::vector<std::string> labels;
    if (mode == 0) {
        for (size_t i = 0; i < sheet.objects.size(); ++i) {
            double total = 0.0;
            for (size_t j = 0; j < sheet.categories.size(); ++j) {
                total += sheet.data[i][j];
            }
            totals.push_back(total);
            labels.push_back(sheet.objects[i]);
        }
    } else {
        for (size_t j = 0; j < sheet.categories.size(); ++j) {
            double total = 0.0;
            for (size_t i = 0; i < sheet.objects.size(); ++i) {
                total += sheet.data[i][j];
            }
            totals.push_back(total);
            labels.push_back(sheet.categories[j]);
        }
    }

    console::clear();
    ui::banner(mode == 0 ? "按行统计结果" : "按列统计结果", kWidth);
    if (totals.empty()) {
        console::writeLine("没有可统计的数据。");
        input::pause();
        return;
    }

    double grandTotal = 0.0;
    double maximum = totals[0];
    double minimum = totals[0];
    for (size_t i = 0; i < totals.size(); ++i) {
        grandTotal += totals[i];
        if (totals[i] > maximum) {
            maximum = totals[i];
        }
        if (totals[i] < minimum) {
            minimum = totals[i];
        }
    }
    const double average = grandTotal / static_cast<double>(totals.size());
    const double scale = (maximum > 0.0) ? maximum : 1.0;

    int labelWidth = 10;
    for (size_t i = 0; i < labels.size(); ++i) {
        const int width = text::displayWidth(labels[i]);
        if (width > labelWidth) {
            labelWidth = width;
        }
    }
    for (size_t i = 0; i < totals.size(); ++i) {
        console::setColor(console::BrightWhite);
        console::writeLine("  " + text::padLeft(labels[i], labelWidth) + "  合计 " +
                           text::padLeft(formatNumber(totals[i]), 12));
        console::setColor(console::BrightGreen);
        drawBar(totals[i], scale);
        console::writeLine(std::string());
        console::resetColor();
    }
    ui::rule("-", kWidth);
    console::setColor(console::BrightCyan);
    console::writeLine("  总计 " + formatNumber(grandTotal) + "    平均 " + formatNumber(average) +
                       "    最大 " + formatNumber(maximum) + "    最小 " + formatNumber(minimum));
    console::resetColor();
    console::writeLine(std::string());
    input::pause();
}

bool exportSheet(const Sheet& sheet, std::string& savedPath, std::string& error) {
    const app::LocalTime now = app::localNow();
    const std::string directory = paths::documentsDirectory() + "\\UildWork";
    if (!paths::ensureDirectory(directory)) {
        error = "无法创建目录：" + directory;
        return false;
    }
    const std::string path = directory + "\\Statistics_" + std::to_string(now.year) +
                             text::pad2(now.month) + text::pad2(now.day) + "_" +
                             text::pad2(now.hour) + text::pad2(now.minute) +
                             text::pad2(now.second) + ".txt";
    std::string content;
    content += "UildWork 图表统计导出\n";
    content += "导出时间: " + app::formatDateTime(now) + "\n";
    content += "配色模板: " + sheet.theme.name + "\n\n";

    for (size_t i = 0; i < sheet.objects.size(); ++i) {
        content += sheet.objects[i];
        double rowTotal = 0.0;
        for (size_t j = 0; j < sheet.categories.size(); ++j) {
            content += "\t" + sheet.categories[j] + ": " + formatNumber(sheet.data[i][j]);
            rowTotal += sheet.data[i][j];
        }
        content += "\t合计: " + formatNumber(rowTotal) + "\n";
    }

    content += "\n按列合计\n";
    for (size_t j = 0; j < sheet.categories.size(); ++j) {
        double columnTotal = 0.0;
        for (size_t i = 0; i < sheet.objects.size(); ++i) {
            columnTotal += sheet.data[i][j];
        }
        content += sheet.categories[j] + "\t" + formatNumber(columnTotal) + "\n";
    }
    if (!fileio::writeText(path, content, error)) {
        return false;
    }
    savedPath = path;
    return true;
}

}  // namespace

int runStatistics() {
    console::init("UildWork 图表统计");
    const ui::Theme theme = ui::chooseTheme("图表统计 - 选择表格模板");
    ui::applyTheme(theme);

    Sheet sheet;
    sheet.theme = theme;

    console::writeLine("欢迎来到 UildWork 图表统计！");
    console::writeLine(std::string());

    const int objectCount = input::readInt("请输入要统计的物体数量（1-50）：", 1, 50);
    for (int i = 0; i < objectCount; ++i) {
        sheet.objects.push_back(
            input::readLine("请输入第 " + std::to_string(i + 1) + " 个物体名称: "));
    }

    const int categoryCount = input::readInt("请输入统计的项目数量（1-50）：", 1, 50);
    for (int i = 0; i < categoryCount; ++i) {
        sheet.categories.push_back(
            input::readLine("请输入第 " + std::to_string(i + 1) + " 个项目名称: "));
    }

    sheet.data.assign(static_cast<size_t>(objectCount),
                      std::vector<double>(static_cast<size_t>(categoryCount), 0.0));

    const std::vector<ui::MenuItem> menu = {
        {"编辑数据", "按行/列修改单个数值"},
        {"清空数据", "所有数值归零"},
        {"统计计算", "合计 / 平均 / 最大 / 最小 + 条形图"},
        {"导出结果", "保存到 文档\\UildWork"},
        {"退出", ""}};

    for (;;) {
        console::clear();
        ui::banner("UildWork 图表统计", kWidth);
        drawTable(sheet);
        const int choice = ui::selectMenu("目录", menu, 0);
        if (choice < 0 || choice == 4) {
            return 0;
        }
        if (choice == 0) {
            console::clear();
            ui::banner("编辑数据", kWidth);
            const int row = input::readInt("请输入要编辑的行号（1-" +
                                               std::to_string(objectCount) + "）：",
                                           1, objectCount) - 1;
            const int column = input::readInt("请输入要编辑的列号（1-" +
                                                  std::to_string(categoryCount) + "）：",
                                              1, categoryCount) - 1;
            console::writeLine(std::string("  当前 ") + sheet.objects[row] + " 的 " +
                               sheet.categories[column] + " 值为 " +
                               formatNumber(sheet.data[row][column]));
            sheet.data[row][column] = input::readDouble("请输入新的数值：", -1.0e12, 1.0e12);
            continue;
        }
        if (choice == 1) {
            for (size_t i = 0; i < sheet.data.size(); ++i) {
                for (size_t j = 0; j < sheet.data[i].size(); ++j) {
                    sheet.data[i][j] = 0.0;
                }
            }
            continue;
        }
        if (choice == 2) {
            computeAndShow(sheet);
            continue;
        }
        if (choice == 3) {
            console::clear();
            ui::banner("导出结果", kWidth);
            std::string path;
            std::string error;
            if (exportSheet(sheet, path, error)) {
                console::ScopedColor ok(console::BrightGreen);
                console::writeLine("导出完成：" + path);
            } else {
                console::ScopedColor fail(console::BrightRed);
                console::writeLine("导出失败：" + error);
            }
            console::writeLine(std::string());
            input::pause();
            continue;
        }
    }
}

}  // namespace uw
