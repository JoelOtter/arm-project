#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "symboltableadt.h"
#define not_found_exception "Not found"
#define memory_address_not_found_exception 404

struct table t;

struct table_elem *table_alloc_elem(void) {
    //allocates memory for a new 'node/entry/elem' in the table
    struct table_elem *elem = malloc(sizeof(struct table_elem));
    elem->label = malloc(512);
    if ( elem == NULL ) {
        exit(EXIT_FAILURE);
    }
    return elem;
}

void table_free_elem(struct table_elem *elem) {
    free(elem->label);
    free(elem);
}


void table_constructor(struct table *table) {
    //initialises table
    table->header         = table_alloc_elem();
    table->footer         = table_alloc_elem();
    table->header->prev   = NULL;
    table->footer->next   = NULL;
    table->header->next   = table->footer;
    table->footer->prev   = table->header;

}

static void insert_elem(struct table *table, table_iter iter, char *label, uint32_t memory_address) {

    //allocate memory to new item and add its values
    struct table_elem *new_elem = table_alloc_elem();
    new_elem->memory_address = memory_address;
    strcpy(new_elem->label,label);

    new_elem->prev = iter->prev;
    new_elem->next = iter;
    
    iter->prev->next = new_elem;
    iter->prev = new_elem;

    printf("\n");

    //increment size of table
    table->size++;

}

table_iter table_begin(struct table *table){
    return table->header->next;
}

table_iter table_end(struct table *table){
    return table->footer;
}

table_iter table_iter_next(table_iter iter){
    return iter->next;
}

static int table_is_internal(table_iter iter){
    return iter->prev != NULL && iter->next != NULL;
}

char* table_iter_label(table_iter iter){
    assert(table_is_internal(iter));
    return(iter->label);
}

uint32_t table_iter_memory_address(table_iter iter){
    assert(table_is_internal(iter));
    return(iter->memory_address);
}

void table_insert_front(struct table *table, char *label, uint32_t memory_address){
    insert_elem(table, table_begin(table), label, memory_address);
}

void table_insert_end(struct table *table, char *label, uint32_t memory_address){
    insert_elem(table, table_end(table), label, memory_address);    
}

char* get_label(struct table *table, uint32_t memory_address) {
 
    // retrives the label given the memory address

    for(table_iter iter = table_begin(table); iter != table_end(table); iter = table_iter_next(iter)){
        if(table_iter_memory_address(iter) == memory_address){
            return iter->label;
        }
    }

    return not_found_exception;

}


int32_t get_memory_address(struct table *table, char *label) {

    //retrives the memory address given the label

    for(table_iter iter = table_begin(table); iter != table_end(table); iter = table_iter_next(iter)){
        if(!(strcmp(table_iter_label(iter), label))){
            return iter->memory_address;
        }
    }

    return memory_address_not_found_exception;

}

void table_destroy(struct table *table){
    struct table_elem *elem = table->header;
    while(elem != NULL){
        struct table_elem *next = elem->next;
        table_free_elem(elem);
        elem = next;
    }
}

void print_table_elem(struct table_elem *table_elem) {

    printf("[ Label: %s \t Address: %i ]\n", table_elem->label, table_elem->memory_address);

}