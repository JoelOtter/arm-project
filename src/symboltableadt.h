#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

#ifndef ST_H
#define ST_H


typedef struct table {
    struct table_elem *first;
    int32_t size;
}table;


typedef struct table_elem {
    char *label;
    int32_t memory_address;
    struct table_elem *prev;
    struct table_elem *next;
}table_elem;


void table_constructor(struct table *table);
void insert_elem(struct table *table, char *label, int32_t memory_address);
char* search_for_label(struct table *table, int32_t memory_address);
int32_t search_for_memory_address(struct table *table, char *label);



#endif
