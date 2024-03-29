#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ass_data_transfer.h"
#include "ass_symbol_table.h"
#include "library.h"
#include "ass_data_processing.h"
#include "ass_multiply.h"
#include "ass_branch.h"
#include "ass_special.h"

int line_number = 0;
int wrong = 0;

enum instruction_type   {DATA_PROCESSING,
                         MULTIPLY,
                         SINGLE_DATA_TRANSFER,
                         BRANCH,
                         SPECIAL};
                         
                         
table symbol_table;
table instruction_table;

uint32_t *add_afters;
int debug;

void write_binary(char *path, uint32_t write){
    FILE *fp;
    fp = fopen(path, "ab");
    fwrite(&write, sizeof(uint32_t), 1, fp);
    fclose(fp);
}

//needed for getInstruction type
char *data_processing_op_codes[] = { "add", "sub", "rsb", "and", "eor", "orr", "mov", "tst", "teq", "cmp" };
unsigned long int data_p_size = (sizeof(data_processing_op_codes)/sizeof(data_processing_op_codes[0]));

enum instruction_type get_instruction_type(char *opcode){ //TODO

    enum instruction_type inst;       
    char first = opcode[0];

    if ( ! is_valid_instruction(opcode) ) {
        wrong = 1;
        line_number = (line_number/4) + 1;
        suggest(opcode, line_number); //TODO from the library
    }

    if (!strcmp(opcode, "mul") || !strcmp(opcode, "mla") )  {
        inst = MULTIPLY;
    } else if ( !strcmp(opcode, "ldr") ||  !strcmp(opcode, "str") ) {
        inst = SINGLE_DATA_TRANSFER;
    } else if ( first == 'b' ) {
        inst = BRANCH;
    } else if ( !strcmp(opcode, "lsl") ||  !strcmp(opcode, "andeq") ) {
        inst = SPECIAL;
    } else if ( is_elem_of(opcode, data_processing_op_codes, data_p_size) ) {
        inst = DATA_PROCESSING;
    } 
    
   return inst;

}  

int main(int argc, char **argv) {
    assert(argc == 3 ||  argc == 4);
    
    debug = (argc == 4);
    
    char *srcpath = argv[1];
    char *destpath = argv[2];

    //Clear the destination file if need be
    FILE *fd;
    char nothing = '\0';
    if ((fd = fopen(destpath, "w")) == NULL) {
            perror("Error opening debug_file!");
            exit(EXIT_FAILURE);
    }
    else{
        fwrite(&nothing, 0, 1, fd);
        fclose(fd);
    }
    
    overwrite_debug_file();
    if ((fd = fopen("debug_suggestions", "w")) == NULL) {
            perror("Error opening debug_suggestions_file!");
            exit(EXIT_FAILURE);
    }
    else{
        fwrite(&nothing, 0, 1, fd);
        fclose(fd);
    }

    int size = 512;
    char curr_line[size];
    FILE *fp;
    
    table_constructor(&symbol_table);
    table_constructor(&instruction_table);
    
    if ((fp = fopen(srcpath, "r")) == NULL) {
            perror("Error opening source file!");
            exit(EXIT_FAILURE);
    }
            
    int i = 0;
   
    //This loop is performing the first pass.    
    while(fgets(curr_line, size, fp) != NULL){
    // this is is purely to remove the \n that fgets adds to end of curr_line
        if (curr_line[strlen(curr_line) - 1] == '\n'){
            curr_line[strlen(curr_line) - 1] = '\0';    
        }

        //If the current line is a label, insert it into symbol table
        if (curr_line[strlen(curr_line) -1] == ':') {
            curr_line[strlen(curr_line)-1] = 0;
            table_insert_end(&symbol_table, curr_line, i);
        } else if(curr_line[0] == ' ' || curr_line[0] == 0){
            continue;
        } else {
            table_insert_end(&instruction_table, curr_line,i);
            i+=4;
        }     
    }

    fclose(fp);

    add_afters = calloc(4 * (&instruction_table)->size, sizeof(uint32_t));
    enum instruction_type inst;
    
    uint32_t result = 0;
    int count = 0;
    
    for(table_iter iter = table_begin(&instruction_table); iter != table_end(&instruction_table); iter = table_iter_next(iter)){

        strcpy(curr_line, table_iter_label(iter));
        line_number = table_iter_memory_address(iter); //TODO
        char *mnemonic = get_mnemonic(curr_line);
        inst = get_instruction_type(mnemonic); //TODO

        switch(inst){
            case(DATA_PROCESSING): 
                result = ass_data_processing(curr_line);
                break;
            case(MULTIPLY):
                 result = ass_multiply(curr_line);
                break;
            case(SINGLE_DATA_TRANSFER):
                result = ass_data_transfer(curr_line, iter->memory_address);
                break;
            case(BRANCH):
                result = ass_branch(curr_line);
                break;
            case(SPECIAL):
                result = ass_special(curr_line);
                break;
        }    

        count++;
        write_binary(destpath, result);
        free(mnemonic);
    }

    for(int i=0; add_afters[i] != 0; i++){
        write_binary(destpath, add_afters[i]);
    }
    
    if (debug) {
        char cmd[100] = "";
        if (!wrong) {
            strcat(cmd, "./emulate ./\"");
            strcat(cmd, destpath);
            strcat(cmd, "\" debug");
            system(cmd);
        }
        memset(cmd, 0, 50);
        strcat(cmd, "python debugger.py \"");
        strcat(cmd, srcpath);
        strcat(cmd, "\" &");
        system(cmd);
    }

    table_destroy(&symbol_table);
    table_destroy(&instruction_table);
    free(add_afters);
}