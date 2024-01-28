#include "Tuple.h"

Tuple *Tuple_create(int size)
{
    Tuple *t = malloc(sizeof(Tuple));
    t->data = malloc(sizeof(void *) * size);
    t->size = size;
    for (int i = 0; i < size; i++)
        t->data[i] = NULL;
    return t;
}

Tuple *Tuple_add(Tuple *t, void *data)
{
    t->data = realloc(t->data, sizeof(void *) * (t->size + 1));
    t->data[t->size] = data;
    t->size++;
    return t;
}
void *Tuple_get(Tuple *t, int index)
{
    return t->data[index];
}
void Tuple_set(Tuple *t, int index, void *data)
{
    t->data[index] = data;
}
void Tuple_free(Tuple *t)
{
    free(t->data);
    free(t);
}
void Tuple_print(Tuple *t)
{
    printf("Tuple(%d) = {", t->size);
    for (int i = 0; i < t->size; i++)
    {
        printf("%p", t->data[i]);
        if (i < t->size - 1)
            printf(", ");
    }
    printf("}\n");
}