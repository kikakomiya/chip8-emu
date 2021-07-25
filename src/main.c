#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "opcodes.h"

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
        case OP_Return:
            fprintf(stdout, "OP_Return instruction executed!\n");
            cpu->sp--;
            cpu->pc = cpu->stack[cpu->sp];
            break;
        case OP_Jump:
            fprintf(stdout, "OP_Jump instruction with argument %x executed!\n", insn.arg1);
            cpu->pc = insn.arg1;
            break;
        case OP_Subroutine:
            fprintf(stdout, "OP_Subroutine instruction with argument %x executed!\n", insn.arg1);
            cpu->stack[cpu->sp] = cpu->pc;
            cpu->sp++;
            cpu->pc = insn.arg1;
            break;
        case OP_SetIToImmediate:
            fprintf(stdout, "OP_SetIToImmediate instruction with argument %x executed!\n", insn.arg1);
            cpu->I = insn.arg1;
            break;
        case OP_JumpToImmediatePlusV0:
            fprintf(stdout, "OP_JumpToImmediatePlusV0 instruction with argument %x executed!\n", insn.arg1);
            cpu->pc = (insn.arg1 + cpu->registers[0]);
            break;
        default:
            fprintf(stderr, "error\n");
            break;
    }
}

int main()
{
    struct cpu cpu;

    executeInstruction(getInstruction(0x2345), &cpu);
    return EXIT_SUCCESS;
}
