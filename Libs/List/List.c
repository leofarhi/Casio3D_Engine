#include "List.h"
#include <stdio.h>
#include <stdlib.h>

/*
typedef struct List_node {
    struct List_node *next;
    struct List_node *prev;
    void *data;
} List_node;

typedef struct List {
    List_node *head;
    List_node *tail;
    int size;
} List;
*/

List *List_new() {
    List *l = malloc(sizeof(List));
    l->head = NULL;
    l->tail = NULL;
    l->size = 0;
    return l;
}
void List_add(List *l, void *data) {
    List_node *node = malloc(sizeof(List_node));
    node->data = data;
    node->next = NULL;
    node->prev = l->tail;
    if (l->tail != NULL) {
        l->tail->next = node;
    }
    l->tail = node;
    if (l->head == NULL) {
        l->head = node;
    }
    l->size++;
}
void List_remove(List *l, void *data) {
    List_node *node = l->head;
    while (node != NULL) {
        if (node->data == data) {
            if (node->prev != NULL) {
                node->prev->next = node->next;
            }
            if (node->next != NULL) {
                node->next->prev = node->prev;
            }
            if (node == l->head) {
                l->head = node->next;
            }
            if (node == l->tail) {
                l->tail = node->prev;
            }
            free(node);
            l->size--;
            return;
        }
        node = node->next;
    }
}
void* List_get(List *l, int index) {
    List_node *node = l->head;
    int i = 0;
    while (node != NULL) {
        if (i == index) {
            return node->data;
        }
        i++;
        node = node->next;
    }
    return NULL;
}
void List_free(List *l) {
    List_node *node = l->head;
    while (node != NULL) {
        List_node *next = node->next;
        free(node);
        node = next;
    }
    free(l);
}
void List_deleteAt(List *l, int index) {
    List_node *node = l->head;
    int i = 0;
    while (node != NULL) {
        if (i == index) {
            if (node->prev != NULL) {
                node->prev->next = node->next;
            }
            if (node->next != NULL) {
                node->next->prev = node->prev;
            }
            if (node == l->head) {
                l->head = node->next;
            }
            if (node == l->tail) {
                l->tail = node->prev;
            }
            free(node);
            l->size--;
            return;
        }
        i++;
        node = node->next;
    }
}
void List_insertAt(List *l, int index, void *data) {
    List_node *node = l->head;
    int i = 0;
    while (node != NULL) {
        if (i == index) {
            List_node *newNode = malloc(sizeof(List_node));
            newNode->data = data;
            newNode->next = node;
            newNode->prev = node->prev;
            if (node->prev != NULL) {
                node->prev->next = newNode;
            }
            node->prev = newNode;
            if (node == l->head) {
                l->head = newNode;
            }
            l->size++;
            return;
        }
        i++;
        node = node->next;
    }
}
void List_clear(List *l) {
    List_node *node = l->head;
    while (node != NULL) {
        List_node *next = node->next;
        free(node);
        node = next;
    }
    l->head = NULL;
    l->tail = NULL;
    l->size = 0;
}
int List_Lenght(List *l) {
    return l->size;
}
void List_print(List *l,char *format) {
    List_node *node = l->head;
    printf("[");
    while (node != NULL) {
        printf(format,node->data);
        if (node->next != NULL) {
            printf(",");
        }
        node = node->next;
    }
    printf("]\n");
}
int List_indexOf(List *l, void *data){
    List_node *node = l->head;
    int i = 0;
    while (node != NULL) {
        if (node->data == data) {
            return i;
        }
        i++;
        node = node->next;
    }
    return -1;
}

void** List_toArray(List *l){
    void **array = malloc(sizeof(void*)*l->size);
    List_node *node = l->head;
    int i = 0;
    while (node != NULL) {
        array[i] = node->data;
        i++;
        node = node->next;
    }
    return array;
}

void* List_pop(List *l)
{
    void *data = l->tail->data;
    List_node *node = l->tail;
    if (node->prev != NULL) {
        node->prev->next = node->next;
    }
    if (node == l->head) {
        l->head = node->next;
    }
    if (node == l->tail) {
        l->tail = node->prev;
    }
    free(node);
    l->size--;
    return data;
}

void* List_popFront(List *l)
{
    void *data = l->head->data;
    List_node *node = l->head;
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    if (node == l->head) {
        l->head = node->next;
    }
    if (node == l->tail) {
        l->tail = node->prev;
    }
    free(node);
    l->size--;
    return data;
}

int ForEach(List *l, List_node **next)
{
    if(*next == NULL)
    {
        *next = l->head;
    }
    else
    {
        *next = (*next)->next;
    }
    if(*next == NULL)
    {
        return 0;
    }
    return 1;
}