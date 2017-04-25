#include <stdlib.h>
#include <stdio.h>
#include <valgrind/valgrind.h>

#include "context.h"
#include "thread.h"


struct tthread_t *tthread_init() {
    struct tthread_t *tthread = malloc(sizeof(struct tthread_t));
    tthread->_state = ACTIVE;
    tthread->_waiting_thread_nbr = 0;
    tthread->_waiting_threads = emptylist();
    tthread->_retval = NULL;

    return tthread;
}

void tthread__free(struct tthread_t* thread){
    destroy(thread->_waiting_threads);
    //free(thread->_context.uc_link);
    //free(thread->_context.uc_stack.ss_sp);
}


void tthread_destroy(struct tthread_t * tthread) {
  destroy(tthread->_waiting_threads);
  free(tthread->name);
  free(tthread->_watchdog_args);
  free(tthread);
  VALGRIND_STACK_DEREGISTER(tthread->_valgrind_stackid);
}


int cxt_watchdog(void *args) {
    struct watchdog_args* arguments = (struct watchdog_args*) args;
    void * value = arguments->_func(arguments->_func_arg);
    free(arguments);
    thread_exit(value);
    return SUCCESS;
}
