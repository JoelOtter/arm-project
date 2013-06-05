#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#define not_found_exception "Not found"
#define memory_address_not_found_exception 404


/*
Symbol table based on a linked listy kinda thing
Stores String int32_t pairs

Can be used for
    (Label, Memory address)
    (Opcode/condition mnemonics, opcodes/conditions themselves)

Have not implemented delete - seems uneccessary for assembler

*/



struct table {
    struct table_elem *first;
    int32_t size;
};


struct table_elem {
    char *label;
    int32_t memory_address;
    struct table_elem *prev;
    struct table_elem *next;
};

struct table t;

struct table_elem *table_alloc_elem(void) {
    //allocates memory for a new 'node/entry/elem' in the table
    struct table_elem *elem = malloc(sizeof(struct table_elem));
    if ( elem == NULL ) {
        exit(EXIT_FAILURE);
    }
    return elem;
}


void table_constructor(struct table *table) {

    //initialises table
    table->first         = table_alloc_elem();
    table->first->prev   = NULL;
    table->first->next   = NULL;
}

/* commented out as it only worked for table t, not for any table
void create_table(void) {

    table_constructor(&t);

}
*/

void insert_elem(struct table *table, char *label, int32_t memory_address) {

    //allocate memory to new item and add its values
    struct table_elem *new_elem = table_alloc_elem();
    new_elem->label = label;
    new_elem->memory_address = memory_address;

    //inserting new item at the front always
    new_elem->next = table->first;
    new_elem->prev = NULL;
    table->first->prev = new_elem;
    table->first = new_elem;

    //increment size of table
    table->size++;

}


char* search_for_label(struct table *table, int32_t memory_address) {
 
    // retrives the label given the memory address

    int i = table->size;
    struct table_elem *curr;
    curr = table->first;

    while ( i > 0 ) {
        if ( memory_address == (curr->memory_address) ) {
            return (curr->label);
        } 
        curr = curr->next;
        i--;
    }

    return not_found_exception;

}


int32_t search_for_memory_address(struct table *table, char *label) {

    //retrives the memory address given the label

    int i = table->size;
    struct table_elem *curr;
    curr = table->first;

    while ( i > 0 ) {
        if ( strcmp(label, curr->label) == 0 ) { // if strings are equal this outputs 0 for some reason 
            return (curr->memory_address);
        } 
        curr = curr->next;
        i--;
    }

    return memory_address_not_found_exception;

}

 /*
int main(int argc, char **argv) {

    create_table();

    insert_elem(&t, "Denise", 100);
    insert_elem(&t, "Zues", 99);
    insert_elem(&t, "Joel", 5);
    insert_elem(&t, "Jack", 1);
    
    printf("%i\n", search_for_memory_address(&t, "Denise")); //100
    printf("%i\n", search_for_memory_address(&t, "Zues")); // 99
    printf("%i\n", search_for_memory_address(&t, "Joe")); //testerror (404)
    printf("%s\n", search_for_label(&t, 1)); // jack
    printf("%s\n", search_for_label(&t, 101)); //testerror (Not found)
    

    return 0;

    

}

*/

















