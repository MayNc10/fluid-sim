#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <execution>
#include <random>
#include <functional>

#include "fluid.h"
#include "particle.h"
#include "point.h"
#include "sdl_render_functions.h"
#include "vector2d.h"
#include "collisionobject.h"
#include "constants.h"

auto RNG = std::default_random_engine {};
const int ITERS_BEFORE_RESET = 1;
int ITER = 0;

Fluid::Fluid(int num_rows, int num_columns, Point base, float offset_per_item, int particle_radius,
             float smoothing_radius, float target_density, float pressure_multiplier, SDL_Texture* tex, SDL_Color color)
    : particle_radius(particle_radius), smoothing_radius(smoothing_radius), target_density(target_density), pressure_multiplier(pressure_multiplier),
      tex(tex), color(color), current_max_density(0)
{
    SDL_Point size;
    SDL_QueryTexture(this->tex, NULL, NULL, &size.x, &size.y);

    // Array is a grid of cells, with width and height given by the smoothing radius
    // The max extent of this grid is the size of the screen
    int cols = (int) std::ceil(((float) size.x / smoothing_radius));
    int rows = (int) std::ceil(((float) size.y / smoothing_radius));
    this->cols = cols;
    this->rows = rows;
    std::vector<std::vector<Particle>> particles = std::vector<std::vector<Particle>>(cols * rows);


    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_columns; j++) {
            Point p = base + Vector2d(j, i) * offset_per_item;
            // which cell does this go to?
            int idx = this->idx(p);
            particles[idx].push_back(Particle(p));
        }
    }

    this->particles = particles;
    // Get the size of the texture to create the pixel buffer
    // FIXME? We could create the texture here instead

    this->pixels = std::vector<uint8_t> (size.x * size.y * 4, 0);
}

Fluid::~Fluid() {
    SDL_DestroyTexture(this->tex);
}

void Fluid::draw(SDL_Renderer* renderer, SDL_Color color) {

    for (int idx = 0; idx < this->rows * this->cols; idx++) {
        std::vector<Particle>& p_list = this->particles[idx];
        for (const Particle& p : p_list) {
            draw_circle(renderer, p.get_position(), this->particle_radius, color);
            // Draw vector

            //CollisionLine movement_line = CollisionLine(p.get_position(), p.get_position() + p.get_velocity());
            //movement_line.render(renderer);
        }
    }
}


void Fluid::draw_texture(SDL_Renderer* renderer) {

    // get texture size
    SDL_Point size;
    SDL_QueryTexture(this->tex, NULL, NULL, &size.x, &size.y);

    // erase all old values
    // FIXME: We could probably do something clever with these but whatever
    if (ITER == ITERS_BEFORE_RESET) {
        std::fill(this->pixels.begin(), this->pixels.end(), 0);
        ITER = -1;
    }
    ITER += 1;
    std::vector<int> pixel_idx = std::vector<int>(this->pixels.size() / 4);
    std::iota(pixel_idx.begin(), pixel_idx.end(), 0);


    std::vector<int> offsets = std::vector<int>(smoothing_radius * 2 * TEX_BLUR_SCALING + 1);
    std::iota(offsets.begin(), offsets.end(), -smoothing_radius * TEX_BLUR_SCALING);

    std::vector<int> indexes = std::vector<int>(this->rows * this->cols);
    std::iota(indexes.begin(), indexes.end(), 0);

    std::for_each(std::execution::par,
        indexes.begin(), indexes.end(),
        [&] (auto&& i) {
            std::vector<Particle>& p_vec = this->particles[i];
            if (p_vec.size() == 0) return;
            Particle p = p_vec[0];

            std::for_each(std::execution::par,
                offsets.begin(), offsets.end(),
                [&] (auto&& x_offset) {
                    int x = p.get_position().x + x_offset;

                    std::for_each(std::execution::par,
                        offsets.begin(), offsets.end(),
                        [&] (auto&& y_offset) {
                            int y = p.get_position().y + y_offset;
                            const unsigned int offset = ( size.x * y * 4 ) + x * 4;
                            if (offset > this->pixels.size() || offset < 0) return;

                            if (pixels[offset + 3] != 0) return;

                            // get the density
                            float density = this->density(Point {(float)x, (float)y});
                            float scaling = density / this->current_max_density * RATIO_SCALING;//Fluid::color_logistic(this->current_max_density, density);
                            //if (density != 0) printf("density: %f, max_density: %f, Scaling: %f\n", density, this->current_max_density, scaling);

                            // calculate color based on density
                            pixels[offset] = this->color.b * scaling; // b
                            pixels[offset + 1] = this->color.g * scaling; // g
                            pixels[offset + 2] = this->color.r * scaling; // r
                            pixels[offset + 3] = SDL_ALPHA_OPAQUE; // a
                        }
                    );
                }
            );
        }
    );


    // update texture
    SDL_UpdateTexture(this->tex, nullptr, pixels.data(), size.x * 4);

    // render texture
    SDL_RenderCopy(renderer, this->tex, nullptr, nullptr);
}

