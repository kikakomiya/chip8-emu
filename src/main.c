#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cpu.h"
#include "graphics.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Not enough arguments!\n");
        return EXIT_FAILURE;
    }
    FILE* rom = fopen(argv[1], "r");

    struct cpu cpu;
    initialiseCPU(&cpu);

    fread(&cpu.ram[512], sizeof(cpu.ram[0]), (sizeof(cpu.ram)-512), rom);

    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not intialise! Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 640, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "Window could not be created! Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        fprintf(stderr, "Renderer could not be created! Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_RenderSetLogicalSize(renderer, 64, 32);
    screenSurface = SDL_CreateRGBSurface(0, 64, 32, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    if (screenSurface == NULL) {
        fprintf(stderr, "Surface could not be created! Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
    while (cpu.pc < 4096) {
        fprintf(stdout, "PC: %d\n", cpu.pc);
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    fprintf(stdout, "Key press detected\n");
                    break;
                case SDL_KEYUP:
                    fprintf(stdout, "Key release detected\n");
                    break;
                case SDL_QUIT:
                    return EXIT_SUCCESS;
                    break;
            }
        }
        uint16_t raw_insn = ((uint16_t)cpu.ram[cpu.pc] << 8) | cpu.ram[cpu.pc+1];
        executeInstruction(getInstruction(raw_insn), &cpu, screenSurface);
        SDL_RenderClear(renderer);
        renderSurface(renderer, screenSurface);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
        cpu.pc += 2;
    }
    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(screenSurface);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
