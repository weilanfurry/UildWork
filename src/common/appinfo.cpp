#include "appinfo.h"

#include "console.h"
#include "text.h"

#include <windows.h>

namespace uw {
namespace app {

const char* const kName = "UildWork";
const char* const kVersion = "ZERO v4.0";
const char* const kHomepage = "https://weilanfurry.github.io/";
const char* const kAuthors = "UildWork & 蔚澜Waverly";

LocalTime localNow() {
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);
    LocalTime result;
    result.year = systemTime.wYear;
    result.month = systemTime.wMonth;
    result.day = systemTime.wDay;
    result.hour = systemTime.wHour;
    result.minute = systemTime.wMinute;
    result.second = systemTime.wSecond;
    result.weekday = systemTime.wDayOfWeek;
    return result;
}

std::string weekdayName(int weekday) {
    switch (weekday) {
        case 0: return "星期日";
        case 1: return "星期一";
        case 2: return "星期二";
        case 3: return "星期三";
        case 4: return "星期四";
        case 5: return "星期五";
        case 6: return "星期六";
        default: return "";
    }
}

std::string formatDate(const LocalTime& time) {
    return std::to_string(time.year) + "/" + text::pad2(time.month) + "/" + text::pad2(time.day);
}

std::string formatClock(const LocalTime& time) {
    return text::pad2(time.hour) + ":" + text::pad2(time.minute) + ":" + text::pad2(time.second);
}

std::string formatDateTime(const LocalTime& time) {
    return formatDate(time) + " " + formatClock(time);
}

std::string greetingForHour(int hour) {
    if (hour >= 18) {
        return "晚上好！";
    }
    if (hour >= 12) {
        return "下午好！";
    }
    if (hour >= 6) {
        return "上午好！";
    }
    return "凌晨好！";
}

void showGreetingDialog() {
    const LocalTime now = localNow();
    std::string message;
    if (now.hour >= 18) {
        message = "晚上好，今天也要早睡哦，祝你身体健康！";
    } else if (now.hour >= 12) {
        message = "下午好，快享受美妙的阳光吧，祝你开开心心！";
    } else if (now.hour >= 6) {
        message = "上午好，今天又会有什么样的好运呢？祝你一帆风顺！";
    } else {
        message = "凌晨好，美好的一天又开始了，祝你事业有成！";
    }
    console::infoDialog(kName, message);
}

}  // namespace app
}  // namespace uw
