#include <stdlib.h>
#include <stdio.h>
#include <valgrind/valgrind.h>

#include "context.h"


struct tthread_t * tthread_init() {
    struct tthread_t* tthread = malloc(sizeof(struct tthread_t));
    tthread->_state = SLEEPING;
    tthread->_join_wait = 0;

    return tthread;
}




void tthread_destroy(struct tthread_t * tthread) {
  free(tthread);
  VALGRIND_STACK_DEREGISTER(tthread->_valgrind_stackid);
}


int cxt_watchdog(struct watchdog_args * args) {  
  args->_thread->retval = &(args->_func(args->_func_arg));

  return SUCCESS;
}
