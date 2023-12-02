#pragma once
#include "vector2d.h"

struct Point {
    float x;
    float y;
};

// This should probably use an enum, but whatever
int orientation(Point p1, Point p2, Point p3);

Point& operator+=(Point& lhs, const Vector2d& rhs);
Point operator+(const Point lhs, const Vector2d& rhs);
Vector2d operator-(const Point& lhs, const Point& rhs);

