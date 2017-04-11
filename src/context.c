#include <stdlib.h>
#include <stdio.h>
#include <valgrind/valgrind.h>

#include "context.h"
#include "thread.h"


struct tthread_t *tthread_init() {
    struct tthread_t *tthread = malloc(sizeof(struct tthread_t));
    tthread->_state = SLEEPING;
    tthread->_waiting_thread_nbr = 0;
    tthread->_waiting_thread_nbr = 0;
    tthread->_waiting_threads = emptylist();
    tthread->_retval = NULL;

    return tthread;
}


void tthread_destroy(struct tthread_t *tthread) {
    destroy(tthread->_waiting_threads);
    free(tthread);
    VALGRIND_STACK_DEREGISTER(tthread->_valgrind_stackid);
}


int cxt_watchdog(struct watchdog_args *args) {
    thread_exit(args->_func(args->_func_arg));
    return SUCCESS;
}
