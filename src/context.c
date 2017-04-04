#include <stdlib.h>
#include "context.h"


struct tthread_t* tthread_init(){
    struct tthread_t* tthread = malloc(sizeof(struct tthread));


    return tthread;
}

void tthread_destroy(struct tthread_t tthread){
    VALGRIND_STACK_DEREGISTER(tthread._valgrind_stackid);
}