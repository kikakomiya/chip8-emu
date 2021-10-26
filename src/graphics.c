#include "cpu.h"
#include "graphics.h"

uint32_t* getPixelAt(int y, int x, SDL_Surface* surface) {
    return ((uint32_t*)surface->pixels)+((surface->pitch/4)*y+x);
}

void togglePixelAt(int y, int x, SDL_Surface* surface, struct cpu* cpu) {
    uint32_t* pixel = getPixelAt(y, x, surface);
    if (*pixel == COLOUR_WHITE)
    {
        cpu->registers[15] = 1;
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