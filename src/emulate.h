#ifndef EMU_H
#define EMU_H

extern int *registers;

unsigned char* loadbinary(const char *filepath);

int checkCondition(uint32_t instruction);



#endif