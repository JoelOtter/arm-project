#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint32_t ass_branch(char *mnemonic, char *rest){
    
    uint32_t condition;
    uint32_t offset;
    char suffix[2];
    suffix[0] = mnemonic[1];
    suffix[1] = mnemonic[2];
    if (!strcmp(suffix, "ne")) {
        cond = 


}
