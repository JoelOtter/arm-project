#ifndef LIB_H
#define LIB_H

extern const uint32_t mask1; 
extern const uint32_t mask2; 
extern const uint32_t mask4;  
extern const uint32_t mask5; 
extern const uint32_t mask8;  

extern int carryout;

void printBits(uint32_t x);

uint32_t generateMask(uint32_t length);

uint32_t logShiftLeft(uint32_t value, uint32_t scale);

uint32_t logShiftRight(uint32_t value, uint32_t scale);

uint32_t arithShiftRight(uint32_t value, uint32_t scale);

uint32_t rotateRight(uint8_t rotate, uint32_t value);

int SignExtend(int value, int length);

#endif
