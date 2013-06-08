#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "library.h"
#include "ass_data_processing.h"
#define error "Not acceptable number"

enum data_instruction_type {RESULT,
                            SINGLE_OPERAND,
                            CPSR};

char *result_instructions[] = { "add", "eor", "sub", "rsb", "and", "orr" };
unsigned long int result_instructions_size = (sizeof(result_instructions)/sizeof(result_instructions[0]));

uint32_t cond = 0;
uint32_t I = 0;
uint32_t opcode = 0;
uint32_t S = 0;
uint32_t RnI = 0;
uint32_t RdI = 0;
uint32_t rotation = 0; // for operand2_i
uint32_t operand2_i = 0;
char Rn[2];
char Rd[2];
char Rm[2];
char operand2[15];

enum data_instruction_type get_type(char *mnemonic) {

    enum data_instruction_type inst;
    char first = mnemonic[0];

    if (first == 'm') {
        inst = SINGLE_OPERAND;
    } else if ( first == 't' || first == 'c' )   {
        inst = CPSR;
    } else if (is_elem_of(mnemonic, result_instructions, result_instructions_size)) {
        inst = RESULT;
    }

    return inst;
}

uint32_t get_opcode(char *mnemonic) {

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

int get_num_zeroes(uint32_t n) {

    int num_zeroes = 0;
    int first_one = 0; 

    if ( n == 0 ) {
        return 32;
    } else {  
        while (!first_one) {
            num_zeroes++;
            first_one = n & 1;
            n >>= 1;
        }
        num_zeroes--;
        return num_zeroes;
    }
}

int within_k_bits(uint32_t n, int k) {

   int num_zeroes = get_num_zeroes(n);
   num_zeroes++;
   n >>= num_zeroes;
   uint32_t mask = (((1 << (32-num_zeroes)) - 1) << (k-1));

   return !((mask & n) > 0 );
}

int is_valid_number(uint32_t number) {

    int num_zeroes = get_num_zeroes(number);

    return ((within_k_bits(number, 8)) && !((num_zeroes & 1) && !(within_k_bits(number, 7))));
}

uint32_t calculate_rotation(uint32_t number) {

    uint32_t rotation;    
    int32_t num_zeroes = get_num_zeroes(number);

    assert(is_valid_number(number));
    if ( number < 256 ) {
        rotation = 0;
    } else {
        if ( num_zeroes & 1 ) num_zeroes = num_zeroes-1;
        rotation = num_zeroes; // DIVIDE ME BY TWO 
    }

    return rotation;
}

uint32_t get_operand2(char *operand2) {

    char first = operand2[0];
    uint32_t operand2_i;

    if ( first == '#' ) {
        I = 1;
        if (operand2[2] == 'x') { //hex
            operand2 = operand2+3;
            operand2_i = strtol(operand2, NULL, 16);
        } else { //dec
            operand2 = operand2+1;
            operand2_i = atoi(operand2);
        }

        if ( is_valid_number(operand2_i)) {

            if (! (operand2_i < 256) ) {
                uint32_t rotation = calculate_rotation(operand2_i);
                operand2_i = rotate_right(rotation, operand2_i);  
                operand2_i = operand2_i | (((32 - rotation) >> 1) << 8 );
            }

        } else {
            operand2_i = 0;
            perror("Error: numeric constant cannot be represented\n");
            exit(EXIT_FAILURE);
        }

    } else {
        I = 0;
        if ( !is_shifted(operand2) ) {
            operand2_i = get_unshifted_register(operand2);
        } else { 
            operand2_i = get_shifted_register(operand2);
        }
    }

    return operand2_i;
}

uint32_t get_unshifted_register(char* operand2) {

    sscanf(operand2, "%s", Rm);
    return reg_from_string(Rm);
}

int is_shifted(char *operand2) {

     char first[30];
     char second[30];
     char *second2;

     sscanf(operand2, "%[^','],%[^\n]", first, second);
     second2 = (remove_leading_spaces(second));   
 
     return !(second2[0] == '\0');
}

uint32_t get_shifted_register(char* operand2) {

    uint32_t rmI;
    char shift_name[5];
    char rest[10];
    uint32_t operand2_i;
    char *shift_name_pointer;
    char *rest_pointer;
    
    sscanf(operand2, "%[^','],%s %s", Rm, shift_name, rest);
    remove_leading_spaces(Rm);
    shift_name_pointer = remove_leading_spaces(shift_name);
    rest_pointer = remove_leading_spaces(rest);

    if ( rest[0] == '#') {
        operand2_i = shift_immediate(shift_name_pointer, &(rest_pointer[1]));
    } else {
        operand2_i = shift_register(shift_name_pointer, rest_pointer);
    }

    rmI = reg_from_string(remove_leading_spaces(Rm));
    operand2_i |= rmI;
    
    return operand2_i;
}

uint32_t shift_immediate(char* shift_name, char* constant) {

    uint32_t result = 0;
    uint32_t immediate = atoi(constant); // might not be unsigned
    uint32_t shift_type = get_shift_type(shift_name);

    if ( immediate >=64 ) {
        perror("Error: numeric constant cannot be represented\n");
        exit(EXIT_FAILURE);
    }

    result = ( immediate << 7 ) | (shift_type << 5 );

    return result;
}

uint32_t shift_register(char* shift_name, char* rs) {

    uint32_t result = 0;
    uint32_t rsI = reg_from_string(rs);
    uint32_t shift_type = get_shift_type(shift_name);
    
    result = (rsI << 8 ) | (shift_type << 5) | (1 << 4);

    return result;
}

uint32_t get_shift_type(char *shift_type){

    if (!(strcmp(shift_type, "lsl"))) {
        return 0;
    } else if (!(strcmp(shift_type, "lsr") )) {
        return 1;
    } else if (!(strcmp(shift_type, "asr"))) {
        return 2;
    }
 
    return 3;
}

void execute_result_inst(char *mnemonic, char *rest) {
    S = 0;
    sscanf(rest, "%[^','],%[^','],%[^\n]", Rd, Rn, operand2);
    RdI = reg_from_string(Rd);
    RnI = reg_from_string(Rn);
}

void execute_sing_operand_inst(char *mnemonic, char *rest) {
    S = 0;
    sscanf(rest, "%[^','],%s", Rd, operand2);
    RnI = 0;
    RdI = reg_from_string(Rd);
}

void execute_cpsr_inst(char *mnemonic, char *rest) {
    S = 1;
    sscanf(rest, "%[^','],%[^\n]", Rn, operand2);
    RnI = reg_from_string(Rn);
    RdI = 0;
}

uint32_t ass_data_processing(char *instruction) {

    char *mnemonic = get_mnemonic(instruction);
    char *rest = get_rest(instruction);
    uint32_t result;

    enum data_instruction_type type = get_type(mnemonic);
    switch(type) {
        case(RESULT): 
            execute_result_inst(mnemonic, rest);
            break;
        case(SINGLE_OPERAND): 
            execute_sing_operand_inst(mnemonic, rest);
            break;
        case(CPSR): 
            execute_cpsr_inst(mnemonic, rest);
            break;
    }    

    cond = 14;
    opcode = get_opcode(mnemonic);
    operand2_i = get_operand2(operand2);

    result = (cond << 28) | (I << 25) | (opcode << 21) | (S << 20) | (RnI << 16) | (RdI << 12) | operand2_i;
   
    return result;
}

