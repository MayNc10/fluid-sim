#pragma once
#include <vector>
#include "particle.h"
#include "SDL.h"
#include "collisionobject.h"

class Fluid {

public:
    Fluid(int num_rows, int num_columns, Point base, Vector2d offset, int particle_radius);
    void draw(SDL_Renderer* renderer, SDL_Color color);
    void update(float dt, const std::vector<CollisionObject*>& objects);

private:
    std::vector<Particle> particles;
    int particle_radius;
};
