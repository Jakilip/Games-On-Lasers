#pragma once

#include "Core.h"

namespace GOL {

/// Checks if two points are equal. The epsilon constant is used for float based values.
template<typename T>
bool pointsAreEqual(const Vec2<T>& a, const Vec2<T>& b) {
    return static_cast<double>(std::abs(a.x - b.x)) < Epsilon && static_cast<double>(std::abs(a.y - b.y)) < Epsilon;
}

/// @returns distance from point a to point b
float distance(const Vec2F& a, const Vec2F& b);

/// @returns angle from observer to target IN DEGREES!!
float angleTo(const Vec2F& observer, const Vec2F& target);

}