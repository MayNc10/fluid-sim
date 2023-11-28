#pragma once
#include <vector>
#include "particle.h"
#include "SDL.h"

class Fluid {

public:
    Fluid(int num_rows, int num_columns, int base_x, int base_y, int x_offset, int y_offset, int particle_radius);
    void draw(SDL_Renderer* renderer, SDL_Color color);
    void update(float dt);

private:
    std::vector<Particle> particles;
    int particle_radius;
};
