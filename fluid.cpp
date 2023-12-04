#include <vector>
#include <cmath>
#include "fluid.h"
#include "particle.h"
#include "point.h"
#include "sdl_render_functions.h"
#include "vector2d.h"
#include "collisionobject.h"
#include "constants.h"

Fluid::Fluid(int num_rows, int num_columns, Point base, Vector2d offset, int particle_radius, float smoothing_radius, float target_density, float pressure_multiplier)
    : particle_radius(particle_radius), smoothing_radius(smoothing_radius), target_density(target_density), pressure_multiplier(pressure_multiplier)
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
        movement_line.render(renderer);
    }
}

void Fluid::update(float dt, const std::vector<CollisionObject*>& objects) {
    // first, calculate densities at each step
    for (Particle & p : this->particles) {
        p.set_density(this->density(p.get_position()));
    }

    for (Particle & p : this->particles) {
        Vector2d pressure_force = this->pressure_gradient(p);
        Vector2d pressure_accel = pressure_force * (1 / p.get_density());


        Vector2d external = Vector2d(0, GRAVITY);

        //printf("Force: (%f, %f), accel: (%f, %f), gravity: (%f, %f)\n",
        //       pressure_force.get_x(), pressure_force.get_y(), pressure_accel.get_x(), pressure_accel.get_y(), external.get_x(), external.get_y());
        //pressure_accel.set_y(pressure_accel.get_y() * -1);
        p.update_position(pressure_accel + external, dt, objects);
    }
}



// Make sure this keeps the same value with larger radius
float Fluid::kernel(float radius, float dst) {
    if (dst >= radius) return 0;

    float volume = (PI * std::pow(radius, 4)) / 6;
    return (radius - dst) * (radius - dst) / volume;
}

float Fluid::kernel_derivative(float radius, float dst) {
    if (dst >= radius) return 0;

    float scale = 12 / (PI * std::pow(radius, 4));
    return (dst - radius) * scale;
}


float Fluid::density(Point p) const {
    float density = 0;

    // FIXME: OPTIMIZE by only checking particles inside the smoothing radius
    for (const Particle& particle : this->particles) {
        float dst = (particle.get_position() - p).magnitude();
        float influence = Fluid::kernel(this->smoothing_radius, dst);
        density += particle.get_mass() * influence;
    }
    return density;
}

Vector2d Fluid::pressure_gradient(Particle p) const {
    Vector2d pressure_gradient = Vector2d();

    for (const Particle& particle : this->particles) {
        float dst = (particle.get_position() - p.get_position()).magnitude();
        if (dst == 0) continue;
        Vector2d dir = (particle.get_position() - p.get_position()) * (1 / dst);
        float slope = Fluid::kernel_derivative(this->smoothing_radius, dst);
        float density = particle.get_density();
        //printf("dst: %f, dir: (%f, %f), slope: %f, density: %f\n", dst, dir.get_x(), dir.get_y(), slope, density);
        float shared_pressure = this->shared_pressure(density, p.get_density());
        pressure_gradient += dir * -shared_pressure * slope * (particle.get_mass() / density);
    }

    return pressure_gradient;
}

// FIXME: Make this more realistic
float Fluid::density_to_pressure(float density) const {
    float density_err = density - this->target_density;
    float pressure = density_err - this->pressure_multiplier;
    return pressure;
}

float Fluid::shared_pressure(float density_a, float density_b) const {
    float pressure_a = this->density_to_pressure(density_a);
    float pressure_b = this->density_to_pressure(density_b);
    return (pressure_a + pressure_b) / 2;
}
