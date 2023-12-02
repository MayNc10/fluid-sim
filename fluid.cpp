#include <vector>
#include "fluid.h"
#include "particle.h"
#include "point.h"
#include "sdl_render_functions.h"
#include "vector2d.h"
#include "collisionobject.h"

const float GRAVITY = 20.0;

Fluid::Fluid(int num_rows, int num_columns, Point base, Vector2d offset, int particle_radius)
    : particle_radius(particle_radius)
{
    float dr = offset.get_y() / (num_rows - 1);
    float dc = offset.get_x() / (num_columns - 1);
    std::vector<Particle> particles;

    for (float y = base.y; y <= offset.get_y() + base.y; y += dr) {
        for (float x = base.x; x <= offset.get_x() + base.x; x += dc) {
            Point p = {x, y};
            particles.push_back(Particle(p));
        }
    }

    this->particles = particles;
}

void Fluid::draw(SDL_Renderer* renderer, SDL_Color color) {
    for (Particle p : this->particles) {
        draw_circle(renderer, p.get_position(), this->particle_radius, color);
        // Draw vector
        CollisionLine movement_line = CollisionLine(p.get_position(), p.get_position() + p.get_velocity());
        //movement_line.render(renderer);
    }
}

void Fluid::update(float dt, const std::vector<CollisionObject*>& objects) {
    for (Particle & p : this->particles) {
        Vector2d accel = Vector2d(0, GRAVITY);
        p.update_position(accel, dt, objects);
    }
}
