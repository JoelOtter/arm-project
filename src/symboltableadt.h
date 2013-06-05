#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

#ifndef ST_H
#define ST_H


typedef struct table {
    struct tableElem *first;
    int32_t size;
}table;


typedef struct tableElem {
    char *label;
    int32_t memoryAddress;
    struct tableElem *prev;
    struct tableElem *next;
}tableElem;


void tableConstructor(struct table *table);
void insertElem(struct table *table, char *label, int32_t memoryAddress);
char* searchForLabel(struct table *table, int32_t memoryAddress);
int32_t searchForMemoryAddress(struct table *table, char *label);



#endif
