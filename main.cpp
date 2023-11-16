#include <cstdio>
#include "SDL.h"
#undef main

int main() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Errror initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    printf("SDL succesfully initialized!\n");
    SDL_Quit();
    return 0;
}