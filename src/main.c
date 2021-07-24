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

void getInstruction(uint16_t opcode)
{
    switch (opcode & 0xF000)
    {
        case 0x0000:
            fprintf(stdout, "opcode starts with 0\n");
            break;
        case 0x1000:
            fprintf(stdout, "opcode 1NNN (OP_Jump) placeholder\n");
            break;
        case 0x2000:
            fprintf(stdout, "opcode starts with 2\n");
            break;
        case 0x3000:
            fprintf(stdout, "opcode starts with 3\n");
            break;
        case 0x4000:
            fprintf(stdout, "opcode starts with 4\n");
            break;
        case 0x5000:
            fprintf(stdout, "opcode starts with 5\n");
            break;
        case 0x6000:
            fprintf(stdout, "opcode starts with 6\n");
            break;
        case 0x7000:
            fprintf(stdout, "opcode starts with 7\n");
            break;
        case 0x8000: {
            struct instruction insn;
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
            fprintf(stdout, "opcode starts with 9\n");
            break;
        case 0xA000:
            fprintf(stdout, "opcode starts with A\n");
            break;
        case 0xB000:
            fprintf(stdout, "opcode starts with B\n");
            break;
        case 0xC000:
            fprintf(stdout, "opcode starts with C\n");
            break;
        case 0xD000:
            fprintf(stdout, "opcode starts with D\n");
            break;
        case 0xE000:
            fprintf(stdout, "opcode starts with E\n");
            break;
        case 0xF000:
            fprintf(stdout, "opcode starts with F\n");
            break;
        default:
            fprintf(stderr, "error\n");
            break;
    }
}

int main()
{
    getInstruction(0x8214);
    return EXIT_SUCCESS;
}