#include <stdlib.h>
#include <stdio.h>
#include <valgrind/valgrind.h>

#include "context.h"
#include "thread.h"

struct tthread_t;

struct tthread_t *tthread_init() {
    struct tthread_t *tthread = malloc(sizeof(struct tthread_t));
    tthread->_state = ACTIVE;
    tthread->_waiting_thread_nbr = 0;
    tthread->_waiting_threads = emptylist();
    tthread->_retval = NULL;

    return tthread;
}


void tthread_destroy(struct tthread_t *tthread) {
    destroy(tthread->_waiting_threads);
    free(tthread->_waiting_threads);
    if(tthread->_watchdog_args != NULL)
        free(tthread->_watchdog_args);
    free(tthread->_context.uc_stack.ss_sp);
    //free(tthread->name);
    VALGRIND_STACK_DEREGISTER(tthread->_valgrind_stackid);
    free(tthread);
}


int cxt_watchdog(void *args) {
    struct watchdog_args *arguments = (struct watchdog_args *) args;
    void *value = arguments->_func(arguments->_func_arg);
    thread_exit(value);
    return SUCCESS;
}


void tthread__end_program(void *last_address) {
    tthread_destroy(last_address);

    exit(0);
}
