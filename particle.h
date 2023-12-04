#pragma once
#include <vector>
#include <cstdio>
#include <optional>
#include "point.h"
#include "vector2d.h"
#include "collisionobject.h"
#include "constants.h"

class Particle {

public:
    Particle(Point initial_position)
        : position(initial_position), past_position(initial_position), velocity(Vector2d(0, 0)), density(0) {}
    Point get_position() const { return this->position; }
    Vector2d get_velocity() const { return this->velocity; } // Account for inverted axis

    // Y is subtracted, because in SDL coords, 0 is at the top
    void update_position(Vector2d accel, float dt, const std::vector<CollisionObject*>& objects) {
        //printf("velocity: (%f, %f), accel: (%f, %f)", this->velocity.get_x() * dt, this->velocity.get_y() * dt, accel.get_x() * dt, accel.get_y() * dt);

        // Before moving, check collisions
        for (const CollisionObject* object : objects) {
            if (auto result = object->has_collided(this->position, this->get_velocity())) { // Use get_velocity to deal with y axis inversion issues
                // Extract collision information
                Vector2d result_vector = std::get<0>(*result);
                Point intersect = std::get<1>(*result);
                float t = std::get<2>(*result);
                // If t > dt or < 0, reject
                // FIXME: This reject should happen earlier as a speed-up?
                if (t <= dt && t > 0) {
                    // update velocity, position, and remaining dt
                    this->velocity = Vector2d(result_vector.get_x(), result_vector.get_y());
                    this->position = this->past_position;
                    dt -= t;
                    //dt += DT_PADDING; // Give a bit of extra time
                    break; // Don't check any more collisions, I think things could get weird
                }
            }
        }
        this->past_position = this->position;

        this->position.x += this->velocity.get_x() * dt;
        this->position.y += this->velocity.get_y() * dt;

        this->velocity += accel * dt;
    }

    void update_position_no_accel(Vector2d accel, float dt, const std::vector<CollisionObject*>& objects) {


        // Before moving, check collisions
        for (const CollisionObject* object : objects) {
            if (auto result = object->has_collided(this->position, this->get_velocity())) { // Use get_velocity to deal with y axis inversion issues
                // Extract collision information
                Vector2d result_vector = std::get<0>(*result);
                Point intersect = std::get<1>(*result);
                float t = std::get<2>(*result);
                // If t > dt or < 0, reject
                // FIXME: This reject should happen earlier as a speed-up?
                if (t <= dt && t > 0) {
                    // update velocity, position, and remaining dt
                    this->velocity = Vector2d(result_vector.get_x(), result_vector.get_y());
                    this->position = this->past_position;
                    dt -= t;
                    //dt += DT_PADDING; // Give a bit of extra time
                    return;
                    break; // Don't check any more collisions, I think things could get weird
                }
            }
        }
        this->past_position = this->position;

        this->position.x += this->velocity.get_x() * dt;
        this->position.y += this->velocity.get_y() * dt;

        this->velocity = accel * dt;
    }

    float get_mass() const {
        return PARTICLE_MASS;
    }

    void set_density(float density) {
        this->density = density;
    }
    float get_density() const {
        return this->density;
    }
private:
    Point position;
    Point past_position;
    Vector2d velocity;
    float density;
};
