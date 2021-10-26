#include "graphics.h"

uint32_t* getPixelAt(int y, int x, SDL_Surface* surface) {
    return ((uint32_t*)surface->pixels)+((surface->pitch/4)*y+x);
}

void togglePixelAt(int y, int x, SDL_Surface* surface) {
    uint32_t* pixel = getPixelAt(y, x, surface);
    if (*pixel == COLOUR_WHITE)
    {
        *pixel = COLOUR_BLACK;
    }
    else
    {
        *pixel = COLOUR_WHITE;
    }
}

void renderSurface(SDL_Renderer* renderer, SDL_Surface* surface) {
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_DestroyTexture(texture);
}