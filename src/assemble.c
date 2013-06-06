#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "symboltableadt.h"
#include "ass_multiply.h"

void writeBinary(char *path, uint32_t *write){
    FILE *fp;
    fp = fopen(path, "ab");
    fwrite(&write, sizeof(uint32_t), 1, fp);
}

enum instructionType    {DATA_PROCESSING,
                         MULTIPLY,
                         SINGLE_DATA_TRANSFER,
                         BRANCH,
                         SPECIAL};

//needed for getInstruction type
char * dataProcessingOpcodes[] = { "add", "sub", "rsb", "and", "eor", "orr",
                         "mov", "tst", "teq", "cmp" };

//needed for getInstruction type
int isElemOf(char *searchString, char * list[] ) {


    int len = sizeof(searchString)/sizeof(searchString[0]);
    int i;

    for(i = 0; i < len; ++i) {
        if(!strcmp(list[i], searchString)){
            return 1;
        }
    }

    return 0;

}

enum instructionType getInstructionType(char *opcode){ 

    enum instructionType inst;       
    char first = opcode[0];

    if ( isElemOf(opcode, dataProcessingOpcodes) ) {
        inst = DATA_PROCESSING;
    } else if (first == 'm')  {
        inst = MULTIPLY;
    } else if ( !strcmp(opcode, "ldr") ||  !strcmp(opcode, "str") ) {
        inst = SINGLE_DATA_TRANSFER;
    } else if ( first == 'b' ) {
        inst = BRANCH;
    } else if ( !strcmp(opcode, "lsl") ||  !strcmp(opcode, "andeq") ) {
        inst = SPECIAL;
    }

   return inst;

}  

int main(int argc, char **argv) {
    assert(argc == 3);
    
    char *srcpath = argv[1];
   // char *destpath = argv[2];
    
   // char *nmonic;
    int size = 512;
    char currLine[size];
    FILE *fp;

    table symbol_table;
    table instruction_table;
    
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
            printf("symbol table ins, (%s), %d\n", currLine, i);
            table_insert_end(&symbol_table, currLine, i);
        } else {
            printf("instruction table ins, (%s), %d\n", currLine, i);
            table_insert_end(&instruction_table, currLine,i);
            i+=4;
        }     
    }

    char nmonic[10];
    char rest[50];
    //Need to add checks to strcpy, check the char array sizes!
    for(table_iter iter = table_begin(&instruction_table); iter != table_end(&instruction_table); iter = table_iter_next(iter)){
        strcpy(currLine, table_iter_label(iter));
        sscanf(currLine, "%s %s", nmonic, rest);
        printf("Nmonic = %s\n", nmonic);
        printf("Rest = %s\n", rest);
        
        enum instructionType inst;

        inst = getInstructionType(nmonic);

        switch(inst){
            case(DATA_PROCESSING): printf("DATA_PROCESSING\n"); break;
            case(MULTIPLY): printf("MULTIPLY\n"); break;
            case(SINGLE_DATA_TRANSFER): printf("SINGLE_DATA_TRANSFER\n"); break;
            case(BRANCH): printf("BRANCH\n"); break;
            case(SPECIAL): printf("SPECIAL\n"); break;
        }    
 
    }

    printf("\n");

    for(table_iter iter = table_begin(&symbol_table); iter != table_end(&symbol_table); iter = table_iter_next(iter)){
        print_table_elem(iter);
    }



    //Now for the second pass
    //Iterate through instruction table
    //Split instruction into nmonic and rest of instruction
    //Work out appropriate toBinary function(data Process etc)
    //This will return the binary
    //Put into array?


    //above iteration has finished
    //Send array to joels function which will turn into little endian 
    //and then output

    //Alternatively, change to little endian before adding to array?
    
        
    fclose(fp);
}
