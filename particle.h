#pragma once

class Particle {

public:
    Particle(float x, float y) :
        x(x), y(y), x_velocity(0), y_velocity(0) {}
    float get_x() { return this->x; }
    float get_y() { return this->y; }

    // Y is subtracted, because in SDL coords, 0 is at the top
    void update_position(float x_accel, float y_accel, float dt) {
        this->x_velocity += x_accel * dt;
        this->y_velocity += y_accel * dt;

        this->x += this->x_velocity * dt;
        this->y -= this->y_velocity * dt;
    }

private:
    float x;
    float y;
    float x_velocity;
    float y_velocity;
};
