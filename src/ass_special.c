#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "library.h"
#include "ass_special.h"

// mov is 1001, cond is 1110 (always)

uint32_t ass_special(char *instruction) {

    char *mnemonic = get_mnemonic(instruction);
    char *rest = get_rest(instruction);

    if (!strcmp(mnemonic, "andeq")) {
        printf("ANDEQ\n");
        return 0;
    } else {
        char Rn[2], expression[20];
        sscanf(rest, "%[^','],#%s", Rn, expression);
        uint32_t result = (111 << 29) | (1 << 24) | (1 << 23) | (1 << 21);
        int RnI = regFromString(Rn);
        result |= (RnI << 12) | RnI;
        uint32_t scale;
        if (expression[1] == 'x') {
            scale = strtol(expression, NULL, 0);
        } else {
            scale = atoi(expression);
        }
        result |= scale << 7;
        printf("%x\n", result);
        return result;
    }
}


/*
int main(int argc, char const *argv[])
{
    assSpecial("andeq", "r0,r0,r0");
    assSpecial("lsl", "r1,#0x1f");
    return 0;
}
*/
