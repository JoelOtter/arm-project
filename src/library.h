#ifndef LIB_H
#define LIB_H

#include <string.h>

extern const uint32_t mask1; 
extern const uint32_t mask2; 
extern const uint32_t mask4;  
extern const uint32_t mask5; 
extern const uint32_t mask8;  

extern int carryout;

char *get_mnemonic(char *instruction);

char *get_rest(char *instruction);

void printBits(uint32_t x);

uint32_t generateMask(uint32_t length);

uint32_t logShiftLeft(uint32_t value, uint32_t scale);

uint32_t logShiftRight(uint32_t value, uint32_t scale);

uint32_t arithShiftRight(uint32_t value, uint32_t scale);

uint32_t rotateRight(uint8_t rotate, uint32_t value);

int signExtend(int value, int length);

uint32_t getFromMemory(unsigned char *memory, int start);

void writeToMemory(unsigned char *memory, int start, uint32_t value);

void printRegisters(int *registers);

int regFromString(char *rstring);

int hasComma(char *address);

int isImmediate(char *rstring);

int isElemOf(char *searchString, char * list[] );

#endif
