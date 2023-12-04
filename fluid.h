#pragma once
#include <vector>
#include <omp.h>

#include "particle.h"
#include "SDL.h"
#include "collisionobject.h"
#include "vector2d.h"

class Fluid {

public:
    Fluid(int num_rows, int num_columns, Point base, Vector2d offset, int particle_radius, float smoothing_radius, float target_density, float pressure_multiplier);
    void draw(SDL_Renderer* renderer, SDL_Color color);
    void update(float dt, const std::vector<CollisionObject*>& objects);

    // kernel function on p3 of tutorial, for 2d space
    // FIXME: Abstract kernel functions!
    static float kernel(float radius, float dst);
    static float kernel_derivative(float radius, float dst);

    float density(Point p) const;
    Vector2d pressure_gradient(Particle p) const;
    float density_to_pressure(float density) const;
    float shared_pressure(float density_a, float density_b) const;
private:
    std::vector<Particle> particles;
    int particle_radius;
    float smoothing_radius;
    float target_density;
    float pressure_multiplier;
};

void loop();
