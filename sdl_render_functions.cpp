#include "SDL.h"
#include "sdl_render_functions.h"

// Ripped from https://stackoverflow.com/questions/65723827/sdl2-function-to-draw-a-filled-circle
void draw_circle(SDL_Renderer* renderer, int32_t center_x, int32_t center_y, int32_t radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int x = 0;
    int y = radius;
    int m = 5 - 4 * radius;

    while (x <= y)
    {
        SDL_RenderDrawLine(renderer, center_x - x, center_y - y, center_x + x, center_y - y);
        SDL_RenderDrawLine(renderer, center_x - y, center_y - x, center_x + y, center_y - x);
        SDL_RenderDrawLine(renderer, center_x - y, center_y + x, center_x + y, center_y + x);
        SDL_RenderDrawLine(renderer, center_x - x, center_y + y, center_x + x, center_y + y);

        if (m > 0)
        {
            y--;
            m -= 8 * y;
        }

        x++;
        m += 8 * x + 4;
    }
}

/*
 *     SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, center_x + dx, center_y + dy);
            }
        }
    }
 *
*/
