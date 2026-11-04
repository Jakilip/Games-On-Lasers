#include "Math.h"
#include "Core.h"

#include <cmath>
#include <random>

namespace GOL {

float distance(const Vec2F& a, const Vec2F& b) {
    return std::hypot(a.x - b.x, a.y - b.y);
}

float angleTo(const Vec2F& observer, const Vec2F& target) {
    return std::atan2(target.y - observer.y, target.x - observer.x)
           * (180.0f / static_cast<float>(M_PI));
}

}