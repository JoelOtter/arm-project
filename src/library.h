#ifndef LIB_H
#define LIB_H

#include <string.h>

extern const uint32_t mask1; 
extern const uint32_t mask2; 
extern const uint32_t mask4;  
extern const uint32_t mask5; 
extern const uint32_t mask8;  

extern int carryout;

char *get_mnemonic(char *instruction);

char *get_rest(char *instruction);

void print_bits(uint32_t x);

uint32_t generate_mask(uint32_t length);

uint32_t log_shift_left(uint32_t value, uint32_t scale);

uint32_t log_shift_right(uint32_t value, uint32_t scale);

uint32_t arith_shift_right(uint32_t value, uint32_t scale);

uint32_t rotate_right(uint8_t rotate, uint32_t value);

int sign_extend(int value, int length);

uint32_t get_from_memory(unsigned char *memory, int start);

void write_to_memory(unsigned char *memory, int start, uint32_t value);

void print_registers(int32_t *registers);

void print_non_zero_memory(unsigned char *memory);

void overwrite_debug_file(void);

void print_to_debug(int32_t *registers, unsigned char *memory);

int reg_from_string(char *rstring);

int has_comma(char *address);

int is_immediate(char *rstring);

int is_elem_of(char *search_string, char *list[] , unsigned long int len);

int has_sqb_before_comma(char *address);

char* remove_leading_spaces(char *string);

void suggest(char *input_string, int line_number); //part of extensoin

int is_valid_instruction(char *string);

#endif
