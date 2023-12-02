#pragma once
#include <optional>
#include <tuple>
#include "point.h"
#include "vector2d.h"
#include "SDL.h"

class CollisionObject {

public:
    virtual std::optional<std::tuple<Vector2d, Point, float>> has_collided(const Point initial_position, const Vector2d incoming) const = 0;
    virtual void render(SDL_Renderer * renderer) = 0;
};

// Create the most basic collision object: a line
class CollisionLine : public CollisionObject {

public:
    CollisionLine(Point p1, Point p2)
        : p1(p1), p2(p2) {}
    std::optional<std::tuple<Vector2d, Point, float>> has_collided(const Point initial_position, const Vector2d incoming) const override;
    bool intersects(CollisionLine other) const;
    Point get_p1() const { return this->p1; }
    Point get_p2() const { return this->p2; }
    void render(SDL_Renderer * renderer) override;

private:
    Point p1;
    Point p2;
};
