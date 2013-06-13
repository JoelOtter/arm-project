#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ass_symbol_table.h"
#include "assemble.h"
#include "ass_branch.h"
#include "library.h"


uint32_t get_result(uint32_t cond, int32_t offset) {

    uint32_t result  = (1 << 27) | (1 << 25);
    
    result |= (cond << 28) | ((offset >> 2) & generate_mask(24));
    
    return result;
}

uint32_t ass_branch(char *instruction){
    
    char *mnemonic = get_mnemonic(instruction);
    char *rest = get_rest(instruction);
    
    uint32_t cond;
    int32_t offset;
    char suffix[3];
    
    suffix[0] = mnemonic[1];
    suffix[1] = mnemonic[2];
    suffix[2] = mnemonic[3];
    if (!strcmp(suffix, "eq")) {
        cond = 0;
    } else if (!strcmp(suffix, "ne")) {
        cond = 1;
    } else if (!strcmp(suffix, "ge")) {
        cond = 10;
    } else if (!strcmp(suffix, "lt")) {
        cond = 11;
    } else if (!strcmp(suffix, "gt")) {
        cond = 12;
    } else if (!strcmp(suffix, "le")) {
        cond = 13;
    } else {
        cond = 14;
    }
    
    int32_t label_address = get_memory_address(&symbol_table, rest);
       
    int32_t current_address = get_memory_address(&instruction_table, instruction);
        
    offset = label_address - current_address - 8; //Magi No is pipeline offset 

    free(mnemonic);
    free(rest);
        
    return(get_result(cond, offset));

}