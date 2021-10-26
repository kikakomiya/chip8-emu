#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define KEY_NOT_PRESSED 0
#define KEY_PRESSED 1

#include "opcodes.h"
#include "sprites.h"

struct instruction
{
    uint16_t opcode;
    uint16_t arg1;
    uint16_t arg2;
    uint16_t arg3;
};

struct cpu
{
    uint8_t registers[16];
    uint16_t I;
    uint8_t ram[4096];
    uint8_t timers[2];
    uint16_t pc;
    uint8_t sp;
    uint16_t stack[16];
};

void initialiseCPU(struct cpu* cpu)
{
    fprintf(stdout, "Initialising CPU...\n");
    memset(cpu, 0, sizeof(struct cpu));
    cpu->pc = 512;

    memcpy(&cpu->ram[SPRITE_ZERO], (uint8_t[]) {0xF0, 0x90, 0x90, 0x90, 0xF0}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_ONE], (uint8_t[]) {0x20, 0x60, 0x20, 0x20, 0x80}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_TWO], (uint8_t[]) {0xF0, 0x10, 0xF0, 0x80, 0xF0}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_THREE], (uint8_t[]) {0xF0, 0x80, 0xF0, 0x80, 0xF0}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_FOUR], (uint8_t[]) {0x90, 0x90, 0xF0, 0x10, 0x10}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_FIVE], (uint8_t[]) {0xF0, 0x80, 0xF0, 0x10, 0xF0}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_SIX], (uint8_t[]) {0xF0, 0x80, 0xF0, 0x90, 0xF0}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_SEVEN], (uint8_t[]) {0xF0, 0x10, 0x20, 0x40, 0x40}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_EIGHT], (uint8_t[]) {0xF0, 0x90, 0xF0, 0x90, 0xF0}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_NINE], (uint8_t[]) {0xF0, 0x90, 0xF0, 0x10, 0xF0}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_A], (uint8_t[]) {0xF0, 0x90, 0xF0, 0x90, 0x90}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_B], (uint8_t[]) {0xE0, 0x90, 0xE0, 0x90, 0xE0}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_C], (uint8_t[]) {0xF0, 0x80, 0x80, 0x80, 0xF0}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_D], (uint8_t[]) {0xE0, 0x90, 0x90, 0x90, 0xE0}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_E], (uint8_t[]) {0xF0, 0x80, 0xF0, 0x80, 0xF0}, SPRITE_LENGTH);
    memcpy(&cpu->ram[SPRITE_F], (uint8_t[]) {0xF0, 0x80, 0xF0, 0x80, 0x80}, SPRITE_LENGTH);
}

uint32_t* getPixelAt(int y, int x, SDL_Surface* surface)
{
    return ((uint32_t*)surface->pixels)+((surface->pitch/4)*y+x);
}

