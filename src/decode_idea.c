#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

enum instructionType    {DATA_PROCESSING,
                         BRANCH,
                         MULTIPLY,
                         SINGLE_DATA_TRANSFER};

enum instructionType decode(uint32_t instruction){ //Instruction is 32 bits

    /*Idea for this function:
    * shift the instruction so that bits 27 - 25 are now bits 2 - 0.
    * perform bitwise and with a mask.
    * the result is different depending on the instruction.
    * explained further in switch comments
    */
    enum instructionType inst;       
    const uint32_t mask1 =  7; //Binary equivelant = ... 0000 0111
    const uint32_t mask2 = 15; // Binary equivalant = ...0000 1111
    uint32_t shiftInst = instruction >> 25;

    switch(mask1 & shiftInst){

        case(0):
            // if result is (000) then could be either data processing or multiply. 
            // further examiination of bits 7 -  4 will determine the instruction type.
            shiftInst = instruction >> 4;
            if((mask2 & shiftInst) == 9){
                inst = MULTIPLY;
            } else {
                inst = DATA_PROCESSING;
            }
            break;
        case(1):
            // if equals 1 (001) can only be a data processing instruction.
            inst = DATA_PROCESSING;
            break;    
        case(2):
            // if equals 2 (010) can only be a SDT instruction.
            inst = SINGLE_DATA_TRANSFER;
            break;
        case(3):
            // if equals 3 (011) can only be a SDT instruction.
            inst = SINGLE_DATA_TRANSFER;
            break;
        case(5):
            // if result equals 4(100), instruction can only be branch.
            inst = BRANCH;
            break;
   }

   return inst;

}  



int main(int argc, char const *argv[])
{
    uint32_t instruction = atoi(argv[1]);

    enum instructionType inst = decode(instruction);
    
    switch(inst){
        case(DATA_PROCESSING): printf("DATA_PROCESSING\n"); break;
        case(MULTIPLY): printf("MULTIPLY\n"); break;
        case(SINGLE_DATA_TRANSFER): printf("SINGLE_DATA_TRANSFER\n"); break;
        case(BRANCH): printf("BRANCH\n"); break;
    }
    return 0;
}