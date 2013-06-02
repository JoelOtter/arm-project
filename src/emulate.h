#ifndef EMU_H
#define EMU_H

//VARIABLES
extern unsigned char *memory;
extern int *registers;

//ENUMS
enum instructionType    {DATA_PROCESSING,
                         BRANCH,
                         MULTIPLY,
                         SINGLE_DATA_TRANSFER};

//METHODS

int main(int argc, char **argv);

static void loadbinary(const char *filepath);

static int checkCondition(uint32_t instruction);

static enum instructionType decode(uint32_t instruction);



#endif