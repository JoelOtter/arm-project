#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int *registers;

//some scary masks
const uint32_t mask1 = 1; //0000 0001
const uint32_t mask4 = 15; //0000 0100
const uint32_t mask12 = 4095; //0000 1100

//Blatantly thieved from dataProcessing_idea.c
int checkCondition(uint32_t instruction) {

    //CSPR register values
    uint32_t cspr = registers[16];
    uint32_t N = mask1 & (cspr >> 31);
    uint32_t Z = mask1 & (cspr >> 30);
    uint32_t V = mask1 & (cspr >> 28);

    uint32_t condNumber = mask4 & (instruction >> 28);
    int condflag = 0;

    printf("instruction = %d\n", instruction);
    printf("Con Num = %d\n", condNumber);
    switch(condNumber) {

        case(0):
            // eq
            condflag = Z;
            break;
        case(1):
            // ne
            condflag = Z;
            break;
        case(10):
            // ge
            condflag = (N == V);
            break;
        case(11): 
            // lt
            condflag = (N != V);
            break;
        case(12):
            // gt
            condflag = ((Z == 0) && (N == V));
            break;
        case(13):
            //le
            condflag = ((Z == 1) || (N != V));
            break;
        case(14):
            // al (always 1)
            condflag = 1;
            break;
    }
    printf("ConditionFlag = %d\n", condflag);
    return condflag;
}

void executeInstruction(uint32_t instruction){
    uint32_t P = ((instruction >> 24) & mask1);
    uint32_t U = ((instruction >> 23) & mask1);
    uint32_t L = ((instruction >> 20) & mask1);
    uint32_t I = ((instruction >> 25) & mask1);
    uint32_t baseReg = ((instruction >> 16) & mask4);
    uint32_t destReg = ((instruction >> 12) & mask4);
    uint32_t offset = (instruction & mask12);
}

int main(int argc, char const *argv[]) {

    uint32_t instruction = atoi(argv[1]);

    //Obviously we can change this when merging it all.
    registers = malloc(17 * sizeof(int));

    for (int i = 0; i < 17; ++i){
        registers[i] = 0;
    }

    if ( checkCondition(instruction) == 1 ) {
        executeInstruction(instruction);
    }
    return 0;
}