void Fluid::update(float dt, const std::vector<CollisionObject*>& objects) {
    // randomize order of particles
    //std::shuffle(this->particles.begin(), this->particles.end(), RNG);

    // calculate densities at each step
    this->current_max_density = 0;
    std::vector<int> indexes = std::vector<int>(this->rows * this->cols);
    std::iota(indexes.begin(), indexes.end(), 0);

    std::for_each(std::execution::par,
        indexes.begin(), indexes.end(),
        [&] (auto&& idx) {
            std::vector<Particle>& cell = this->particles[idx];
            // This could be another parallel loop
            float cell_max_density = 0;

            std::for_each(std::execution::par,
                cell.begin(), cell.end(),
                [&] (auto&& particle) {
                    float density = this->density(particle.get_position());
                    if (density == 0) {
                        printf("ERROR: density is zero\n");
                        printf("Particle position: (%f, %f)\n", particle.get_position().x, particle.get_position().y);
                        abort();
                    }

                    particle.set_density(density);
                    if (density > cell_max_density) cell_max_density = density;
                }
            );

            if (cell_max_density > this->current_max_density) {
                this->current_max_density = cell_max_density;
            }
        }
    );


    std::for_each(std::execution::par,
        indexes.begin(), indexes.end(),
        [&] (auto&& idx) {
            std::vector<Particle>& cell = this->particles[idx];
            for (Particle& particle : cell) {
                Vector2d pressure_force = this->pressure_gradient(particle);
                Vector2d pressure_accel = Vector2d(0, 0);

                if (pressure_force.magnitude() != 0) {
                    // should avoid errors with having zero density
                    pressure_accel = pressure_force * (1 / particle.get_density());
                }

                Vector2d external = Vector2d(0, GRAVITY);

                Vector2d net_accel = pressure_accel + external;
                //printf("Net accel: (%f, %f), particle coords: (%f, %f)\n", net_accel.get_x(), net_accel.get_y(),
                //       particle.get_position().x, particle.get_position().y);

                particle.update_position(net_accel, dt, objects);
            }
        }
    );


    // I don't think this would work in parallel, too many (actually important) data races
    std::for_each(indexes.begin(), indexes.end(),
        [&] (auto&& idx) {
            std::vector<Particle>& cell = this->particles[idx];
            //printf("Address of cell %d BEFORE deleting elements: %p, address of reference: %p\n", idx, (void *)&this->particles[idx], (void*)&cell);
            std::vector<Particle> new_cell = std::vector<Particle>();

            for (int cell_idx = 0; cell_idx < cell.size(); cell_idx++) {
                Particle particle = cell[cell_idx];

                int v_idx = this->idx(particle.get_position());

                // reassign particle
                if (v_idx != idx) {
                    if(v_idx < this->particles.size()) {
                        this->particles[v_idx].push_back(particle);
                    }
                }
                else {
                    new_cell.push_back(particle);
                }
            }
            //printf("Address of cell %d AFTER deleting elements: %p, address of reference: %p\n", idx, (void *)&this->particles[idx], (void*)&cell);
            this->particles[idx] = new_cell;
        }
    );

}



// Make sure this keeps the same value with larger radius
float Fluid::kernel(float radius, float dst) {
    if (dst >= radius) return 0;

    float volume = (PI * std::pow(radius, 4)) / 6;
    return (radius - dst) * (radius - dst) / volume;

    //float volume = (PI * std::pow(radius, 4)) / 6;
    //return (radius - dst) * (radius - dst) * (radius - dst) / volume;
}

