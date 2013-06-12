#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "emu_data_transfer.h"
#include "emu_multiply.h"
#include "emu_data_process.h"
#include "emu_branch.h"
#include "emulate.h"
#include "library.h"

unsigned char *memory;
int32_t *registers;
int debug;

static void load_binary(const char *filepath);

static int check_condition(uint32_t instruction);

static enum instruction_type decode(uint32_t instruction);

void print_memory(void);

int main(int argc, char **argv) {

    memory = calloc(SIZE_OF_MEMORY, sizeof(char));
    registers = calloc(NUM_REGISTERS, sizeof(int32_t));

    assert(argc == 2 || argc == 3);

    if (argc == 3) {
        debug = 1;
        overwrite_debug_file();
    } else {
        debug = 0;
    }

    load_binary(argv[1]);
    int32_t *PC = &registers[15];
    *PC = 8;
    uint32_t fetched = get_from_memory(memory, 4);
    uint32_t decoded = get_from_memory(memory, 0);
    enum instruction_type current_inst_type = decode(decoded);
    int skip_to_next = 0;

    while(decoded != 0){

        skip_to_next = 0;

        // If condition is satisfied, execute current instruction!
        if(check_condition(decoded)){
            switch(current_inst_type){
                case(DATA_PROCESSING):
                    data_process(decoded);
                break;
                case(BRANCH):
                    branch(decoded);
                    fetched = get_from_memory(memory, *PC + 4);
                    decoded = get_from_memory(memory, *PC);
                    current_inst_type = decode(decoded);
                    *PC += 8;
                    skip_to_next = 1;
                break;
                case(MULTIPLY):
                    multiply(decoded);
                break;
                case(SINGLE_DATA_TRANSFER):
                    single_data_transfer(decoded);
                break;
             }        
        } 

        if (debug) print_to_debug(registers, memory);

        if (skip_to_next) continue;

        //Decode the fetched instruction.
        current_inst_type = decode(fetched);
        decoded = fetched;

        //Fetch new instruction
        fetched = get_from_memory(memory, *PC);

        //Increment PC to next instruction
        *PC += 4; 

    }
    if (!debug) {
        print_registers(registers);
        print_non_zero_memory(memory);
    }

    free(memory);
    free(registers);
}

static void load_binary(const char *filepath) {

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

static int check_condition(uint32_t instruction) {

    //CSPR register values
    uint32_t cspr = registers[16];
    uint32_t N = mask1 & (cspr >> 31);
    uint32_t Z = mask1 & (cspr >> 30);
    uint32_t V = mask1 & (cspr >> 28);

    uint32_t cond_number = mask4 & (instruction >> 28);
    int cond_flag = 0;

    switch(cond_number) {

        case(0):
            // eq
            cond_flag = Z;
            break;
        case(1):
            // ne
            cond_flag = 1-Z;
            break;
        case(10):
            // ge
            cond_flag = (N == V);
            break;
        case(11): 
            // lt
            cond_flag = (N != V);
            break;
        case(12):
            // gt
            cond_flag = ((Z == 0) && (N == V));
            break;
        case(13):
            //le
            cond_flag = ((Z == 1) || (N != V));
            break;
        case(14):
            // al (always 1)
            cond_flag = 1;
            break;
    }
    return cond_flag;
}

static enum instruction_type decode(uint32_t instruction){ //Instruction is 32 bits

    /*Idea for this function:
    * shift the instruction so that bits 27 - 25 are now bits 2 - 0.
    * perform bitwise and with a mask.
    * the result is different depending on the instruction.
    * explained further in switch comments
    */
    enum instruction_type inst;       
    const uint32_t mask3 =  7; //Binary equivelant = ... 0000 0111
    const uint32_t mask4 = 15; // Binary equivalant = ...0000 1111
    uint32_t shift_inst = instruction >> 25;

    switch(mask3 & shift_inst){

        case(0):
            // if result is (000) then could be either data processing or multiply. 
            // further examiination of bits 7 -  4 will determine the instruction type.
            shift_inst = instruction >> 4;
            if((mask4 & shift_inst) == 9){
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

void print_8_bits(uint8_t x) {
    
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
