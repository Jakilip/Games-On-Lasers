#pragma once

#include "../Utilities/Core.h"
#include "Graphic.h"

namespace GOL {

/// The core class for utilizing the GOL rendering pipeline, building on top of Graphic.
///
/// Object stores a copy of a graphic, and offers transform functionality which alters the resulting picture in very useful ways.
///
/// There is also collision functionality which is perfectly accurate due to the graphics being vector-based. If you still prefer classic collision methods such as AABB, you'll have to implement them yourself.
class Object {
private:
    friend class Projector;
    Graphic sourceGraphic{};
    Graphic transformGraphic{};

    Vec2F position = {0.f, 0.f};
    Vec2F scale = {1.f, 1.f};
    float rotation = 0.f;

    Vec2F originPointNormalized = {0.5f, 0.5f};
    void applyTransformation();
public:
    /// Sets the Object's rotation in degrees. 0 degrees point directly to the right. Any value works.
    /// @param newRotation Rotation IN DEGREES!!
    void setRotation(float newRotation);

    /// Sets the Object's scale relative to source graphic. For example {2.f, 2.f} will make it 2x bigger. If you do {2.f, 1.f}, it will only become 1x wider. You get it..
    /// @param newScale Scale relative to source graphic.
    void setScale(Vec2F newScale);

    /// Sets the Object's position in Canvas coordinate space. Canvas range is {0 - 65535} in both axes.
    /// @param newPosition Object's position in Canvas coordinate space
    void setPosition(Vec2F newPosition);

    /// Sets the Object's origin point around which the transformations will happen. Origin point is NORMALIZED AROUND THE SOURCE LSGRAPHIC'S SIZE,
    /// for example {0.5f, 0.5f} will set the origin directly to the center (that is also the default value after creating an object).
    /// @param newOrigin Origin point, NORMALIZED TO THE LSGRAPHIC'S SIZE!!
    void setOrigin(Vec2F newOrigin);

    /// Sets the Object's source graphic to a new one. Transformations are preserved by default.
    /// @param sourceGraphic An Graphic
    void setSourceGraphic(const Graphic& sourceGraphic, bool resetTransform = false);

    /// @returns The Object's position.
    Vec2F getPosition();

    /// @returns The Object's scale.
    Vec2F getScale();

    /// @returns The Object's rotation.
    float getRotation();

    /// @returns The Object's origin.
    Vec2F getOrigin();

    /// @returns A copy of the resulting transformed picture of this Object.
    Graphic getTransformedGraphic();

    /// Allows for direct manipulation of the source graphic via a reference. Mainly useful for changing color values in real time.
    /// @returns A reference to this Object's source graphic.
    Graphic& getSourceGraphic();

    /// Checks if any of the Object's transformed lines intersect with any transformed line from another Object.
    bool collidesWith(Object& other);

    /// Checks if any of the Object's transformed lines intersect with a line.
    bool collidesWith(const Line& other);

    /// Constructs an Object and sets a source Graphic right away
    Object(const Graphic& sourceGraphic);
};

}