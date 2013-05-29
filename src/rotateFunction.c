#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

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

uint32_t rotateRight(uint8_t degree, uint32_t value){

    // degree has to be multiple of 2 between 0 and 30
    assert(degree <= 30 && degree % 2 == 0);

    // Creates a mask of 1's that's as long as the bits that need to be copied
    uint32_t mask = 0;
    for (int i = 0; i < degree; ++i)
    {
      mask <<= 1;
      mask += 1;
    }  
    // Cuts out the bits that will be lost in the shift
    uint32_t cut_bits = value & mask; 
    // shifts the remaining bits the appropriate amount
    uint32_t remaining_bits = value >> degree;
     // positions the cut bits correctly
    cut_bits <<= (32 - degree);
    
    uint32_t result = (cut_bits | remaining_bits);
    
//    carryout = result >> 31;

    return (result);     
}

int main(void){

    uint32_t operand2 = 3529;
    uint32_t answer = 114;
    uint32_t imm;
    uint32_t rotate;
    uint32_t mask8 = 255;
    uint32_t mask4 = 15;

    imm = mask8 & operand2;
    rotate = (mask4 & (operand2 >>  8)) << 1;
    printf("Operand2 = %d  : ", operand2); printBits(operand2);
    printf("Immediate =      : "); printBits(imm);
    printf("Rotate =         : "); printBits(rotate);

    printf("Answer should be : "); printBits(answer);
    printf("Answer is =      : "); printBits(rotateRight(rotate, imm));

    return 0;
}
