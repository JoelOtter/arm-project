#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "sdatatrans.h"
#include "emulate.h"
#include "library.h"


static uint32_t getOperand2(uint32_t instruction) {
    //Operand2 is either immediate (in which case we just take the value and extend)
    //Or it is in a register in which case we have to apply shift operations

    //Pushes these sections to the first bits of a 32-bit int
    
    uint32_t result;

    uint32_t I = mask1 & (instruction >> 25); //gets the value of I alone

    if ( I != 1 ) {

        //Operand2 immediate

        uint32_t imm = mask8 & instruction;
        uint32_t rotate = (mask4 & (instruction >>  8)) << 1; 
        // shifted the instruction by 8 to get the rotate, applied the mask, then x2.
        result = rotateRight(rotate, imm);

    } else {

        //Operand2 register

        uint32_t shift = mask8 & (instruction >> 4);    // shift   instruction(11-4)
        uint32_t shiftType = mask2 & (shift >> 1); //  - shift type shift(6 -5)
        uint32_t scale   = mask5 & (shift >> 3); //  - integer    shift(11-7)
        uint32_t rm    = mask4 & instruction;      // rm      instruction(3 -0)
        uint32_t rmValue = registers[rm];
        switch(shiftType) {
            case(0): 
                result = logShiftLeft(rmValue, scale);
                break;
            case(1):
                result = logShiftRight(rmValue, scale);
                break;
            case(2):
                result = arithShiftRight(rmValue, scale);
                break;
            case(3):
                result = rotateRight(rmValue, scale);
                break;
        }

    }

    return result;
}

static void load(uint32_t P, uint32_t U, uint32_t offset, uint32_t baseReg, uint32_t destReg){
    int32_t offsetNew = offset;
    if (!U) offsetNew = offsetNew * (-1);
    uint32_t regValue = registers[baseReg];
    if (P) regValue += offsetNew;
    //printf("0x%08x\n", regValue);
    if (regValue + 3 < SIZE_OF_MEMORY) registers[destReg] = getFromMemory(memory, regValue);
    else if (regValue == 0x20200000) {
        printf("One GPIO pin from 0 to 9 has been accessed\n");
        registers[destReg] = regValue;
    }
    else if (regValue == 0x20200004) {
        printf("One GPIO pin from 10 to 19 has been accessed\n"); 
        registers[destReg] = regValue;
    }
    else if (regValue == 0x20200008) {
        printf("One GPIO pin from 20 to 29 has been accessed\n");
        registers[destReg] = regValue;
    }
    else printf("Error: Out of bounds memory access at address 0x%08x\n", regValue);
    if (!P) registers[baseReg] += offsetNew;
}

static void store(uint32_t P, uint32_t U, uint32_t offset, uint32_t baseReg, uint32_t destReg){
    int32_t offsetNew = offset;
    if (!U) offsetNew = offsetNew * (-1);
    uint32_t regValue = registers[baseReg];
    if (P) regValue += offsetNew;
    if (regValue + 3 < SIZE_OF_MEMORY) writeToMemory(memory, regValue, registers[destReg]);
    else if (regValue == 0x20200000) printf("One GPIO pin from 0 to 9 has been accessed\n");
    else if (regValue == 0x20200004) printf("One GPIO pin from 10 to 19 has been accessed\n");
    else if (regValue == 0x20200008) printf("One GPIO pin from 20 to 29 has been accessed\n");
    else if (regValue == 0x20200028) printf("LED OFF!\n");
    else if (regValue == 0x2020001c) printf("LED ON!\n");
    else printf("Error: Out of bounds memory access at address 0x%08x\n", regValue);
    if (!P) registers[baseReg] += offsetNew;
}

void singleDataTransfer(uint32_t instruction){
    uint32_t P = ((instruction >> 24) & mask1);
    uint32_t U = ((instruction >> 23) & mask1);
    uint32_t L = ((instruction >> 20) & mask1);
    uint32_t baseReg = ((instruction >> 16) & mask4);
    uint32_t destReg = ((instruction >> 12) & mask4);
    uint32_t offset = getOperand2(instruction);
    
    if (L) load(P, U, offset, baseReg, destReg);
    else store(P, U, offset, baseReg, destReg);
}

