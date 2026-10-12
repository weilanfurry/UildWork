#include "timer.h"

#include <windows.h>

#include <chrono>

namespace uw {
namespace timer {

long long nowMs() {
    return static_cast<long long>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count());
}

void sleepMs(int milliseconds) {
    if (milliseconds <= 0) {
        return;
    }
    Sleep(static_cast<DWORD>(milliseconds));
}

long long secondsFloor(long long milliseconds) {
    return milliseconds > 0 ? milliseconds / 1000 : 0;
}

long long secondsCeil(long long milliseconds) {
    return milliseconds > 0 ? (milliseconds + 999) / 1000 : 0;
}

Timer::Timer() : durationMs_(-1), accumulatedMs_(0), startedAtMs_(nowMs()), paused_(false) {}

Timer::Timer(long long durationMs)
    : durationMs_(durationMs), accumulatedMs_(0), startedAtMs_(nowMs()), paused_(false) {}

void Timer::setDuration(long long durationMs) {
    durationMs_ = durationMs;
    reset();
}

long long Timer::durationMs() const {
    return durationMs_;
}

bool Timer::isCountdown() const {
    return durationMs_ >= 0;
}

void Timer::pause() {
    if (paused_) {
        return;
    }
    accumulatedMs_ += nowMs() - startedAtMs_;
    paused_ = true;
}

void Timer::resume() {
    if (!paused_) {
        return;
    }
    startedAtMs_ = nowMs();
    paused_ = false;
}

void Timer::togglePause() {
    if (paused_) {
        resume();
    } else {
        pause();
    }
}

void Timer::reset() {
    accumulatedMs_ = 0;
    startedAtMs_ = nowMs();
    paused_ = false;
}

void Timer::restart(long long durationMs) {
    durationMs_ = durationMs;
    reset();
}

bool Timer::paused() const {
    return paused_;
}

bool Timer::running() const {
    return !paused_;
}

long long Timer::elapsedMs() const {
    if (paused_) {
        return accumulatedMs_;
    }
    return accumulatedMs_ + (nowMs() - startedAtMs_);
}

long long Timer::remainingMs() const {
    if (!isCountdown()) {
        return elapsedMs();
    }
    const long long left = durationMs_ - elapsedMs();
    return left > 0 ? left : 0;
}

bool Timer::finished() const {
    return isCountdown() && remainingMs() <= 0;
}

double Timer::progress() const {
    if (!isCountdown() || durationMs_ <= 0) {
        return 0.0;
    }
    const double left = static_cast<double>(remainingMs());
    const double total = static_cast<double>(durationMs_);
    return left / total;
}

Interval::Interval(int periodMs) : periodMs_(periodMs > 0 ? periodMs : 100), lastMs_(nowMs()) {}

bool Interval::ready() {
    const long long current = nowMs();
    if (current - lastMs_ < periodMs_) {
        return false;
    }
    lastMs_ = current;
    return true;
}

void Interval::reset() {
    lastMs_ = nowMs();
}

}  // namespace timer
}  // namespace uw
