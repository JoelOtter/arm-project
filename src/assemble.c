#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "symboltableadt.h"


void writeBinary(char *path, char *write){
    FILE *fp;
    int k = 3;
    int zeroHit = 0;
    int zero = 0;

    fp = fopen(path, "wb");
    while(1) {
        if (write[k] == 0){
            fwrite(&zero, sizeof(char), 1, fp);
            zeroHit = 1;
        }
        else {
            fwrite(&write[k], sizeof(write[k]), 1, fp);
        }
        if (k % 4 == 0 && zeroHit) break;
        else if (k % 4 == 0) k += 7;
        else --k;
    }
}

char* getNmonic(char *instruction){
    
    char *nmonic = malloc(15);
    sscanf(instruction, "%s", nmonic);
    return nmonic;
}

int main(int argc, char **argv) {
    assert(argc == 3);
    
    char *srcpath = argv[1];
    char *destpath = argv[2];
    
    char *nmonic;
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
        printf("symbol table ins, (%s), %d\n", currLine, i*4);
        table_insert_end(&symbol_table, currLine, i);
    } else {
        printf("instruction table ins, (%s), %d\n", currLine, i*4);
        table_insert_end(&instruction_table, currLine,i*4);
        i++;
    }
     //printf("%s\n", currLine);
     
    }

    for(table_iter iter = table_begin(&instruction_table); iter != table_end(&instruction_table); iter = table_iter_next(iter)){
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
