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
        x = x << 1;
    }

    printf("\n");
}

uint32_t rotateRight(uint32_t degree, uint8_t value){

    assert(degree <= 30);

    /* It's easiest to understand if you work one out on paper first

        e.g

        Operand2 = 1111 00001111 : 1111 = rotate, 0000 1111 = immediate value

        1. Work out the real rotate amount.
            - 1111 = 30
            - 30 mod 8 (size of the immediate) = 6 = shift_amount ; A rotate of 30 is the same as a rotate of 6

        2. 'cut out' bits that will be lost in a shit. These bits will be the bottom 6 bits, same as the shift amount above.
            - immediate = 00001111, bits to cut  = 001111.
            - create mask 111111 to cut bits out. 111111 = (2^6) - 1; (2^shiftamount)  1
            - cut by performing bitwise & with mask = 00001111 & 00111111 = 001111 = cut_bits

        3. shift the remaing bits by the shift_amount.
            - immediate = immediate >> shift_amount
            - 00001111 >> 6  = 00000000

        4. paste the cutbits back in at the appropriate place. 
            - 00001111 rotated right by 6 = 00111100, so can see actually = cutbits << 2
              cut_bits = cut_bits << 8 - 6 ; cut_bits = cut_bits << size of immediate - shift_amount

        5. result is bit wise or of the cut_bits and the immedate we shifted in step 3



    */
    uint32_t shift_amount = degree % 8; 
    // Creates a mask of 1's that's as long as the bits that need to be copied
    uint8_t mask = pow(2, shift_amount) - 1;  
    // Cuts out the bits that will be lost in the shift
    uint8_t cut_bits = value & mask; 
    // shifts the remaining bits the appropriate amount
    uint8_t remaining_bits = value >> shift_amount;
     // positions the cut bits correctly
    cut_bits = cut_bits << (8 - shift_amount);

    return (cut_bits | remaining_bits);    
}

int main(void){

    uint32_t operand2 = 3529;
    uint32_t answer = 39;
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