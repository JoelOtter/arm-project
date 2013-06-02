#include <stdlib.h>
#include <stdint.h>
#include "library.h"
#include "branch.h"
#include "emulate.h"

static int getOffset(uint32_t instruction){

    // We want to increase PC by the amount specified by the offset.
    // We want to add to the value of the PC when the instruction was fetched
    // i.e we have to add x amount of bytes to the address of the branch instruction.
    // The PC is 8 bytes ahead from when the instruction was called so we must subtract 8 
    // from whatever the offset is.
    
    uint32_t mask24 = generateMask(24); //Generate's mask of length 24; bit 23- bit0
    int offset = instruction & mask24;  //Get mask
    offset = SignExtend(offset, 24);    
    offset <<= 2;                       //Shift offset to the left by 2.
    offset -= 8;                        // Subtract 8 from the offset.
    
    return offset; 
     
}

void branch(uint32_t instruction){
    registers[15] += getOffset(instruction);
}
