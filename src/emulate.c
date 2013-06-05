#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "sdatatrans.h"
#include "multiply.h"
#include "data_Process.h"
#include "branch.h"
#include "emulate.h"
#include "library.h"

unsigned char *memory;
int32_t *registers;

static void loadBinary(const char *filepath);

static int checkCondition(uint32_t instruction);

static enum instructionType decode(uint32_t instruction);

void printMemory(void);

void printNonZeroMemory(void);

int main(int argc, char **argv) {

    memory = calloc(SIZE_OF_MEMORY, sizeof(char));
    registers = calloc(NUM_REGISTERS, sizeof(int32_t));

    assert(argc == 2);

    loadBinary(argv[1]);
    int32_t *PC = &registers[15];
    *PC = 8;
    uint32_t fetched = getFromMemory(memory, 4);
    uint32_t decoded = getFromMemory(memory, 0);
    enum instructionType currentInstType = decode(decoded);
    int skipToNext = 0;

    while(decoded != 0){

        skipToNext = 0;

        // If condition is satisfied, execute current instruction!
        if(checkCondition(decoded)){
            switch(currentInstType){
                case(DATA_PROCESSING):
                    dataProcess(decoded);
                break;
                case(BRANCH):
                    branch(decoded);
                    fetched = getFromMemory(memory, *PC + 4);
                    decoded = getFromMemory(memory, *PC);
                    currentInstType = decode(decoded);
                    *PC += 8;
                    skipToNext = 1;
                break;
                case(MULTIPLY):
                    multiply(decoded);
                break;
                case(SINGLE_DATA_TRANSFER):
                    singleDataTransfer(decoded);
                break;
             }        
        } 

        if (skipToNext) continue;

        //Decode the fetched instruction.
        currentInstType = decode(fetched);
        decoded = fetched;

        //Fetch new instruction
        fetched = getFromMemory(memory, *PC);

        //Increment PC to next instruction
        *PC += 4; 

    }

    printRegisters(registers);
    printNonZeroMemory();

    free(memory);
    free(registers);
}

static void loadBinary(const char *filepath) {

    // should malloc fail, print error message and return failure
    if (memory == NULL) {
        perror("malloc for memory failed");
        exit(EXIT_FAILURE);
    }

    FILE *fp;
    unsigned char k;
    int count = 0;

    if ((fp = fopen(filepath, "rb")) == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }


    while(1 == fread(&k,sizeof(k),1,fp)) {
        memory[count] = k;
        count ++;
    }

    fclose(fp);
}

static int checkCondition(uint32_t instruction) {

    //CSPR register values
    uint32_t cspr = registers[16];
    uint32_t N = mask1 & (cspr >> 31);
    uint32_t Z = mask1 & (cspr >> 30);
    uint32_t V = mask1 & (cspr >> 28);

    uint32_t condNumber = mask4 & (instruction >> 28);
    int condflag = 0;

    switch(condNumber) {

        case(0):
            // eq
            condflag = Z;
            break;
        case(1):
            // ne
            condflag = 1-Z;
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
    return condflag;
}

static enum instructionType decode(uint32_t instruction){ //Instruction is 32 bits

    /*Idea for this function:
    * shift the instruction so that bits 27 - 25 are now bits 2 - 0.
    * perform bitwise and with a mask.
    * the result is different depending on the instruction.
    * explained further in switch comments
    */
    enum instructionType inst;       
    const uint32_t mask3 =  7; //Binary equivelant = ... 0000 0111
    const uint32_t mask4 = 15; // Binary equivalant = ...0000 1111
    uint32_t shiftInst = instruction >> 25;

    switch(mask3 & shiftInst){

        case(0):
            // if result is (000) then could be either data processing or multiply. 
            // further examiination of bits 7 -  4 will determine the instruction type.
            shiftInst = instruction >> 4;
            if((mask4 & shiftInst) == 9){
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

// Functions for testing!!!

void print8Bits(uint8_t x) {
    
    int i;

    uint8_t mask = 1 << 7;

    for(i=0; i<8; ++i) {
        if((x & mask) == 0){
            printf("0");
        }else {
            printf("1");
        }
        mask >>= 1;
    }

    printf("\n");
}

void printNonZeroMemory(void) {
    printf("Non-zero memory:\n");
    for (int i = 0; i< 65536; i+=4) {
        uint32_t memoryContent = getFromMemory(memory, i);
        if (memoryContent != 0) {
            printf("0x%08x: 0x%08x\n", i, memoryContent);
        }
    }
}

