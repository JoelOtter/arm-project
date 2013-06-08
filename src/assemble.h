#ifndef ASS_H
#define ASS_H

extern table symbol_table;
extern table instruction_table;
extern uint32_t *add_afters;

void write_binary(char *path, char *write);

int main(int argc, char **argv); 

#endif