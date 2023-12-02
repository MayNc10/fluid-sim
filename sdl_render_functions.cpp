#include "SDL.h"
#include "sdl_render_functions.h"
#include "point.h"

// Ripped from https://stackoverflow.com/questions/65723827/sdl2-function-to-draw-a-filled-circle
void draw_circle(SDL_Renderer* renderer, Point center, int32_t radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int x = 0;
    int y = radius;
    int m = 5 - 4 * radius;

    while (x <= y)
    {
        SDL_RenderDrawLine(renderer, center.x - x, center.y - y, center.x + x, center.y - y);
        SDL_RenderDrawLine(renderer, center.x - y, center.y - x, center.x + y, center.y - x);
        SDL_RenderDrawLine(renderer, center.x - y, center.y + x, center.x + y, center.y + x);
        SDL_RenderDrawLine(renderer, center.x - x, center.y + y, center.x + x, center.y + y);

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
