#include "point.h"
#include "vector2d.h"

int orientation(Point p1, Point p2, Point p3) {
    float val = (p2.y - p1.y) * (p3.x - p2.x) - (p2.x - p1.x) * (p3.y - p2.y);
    if (val > 0) { return 1; } // Clockwise
    else if (val < 0) { return 2; } // Counter-Clockwise
    else { return 0; } // Collinear
}

Point& operator+=(Point& lhs, const Vector2d& rhs) {
    lhs.x += rhs.get_x();
    lhs.y += rhs.get_y();
    return lhs;
}

Point operator+(Point lhs, const Vector2d& rhs) {
    lhs += rhs;
    return lhs;
}

Vector2d operator-(const Point& lhs, const Point& rhs) {
    return Vector2d(lhs.x - rhs.x, lhs.y - rhs.y);
}
