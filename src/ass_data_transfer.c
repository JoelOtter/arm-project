#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "library.h"
#include "symboltableadt.h"
#include "assemble.h"

table instruction_table;

uint32_t str_to_hex(char *xstr){
    return strtol(&xstr[1], NULL, 0);
}


uint32_t do_mov(uint32_t rd, uint32_t location){
    uint32_t result = 0;
    result += (14 << 28); //cond
    result += (1 << 25);  //I
    result += (13 << 21); //opcode for mov
    result += (rd << 12); //destination reg
    result += location;   //and the value
    return result;
}

uint32_t direct_register(uint32_t rd, uint32_t rn, int load){
    uint32_t result = 0;
    result += (14 << 28); //cond
    result += (2 << 25);  //01I
    result += (3 << 23);  //PU
    result += (load << 20);  //L
    result += (rn << 16); //Rn
    result += (rd << 12); //Rd
    return result;
}

uint32_t place_at_end(uint32_t rd, uint32_t value, int load, int place){
    int num_in = 0;
    for (int i=0; add_afters[i] != 0; i+=4){
        ++num_in;
    }
    
    add_afters[num_in] = value;

    uint32_t address_of_last_instruction = (table_end(&instruction_table))->prev->memory_address;

    //char *last_value = (table_end(&instruction_table))->label;


    uint32_t offset = address_of_last_instruction - (place + 8);
    offset += 4;
    offset += (num_in * 4);
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

uint32_t do_offset(uint32_t rd, uint32_t rn, uint32_t offset, int load, int pre, int imm){
    assert(offset <= 4095);
    uint32_t result = 0;
    result += (14 << 28); //cond
    result += ((2+imm) << 25);  //01I
    result += (pre << 24);
    result += (1 << 23);  //PU
    result += (load << 20);  //L
    result += (rn << 16); //Rn
    result += (rd << 12); //Rd
    result += offset;     //offset
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

    char *rd_str = malloc(10);
    char *address = malloc(30);
    
    
    sscanf(args, "%[^','],%[^\n]", rd_str, address);
    
 
    address = remove_leading_space(address);

    int rd = reg_from_string(rd_str);
    int load = 1;

    if (!strcmp(mnemonic, "str")){
        load = 0;
    }


    if (address[0] == '='){
        uint32_t loc = str_to_hex(address);
        if (loc <= 0xff){
            return do_mov(rd, loc);
        } else {
            return place_at_end(rd, loc, load, place);
        }

    } else if (!is_immediate(address)){
        char *pre_reg = malloc(15);
        char *pre_off = malloc(15);

        if (has_sqb_before_comma(address)){
            sscanf(address, "%[^','],%s", pre_reg, pre_off);
            if (pre_off[0] == 'r'){
                return do_offset(rd, reg_from_string(pre_reg), atoi(&pre_off[1]), load, 0, 1);
            }
            else return do_offset(rd, reg_from_string(pre_reg), atoi(&pre_off[1]), load, 0, 0);
        }
        if (has_comma(address)){
            sscanf(address, "%[^','],%[^\n]", pre_reg, pre_off);
           
            pre_reg = remove_leading_space(pre_reg);
            pre_off = remove_leading_space(pre_off);
            return do_offset(rd, reg_from_string(pre_reg), atoi(&pre_off[1]), load, 1, 0);
        }
        else return direct_register(rd, reg_from_string(address), load);
    }
    return 0;
}




