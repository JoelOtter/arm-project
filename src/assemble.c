#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "symboltableadt.h"


table symbol_table;
table instruction_table;

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
    
    /*int i = 0;
    char *nmonic = malloc(10);
    while(instruction[i] != ' '){
        nmonic[i] = instruction[i];
        i++;
    }
    
    return nmonic;
    */
}

int main(int argc, char **argv) {
    assert(argc == 3);
    
    char *srcpath = argv[1];
    char *destpath = argv[2];
    
    char *nmonic;
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
        
    while(fgets(currLine, size, fp) != NULL){
        
     /* int i = 0;
      
      nmonic = getNmonic(currLine);
      if (nmonic[strlen(nmonic) - 1] == ':') {
          printf("symbol table insertion, 
          insert_elem(&symbol_table, nmonic, i);
      } else {
          insert_elem(&instruction_table, currLine,i++);
      }
      
      printf("%s\n", nmonic);
    */
    // this is is purely to remove the \n that fgets adds to end of currLine
     if (currLine[strlen(currLine) - 1] == '\n') currLine[strlen(currLine) - 1] = '\0';
      if (currLine[strlen(currLine) -1] == ':') {
          printf("symbol table ins, (%s), %d\n", currLine, i*4);
          insert_elem(&symbol_table, currLine, i);
      } else {
          printf("instruction tab ins, (%s), %d\n", currLine, i*4);
          insert_elem(&instruction_table, currLine,i++);
      }
      
     //printf("%s\n", currLine);
     
    }
    
        
    fclose(fp);


/*    char *toWrite = malloc(4 * sizeof(char));
    toWrite[0] = 1;
    toWrite[1] = 2;
    toWrite[2] = 3;
    toWrite[3] = 4;
    writeBinary(destpath, toWrite);
*/
}
