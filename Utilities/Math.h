#pragma once

#include "Core.h"
#include <vector>

namespace GOL {

/// Checks if two points are equal. The epsilon constant is used for floating point values.
template<typename T>
bool pointsAreEqual(const Vec2<T>& a, const Vec2<T>& b) {
    return static_cast<double>(std::abs(a.x - b.x)) < Epsilon && static_cast<double>(std::abs(a.y - b.y)) < Epsilon;
}

/// @returns distance from point a to point b
float distance(const Vec2F& a, const Vec2F& b);

/// @returns angle from observer to target IN DEGREES!! Range [0-360]
float angleTo(const Vec2F& observer, const Vec2F& target);

/// @returns a collection of lines creating a circle
std::vector<Line> makeCircle(Vec2F size, int linesToUse = 64);

std::vector<Line> makeRectangle(Vec2F size);

std::vector<Line> makeTriangle(Vec2F size);

std::vector<Line> makeHexagon(Vec2F size);

}