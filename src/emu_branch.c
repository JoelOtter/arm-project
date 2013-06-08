#include <stdlib.h>
#include <stdint.h>
#include "library.h"
#include "emu_branch.h"
#include "emulate.h"

static int get_offset(uint32_t instruction){

    // We want to increase PC by the amount specified by the offset.
    // We want to add to the value of the PC when the instruction was fetched
    // i.e we have to add x amount of bytes to the address of the branch instruction.
    // The PC is 8 bytes ahead from when the instruction was called so we must subtract 8 
    // from whatever the offset is.
    
    uint32_t mask24 = generate_mask(24); //Generate's mask of length 24; bit 23- bit0
    int offset = instruction & mask24;  //Get mask
    offset = sign_extend(offset, 24);    
    offset <<= 2;                       //Shift offset to the left by 2.
    
    return offset; 
     
}

void branch(uint32_t instruction){
    registers[15] += get_offset(instruction);
}
