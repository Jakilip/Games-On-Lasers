#include "Timer.h"

#include <chrono>

namespace GOL {

void Timer::restart() { this->startPoint = std::chrono::steady_clock::now(); }
Timer::Timer() { this->restart(); }
float Timer::getTimeAsMicroseconds() const { return std::chrono::duration<float, std::micro>(std::chrono::steady_clock::now() - startPoint).count(); }
float Timer::getTimeAsMilliseconds() const { return std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - startPoint).count(); }
float Timer::getTimeAsSeconds() const { return std::chrono::duration<float>(std::chrono::steady_clock::now() - startPoint).count(); }

}