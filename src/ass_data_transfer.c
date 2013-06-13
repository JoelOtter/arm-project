#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "library.h"
#include "ass_symbol_table.h"
#include "assemble.h"

table instruction_table;

uint32_t In = 0;
uint32_t P = 0;
uint32_t U = 0;
uint32_t L = 0;
uint32_t Rn = 0;
uint32_t Rd = 0;
uint32_t Offset = 0;
uint32_t condition = 0;

const int SDT_mask = 67108864;

uint32_t get_shifted_register(char* operand2);

uint32_t build_instruction(void){

    uint32_t result = SDT_mask; //call this SDT_mask or something?
    condition = 14;
    // = 00000100000000000000000000000000

    result |= (condition << 28); //cond
    result |= (In << 25);  //01I p
    result |= (P << 24);  //P
    result |= (U << 23);  //U
    result |= (L << 20);  //L
    result |= (Rn << 16); //Rn
    result |= (Rd << 12); //Rd
    result |= Offset;
    return result;

}

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

uint32_t direct_register(void){
    uint32_t result = 0;
    result += (14 << 28); //cond
    result += (2 << 25);  //01I
    result += (3 << 23);  //PU
    result += (L << 20);  //L
    result += (Rn << 16); //Rn
    result += (Rd << 12); //Rd
    return result;
}

uint32_t place_at_end(uint32_t value, int place){
    
    int num_in = 0;
    for (int x=0; add_afters[x] != 0; x+=4){
        ++num_in;
    }
    
    add_afters[num_in] = value;

    uint32_t address_of_last_instruction = (table_end(&instruction_table))->prev->memory_address;
    Offset = address_of_last_instruction - (place + 8);
    Offset += 4;
    Offset += (num_in * 4);

    P = 1;
    U = 1;
    In = 0;
    Rn = 15; //Base register should be the PC = reg[15]. Enum PC?

    return build_instruction();
}

void calculate_offset(char *offset_str){
// Determines whether offset is #expression or {+/-}Rm{,<shift>}

    offset_str = remove_leading_spaces(offset_str);
    //In = 1; // Might have to go after the if??
    if(offset_str[0] == '#'){
        In = 0;
        U = 1;
        Offset = atoi(offset_str+1); //CHECK DEF OF atoi!!
        return;
    }
    In = 1;
     // offset_str is of form {+/-}Rm{,<shift>}
    U = !(offset_str[0] == '-'); // if - Rm <shift>, U = 0;
    Offset = get_shifted_register(offset_str); // calls function in data_processing
}


uint32_t get_shifted_register(char* operand2) {
    //PRE: operand2 is of form: Rm, shift_type {#expression or register}
    //POST: Returns contents of Rm shifted by the shift type by a degree
    //      specified by the constant expression or register. 
   
    uint32_t rmI;
    char shift_name[5];
    char rest[10];
    uint32_t operand2_i;
    char *shift_name_pointer;
    char *rest_pointer;
    char Rm[2];

    shift_name[0] = '\0';
    rest[0] = '\0';

    sscanf(operand2, "%[^','],%s %s", Rm, shift_name, rest);
    remove_leading_spaces(Rm);
    
    if(shift_name[0] == '\0' && L == 0){
        return (0 + reg_from_string(Rm)); 
    } 

    shift_name_pointer = remove_leading_spaces(shift_name);
    rest_pointer = remove_leading_spaces(rest);


    if ( rest[0] == '#') {
        In = 1;
        operand2_i = shift_immediate(shift_name_pointer, &(rest_pointer[1]));
    } else {
        operand2_i = shift_register(shift_name_pointer, rest_pointer);
    } 

    rmI = reg_from_string(remove_leading_spaces(Rm));
    operand2_i |= rmI;
    
    return operand2_i;
}

uint32_t ass_data_transfer(char *given, int place){

    char *mnemonic = get_mnemonic(given);
    char *args = get_rest(given);
    char rd_str[10];
    char address[30];

    char *rd_strp = rd_str;
    char *addressp = address;
    uint32_t result;
    
    
    sscanf(args, "%[^','],%[^\n]", rd_str, address);
    addressp = remove_leading_spaces(address);
    Rd = reg_from_string(rd_strp);
    L = (!(strcmp(mnemonic, "str") == 0));

    free(mnemonic);
    free(args);

    if (address[0] == '='){
        In = 1;
        //address is of form <=expression> (ldr only).
        uint32_t location = str_to_hex(addressp);

        if (location <= 0xff){
            return do_mov(Rd, location);
        } else {
            return place_at_end(location, place);
        }

    } else if (!is_immediate(addressp)){
        In = 0;
        // address is of form: - [Rn,<#expression>]
        //                     - [Rn]

        char base_reg[15];
        char offset_str[15];

        char *base_regp;

        if (is_post_indexed(addressp)){ //CHECK THIS IS RIGHT
            // Post-indexing
            // Address is of form: - [Rn], <#expression>
            //                     - [Rn], {+/-}Rm{,<shift>}
            P = 0; 
            sscanf(address, "%[^','],%s", base_reg, offset_str);
            Rn = reg_from_string(base_reg);
            calculate_offset(offset_str);
            assert(Offset <= 4095); // To fit into 12 bits!

            result = build_instruction();

        } else {
            //Pre indexing
            // address of form: - [Rn, <#expression>]
            //                  - [Rn, {+/-}Rm{,<shift>}]
            //                  - [Rn]

            P = 1;

            sscanf(address, "%[^','],%[^\n]", base_reg, offset_str);

            if(!has_comma(addressp)){
                U = 1;
                Offset = 0;
            } else {
                calculate_offset(offset_str);
            }
            base_regp = remove_leading_spaces(base_reg);
            Rn = reg_from_string(base_regp);

            result = build_instruction();

        }

        return result;

    } else {
        Rn = reg_from_string(address);
        return direct_register();
        result = 0;
    }
    
    return result;
}