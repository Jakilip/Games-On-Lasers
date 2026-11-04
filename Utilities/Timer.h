#pragma once

#include <chrono>

namespace GOL {

/// A simple class for measuring real time. Timer ALWAYS runs after being constructed.
class Timer {
private:
    std::chrono::time_point<std::chrono::steady_clock> startPoint;
public:
    void restart();

    /// @returns The amount of time since the timer was constructed or the last restart() call.
    float getTimeAsSeconds() const;

    /// @returns The amount of time since the timer was constructed or the last restart() call.
    float getTimeAsMilliseconds() const;

    /// @returns The amount of time since the timer was constructed or the last restart() call.
    float getTimeAsMicroseconds() const;

    Timer();
};

}
