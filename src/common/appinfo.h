#pragma once
// UildWork 应用级公共信息：名称、版本、问候语、本地时间格式化。
//
// 原来这些内容在 main.cpp、Stopwatch.cpp、各个计时器里各写了一份
// （问候语判断出现了三次），现在集中维护，改一处即可全局生效。

#include <string>

namespace uw {
namespace app {

extern const char* const kName;      // UildWork
extern const char* const kVersion;   // ZERO v4.0
extern const char* const kHomepage;  // https://weilanfurry.github.io/
extern const char* const kAuthors;   // UildWork & 蔚澜Waverly

struct LocalTime {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int weekday;  // 0 = 星期日
};

LocalTime localNow();

std::string weekdayName(int weekday);
std::string formatDate(const LocalTime& time);      // 2026/10/12
std::string formatClock(const LocalTime& time);     // 14:23:45
std::string formatDateTime(const LocalTime& time);  // 2026/10/12 14:23:45

// 按小时返回问候语：凌晨好 / 上午好 / 下午好 / 晚上好
std::string greetingForHour(int hour);

// 启动时弹出问候对话框（智能秒表原来的行为）
void showGreetingDialog();

}  // namespace app
}  // namespace uw
