#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "symboltableadt.h"


table symbolTable;
table instructionTable;

void writeBinary(char *path, uint32_t write){
    //This takes one uint32_t at a time, and appends it to the
    //end of the binary file in little-endian format.
    FILE *fp;
    fp = fopen(path, "ab");
    /* //This is conversion code, which apparently we don't
    //need - fwrite writes in little endian!
    uint32_t toWrite = 0;
    uint32_t mask8 = 255;
    toWrite += ((write >> 24) & mask8);
    toWrite += (((write >> 16) & mask8) << 8);
    toWrite += (((write >> 8) & mask8) << 16);
    toWrite += ((write & mask8) << 24);
    fwrite(&write, sizeof(uint32_t), 1, fp);
    fwrite(&toWrite, sizeof(uint32_t), 1, fp);*/
    fwrite(&write, sizeof(uint32_t), 1, fp);
    fclose(fp);
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
    
    tableConstructor(&symbolTable);
    tableConstructor(&instructionTable);
    
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
          insertElem(&symbolTable, nmonic, i);
      } else {
          insertElem(&instructionTable, currLine,i++);
      }
      
      printf("%s\n", nmonic);
    */
    // this is is purely to remove the \n that fgets adds to end of currLine
    if (currLine[strlen(currLine) - 1] == '\n') currLine[strlen(currLine) - 1] = '\0';
      if (currLine[strlen(currLine) -1] == ':') {
          printf("symbol table ins, (%s), %d\n", currLine, i*4);
          insertElem(&symbolTable, currLine, i);
      } else {
          printf("instruction tab ins, (%s), %d\n", currLine, i*4);
          insertElem(&instructionTable, currLine,i++);
      }
      
     //printf("%s\n", currLine);
     
    }
    
        
    fclose(fp);
}
