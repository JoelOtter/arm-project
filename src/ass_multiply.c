#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ass_multiply.h"
#include "library.h"

uint32_t assMultiply(char *mnemonic, char *rest) {

    char Rd[2], Rm[2], Rs[2], Rn[2];
    uint32_t RdI, RmI, RsI, RnI;

    uint32_t result = (111 << 29) | (1 << 4) | (1 << 7);

    if (strcmp(mnemonic,"mul") == 0) {
        sscanf(rest, "%[^','],%[^','],%s", Rd, Rm, Rs);
        RnI = 0;
    } else {
        sscanf(rest, "%[^','], %[^','], %[^','],%s", Rd, Rm, Rs, Rn);
        result |= (1 << 21);
        RnI = regFromString(Rn);
    }

    RdI = regFromString(Rd);
    RmI = regFromString(Rm);
    RsI = regFromString(Rs);

    result |= (RdI << 16) | (RnI << 12) | (RsI << 8) | (RmI & mask4);

    //printf("%x\n", result);

    return result;
}

/*
int main(int argc, char const *argv[])
{
    return assMultiply("mla", "r1,r2,r3,r4");
}
*/