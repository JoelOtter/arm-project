#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ass_multiply.h"
#include "library.h"

uint32_t ass_multiply(char *instruction) {

    char *mnemonic = get_mnemonic(instruction);
    char *rest = get_rest(instruction);

    char Rd[2], Rm[2], Rs[2], Rn[2];
    uint32_t RdI, RmI, RsI, RnI;

    uint32_t result = (111 << 29) | (1 << 4) | (1 << 7);

    if (strcmp(mnemonic,"mul") == 0) {
        sscanf(rest, "%[^','],%[^','],%s", Rd, Rm, Rs);
        RnI = 0;
    } else {
        sscanf(rest, "%[^','], %[^','], %[^','],%s", Rd, Rm, Rs, Rn);
        result |= (1 << 21);
        RnI = reg_from_string(Rn);
    }

    RdI = reg_from_string(Rd);
    RmI = reg_from_string(Rm);
    RsI = reg_from_string(Rs);

    result |= (RdI << 16) | (RnI << 12) | (RsI << 8) | (RmI & mask4);

    free(mnemonic);
    free(rest);

    return result;
}