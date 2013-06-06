#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "library.h"
#include "symboltableadt.h"
#include "assemble.h"

table instruction_table;

uint32_t strToHex(char *xStr){
    return strtol(&xStr[1], NULL, 0);
}

uint32_t doMov(uint32_t rd, uint32_t location){
    uint32_t result = 0;
    result += (14 << 28); //cond
    result += (1 << 25);  //I
    result += (13 << 21); //opcode for mov
    result += (rd << 12); //destination reg
    result += location;   //and the value
    return result;
}

uint32_t directRegister(uint32_t rd, uint32_t rn, int load){
    uint32_t result = 0;
    result += (14 << 28); //cond
    result += (3 << 25);  //01I
    result += (3 << 23);  //PU
    result += (load << 20);  //L
    result += (rn << 16); //Rn
    result += (rd << 12); //Rd
    return result;
}

uint32_t placeAtEnd(uint32_t rd, uint32_t value, int load){
    int numIn = 0;
    for (int i=0; add_afters[i] != 0; i+4){
        ++numIn;
    }
    add_afters[numIn] = value;
    uint32_t offset = ((&instruction_table)->size + numIn) * 4;
    uint32_t result = 0;
    result += (14 << 28); //cond
    result += (3 << 25);  //01I
    result += (3 << 23);  //PU
    result += (load << 20);  //L
    result += (15 << 16); //Rn
    result += (rd << 12); //Rd
    result += offset;
    return result;
}

uint32_t preOffset(uint32_t rd, uint32_t rn, uint32_t offset, int load){
    assert(offset <= 4095);
    uint32_t result = 0;
    result += (14 << 28); //cond
    result += (3 << 25);  //01I
    result += (3 << 23);  //PU
    result += (load << 20);  //L
    result += (rn << 16); //Rn
    result += (rd << 12); //Rd
    result += offset;     //offset
    printf("%x\n", result);
    return result;
}

uint32_t dataTransfer(char *given, int place){
    char mnemonic[5];
    char args[20];
    sscanf(given, "%[^','],%s", mnemonic, args);
    char rdStr[15];
    char address[15];
    sscanf(args, "%[^','],%s", rdStr, address);
    int rd = regFromString(rdStr);
    int load = 1;
    if (mnemonic == "str") load = 0;
    if (address[0] == '='){
        uint32_t loc = strToHex(address);
        if (loc <= 0xff) return doMov(rd, loc);
        else return placeAtEnd(rd, loc, load);
    }
    else if (!isImmediate(address)){
        if (hasComma(address)){
            char preReg[15];
            char preOff[15];
            sscanf(address, "%[^','],%s", preReg, preOff);
            return preOffset(rd, regFromString(preReg), atoi(&preOff[1]), load);
        }
        else return directRegister(rd, regFromString(address), load);
    }
}