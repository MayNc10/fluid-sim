#include <cstdio>
#include <chrono>
#include <thread>

#include "SDL.h"
#undef main
#include "fluid.h"

// Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 1024;

// The window renderer
SDL_Renderer* renderer = NULL;

//The window we'll be rendering to
SDL_Window* window = NULL;

const SDL_Color BACKGROUND_COLOR = { 0xFF, 0xFF, 0xFF, 0xFF};

const int RADIUS = 5;
const int ROWS = 5;
const int COLS = 10;

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

    Fluid fluid = Fluid(ROWS, COLS, 100, 100, COLS * 30, ROWS * 30, RADIUS);
    SDL_Color color = {0x42, 0x87, 0xF5, 0xFF};

    // Poll events
    SDL_Event e;

    bool quit = false;
    while( quit == false ) {
        while( SDL_PollEvent( &e ) ) {
            if( e.type == SDL_QUIT ) quit = true;
        }

        // Time the start of this render cycle
        Clock::time_point start_time = Clock::now();


        // We clear what we draw before
        SDL_RenderClear(renderer);
        // Update fluid
        fluid.update(0.02); // FIXME
        // Draw fluid
        fluid.draw(renderer, color);
        // Set the color to what was before
        SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a);
        // .. you could do some other drawing here
        // And now we present everything we draw after the clear.
        SDL_RenderPresent(renderer);

        Clock::time_point current_time = Clock::now();
        milliseconds diff = std::chrono::duration_cast<milliseconds>(current_time - start_time);
        // sleep until the next frame
        std::this_thread::sleep_for(WAIT_TIME - diff);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
