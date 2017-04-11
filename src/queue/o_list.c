#include "o_list.h"

struct node *createnode(void *data) {
    struct node *newNode = malloc(sizeof(struct node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

struct list *emptylist() {
    struct list *list = malloc(sizeof(struct list));
    list->head = NULL;
    return list;
}

void add(void *data, struct list *list) {
    struct node *current = NULL;
    if (list->head == NULL) {
        list->head = createnode(data);
    } else {
        current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = createnode(data);
    }
}

void delete(void *data, struct list *list) {
    struct node *current = list->head;
    struct node *previous = current;
    while (current != NULL) {
        if (current->data == data) {
            previous->next = current->next;
            if (current == list->head)
                list->head = current->next;
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

void delete_node(struct node *node, struct list *list) {
    struct node *current = list->head;
    struct node *previous = current;
    while (current != NULL) {
        if (current == node) {
            previous->next = current->next;
            if (current == list->head)
                list->head = current->next;
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

int hasNext(struct node *node) {
    return node->next == NULL;
}

void destroy(struct list *list) {
    struct node *current = list->head;
    struct node *next = current;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

void find(struct list * list, void * element){
    struct node *current = list->head;
    while (current != NULL) {
        if (current->data == element)
          return 1;

        current = current->next;;
    }
  return 0;
}
