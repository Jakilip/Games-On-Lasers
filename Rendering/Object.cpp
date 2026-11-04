#include "Object.h"

#include "../Utilities/Collisions.h"

#include <algorithm>
#include <cmath>

namespace GOL {

//save and load are implemented in ILDAFormat.cpp

Vec2F Object::getPosition() { return position; }

float Object::getRotation() { return rotation; }

Vec2F Object::getScale() { return scale; }

Vec2F Object::getOrigin() { return originPointNormalized; }

Graphic Object::getTransformedGraphic() { applyTransformation(); return transformGraphic; }

Graphic& Object::getSourceGraphic() { return sourceGraphic; }

void Object::setPosition(Vec2F newPosition) { this->position = newPosition; }

void Object::setScale(Vec2F newScale) { this->scale = newScale; }

void Object::setRotation(float newRotation) { this->rotation = newRotation; }

void Object::setOrigin(Vec2F newOrigin) { this->originPointNormalized = newOrigin; }

void Object::setSourceGraphic(const Graphic& sourceGraphic, bool resetTransform) {
    this->sourceGraphic = sourceGraphic;
    if(resetTransform) {
        this->position = {0.f, 0.f};
        this->scale = {1.f, 1.f};
        this->rotation = 0.f;
        this->originPointNormalized = {0.5f, 0.5f};
    }
}

bool Object::collidesWith(Object &other) {
    this->applyTransformation();
    const auto& otherLines = other.getTransformedGraphic().lines;

    for(auto& thisLine : this->transformGraphic.lines) {
        for(auto& otherLine : otherLines) {
            if(linesCollide(thisLine, otherLine))
                return true;
        }
    }
    return false;
}

bool Object::collidesWith(const Line& other) {
    this->applyTransformation();

    for(auto& line : this->transformGraphic.lines) {
        if(linesCollide(other, line))
            return true;
    }
    return false;
}

Object::Object(const Graphic& sourceGraphic) {
    this->sourceGraphic = sourceGraphic;
    this->transformGraphic = sourceGraphic;
}

void Object::applyTransformation() {
    this->transformGraphic = this->sourceGraphic;

    auto& lines = transformGraphic.lines;
    if(lines.empty())
        return;

    const Vec2F origin = this->sourceGraphic.getSize() * this->originPointNormalized;

    const float angle = rotation * (std::numbers::pi_v<float> / 180.0f);
    const float cosA = std::cos(angle);
    const float sinA = std::sin(angle);

    auto transformPoint = [&](Vec2F& p) {
        // make relative to origin
        float dx = p.x - origin.x;
        float dy = p.y - origin.y;

        // scale (around origin)
        dx *= scale.x;   // use `scale` directly if it's a float
        dy *= scale.y;

        // rotate (around origin)
        const float rx = dx * cosA - dy * sinA;
        const float ry = dx * sinA + dy * cosA;

        // origin lands on position
        p.x = position.x + rx;
        p.y = position.y + ry;
    };

    for(auto& line : lines) {
        transformPoint(line.start);
        transformPoint(line.end);
    }
}

}