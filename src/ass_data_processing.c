#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "library.h"
#include "ass_data_processing.h"
#define error "Not acceptable number"

enum dataInstructionType {RESULT,
                          SINGLE_OPERAND,
                          CPSR};

char * resultInstructions[] = { "add", "eor", "sub", "rsb", "add", "orr" };

uint32_t cond;
uint32_t I;
uint32_t opcode;
uint32_t S = 0;
uint32_t RnI;
uint32_t RdI;
uint32_t rotation; // for operand2i
uint32_t operand2I;
char Rn[2];
char Rd[2];
char Rm[2];
char operand2[15];


enum dataInstructionType getType(char *mnemonic) {

    enum dataInstructionType inst;

    char first = mnemonic[0];

    if (first == 'm') {
        inst = SINGLE_OPERAND;
    } else if ( first == 't' || first == 'c' )   {
        inst = CPSR;
    } else if (isElemOf(mnemonic, resultInstructions)) {
        inst = RESULT;
    }

    return inst;
}


uint32_t getOpcode(char *mnemonic) {

    uint32_t opcode;

    char first = mnemonic[0];
    char second = mnemonic[1];

    switch(first) {
        case('a'): (second == 'n') ? (opcode = 0) : (opcode = 4); break;
        case('e'): opcode = 1;  break;
        case('s'): opcode = 2;  break;
        case('r'): opcode = 3;  break;
        case('o'): opcode = 12; break;
        case('m'): opcode = 13; break;
        case('t'): (second == 's') ? (opcode = 8) : (opcode = 9); break;
        case('c'): opcode = 10; break;
    }

    return opcode; 
}


int getNumZeros(uint32_t n) {
    int numZeros = 0;
    int firstOne = 0; 
    if ( n == 0 ) {
        return 32;
    } else {  
        while (!firstOne) {
            numZeros++;
            firstOne = n & 1;
            n >>= 1;
        }
        numZeros--;
        return numZeros;
    }
}


int withinKBits(uint32_t n, int k) {

   int numZeros = getNumZeros(n);
   numZeros++;
   n >>= numZeros;
   uint32_t mask = (((1 << (32-numZeros)) - 1) << (k-1));
   return !((mask & n) > 0 );
}


int isValidNumber(uint32_t number) {

    int numZeros = getNumZeros(number);
    return ((withinKBits(number, 8)) && !((numZeros & 1) && !(withinKBits(number, 7))));

//if its not within 8bits then you wont be able to accurately reprsent it. if it has 8 bits but is in an ugly position 0001 1111 1110, it cant be used either because we can only shift by x2 multiple.
}


uint32_t calculateRotation(uint32_t number) {

    assert(isValidNumber(number));
    uint32_t rotation;    
    int32_t numZeros = getNumZeros(number);
    if ( number < 256 ) {
        rotation = 0;
    } else {
        if ( numZeros & 1 ) numZeros = numZeros-1;
        rotation = numZeros; // DIVIDE ME BY TWO 
    }
    return rotation;

}


uint32_t getOperand2(char *operand2) {

    char first = operand2[0];
    uint32_t operand2I;

    if ( first == '#' ) {
        I = 1;

        if (operand2[2] == 'x') { //hex
            operand2 = operand2+3;
            operand2I = strtol(operand2, NULL, 16);
        } else { //dec
            operand2 = operand2+1;
            operand2I = atoi(operand2);
        }

        if ( isValidNumber(operand2I)) {
            if (! (operand2I < 256) ) {
                uint32_t rotation = calculateRotation(operand2I);
                operand2I = rotateRight(rotation, operand2I);  
                operand2I = operand2I | (((32 - rotation) >> 1) << 8 );
            }
        } else {
            operand2I = 0;
            printf("Error: numeric constant cannot be represented\n");
        }

    } else {
        I = 0;
        //no shift implemented yet
        sscanf(operand2, "%s", Rm);
        operand2I = regFromString(Rm);

    }

    return operand2I;

}


void executeResultInst(char *mnemonic, char *rest) {
    sscanf(rest, "%[^','],%[^','],%s", Rd, Rn, operand2);
    RdI = regFromString(Rd);
    RnI = regFromString(Rn);
}


void executeSingleOperandInst(char *mnemonic, char *rest) {
    sscanf(rest, "%[^','],%s", Rd, operand2);
    RnI = 0;
    RdI = regFromString(Rd);
}


void executeCPSRInst(char *mnemonic, char *rest) {
    S = 1;
    sscanf(rest, "%[^','],%s", Rn, operand2);
    RnI = regFromString(Rn);
    RdI = 0;
}


uint32_t ass_data_processing(char *instruction) {

    char *mnemonic = get_mnemonic(instruction);
    char *rest = get_rest(instruction);

    uint32_t result;

    enum dataInstructionType type = getType(mnemonic);
    switch(type) {
            case(RESULT): 
                executeResultInst(mnemonic, rest);
                //printf("result\n");
                break;
            case(SINGLE_OPERAND): 
                executeSingleOperandInst(mnemonic, rest);
                //printf("single\n");
                break;
            case(CPSR): 
                executeCPSRInst(mnemonic, rest);
                //printf("cpsr\n");
                break;
    }    

    cond = 14;
    opcode = getOpcode(mnemonic);
    operand2I = getOperand2(operand2);

    result = (cond << 28) | (I << 25) | (opcode << 21) | (S << 20) | (RnI << 16) | (RdI << 12) | operand2I ;
 

    /*
    //Content of all the sections for testing
    printf("\n\nContents of instruction: \n");
    printf("cond: %d\n", cond);
    printf("I: %d\n", I);
    printf("opcode: %d\n", opcode);
    printf("S: %d\n", S);
    printf("RnI: %d\n", RnI);
    printf("RdI: %d\n", RdI);
    printf("operand2: %d\n", operand2I);
    printf("instruction: %x\n\n", result);
    */
    
    return result;

}






