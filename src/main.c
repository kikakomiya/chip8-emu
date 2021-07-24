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
                    fprintf(stdout, "opcode 8XY7 - OP_SetRegToSubRegValFromReg");
                    insn.opcode = OP_SetRegToSubRegValFromReg;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                case 0x000E:
                    fprintf(stdout, "opcode 8XYE - OP_StoreMostSignifAndShiftLeft");
                    insn.opcode = OP_StoreMostSignifAndShiftLeft;
                    insn.arg1 = (opcode & 0x0F00) >> 8;
                    insn.arg2 = (opcode & 0x00F0) >> 4;
                    break;
                default:
                    fprintf(stderr, "error");
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
                fprintf(stdout, "opcode EX9E - OP_SkipIfKeyPressed");
                insn.opcode = OP_SkipIfKeyPressed;
                insn.arg1 = (opcode & 0x0F00) >> 8;
            }
            else if ((opcode & 0xF0FF) == 0xE0A1)
            {
                fprintf(stdout, "opcode EXA1 - OP_SkipIfKeyNotPressed");
                insn.opcode = OP_SkipIfKeyNotPressed;
                insn.arg1 = (opcode & 0x0F00) >> 8;
            }
            break;
        case 0xF000:
            fprintf(stdout, "opcode starts with F\n");
            break;
        default:
            fprintf(stderr, "error\n");
            break;
    }
    return insn;
};

int main()
{
    getInstruction(0x8214);
    return EXIT_SUCCESS;
}