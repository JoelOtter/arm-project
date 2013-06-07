#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ass_data_transfer.h"
#include "symboltableadt.h"
#include "library.h"
#include "ass_data_processing.h"
#include "ass_multiply.h"
#include "ass_branch.h"
#include "ass_special.h"




enum instructionType    {DATA_PROCESSING,
                         MULTIPLY,
                         SINGLE_DATA_TRANSFER,
                         BRANCH,
                         SPECIAL};
                         
                         
table symbol_table;
table instruction_table;

uint32_t *add_afters;

void writeBinary(char *path, uint32_t write){
    FILE *fp;
    fp = fopen(path, "ab");
    fwrite(&write, sizeof(uint32_t), 1, fp);
    fclose(fp);
}

//needed for getInstruction type
char *dataProcessingOpcodes[] = { "add", "sub", "rsb", "and", "eor", "orr", "mov", "tst", "teq", "cmp" };
unsigned long int dataPSize = (sizeof(dataProcessingOpcodes)/sizeof(dataProcessingOpcodes[0]));

enum instructionType getInstructionType(char *opcode){ 

    enum instructionType inst;       
    char first = opcode[0];

    if (!strcmp(opcode, "mul") || !strcmp(opcode, "mla") )  {
        inst = MULTIPLY;
    } else if ( !strcmp(opcode, "ldr") ||  !strcmp(opcode, "str") ) {
        inst = SINGLE_DATA_TRANSFER;
    } else if ( first == 'b' ) {
        inst = BRANCH;
    } else if ( !strcmp(opcode, "lsl") ||  !strcmp(opcode, "andeq") ) {
        inst = SPECIAL;
    } else if ( isElemOf(opcode, dataProcessingOpcodes, dataPSize) ) { //|| !strcmp(opcode, "cmp") || !strcmp(opcode, "teq") ) {
        inst = DATA_PROCESSING;
    } 

   return inst;

}  

int main(int argc, char **argv) {
    assert(argc == 3);
    
    char *srcpath = argv[1];
    char *destpath = argv[2];
    
   // char *nmonic;
    int size = 512;
    char currLine[size];
    FILE *fp;

    
    table_constructor(&symbol_table);
    table_constructor(&instruction_table);
    
    if ((fp = fopen(srcpath, "r")) == NULL) {
            perror("Error opening file.txt!");
            exit(EXIT_FAILURE);
    }
            
    int i = 0;

    
    //This loop is performing the first pass.    
    while(fgets(currLine, size, fp) != NULL){
    // this is is purely to remove the \n that fgets adds to end of currLine
        if (currLine[strlen(currLine) - 1] == '\n'){
            currLine[strlen(currLine) - 1] = '\0';    
        }

        //If the current line is a label, insert it into symbol table
        if (currLine[strlen(currLine) -1] == ':') {
            currLine[strlen(currLine)-1] = 0;
            printf("symbol table ins, (%s), %d\n", currLine, i);
            table_insert_end(&symbol_table, currLine, i);
        } else {
            printf("instruction table ins, (%s), %d\n", currLine, i);
            table_insert_end(&instruction_table, currLine,i);
            i+=4;
        }     
    }

    fclose(fp);

    add_afters = calloc(4 * (&instruction_table)->size, sizeof(uint32_t));
    enum instructionType inst;
    
    uint32_t result = 0;
    int count = 0;
    
    for(table_iter iter = table_begin(&instruction_table); iter != table_end(&instruction_table); iter = table_iter_next(iter)){
    
    
        strcpy(currLine, table_iter_label(iter));
        inst = getInstructionType(get_mnemonic(currLine));

        switch(inst){
            case(DATA_PROCESSING): 
                printf("IterNO: %d data procesing %s \n", count, get_mnemonic(currLine));
                result = ass_data_processing(currLine);
                printf("%x\n", result);
                break;
            case(MULTIPLY):
                 result = ass_multiply(currLine);
                break;
            case(SINGLE_DATA_TRANSFER):
                result = ass_data_transfer(currLine, iter->memory_address);
                break;
            case(BRANCH):
                result = ass_branch(currLine);
                break;
            case(SPECIAL):
                result = ass_special(currLine);
                break;
        }    

        count++;
        //printf("result: %x\n", result);
        writeBinary(destpath, result);
 
    }

    for(int i=0; add_afters[i] != 0; i++){
        writeBinary(destpath, add_afters[i]);
    }

    printf("\n");
    
        
   
}
