#ifndef FRED_CIE_O_LIST_H
#define FRED_CIE_O_LIST_H

#include <stdio.h>
#include <stdlib.h>

struct node {
    void* data;
    struct node * next;
};

struct list {
    struct node * head;
};

struct list * emptylist();
void add(void* data, struct list * list);
void delete(void* data, struct list * list);
void destroy(struct list * list);
int has_next(struct node* node);
void delete_node(struct node* node, struct list * list);
int find(struct list * list, void * elem);

#endif //FRED_CIE_O_LIST_H
