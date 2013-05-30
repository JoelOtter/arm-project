#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

uint32_t generateMask(uint32_t length){

    uint32_t mask = 0;

    for(uint32_t i = 0; i < length; i ++){

        mask <<= 1;
        mask += 1;

    }
    return mask;
}

void printBits(uint32_t x) {
    
    int i;

    uint32_t mask = 1 << 31;

    for(i=0; i<32; ++i) {
        if((x & mask) == 0){
            printf("0");
        }else {
            printf("1");
        }
        mask >>= 1;
    }

    printf("\n");
}

int SignExtend(int value, int length){

    uint32_t sign = value >> (length - 1);    //Gets most significant bit, the sign!!
    uint32_t extension = 32 - length;    

    if (sign == 1) {

        uint32_t mask = 0;

        for (uint32_t i = 0; i < extension; ++i)
        {
          mask <<= 1;
          mask += 1;
        }  
        mask <<= length;
        value |= mask;
    }
    return value;

}


int getOffset(uint32_t instruction){

    // We want to increase PC by the amount specified by the offset.
    // We want to add to the value of the PC when the instruction was fetched
    // i.e we have to add x amount of bytes to the address of the branch instruction.
    // The PC is 8 bytes ahead from when the instruction was called so we must subtract 8 
    // from whatever the offset is.
    
    uint32_t mask24 = generateMask(24); //Generate's mask of length 24; bit 23- bit0
    int offset = instruction & mask24;  //Get mask
    offset =SignExtend(offset, 24);    
    offset <<= 2;                       //Shift offset to the left by 2.
    offset -= 8;                        // Subtract 8 from the offset.
    
    return offset; 
     
}

int main(int argc, char **argv){

    uint32_t instruction = atoi(argv[1]);
    
    printf("Instruction = "); printBits(instruction);
    printf("Offset =      "); printBits(getOffset(instruction));

}
