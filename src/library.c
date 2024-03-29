#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
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
    char *mnemonic_result = malloc(10);
    sscanf(instruction, "%s", mnemonic_result);
    return mnemonic_result;
}

char *get_rest(char *instruction){
    char *rest_result = malloc(30);
    sscanf(instruction, "%*s %[^\n]", rest_result);
    return rest_result;
}

uint32_t get_from_memory(unsigned char *memory, int start) {

    uint32_t p1 = memory[start+3] << 24;
    uint32_t p2 = memory[start+2] << 16;
    uint32_t p3 = memory[start+1] << 8;
    uint32_t p4 = memory[start];

    return (p1 | p2 | p3 | p4);
}

void write_to_memory(unsigned char *memory, int start, uint32_t value) {
    memory[start+3] = value >> 24;
    memory[start+2] = value >> 16;
    memory[start+1] = value >> 8;
    memory[start] = value;
}

void print_registers(int32_t *registers){

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

void print_non_zero_memory(unsigned char *memory) {
    printf("Non-zero memory:\n");
    for (int i = 0; i< SIZE_OF_MEMORY; i+=4) {
        uint32_t memory_content = get_from_memory(memory, i);
        if (memory_content != 0) {
            printf("0x%08x: 0x%08x\n", i, memory_content);
        }
    }
}

int has_next_non_zero(unsigned char *memory, int from) {
    for (int i = from+4; i< SIZE_OF_MEMORY; i+=4) {
        uint32_t memory_content = get_from_memory(memory, i);
        if (memory_content != 0) {
            return 1;
        }
    }
    return 0;
}

// returns strring in array you pass.
char *itoa(uint i, char b[]){
    for(int i = 0; i < 16; ++i) {
        b[i] = '\0';
    }

    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i = -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

void overwrite_debug_file(void) {
    FILE *df;
    char nothing = '\0';
    if ((df = fopen("debug_file", "w")) == NULL) {
        perror("Error opening file!");
        exit(EXIT_FAILURE);
    } else {
        fwrite(&nothing, 0, 1, df);
        fclose(df);
    }
}

void print_to_debug(int32_t *registers, unsigned char *memory) {
    // register value b= 10 digits = 10 chars *15 cos 15 registers
    // 16 chars for address + 10 chars for content
    FILE *df; 
    if ((df = fopen("debug_file", "a")) == NULL) {
        perror("Error opening file!");
        exit(EXIT_FAILURE);
    }

  //  char int_as_string[16];
  
    fprintf(df, "[[");
    for(int i = 0; i < 17; ++i) {
        fprintf(df, "%d", registers[i]);
        if(i != 16){
            fprintf(df, ",");
        }
    }
    fprintf(df, "],[");

    for (int i = 0; i< SIZE_OF_MEMORY; i+=4) {
        uint32_t memory_content = get_from_memory(memory, i);
        if (memory_content != 0) {
            
            fprintf(df, "[");
            
            fprintf(df, "%d", i);

            fprintf(df, ",");

            fprintf(df, "%d", memory_content);

            fprintf(df, "]");

            if (has_next_non_zero(memory, i)) {
                fprintf(df, ",");
            }
        }
    }


    fprintf(df, "]]\n"); 
    fclose(df);
}

void print_bits(uint32_t x) {
    
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

void print_memory(uint8_t x) {
    
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

uint32_t generate_mask(uint32_t length){

    uint32_t mask = 0;

    for(uint32_t i = 0; i < length; i ++){

        mask <<= 1;
        mask += 1;

    }
    return mask;
}

uint32_t log_shift_left(uint32_t value, uint32_t scale) {

    carryout = (value >> (32 - scale)) & mask1;
    if (scale == 0) carryout = 0;
    value <<= scale;
    return value;

}

uint32_t log_shift_right(uint32_t value, uint32_t scale) {
    
    carryout = 0;
    if (scale > 0) {
    carryout = (value >> (scale - 1)) & mask1;
    }
    value >>= scale;
    return value;
}

uint32_t arith_shift_right(uint32_t value, uint32_t scale) {

    uint32_t sign = value >> 31;

    value = log_shift_right(value, scale);
    
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

uint32_t rotate_right(uint8_t rotate, uint32_t value){

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

int sign_extend(int value, int length){

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

int reg_from_string(char *rstring){
    //PRE:
    //POST: returns register number i.e  [r3] -> 3
    if (rstring[0] == '['){
        return reg_from_string(&rstring[1]);
    }
    char value[5];
    sscanf(rstring, "r%s", value);
    return (atoi(value));
}

int is_immediate(char *rstring){
    return !(rstring[0] == '[');
}

int has_comma(char *address){
    int i = 0;
    while (address[i] != '\0'){
        if (address[i++] == ',') return 1;
    }
    return 0;
}

int is_post_indexed(char *address){
    //int hassqb = 0;
    char *before_bracket = calloc(20,1);
    char *after_bracket = calloc(20,1);
    int post_indexed;

    address = remove_leading_spaces(address);
    int i = 0;
    while (address[i] != '\0'){
        if (address[i++] == ']'){
            post_indexed = 1; //[r4     ]
            break;
        }
        if (address[i] == ','){
            post_indexed = 0; //[r2,]
            break;
        }
    }

    if(post_indexed){
        if(!(has_comma(address))){
            post_indexed = 0;
        } 
    }

    free(before_bracket);
    free(after_bracket);
    return post_indexed;
}

// suggestion thing
int string_compare(char *s1, char *s2) {

    int length = strlen(s1);
    int count = 0;

    for ( int i = 0; i < length; i++ ) {
        if ( s1[i] == s2[i] ) {
            count++;
        } 
    }
    return count;
}

char *suggestions;

char *get_suggestions(char *s, char *list[], int length) {

    int max_suggestion_size = 5 * sizeof(char);
    suggestions = calloc((length * max_suggestion_size) + (20 * sizeof(char)), 1);
    char *append;
    
    if ( strlen(s) == 1 ) {
        strcat(suggestions, "b");
    } else if ( strlen(s) >= 5 ) {
        strcat(suggestions, "andeq");
    } else {
        char s2[5];
        strcpy(s2, " ");
        strcat(s2, s);
        char *s3 = s++; 
        for(int i = 0; i < length; i++) {
            if( string_compare(s, list[i]) >= 2 || string_compare(s2, list[i]) >= 2 || string_compare(s3, list[i]) >= 2 ) {
                append = list[i];
                strcat(suggestions, " ");
                strcat(suggestions, append);
            }
        }
    }
    return remove_leading_spaces(suggestions);
}


char *valid_instructions[] = { "add", "sub", "rsb", "and", "eor", "orr", "mov", 
                                   "tst", "teq", "cmp", "mul", "mla", "ldr", "str", 
                                   "beq", "bne", "bge", "blt", "bgt", "ble", "lsl",
                                   "bal", "b"};

int valid_instructions_length = sizeof(valid_instructions)/8;

void suggest(char *input_string, int line_number) {

    char *suggestion = get_suggestions(input_string, valid_instructions, valid_instructions_length);
    FILE *df;
    if ((df = fopen("debug_suggestions", "a")) == NULL) {
        perror("Error opening file!");
        exit(EXIT_FAILURE);
    }
    fprintf(df, "[%d, \"%s\", \"%s\"]\n", line_number, input_string, suggestion);
    fclose(df);
    free(suggestions);  //  return result;
}

int is_valid_instruction(char *string) {
    return ( is_elem_of(string, valid_instructions, valid_instructions_length));
}

int is_elem_of(char *search_string, char *list[] , unsigned long int len) {

    int i;

    for(i = 0; i < len; ++i) {
        
        if(!strcmp(list[i], search_string)){
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