#pragma once


class Vector2d {

public:
    Vector2d()
        : x(0), y(0) {}

    Vector2d(float x, float y)
        : x(x), y(y) {}

    Vector2d(const Vector2d& v)
        : x(v.x), y(v.y) {}

    Vector2d& operator=(const Vector2d& v) {
        this->x = v.x;
        this->y = v.y;
        return *this;
    }

    float get_x() const;
    float get_y() const;

    void set_x(float x);
    void set_y(float y);

    Vector2d& operator+=(const Vector2d& rhs);

    friend Vector2d operator+(Vector2d lhs, const Vector2d& rhs);

    Vector2d& operator*=(const int& rhs);

    friend Vector2d operator*(Vector2d lhs, const int& rhs);

    Vector2d& operator*=(const float& rhs);

    friend Vector2d operator*(Vector2d lhs, const float& rhs);

    Vector2d& operator*=(const Vector2d rhs);
    Vector2d operator*(const Vector2d rhs);

    static float cross(const Vector2d lhs, const Vector2d rhs);

    static float dot(const Vector2d lhs, const Vector2d rhs);

    float magnitude() const;

private:
    float x;
    float y;
};


