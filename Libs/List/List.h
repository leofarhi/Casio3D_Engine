#ifndef List_H
#define List_H

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

List *List_new();
void List_add(List *l, void *data);
void List_remove(List *l, void *data);
void* List_get(List *l, int index);
void List_free(List *l);
void List_deleteAt(List *l, int index);
void List_insertAt(List *l, int index, void *data);
void List_clear(List *l);
int List_Lenght(List *l);
void List_print(List *l,char *format);
int List_indexOf(List *l, void *data);
void** List_toArray(List *l);
void* List_pop(List *l);
void* List_popFront(List *l);

int ForEach(List *l, List_node **next);


#endif