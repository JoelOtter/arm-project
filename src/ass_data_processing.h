#ifndef ASSDP_H
#define ASSDP_H

uint32_t ass_data_processing(char *instruction);
int is_shifted(char *operand2);
uint32_t get_unshifted_register(char* operand2) ;
uint32_t get_shifted_register(char* operand2);
uint32_t shift_immediate(char* shift_name, char* constant);
uint32_t shift_register(char* shift_name, char* rs);
uint32_t get_shift_type(char *shift_type);

#endif
