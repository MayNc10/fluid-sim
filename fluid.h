#pragma once
#include <vector>
#include <omp.h>

#include "particle.h"
#include "SDL.h"
#include "collisionobject.h"
#include "vector2d.h"

class Fluid {

public:
    Fluid(int num_rows, int num_columns, Point base, float offset_per_item, int particle_radius,
          float smoothing_radius, float target_density, float pressure_multiplier, SDL_Texture* tex, SDL_Color color);
    void draw(SDL_Renderer* renderer, SDL_Color color);
    void draw_texture(SDL_Renderer* renderer);
    void update(float dt, const std::vector<CollisionObject*>& objects);

    // kernel function on p3 of tutorial, for 2d space
    // FIXME: Abstract kernel functions!
    static float kernel(float radius, float dst);
    static float kernel_derivative(float radius, float dst);
    static float color_logistic(float max_density, float density);

    float density(Point p) const;
    Vector2d pressure_gradient(Particle p) const;
    float density_to_pressure(float density) const;
    float shared_pressure(float density_a, float density_b) const;

    int idx(Point p) const;
    int idx(int row_idx, int col_idx) const;

    ~Fluid();
private:
    int particle_radius;
    float smoothing_radius;
    float target_density;
    float pressure_multiplier;

    SDL_Texture* tex;
    std::vector<uint8_t> pixels;
    SDL_Color color;

    float current_max_density;

    // FIXME: Only based on particles that exist on screen - this is bad in general
    std::vector<std::vector<Particle>> particles; // divide space into cells
    int cols;
    int rows;
};

void loop();