void togglePixelAt(int y, int x, SDL_Surface* surface)
{
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

void renderSurface(SDL_Renderer* renderer, SDL_Surface* surface)
{
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_DestroyTexture(texture);
}

struct instruction getInstruction(uint16_t opcode)
{
    struct instruction insn;
    switch (opcode & 0xF000)
    {
        case 0x0000:
            if (opcode == 0x00E0) // 00E0
            {
                insn.opcode = OP_ClearScreen;
            }
            else if (opcode == 0x00EE) // 00EE
            {
                insn.opcode = OP_Return;
            }
            break;
        case 0x1000: // 1NNN
            insn.opcode = OP_Jump;
            insn.arg1 = opcode & 0x0FFF;
            break;
        case 0x2000: // 2NNN
            insn.opcode = OP_Subroutine;
            insn.arg1 = opcode & 0x0FFF;
            break;
        case 0x3000: // 3XNN
            insn.opcode = OP_SkipIfEqualsImmediate;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = opcode & 0x00FF;
            break;
        case 0x4000: // 4XNN
            insn.opcode = OP_SkipIfDoesNotEqualImmediate;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = opcode & 0x00FF;
            break;
        case 0x5000: // 5XY0
            insn.opcode = OP_SkipIfEqualsReg;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = (opcode & 0x00F0) >> 4;
            break;
        case 0x6000: // 6XNN
            insn.opcode = OP_SetRegToImmediate;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = opcode & 0x00FF;
            break;
        case 0x7000: // 7XNN
            insn.opcode = OP_AddImmediateToReg;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = opcode & 0x00FF;
            break;
        case 0x8000:
            switch (opcode & 0x000F)
            {
                case 0x0000: // 8XY0
                    insn.opcode = OP_SetRegValToReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0001: // 8XY1
                    insn.opcode = OP_RegORReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0002: // 8XY2
                    insn.opcode = OP_RegANDReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0003: // 8XY3
                    insn.opcode = OP_RegXORReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0004: // 8XY4
                    insn.opcode = OP_AddRegValToReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0005: // 8XY5
                    insn.opcode = OP_SubRegValFromReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0006: // 8XY6
                    insn.opcode = OP_StoreLeastSignifAndShiftRight;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0007: // 8XY7
                    insn.opcode = OP_SetRegToSubRegValFromReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x000E: // 8XYE
                    insn.opcode = OP_StoreMostSignifAndShiftLeft;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                default:
                    break;
            }
            break;
        case 0x9000: // 9XY0
            insn.opcode = OP_SkipIfRegDoesNotEqualReg;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = (opcode & 0x00F0) >> 4;
            break;
        case 0xA000: // ANNN
            insn.opcode = OP_SetIToImmediate;
            insn.arg1 = opcode & 0x0FFF;
            break;
        case 0xB000: // BNNN
            insn.opcode = OP_JumpToImmediatePlusV0;
            insn.arg1 = opcode & 0x0FFF;
            break;
        case 0xC000: // CXNN
            insn.opcode = OP_SetRegToRandANDImmediate;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = opcode & 0x00FF;
            break;
        case 0xD000: // DXYN
            insn.opcode = OP_DrawSpriteAtCoords;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = (opcode & 0x00F0) >> 4;
            insn.arg3 = opcode & 0x000F;
            break;
        case 0xE000:
            if ((opcode & 0xF0FF) == 0xE09E) // EX9E
            {
                insn.opcode = OP_SkipIfKeyPressed;
                insn.arg1 = (opcode & 0x0F00) >> 8;
            }
            else if ((opcode & 0xF0FF) == 0xE0A1) // EXA1
            {
                insn.opcode = OP_SkipIfKeyNotPressed;
                insn.arg1 = (opcode & 0x0F00) >> 8;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x0007: // FX07
                    insn.opcode = OP_SetRegValFromDelayTimer;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x000A: // FX0A
                    insn.opcode = OP_SetRegValToKeyPressed;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x0015: // FX15
                    insn.opcode = OP_SetDelayTimerFromRegVal;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x0018: // FX18
                    insn.opcode = OP_SetSoundTimerFromRegVal;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x001E: // FX1E
                    insn.opcode = OP_AddRegToI;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x0029: // FX29
                    insn.opcode = OP_SetIToSpriteLocationInReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x0033: // FX33
                    insn.opcode = OP_SetBCDOfReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x0055: // FX55
                    insn.opcode = OP_StoreRegsInMemory;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x0065: // FX65
                    insn.opcode = OP_GetRegsFromMemory;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return insn;
};

void executeInstruction(struct instruction insn, struct cpu* cpu, SDL_Surface* surface)
{
    switch(insn.opcode)
    {
        case OP_Return: // 00EE
            cpu->sp--;
            cpu->pc = cpu->stack[cpu->sp];
            break;
        case OP_Jump: // 1NNN
            cpu->pc = insn.arg1-2;
            break;
        case OP_Subroutine: // 2NNN
            cpu->stack[cpu->sp] = cpu->pc;
            cpu->sp++;
            cpu->pc = insn.arg1-2;
            break;
        case OP_SkipIfEqualsImmediate: // 3XNN
            if (cpu->registers[insn.arg1] == insn.arg2)
            {
                cpu->pc += 2;
            }
            break;
        case OP_SkipIfDoesNotEqualImmediate: // 4XNN
            if (!(cpu->registers[insn.arg1] == insn.arg2))
            {
                cpu->pc += 2;
            }
            break;
        case OP_SkipIfEqualsReg: // 5XY0
            if (cpu->registers[insn.arg1] == cpu->registers[insn.arg2])
            {
                cpu->pc += 2;
            }
            break;
        case OP_SetRegToImmediate: // 6XNN
            cpu->registers[insn.arg1] = insn.arg2;
            break;
        case OP_AddImmediateToReg: // 7XNN
            cpu->registers[insn.arg1] += insn.arg2;
            break;
        case OP_SetRegValToReg: // 8XY0
            cpu->registers[insn.arg1] = cpu->registers[insn.arg2];
            break;
        case OP_RegORReg: // 8XY1
            cpu->registers[insn.arg1] = cpu->registers[insn.arg1] | cpu->registers[insn.arg2];
            break;
        case OP_RegANDReg: // 8XY2
            cpu->registers[insn.arg1] = cpu->registers[insn.arg1] & cpu->registers[insn.arg2];
            break;
        case OP_RegXORReg: // 8XY3
            cpu->registers[insn.arg1] = cpu->registers[insn.arg1] ^ cpu->registers[insn.arg2];
            break;
        case OP_AddRegValToReg: { // 8XY4
            uint16_t result = cpu->registers[insn.arg1] + cpu->registers[insn.arg2];
            if (result > 255)
            {
                cpu->registers[15] = 1;
            }
            else
            {
                cpu->registers[15] = 0;
            }
            cpu->registers[insn.arg1] = (result & 0xFF);
            break;
        }
        case OP_SubRegValFromReg: // 8XY5
            if (cpu->registers[insn.arg1] > cpu->registers[insn.arg2])
            {
                cpu->registers[15] = 1;
            }
            else
            {
                cpu->registers[15] = 0;
            }
            cpu->registers[insn.arg1] -= cpu->registers[insn.arg2];
            break;
        case OP_StoreLeastSignifAndShiftRight: // 8XY6
            if ((cpu->registers[insn.arg1] & 1) == 1)
            {
                cpu->registers[15] = 1;
            }
            else
            {
                cpu->registers[15] = 0;
            }
            cpu->registers[insn.arg1] /= 2;
            break;
        case OP_SetRegToSubRegValFromReg: // 8XY7
            if (cpu->registers[insn.arg2] > cpu->registers[insn.arg1])
            {
                cpu->registers[15] = 1;
            }
            else
            {
                cpu->registers[15] = 0;
            }
            cpu->registers[insn.arg1] = cpu->registers[insn.arg2] - cpu->registers[insn.arg1];
            break;
        case OP_StoreMostSignifAndShiftLeft: // 8XYE
            if ((cpu->registers[insn.arg1] >> 7) == 1)
            {
                cpu->registers[15] = 1;
            }
            else
            {
                cpu->registers[15] = 0;
            }
            cpu->registers[insn.arg1] *= 2;
            break;
        case OP_SkipIfRegDoesNotEqualReg: // 9XY0
            if (!(cpu->registers[insn.arg1] == cpu->registers[insn.arg2]))
            {
                cpu->pc += 2;
            }
            break;
        case OP_SetIToImmediate: // ANNN
            cpu->I = insn.arg1;
            break;
        case OP_JumpToImmediatePlusV0: // BNNN
            cpu->pc = (insn.arg1 + cpu->registers[0] - 2);
            break;
        case OP_SetRegToRandANDImmediate: { // CXNN
            uint8_t random_number = rand() % 256;
            cpu->registers[insn.arg1] = random_number & insn.arg2;
            break;
        }
        // TODO: fix Windows support
        case OP_DrawSpriteAtCoords: // DXYN
            SDL_LockSurface(surface);
            int x = cpu->registers[insn.arg1];
            int y = cpu->registers[insn.arg2];
            int n = insn.arg3;

            for (int i = 0; i < n; i++)
            {
                int line = cpu->ram[cpu->I+i];
                int k = 7;
                for (int j = 0; j < 8; j++)
                {
                    if ((line & (1 << (k - j))) > 0)
                    {
                        togglePixelAt(y+i, x+j, surface);
                    }
                }
            }

            SDL_UnlockSurface(surface);
            break;
        case OP_SkipIfKeyPressed: { // EX9E
            const uint8_t* state = SDL_GetKeyboardState(NULL);
            switch (insn.arg1)
            {
                case 0x0:
                    if (state[SDL_SCANCODE_1] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x1:
                    if (state[SDL_SCANCODE_2] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x2:
                    if (state[SDL_SCANCODE_3] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x3:
                    if (state[SDL_SCANCODE_4] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x4:
                    if (state[SDL_SCANCODE_Q] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x5:
                    if (state[SDL_SCANCODE_W] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x6:
                    if (state[SDL_SCANCODE_E] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x7:
                    if (state[SDL_SCANCODE_R] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x8:
                    if (state[SDL_SCANCODE_A] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x9:
                    if (state[SDL_SCANCODE_S] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0xA:
                    if (state[SDL_SCANCODE_D] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0xB:
                    if (state[SDL_SCANCODE_F] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0xC:
                    if (state[SDL_SCANCODE_Z] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0xD:
                    if (state[SDL_SCANCODE_X] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0xE:
                    if (state[SDL_SCANCODE_C] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0xF:
                    if (state[SDL_SCANCODE_V] == KEY_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
            }
            break;
        }
        case OP_SkipIfKeyNotPressed: { // EXA1
            const uint8_t* state = SDL_GetKeyboardState(NULL);
            switch (insn.arg1)
            {
                case 0x0:
                    if (state[SDL_SCANCODE_1] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x1:
                    if (state[SDL_SCANCODE_2] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x2:
                    if (state[SDL_SCANCODE_3] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x3:
                    if (state[SDL_SCANCODE_4] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x4:
                    if (state[SDL_SCANCODE_Q] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x5:
                    if (state[SDL_SCANCODE_W] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x6:
                    if (state[SDL_SCANCODE_E] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x7:
                    if (state[SDL_SCANCODE_R] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x8:
                    if (state[SDL_SCANCODE_A] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0x9:
                    if (state[SDL_SCANCODE_S] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0xA:
                    if (state[SDL_SCANCODE_D] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0xB:
                    if (state[SDL_SCANCODE_F] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0xC:
                    if (state[SDL_SCANCODE_Z] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0xD:
                    if (state[SDL_SCANCODE_X] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0xE:
                    if (state[SDL_SCANCODE_C] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
                case 0xF:
                    if (state[SDL_SCANCODE_V] == KEY_NOT_PRESSED)
                    {
                        cpu->pc += 2;
                    }
                    break;
            }
            break;
        }
        case OP_SetRegValFromDelayTimer: // FX07
            cpu->registers[insn.arg1] = cpu->timers[0];
            break;
        case OP_SetRegValToKeyPressed: { // FX0A
            const uint8_t* state = SDL_GetKeyboardState(NULL);
            int keycodes[16] = {SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F, SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V};
            while (1)
            {
                for (int i = 0; i < 16; i++)
                {
                    if (state[keycodes[i]] == KEY_PRESSED)
                    {
                        cpu->registers[insn.arg1] = i;
                        goto exit;
                    }
                }
                SDL_PumpEvents();
            }
            exit:
            break;
        }
        case OP_SetDelayTimerFromRegVal: // FX15
            cpu->timers[0] = cpu->registers[insn.arg1];
            break;
        case OP_SetSoundTimerFromRegVal: // FX18
            cpu->timers[1] = cpu->registers[insn.arg1];
            break;
        case OP_AddRegToI: // FX1E
            cpu->I += cpu->registers[insn.arg1];
            break;
        case OP_SetIToSpriteLocationInReg: // FX29
            switch(insn.arg1)
            {
                case 0x0:
                    cpu->I = SPRITE_ZERO;
                    break;
                case 0x1:
                    cpu->I = SPRITE_ONE;
                    break;
                case 0x2:
                    cpu->I = SPRITE_TWO;
                    break;
                case 0x3:
                    cpu->I = SPRITE_THREE;
                    break;
                case 0x4:
                    cpu->I = SPRITE_FOUR;
                    break;
                case 0x5:
                    cpu->I = SPRITE_FIVE;
                    break;
                case 0x6:
                    cpu->I = SPRITE_SIX;
                    break;
                case 0x7:
                    cpu->I = SPRITE_SEVEN;
                    break;
                case 0x8:
                    cpu->I = SPRITE_EIGHT;
                    break;
                case 0x9:
                    cpu->I = SPRITE_NINE;
                    break;
                case 0xA:
                    cpu->I = SPRITE_A;
                    break;
                case 0xB:
                    cpu->I = SPRITE_B;
                    break;
                case 0xC:
                    cpu->I = SPRITE_C;
                    break;
                case 0xD:
                    cpu->I = SPRITE_D;
                    break;
                case 0xE:
                    cpu->I = SPRITE_E;
                    break;
                case 0xF:
                    cpu->I = SPRITE_F;
                    break;
                default:
                    fprintf(stderr, "error\n");
                    break;
            }
            break;
        case OP_SetBCDOfReg: // FX33
            cpu->ram[cpu->I] = cpu->registers[insn.arg1] / 100;
            cpu->ram[cpu->I+1] = (cpu->registers[insn.arg1] / 10) % 10;
            cpu->ram[cpu->I+2] = cpu->registers[insn.arg1] % 10;
            break;
        case OP_StoreRegsInMemory: // FX55
            for (int i = 0; i <= insn.arg1; i++)
            {
                cpu->ram[cpu->I+i] = cpu->registers[i];
            }
            break;
        case OP_GetRegsFromMemory: // FX65
            for (int i = 0; i <= insn.arg1; i++)
            {
                cpu->registers[i] = cpu->ram[cpu->I+i];
            }
            break;
        default:
            break;
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
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

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL could not intialise! Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 640, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        fprintf(stderr, "Window could not be created! Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL)
    {
        fprintf(stderr, "Renderer could not be created! Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_RenderSetLogicalSize(renderer, 64, 32);
    screenSurface = SDL_CreateRGBSurface(0, 64, 32, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    if (screenSurface == NULL)
    {
        fprintf(stderr, "Surface could not be created! Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
    while (cpu.pc < 4096)
    {
        fprintf(stdout, "PC: %d\n", cpu.pc);
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
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
