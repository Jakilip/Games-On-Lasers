#include "Math.h"
#include "Core.h"

#include <cmath>
#include <random>

namespace GOL {

float distance(const Vec2F& a, const Vec2F& b) {
    return std::hypot(a.x - b.x, a.y - b.y);
}

float angleTo(const Vec2F& observer, const Vec2F& target) {
    float angle = std::atan2(target.y - observer.y,
                             target.x - observer.x)
                  * (180.0f / static_cast<float>(M_PI));

    return angle < 0.0f ? angle + 360.0f : angle;
}

std::vector<Line> makeCircle(Vec2F size, int linesToUse) {
    std::vector<Line> lines;
    lines.reserve(linesToUse);

    float degreesPerLine = 360.f / static_cast<float>(linesToUse);
    Vec2F center = size / 2.f;
    Vec2F radius = size / 2.f;

    auto pointAt = [&](int index) -> Vec2F {
        float rad = static_cast<float>(index) * degreesPerLine * (std::numbers::pi_v<float> / 180.f);
        return Vec2F{ center.x + std::cos(rad) * radius.x,
                      center.y + std::sin(rad) * radius.y };
    };

    for (int i = 0; i < linesToUse; ++i) {
        lines.emplace_back(pointAt(i), pointAt((i + 1) % linesToUse));
    }

    return lines;
}

std::vector<Line> makeRectangle(Vec2F size) {
    return {
        {{0.f, 0.f}, {size.x, 0.f}},
    {{size.x, 0.f}, {size.x, size.y}},
    {{size.x, size.y}, {0.f, size.y}},
    {{0.f, size.y}, {0.f, 0.f}}
    };
}

std::vector<Line> makeTriangle(Vec2F size) {
    return {
        {{0.f, 0.f}, {size.x, 0.f}},
    {{size.x, 0.f}, {size.x/2.f, size.y}},
    {{size.x/2.f, size.y}, {0.f, 0.f}}
    };
}

std::vector<Line> makeHexagon(Vec2F size) {
    return makeCircle(size, 6);
}

}