float Fluid::kernel_derivative(float radius, float dst) {
    if (dst >= radius) return 0;


    float scale = 12 / (PI * std::pow(radius, 4));
    return -(radius - dst) * scale;

    //float scale = 18 / (PI * std::pow(radius, 4));
    //return -(radius - dst) * (radius - dst) * scale;
}


float Fluid::density(Point p) const {
    float density = 0;

    // get cell idx
    int row_idx = (int) p.y / smoothing_radius;
    int col_idx = (int) p.x / smoothing_radius;

    for (int drow = -1; drow <= 1; drow++) {
        for (int dcol = -1; dcol <= 1; dcol += 1) {
            int idx = this->idx(row_idx + drow, col_idx + dcol);

            if (idx > this->rows * this->cols) return density; // there should've be any more valid cells
            if(idx < 0) continue;
            const std::vector<Particle>& particles = this->particles[idx];


            for (const Particle& particle : particles) {
                if (&particle == nullptr) {
                    printf("MASSIVE ERROR: PARTICLE WAS NULL\n");
                    continue;
                }

                float dst = (particle.get_position() - p).magnitude();
                float influence = Fluid::kernel(this->smoothing_radius, dst);

                density += particle.get_mass() * influence;
            }
        }
    }

    return density;
}

Vector2d Fluid::pressure_gradient(Particle p) const {
    Vector2d pressure_gradient = Vector2d();
    // get cell idx
    int row_idx = (int) p.get_position().y / smoothing_radius;
    int col_idx = (int) p.get_position().x / smoothing_radius;

    for (int drow = -1; drow <= 1; drow++) {
        for (int dcol = -1; dcol <= 1; dcol += 1) {
            int idx = this->idx(row_idx + drow, col_idx + dcol);

            if (idx > this->rows * this->cols) return pressure_gradient; // there should've be any more valid cells
            if(idx < 0) continue;

            const std::vector<Particle>& particles = this->particles[idx];

            for (const Particle& particle : particles) {
                float dst = (particle.get_position() - p.get_position()).magnitude();
                if (dst == 0) continue;
                Vector2d dir = (particle.get_position() - p.get_position()) * (1 / dst);
                float slope = Fluid::kernel_derivative(smoothing_radius, dst);
                float density = particle.get_density();
                //printf("dst: %f, dir: (%f, %f), slope: %f, density: %f\n", dst, dir.get_x(), dir.get_y(), slope, density);
                float shared_pressure = this->shared_pressure(density, p.get_density());
                pressure_gradient += dir * -shared_pressure * slope * (particle.get_mass() / density);
            }
        }
    }

    return pressure_gradient;
}

// FIXME: Make this more realistic
float Fluid::density_to_pressure(float density) const {
    float density_err = density - this->target_density;
    float pressure = density_err * this->pressure_multiplier;
    return pressure;
}

float Fluid::shared_pressure(float density_a, float density_b) const {
    float pressure_a = this->density_to_pressure(density_a);
    float pressure_b = this->density_to_pressure(density_b);
    return (pressure_a + pressure_b) / 2;
}

int Fluid::idx(Point p) const {
    int row_idx = (int) p.y / smoothing_radius;
    int col_idx = (int) p.x / smoothing_radius;

    return row_idx * this->cols + col_idx;
}

int Fluid::idx(int row_idx, int col_idx) const {
    return row_idx * this->cols + col_idx;
}

// from https://math.stackexchange.com/questions/1832177/sigmoid-function-with-fixed-bounds-and-variable-steepness-partially-solved
float Fluid::color_logistic(float max_density, float density) {
    float b = (max_density / density - 1 + LOGISTIC_EPSILON);
    float b_k = std::pow(b, -LOGISTIC_GROWTH_RATE);
    float denom = 1 + b_k;
    //if (density != 0) printf("density: %f, max density: %f, b: %f, b_k: %f, denom: %f, ans: %f\n", density, max_density, b, b_k, denom, 1 - 1 / denom);
    return 1 - 1 / denom;
}

