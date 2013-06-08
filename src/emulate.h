#ifndef EMU_H
#define EMU_H

//VARIABLES
extern unsigned char *memory;
extern int *registers;
extern const int SIZE_OF_MEMORY;
extern const int NUM_REGISTERS;

//ENUMS
enum instruction_type    {DATA_PROCESSING,
                         BRANCH,
                         MULTIPLY,
                         SINGLE_DATA_TRANSFER};

//METHODS

int main(int argc, char **argv);

#endif
