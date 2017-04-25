#include "o_queue.h"

int queue__push_back(void* item){
    struct entry* en = malloc(sizeof(struct entry));
    en->item = item;
    TAILQ_INSERT_TAIL(&head, en, entries);

    //What should we return else ?? é_è
    return 1;
}

void* queue__pop(){
    struct entry* en = TAILQ_FIRST(&head);
    TAILQ_REMOVE(&head, en, entries);
    return en->item;
}

void* queue__first(){
    return TAILQ_FIRST(&head) == NULL ? NULL : TAILQ_FIRST(&head)->item;
}

void* queue__second(){

    return TAILQ_NEXT(TAILQ_FIRST(&head), entries) == NULL ? NULL : TAILQ_NEXT(TAILQ_FIRST(&head), entries)->item;
}

int queue__empty(){
    return TAILQ_EMPTY(&head);
}

void queue__init(){
    TAILQ_INIT(&head);
}
