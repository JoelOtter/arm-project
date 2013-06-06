#ifndef ASS_H
#define ASS_H

extern table symbol_table;
extern table instruction_table;

void writeBinary(char *path, char *write);

char* getNmonic(char *instruction);

int main(int argc, char **argv); 

#endif