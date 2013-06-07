#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "library.h"

// All the masks are up here
const uint32_t mask1 =   1; // 0000 0001
const uint32_t mask2 =   3; // 0000 0011
const uint32_t mask4 =  15; // 0000 1111 
const uint32_t mask5 =  31; // 0001 1111
const uint32_t mask8 = 255; // 1111 1111 
const int SIZE_OF_MEMORY = 65536;
const int NUM_REGISTERS = 17;

int carryout;

char *get_mnemonic(char *instruction){
    char *result = malloc(30);
    sscanf(instruction, "%s", result);
    return result;
}

char *get_rest(char *instruction){
    char *result = malloc(5);
    sscanf(instruction, "%*s %[^\n]", result);
    return result;
}


uint32_t getFromMemory(unsigned char *memory, int start) {

    uint32_t p1 = memory[start+3] << 24;
    uint32_t p2 = memory[start+2] << 16;
    uint32_t p3 = memory[start+1] << 8;
    uint32_t p4 = memory[start];

    return (p1 | p2 | p3 | p4);
}

void writeToMemory(unsigned char *memory, int start, uint32_t value) {
    memory[start+3] = value >> 24;
    memory[start+2] = value >> 16;
    memory[start+1] = value >> 8;
    memory[start] = value;
}

void printRegisters(int *registers){

    printf("Registers:\n");
    printf("$0  : %10d (0x%08x)\n", registers[0],registers[0]);
    printf("$1  : %10d (0x%08x)\n", registers[1],registers[1]);
    printf("$2  : %10d (0x%08x)\n", registers[2],registers[2]);
    printf("$3  : %10d (0x%08x)\n", registers[3],registers[3]);
    printf("$4  : %10d (0x%08x)\n", registers[4],registers[4]);
    printf("$5  : %10d (0x%08x)\n", registers[5],registers[5]);
    printf("$6  : %10d (0x%08x)\n", registers[6],registers[6]);
    printf("$7  : %10d (0x%08x)\n", registers[7],registers[7]);
    printf("$8  : %10d (0x%08x)\n", registers[8],registers[8]);
    printf("$9  : %10d (0x%08x)\n", registers[9],registers[9]);
    printf("$10 : %10d (0x%08x)\n", registers[10],registers[10]);
    printf("$11 : %10d (0x%08x)\n", registers[11],registers[11]);
    printf("$12 : %10d (0x%08x)\n", registers[12],registers[12]);
    printf("PC  : %10d (0x%08x)\n", registers[15],registers[15]);
    printf("CPSR: %10d (0x%08x)\n", registers[16],registers[16]);
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

void printMemory(uint8_t x) {
    
    int i;

    uint8_t mask = 1 << 7;

    for(i=0; i<8; ++i) {
        if((x & mask) == 0){
            printf("0");
        }else {
            printf("1");
        }
        mask >>= 1;
    }

    printf("\n");
}

uint32_t generateMask(uint32_t length){

    uint32_t mask = 0;

    for(uint32_t i = 0; i < length; i ++){

        mask <<= 1;
        mask += 1;

    }
    return mask;
}

uint32_t logShiftLeft(uint32_t value, uint32_t scale) {

    carryout = (value >> (32 - scale)) & mask1;
    if (scale == 0) carryout = 0;
    value <<= scale;
    return value;

}

uint32_t logShiftRight(uint32_t value, uint32_t scale) {
    
    carryout = 0;
    if (scale > 0) {
    carryout = (value >> (scale - 1)) & mask1;
    }
    value >>= scale;
    return value;
}

uint32_t arithShiftRight(uint32_t value, uint32_t scale) {

    uint32_t sign = value >> 31;

    value = logShiftRight(value, scale);
    
    if (sign == 1) {
        uint32_t mask = 0;
        for (uint32_t i = 0; i < scale; ++i)
        {
          mask <<= 1;
          mask += 1;
        }  
        mask <<= 32-scale;
        value |= mask;
    }
    return value;
}

uint32_t rotateRight(uint8_t rotate, uint32_t value){

    // rotate has to be multiple of 2 and between 0 and 30
    assert(rotate <= 30 && rotate % 2 == 0);

    // Creates a mask of 1's that's as long as the bits that need to be copied
    uint32_t mask = 0;
    for (uint32_t i = 0; i < rotate; ++i)
    {
      mask <<= 1;
      mask += 1;
    }  
    // Cuts out the bits that will be lost in the shift
    uint32_t cut_bits = value & mask; 
    // shifts the remaining bits the appropriate amount
    uint32_t remaining_bits = value >> rotate;
     // positions the cut bits correctly
    cut_bits <<= (32 - rotate);
    
    uint32_t result = (cut_bits | remaining_bits);
    
    carryout = result >> 31;

    return (result);    
}

int signExtend(int value, int length){

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

int regFromString(char *rstring){
    if (rstring[0] == '['){
        return regFromString(&rstring[1]);
    }
    char value[5];
    sscanf(rstring, "r%s", value);
    return (atoi(value));
}

int isImmediate(char *rstring){
    return !(rstring[0] == '[');
}

int hasComma(char *address){
    int i = 0;
    while (address[i] != '\0'){
        if (address[i++] == ',') return 1;
    }
    return 0;
}

int has_sqb_before_comma(char *address){
    int hassqb = 0;
    int i = 0;
    while (address[i] != '\0'){
        if (address[i++] == ']'){
            hassqb = 1;
        }
        if (address[i] == ','){
            return hassqb;
        }
    }
    return 0;
}

int isElemOf(char *searchString, char *list[] , unsigned long int len) {

  //  int len = sizeof(list);  
    printf("%lu\n", len);
    int i;

    for(i = 0; i < len; ++i) {
        
        if(!strcmp(list[i], searchString)){
            return 1;
        }
    }

    return 0;
}

char* remove_leading_spaces(char *string){

    while(string[0] == ' '){
        string = &(string[1]);
    }
        
    return string;

}





