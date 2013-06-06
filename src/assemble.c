#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "symboltableadt.h"

void writeBinary(char *path, uint32_t *write){
    FILE *fp;
    fp = fopen(path, "ab");
    fwrite(&write, sizeof(uint32_t), 1, fp);
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
        //Call denise's function on nmonic to test instrucion type.
        // Some kind of switch statment and send!        
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
