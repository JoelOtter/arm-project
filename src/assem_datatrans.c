#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "library.h"

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
    printf("%u\n", result);
    return result;
}

uint32_t load(int rd, char *address){
    if (address[0] == '='){
        uint32_t loc = strToHex(address);
        if (loc <= 0xff) return doMov(rd, loc);
    }
}

uint32_t store(int rd, char *address){

}

uint32_t dataTransfer(char *mnemonic, char *args){
    char rdStr[15];
    char addressStr[15];
    sscanf(args, "%[^','],%s", rdStr, addressStr);
    if (mnemonic == "str") {
        return store(regFromString(rdStr), addressStr);
    }
    else if (mnemonic == "ldr") {
        return load(regFromString(rdStr), addressStr);
    }
}

int main(int argc, char const *argv[])
{
    dataTransfer("ldr", "r15, =0x13");
    return 0;
}