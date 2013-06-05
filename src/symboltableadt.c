#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#define notFoundException "Not found"
#define memoryAddressNotFoundException 404


/*
Symbol table based on a linked listy kinda thing
Stores String int32_t pairs

Can be used for
    (Label, Memory address)
    (Opcode/condition mnemonics, opcodes/conditions themselves)

Have not implemented delete - seems uneccessary for assembler

*/



struct table {
    struct tableElem *first;
    int32_t size;
};


struct tableElem {
    char *label;
    int32_t memoryAddress;
    struct tableElem *prev;
    struct tableElem *next;
};

struct table t;

struct tableElem *tableAllocElem(void) {
    //allocates memory for a new 'node/entry/elem' in the table
    struct tableElem *elem = malloc(sizeof(struct tableElem));
    if ( elem == NULL ) {
        exit(EXIT_FAILURE);
    }
    return elem;
}


void tableConstructor(struct table *table) {

    //initialises table
    table->first         = tableAllocElem();
    table->first->prev   = NULL;
    table->first->next   = NULL;
}

/* commented out as it only worked for table t, not for any table
void create_table(void) {

    tableConstructor(&t);

}
*/

void insert_elem(struct table *table, char *label, int32_t memoryAddress) {

    //allocate memory to new item and add its values
    struct tableElem *newElem = tableAllocElem();
    newElem->label = label;
    newElem->memoryAddress = memoryAddress;

    //inserting new item at the front always
    newElem->next = table->first;
    newElem->prev = NULL;
    table->first->prev = newElem;
    table->first = newElem;

    //increment size of table
    table->size++;

}


char* searchForLabel(struct table *table, int32_t memoryAddress) {
 
    // retrives the label given the memory address

    int i = table->size;
    struct tableElem *curr;
    curr = table->first;

    while ( i > 0 ) {
        if ( memoryAddress == (curr->memoryAddress) ) {
            return (curr->label);
        } 
        curr = curr->next;
        i--;
    }

    return notFoundException;

}


int32_t searchForMemoryAddress(struct table *table, char *label) {

    //retrives the memory address given the label

    int i = table->size;
    struct tableElem *curr;
    curr = table->first;

    while ( i > 0 ) {
        if ( strcmp(label, curr->label) == 0 ) { // if strings are equal this outputs 0 for some reason 
            return (curr->memoryAddress);
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
    
    printf("%i\n", searchForMemoryAddress(&t, "Denise")); //100
    printf("%i\n", searchForMemoryAddress(&t, "Zues")); // 99
    printf("%i\n", searchForMemoryAddress(&t, "Joe")); //testerror (404)
    printf("%s\n", searchForLabel(&t, 1)); // jack
    printf("%s\n", searchForLabel(&t, 101)); //testerror (Not found)
    

    return 0;

    

}

*/

















