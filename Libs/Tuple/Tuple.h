#ifndef TUPLE_H
#define TUPLE_H
#include "stdio.h"
#include "stdlib.h"
#include <string.h>

typedef struct Tuple {
    void **data;
    int size;
} Tuple;

Tuple *Tuple_create(int size);
Tuple *Tuple_add(Tuple *t, void *data);
void *Tuple_get(Tuple *t, int index);
void Tuple_set(Tuple *t, int index, void *data);
void Tuple_free(Tuple *t);
void Tuple_print(Tuple *t);


#endif