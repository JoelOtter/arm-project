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
    result += (2 << 25);  //01I
    result += (3 << 23);  //PU
    result += (load << 20);  //L
    result += (rn << 16); //Rn
    result += (rd << 12); //Rd
    return result;
}

uint32_t placeAtEnd(uint32_t rd, uint32_t value, int load, int place){
    int numIn = 0;
    for (int i=0; add_afters[i] != 0; i+=4){
        ++numIn;
    }
    
    add_afters[numIn] = value;

    uint32_t address_of_last_instruction = (table_end(&instruction_table))->prev->memory_address;

    //char *last_value = (table_end(&instruction_table))->label;


    uint32_t offset = address_of_last_instruction - (place + 8);
    offset += 4;
    offset += (numIn * 4);
    uint32_t result = 0;
    result += (14 << 28); //cond
    result += (2 << 25);  //01I
    result += (3 << 23);  //PU
    result += (load << 20);  //L
    result += (15 << 16); //Rn
    result += (rd << 12); //Rd
    result += offset;
    return result;
}

uint32_t doOffset(uint32_t rd, uint32_t rn, uint32_t offset, int load, int pre){
    assert(offset <= 4095);
    uint32_t result = 0;
    result += (14 << 28); //cond
    result += (2 << 25);  //01I
    result += (pre << 24);
    result += (1 << 23);  //PU
    result += (load << 20);  //L
    result += (rn << 16); //Rn
    result += (rd << 12); //Rd
    result += offset;     //offset
    printf("%x\n", result);
    return result;
}

char* remove_leading_space(char *address){

    if(address[0] == ' '){
       return &(address[1]);
    }

    return address;
}

uint32_t ass_data_transfer(char *given, int place){

    char *mnemonic = get_mnemonic(given);
    char *args = get_rest(given);

    char *rdStr = malloc(10);
    char *address = malloc(30);
    
    
    sscanf(args, "%[^','],%[^\n]", rdStr, address);
    
 
    address = remove_leading_space(address);

    printf("Address:%s\n", address);
//  strcpy(address[1], *address[0]);   

   // address = get_rest(&address);

    int rd = regFromString(rdStr);
    int load = 1;

    if (!strcmp(mnemonic, "str")){
        load = 0;
    }


    if (address[0] == '='){
        uint32_t loc = strToHex(address);
        if (loc <= 0xff){
            return doMov(rd, loc);
        } else {
            return placeAtEnd(rd, loc, load, place);
        }

    } else if (!isImmediate(address)){
        char *preReg = malloc(15);
        char *preOff = malloc(15);
        printf("HERERERE One\n");

        if (has_sqb_before_comma(address)){
            sscanf(address, "%[^','],%s", preReg, preOff);
            printf("HERERERE Two\n");
            return doOffset(rd, regFromString(preReg), atoi(&preOff[1]), load, 0);
        }
        if (hasComma(address)){
            sscanf(address, "%[^','],%[^\n]", preReg, preOff);
           
            preReg = remove_leading_space(preReg);
            preOff = remove_leading_space(preOff);

 printf("preReg = %s\n", preReg);
            printf("preOff = %s\n", preOff);
            return doOffset(rd, regFromString(preReg), atoi(&preOff[1]), load, 1);
        }
        else return directRegister(rd, regFromString(address), load);
    }
    printf("HERERERE Three\n");
    return 0;
}




