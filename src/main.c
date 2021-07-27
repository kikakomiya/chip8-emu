#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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

struct instruction getInstruction(uint16_t opcode)
{
    struct instruction insn;
    switch (opcode & 0xF000)
    {
        case 0x0000:
            if (opcode == 0x00E0)
            {
                fprintf(stdout, "opcode 00E0 - OP_ClearScreen\n");
                insn.opcode = OP_ClearScreen;
            }
            else if (opcode == 0x00EE)
            {
                fprintf(stdout, "opcode 00EE - OP_Return\n");
                insn.opcode = OP_Return;
            }
            break;
        case 0x1000:
            fprintf(stdout, "opcode 1NNN - OP_Jump\n");
            insn.opcode = OP_Jump;
            insn.arg1 = opcode & 0x0FFF;
            break;
        case 0x2000:
            fprintf(stdout, "opcode 2NNN - OP_Subroutine\n");
            insn.opcode = OP_Subroutine;
            insn.arg1 = opcode & 0x0FFF;
            break;
        case 0x3000:
            fprintf(stdout, "opcode 3XNN - OP_SkipIfEqualsImmediate\n");
            insn.opcode = OP_SkipIfEqualsImmediate;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = opcode & 0x00FF;
            break;
        case 0x4000:
            fprintf(stdout, "opcode 4XNN - OP_SkipIfDoesNotEqualImmediate\n");
            insn.opcode = OP_SkipIfDoesNotEqualImmediate;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = opcode & 0x00FF;
            break;
        case 0x5000:
            fprintf(stdout, "opcode 5XY0 - OP_SkipIfEqualsReg\n");
            insn.opcode = OP_SkipIfEqualsReg;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = (opcode & 0x00F0) >> 4;
            break;
        case 0x6000:
            fprintf(stdout, "opcode 6XNN - OP_SetRegToImmediate\n");
            insn.opcode = OP_SetRegToImmediate;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = opcode & 0x00FF;
            break;
        case 0x7000:
            fprintf(stdout, "opcode 7XNN - OP_AddImmediateToReg\n");
            insn.opcode = OP_AddImmediateToReg;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = opcode & 0x00FF;
            break;
        case 0x8000: {
            switch (opcode & 0x000F)
            {
                case 0x0000:
                    fprintf(stdout, "opcode 8XY0 - OP_SetRegValToReg\n");
                    insn.opcode = OP_SetRegValToReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0001:
                    fprintf(stdout, "opcode 8XY1 - OP_RegORReg\n");
                    insn.opcode = OP_RegORReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0002:
                    fprintf(stdout, "opcode 8XY2 - OP_RegANDReg\n");
                    insn.opcode = OP_RegANDReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0003:
                    fprintf(stdout, "opcode 8XY3 - OP_RegXORReg\n");
                    insn.opcode = OP_RegXORReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0004:
                    fprintf(stdout, "opcode 8XY4 - OP_AddRegValToReg\n");
                    insn.opcode = OP_AddRegValToReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0005:
                    fprintf(stdout, "opcode 8XY5 - OP_SubRegValFromReg\n");
                    insn.opcode = OP_SubRegValFromReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0006:
                    fprintf(stdout, "opcode 8XY6 - OP_StoreLeastSignifAndShiftRight\n");
                    insn.opcode = OP_StoreLeastSignifAndShiftRight;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x0007:
                    fprintf(stdout, "opcode 8XY7 - OP_SetRegToSubRegValFromReg\n");
                    insn.opcode = OP_SetRegToSubRegValFromReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x000E:
                    fprintf(stdout, "opcode 8XYE - OP_StoreMostSignifAndShiftLeft\n");
                    insn.opcode = OP_StoreMostSignifAndShiftLeft;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                default:
                    fprintf(stderr, "error\n");
                    break;
            }
            break;
        }
        case 0x9000:
            fprintf(stdout, "opcode 9XY0 - OP_SkipIfRegDoesNotEqualReg\n");
            insn.opcode = OP_SkipIfRegDoesNotEqualReg;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = (opcode & 0x00F0) >> 4;
            break;
        case 0xA000:
            fprintf(stdout, "opcode ANNN - OP_SetIToImmediate\n");
            insn.opcode = OP_SetIToImmediate;
            insn.arg1 = opcode & 0x0FFF;
            break;
        case 0xB000:
            fprintf(stdout, "opcode BNNN - OP_JumpToImmediatePlusV0\n");
            insn.opcode = OP_JumpToImmediatePlusV0;
            insn.arg1 = opcode & 0x0FFF;
            break;
        case 0xC000:
            fprintf(stdout, "opcode CXNN - OP_SetRegToRandANDImmediate\n");
            insn.opcode = OP_SetRegToRandANDImmediate;
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = opcode & 0x00FF;
            break;
        case 0xD000:
            fprintf(stdout, "opcode DXYN - OP_DrawSpriteAtCoords\n");
            insn.arg1 = (opcode & 0x0F00) >> 8;
            insn.arg2 = (opcode & 0x00F0) >> 4;
            insn.arg3 = opcode & 0x000F;
            break;
        case 0xE000:
            if ((opcode & 0xF0FF) == 0xE09E)
            {
                fprintf(stdout, "opcode EX9E - OP_SkipIfKeyPressed\n");
                insn.opcode = OP_SkipIfKeyPressed;
                insn.arg1 = (opcode & 0x0F00) >> 8;
            }
            else if ((opcode & 0xF0FF) == 0xE0A1)
            {
                fprintf(stdout, "opcode EXA1 - OP_SkipIfKeyNotPressed\n");
                insn.opcode = OP_SkipIfKeyNotPressed;
                insn.arg1 = (opcode & 0x0F00) >> 8;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x0007:
                    fprintf(stdout, "opcode FX07 - OP_SetRegValFromDelayTimer\n");
                    insn.opcode = OP_SetRegValFromDelayTimer;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x000A:
                    fprintf(stdout, "opcode FX0A - OP_SetRegValToKeyPressed\n");
                    insn.opcode = OP_SetRegValToKeyPressed;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x0015:
                    fprintf(stdout, "opcode FX15 - OP_SetDelayTimerFromRegVal\n");
                    insn.opcode = OP_SetDelayTimerFromRegVal;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x0018:
                    fprintf(stdout, "opcode FX18 - OP_SetSoundTimerFromRegVal\n");
                    insn.opcode = OP_SetSoundTimerFromRegVal;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x001E:
                    fprintf(stdout, "opcode FX1E - OP_AddRegToI\n");
                    insn.opcode = OP_AddRegToI;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x0029:
                    fprintf(stdout, "opcode FX29 - OP_SetIToSpriteLocationInReg\n");
                    insn.opcode = OP_SetIToSpriteLocationInReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x0033:
                    fprintf(stdout, "opcode FX33 - OP_SetBCDOfReg\n");
                    insn.opcode = OP_SetBCDOfReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x0055:
                    fprintf(stdout, "opcode FX55 - OP_StoreRegsInMemory\n");
                    insn.opcode = OP_StoreRegsInMemory;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                case 0x0065:
                    fprintf(stdout, "opcode FX65 - OP_GetRegsFromMemory\n");
                    insn.opcode = OP_GetRegsFromMemory;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    break;
                default:
                    fprintf(stderr, "error\n");
                    break;
            }
            break;
        default:
            fprintf(stderr, "error\n");
            break;
    }
    return insn;
};

