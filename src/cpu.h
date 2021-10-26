#pragma once

#include <SDL2/SDL.h>
#include <stdint.h>

enum opcodes {
    OP_Call, // 0NNN
    OP_ClearScreen, // 00E0
    OP_Return, // 00EE
    OP_Jump, // 1NNN
    OP_Subroutine, // 2NNN
    OP_SkipIfEqualsImmediate, // 3XNN
    OP_SkipIfDoesNotEqualImmediate, // 4XNN
    OP_SkipIfEqualsReg, // 5XY0
    OP_SetRegToImmediate, // 6XNN
    OP_AddImmediateToReg, // 7XNN
    OP_SetRegValToReg, // 8XY0
    OP_RegORReg, // 8XY1
    OP_RegANDReg, // 8XY2
    OP_RegXORReg, // 8XY3
    OP_AddRegValToReg, // 8XY4
    OP_SubRegValFromReg, // 8XY5
    OP_StoreLeastSignifAndShiftRight, // 8XY6
    OP_SetRegToSubRegValFromReg, // 8XY7
    OP_StoreMostSignifAndShiftLeft,  // 8XYE
    OP_SkipIfRegDoesNotEqualReg, // 9XY0
    OP_SetIToImmediate, // ANNN
    OP_JumpToImmediatePlusV0, // BNNN
    OP_SetRegToRandANDImmediate, // CXNN
    OP_DrawSpriteAtCoords, // DXYN
    OP_SkipIfKeyPressed, // EX9E
    OP_SkipIfKeyNotPressed, // EXA1
    OP_SetRegValFromDelayTimer, // FX07
    OP_SetRegValToKeyPressed, // FX0A
    OP_SetDelayTimerFromRegVal, // FX15
    OP_SetSoundTimerFromRegVal, // FX18
    OP_AddRegToI, // FX1E
    OP_SetIToSpriteLocationInReg, // FX29
    OP_SetBCDOfReg, // FX33
    OP_StoreRegsInMemory, // FX55
    OP_GetRegsFromMemory // FX65
};

struct instruction {
    uint16_t opcode;
    uint16_t arg1;
    uint16_t arg2;
    uint16_t arg3;
};

struct cpu {
    uint8_t registers[16];
    uint16_t I;
    uint8_t ram[4096];
    uint8_t timers[2];
    uint16_t pc;
    uint8_t sp;
    uint16_t stack[16];
};

void initialiseCPU(struct cpu* cpu);
struct instruction getInstruction(uint16_t opcode);
void executeInstruction(struct instruction insn, struct cpu* cpu, SDL_Surface* surface);