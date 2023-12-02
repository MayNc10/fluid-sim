#include <cmath>
#include "vector2d.h"

float Vector2d::get_x() const {
    return this->x;
}

float Vector2d::get_y() const {
    return this->y;
}

Vector2d& Vector2d::operator+=(const Vector2d& rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}

Vector2d operator+(Vector2d lhs, const Vector2d& rhs) {
    lhs += rhs;
    return lhs;
}

Vector2d& Vector2d::operator*=(const int& rhs) {
    this->x *= rhs;
    this->y *= rhs;
    return *this;
}

Vector2d operator*(Vector2d lhs, const int& rhs) {
    lhs *= rhs;
    return lhs;
}

Vector2d& Vector2d::operator*=(const float& rhs) {
    this->x *= rhs;
    this->y *= rhs;
    return *this;
}

Vector2d operator*(Vector2d lhs, const float& rhs) {
    lhs *= rhs;
    return lhs;
}

float Vector2d::cross(const Vector2d lhs, const Vector2d rhs) {
    return lhs.x * rhs.y  - lhs.y * rhs.x;
}

float Vector2d::dot(const Vector2d lhs, const Vector2d rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

float Vector2d::magnitude() const {
    return std::sqrt(Vector2d::dot(*this, *this));
}

Vector2d& Vector2d::operator*=(const Vector2d rhs) {
    this->x *= rhs.x;
    this->y *= rhs.y;
    return *this;
}

Vector2d Vector2d::operator*(const Vector2d rhs) {
    *this *= rhs;
    return *this;
}
