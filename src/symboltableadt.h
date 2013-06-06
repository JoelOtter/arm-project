#ifndef ST_H
#define ST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>


typedef struct table {
    struct table_elem *header;
    struct table_elem *footer;
    int32_t size;
}table;


typedef struct table_elem {
    char *label;
    uint32_t memory_address;
    struct table_elem *prev;
    struct table_elem *next;
}table_elem;

typedef struct table_elem *table_iter;

void table_constructor(struct table *table);

char* get_label(struct table *table, uint32_t memory_address);

int32_t get_memory_address(struct table *table, char *label);

table_iter table_begin(struct table *table);

table_iter table_end(struct table *table);

table_iter table_iter_next(table_iter iter);

char* table_iter_label(table_iter iter);

uint32_t table_iter_memory_address(table_iter iter);

void table_insert_front(struct table *table, char *label, uint32_t memory_address);

void table_insert_end(struct table *table, char *label, uint32_t memory_address);

void table_destroy(struct table *table);

void print_table_elem(struct table_elem *table_elem);


#endif
