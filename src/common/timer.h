#pragma once
// UildWork 计时层：基于单调时钟的计时器与周期触发器。
//
// 原实现全部是 “Sleep(1000); seconds--;” 的写法，有两个问题：
//   1. Sleep 只保证“至少睡这么久”，误差会不断累积（跑一小时能差好几秒）；
//   2. 无法暂停，也不能在计时过程中响应按键（只能 Ctrl+C 结束进程）。
// 这里统一改成基于 std::chrono::steady_clock 的截止时间计算，误差不累积。

namespace uw {
namespace timer {

// 单调毫秒时钟（不受系统时间调整影响）
long long nowMs();
void sleepMs(int milliseconds);

// 毫秒转秒：倒计时显示用 ceil（25 分钟开始时显示 25:00 而不是 24:59）
long long secondsFloor(long long milliseconds);
long long secondsCeil(long long milliseconds);

class Timer {
public:
    Timer();                               // 正计时
    explicit Timer(long long durationMs);  // 倒计时

    void setDuration(long long durationMs);
    long long durationMs() const;
    bool isCountdown() const;

    void pause();
    void resume();
    void togglePause();
    void reset();                    // 归零（倒计时则回到初始时长）
    void restart(long long durationMs);

    bool paused() const;
    bool running() const;
    long long elapsedMs() const;
    long long remainingMs() const;   // 正计时无意义，返回已过时间
    bool finished() const;           // 倒计时是否归零
    double progress() const;         // [0, 1]，倒计时为剩余比例

private:
    long long durationMs_;
    long long accumulatedMs_;
    long long startedAtMs_;
    bool paused_;
};

// 周期触发：适合 100ms 刷新一次界面这种场景
class Interval {
public:
    explicit Interval(int periodMs);
    bool ready();
    void reset();

private:
    int periodMs_;
    long long lastMs_;
};

}  // namespace timer
}  // namespace uw
