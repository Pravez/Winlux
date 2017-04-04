#ifndef FRED_CIE_O_QUEUE_H
#define FRED_CIE_O_QUEUE_H

#include <stdlib.h>
#include "queue.h"

TAILQ_HEAD(tailhead, entry) head;

struct tailhead *headp;
struct entry {
    TAILQ_ENTRY(entry) entries;
    void* item;
};

void queue__init();
int queue__push_back(void* item);
void* queue__pop();
void* queue__second();
void* queue__first();
int queue__empty();


#endif //FRED_CIE_O_QUEUE_H
