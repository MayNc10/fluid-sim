#include <vector>
#include "fluid.h"
#include "particle.h"
#include "sdl_render_functions.h"

const float GRAVITY = -20.0;

Fluid::Fluid(int num_rows, int num_columns, int base_x, int base_y, int x_offset, int y_offset, int particle_radius)
    : particle_radius(particle_radius)
{
    float dr = y_offset / (num_rows - 1);
    float dc = x_offset / (num_columns - 1);
    std::vector<Particle> particles;

    for (float y = base_y; y <= y_offset + base_y; y += dr) {
        for (float x = base_x; x <= x_offset + base_x; x += dc) {
            particles.push_back(Particle(x, y));
        }
    }

    this->particles = particles;
}

void Fluid::draw(SDL_Renderer* renderer, SDL_Color color) {
    for (Particle p : this->particles) {
        draw_circle(renderer, (int) p.get_x(), (int) p.get_y(), this->particle_radius, color);
    }
}

void Fluid::update(float dt) {
    for (Particle & p : this->particles) {
        p.update_position(0, GRAVITY, dt);
    }
}