void executeInstruction(struct instruction insn, struct cpu* cpu)
{
    switch(insn.opcode)
    {
        case OP_Return: // 00EE
            fprintf(stdout, "OP_Return instruction executed!\n");
            cpu->sp--;
            cpu->pc = cpu->stack[cpu->sp];
            break;
        case OP_Jump: // 1NNN
            fprintf(stdout, "OP_Jump instruction with argument %x executed!\n", insn.arg1);
            cpu->pc = insn.arg1;
            break;
        case OP_Subroutine: // 2NNN
            fprintf(stdout, "OP_Subroutine instruction with argument %x executed!\n", insn.arg1);
            cpu->stack[cpu->sp] = cpu->pc;
            cpu->sp++;
            cpu->pc = insn.arg1;
            break;
        case OP_SkipIfEqualsImmediate: // 3XNN
            fprintf(stdout, "OP_SkipIfEqualsImmediate instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
            if (cpu->registers[insn.arg1] == insn.arg2)
            {
                cpu->pc += 2;
            }
            break;
        case OP_SkipIfDoesNotEqualImmediate: // 4XNN
            fprintf(stdout, "OP_SkipIfDoesNotEqualImmediate instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
            if (!(cpu->registers[insn.arg1] == insn.arg2))
            {
                cpu->pc += 2;
            }
            break;
        case OP_SkipIfEqualsReg: // 5XY0
            fprintf(stdout, "OP_SkipIfEqualsReg instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
            if (cpu->registers[insn.arg1] == cpu->registers[insn.arg2])
            {
                cpu->pc += 2;
            }
            break;
        case OP_SetRegToImmediate: // 6XNN
            fprintf(stdout, "OP_SetRegToImmediate instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
            cpu->registers[insn.arg1] = insn.arg2;
            break;
        case OP_AddImmediateToReg: // 7XNN
            fprintf(stdout, "OP_AddImmediateToReg instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
            cpu->registers[insn.arg1] += insn.arg2;
            break;
        case OP_SetRegValToReg: // 8XY0
            fprintf(stdout, "OP_AddRegValToReg instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
            cpu->registers[insn.arg1] = cpu->registers[insn.arg2];
            break;
        case OP_RegORReg: // 8XY1
            fprintf(stdout, "OP_RegORReg instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
            cpu->registers[insn.arg1] = cpu->registers[insn.arg1] | cpu->registers[insn.arg2];
            break;
        case OP_RegANDReg: // 8XY2
            fprintf(stdout, "OP_RegANDReg instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
            cpu->registers[insn.arg1] = cpu->registers[insn.arg1] & cpu->registers[insn.arg2];
            break;
        case OP_RegXORReg: // 8XY3
            fprintf(stdout, "OP_RegXORReg instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
            cpu->registers[insn.arg1] = cpu->registers[insn.arg1] ^ cpu->registers[insn.arg2];
            break;
        case OP_AddRegValToReg: // 8XY4
            fprintf(stdout, "OP_AddRegValToReg instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
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
        case OP_SubRegValFromReg: // 8XY5
            fprintf(stdout, "OP_SubRegValFromReg instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
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
            fprintf(stdout, "OP_StoreLeastSignifAndShiftRight instruction with argument %x executed!\n", insn.arg1);
            if ((cpu->registers[insn.arg1] & 1) == 1)
            {
                fprintf(stdout, "OP_StoreLeastSignifAndShiftRight: Least significant bit equals 1.\n");
                cpu->registers[15] = 1;
            }
            else
            {
                fprintf(stdout, "OP_StoreLeastSignifAndShiftRight: Least significant bit does not equal 1.\n");
                cpu->registers[15] = 0;
            }
            cpu->registers[insn.arg1] /= 2;
            break;
        case OP_SetRegToSubRegValFromReg: // 8XY7
            fprintf(stdout, "OP_SetRegToSubRegValFromReg instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
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
            fprintf(stdout, "OP_StoreMostSignifAndShiftLeft instruction with argument %x executed!\n", insn.arg1);
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
            fprintf(stdout, "OP_SkipIfRegDoesNotEqualReg instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
            if (!(cpu->registers[insn.arg1] == cpu->registers[insn.arg2]))
            {
                cpu->pc += 2;
            }
            break;
        case OP_SetIToImmediate: // ANNN
            fprintf(stdout, "OP_SetIToImmediate instruction with argument %x executed!\n", insn.arg1);
            cpu->I = insn.arg1;
            break;
        case OP_JumpToImmediatePlusV0: // BNNN
            fprintf(stdout, "OP_JumpToImmediatePlusV0 instruction with argument %x executed!\n", insn.arg1);
            cpu->pc = (insn.arg1 + cpu->registers[0]);
            break;
        case OP_SetRegToRandANDImmediate: // CXNN
            fprintf(stdout, "OP_SetRegToRandANDImmediate instruction with arguments %x and %x executed!\n", insn.arg1, insn.arg2);
            uint8_t random_number = rand() % 256;
            cpu->registers[insn.arg1] = random_number & insn.arg2;
            break;
        // put DXYN here
        // put EX9E here
        // put EXA1 here
        case OP_SetRegValFromDelayTimer: // FX07
            fprintf(stdout, "OP_SetRegValFromDelayTimer instruction with argument %x executed!\n", insn.arg1);
            cpu->registers[insn.arg1] = cpu->timers[0];
            break;
        // put FX0A here
        case OP_SetDelayTimerFromRegVal: // FX15
            fprintf(stdout, "OP_SetDelayTimerFromRegVal instruction with argument %x executed!\n", insn.arg1);
            cpu->timers[0] = cpu->registers[insn.arg1];
            break;
        case OP_SetSoundTimerFromRegVal: // FX18
            fprintf(stdout, "OP_SetSoundTimerFromRegVal instruction with argument %x executed!\n", insn.arg1);
            cpu->timers[1] = cpu->registers[insn.arg1];
            break;
        case OP_AddRegToI: // FX1E
            fprintf(stdout, "OP_AddRegToI instruction with argument %x executed!\n", insn.arg1);
            cpu->I += cpu->registers[insn.arg1];
            break;
        case OP_SetIToSpriteLocationInReg: // FX29
            fprintf(stdout, "OP_SetIToSpriteLocationInReg instruction with argument %x executed!\n", insn.arg1);
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
            fprintf(stdout, "OP_SetBCDOfReg instruction with argument %x executed!\n", insn.arg1);
            cpu->ram[cpu->I] = cpu->registers[insn.arg1] / 100;
            cpu->ram[cpu->I+1] = (cpu->registers[insn.arg1] / 10) % 10;
            cpu->ram[cpu->I+2] = cpu->registers[insn.arg1] % 10;
            break;
        case OP_StoreRegsInMemory: // FX55
            fprintf(stdout, "OP_StoreRegsInMemory instruction with argument %x executed!\n", insn.arg1);
            for (int i = 0; i <= insn.arg1; i++)
            {
                cpu->ram[cpu->I+i] = cpu->registers[i];
            }
            break;
        case OP_GetRegsFromMemory: // FX65
            fprintf(stdout, "OP_GetRegsFromMemory instruction with argument %x executed!\n", insn.arg1);
            for (int i = 0; i <= insn.arg1; i++)
            {
                cpu->registers[i] = cpu->ram[cpu->I+1];
            }
            break;
        default:
            fprintf(stderr, "error\n");
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

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL could not intialise! Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    else
    {
        window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64, 32, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            fprintf(stderr, "Window could not be created! Error: %s\n", SDL_GetError());
            return EXIT_FAILURE;
        }
        else
        {
            screenSurface = SDL_GetWindowSurface(window);
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
            while (cpu.pc < 4096)
            {
                uint16_t raw_insn = ((uint16_t)cpu.ram[cpu.pc] << 8) | cpu.ram[cpu.pc+1];
                executeInstruction(getInstruction(raw_insn), &cpu);
            }
            SDL_UpdateWindowSurface(window);
            SDL_Delay(2000);
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
