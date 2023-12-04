#include <cstdio>
#include <chrono>
#include <thread>
#include <vector>
#include <omp.h>

#include "SDL.h"
#undef main
#include "fluid.h"
#include "collisionobject.h"

// Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 1024;

// The window renderer
SDL_Renderer* renderer = NULL;

//The window we'll be rendering to
SDL_Window* window = NULL;

const SDL_Color BACKGROUND_COLOR = { 0xFF, 0xFF, 0xFF, 0xFF};

const int RADIUS = 3;
const int ROWS = 20;
const int COLS = 20;
const int PARTICLE_DISTANCE = 15;
const float PRESSURE_MULTIPLIER = 15;
const float SMOOTHING_RADIUS = PARTICLE_DISTANCE * 2;
const float TARGET_DENSITY = 5;

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

const int TARGET_FPS = 100;
const milliseconds WAIT_TIME = milliseconds((int)(1000.0 / ((float) TARGET_FPS)));

int main() {

    //The surface contained by the window
  SDL_Surface* screenSurface = NULL;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
      printf("Errror initializing SDL: %s\n", SDL_GetError());
      return 1;
  }

  printf("SDL succesfully initialized!\n");

  //  window
  window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
  if (!window) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
      return 1;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(!renderer)
  {
      printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
      return 1;
  }

  Fluid fluid = Fluid(ROWS, COLS, Point {500, 400}, Vector2d(COLS * PARTICLE_DISTANCE, ROWS * PARTICLE_DISTANCE), RADIUS, SMOOTHING_RADIUS, TARGET_DENSITY, PRESSURE_MULTIPLIER);
  // Create a collision plane
  CollisionLine ground = CollisionLine(Point {50, 700}, Point {1000, 700} );
  CollisionLine top = CollisionLine(Point {50, 50}, Point {1000, 50} );
  CollisionLine left = CollisionLine(Point {50, 50}, Point {50, 700});
  CollisionLine right = CollisionLine(Point {1000, 50}, Point {1000, 700});
  std::vector<CollisionObject*> objects = {&ground, &top, &left, &right};
  SDL_Color color = {0x42, 0x87, 0xF5, 0xFF};

  // Poll events
  SDL_Event e;

  bool quit = false;
  bool paused = false;
  bool will_repause = false;

  while( quit == false ) {
      while( SDL_PollEvent( &e ) ) {
          if( e.type == SDL_QUIT ) quit = true;
          else if (e.type == SDL_KEYDOWN) {
              if (e.key.keysym.sym == SDLK_SPACE) paused = !paused;
              else if (e.key.keysym.sym == SDLK_RETURN && paused) {
                  paused = false;
                  will_repause = true;
              }
          }
      }
      if (paused) { continue; }

      // Time the start of this render cycle
      Clock::time_point start_time = Clock::now();

      // We clear what we draw before
      SDL_RenderClear(renderer);
      // Update fluid
      fluid.update(0.02, objects); // FIXME
      // Draw fluid
      fluid.draw(renderer, color);
      // Render ground
      for (CollisionObject* object : objects) {
          object->render(renderer);
      }

      // Set the color to what was before
      SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a);
      // .. you could do some other drawing here
      // And now we present everything we draw after the clear.
      SDL_RenderPresent(renderer);

      Clock::time_point current_time = Clock::now();
      milliseconds diff = std::chrono::duration_cast<milliseconds>(current_time - start_time);
      printf("Time waiting: %d\n", (int)(WAIT_TIME - diff).count());
      // sleep until the next frame
      std::this_thread::sleep_for(WAIT_TIME - diff);

      if (will_repause) {
          will_repause = false;
          paused = true;
      }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

    return 0;
}


