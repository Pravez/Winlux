#include <stdlib.h>
#include <stdio.h>
#include <valgrind/valgrind.h>
#include <time.h>
#include <unistd.h>

#include "context.h"
#include "thread.h"

#define TIMESLICE 4 //en ms

struct tthread_t;

struct tthread_t *tthread_init() {
    struct tthread_t *tthread = malloc(sizeof(struct tthread_t));
    tthread->_state = ACTIVE;
    tthread->_waiting_thread_nbr = 0;
    tthread->_waiting_threads = emptylist();
    tthread->_retval = NULL;
    tthread->_priority = 1;

    int timeslice = tthread->_priority * TIMESLICE;
    tthread->_timerspec.it_value.tv_sec = 0;
    tthread->_timerspec.it_value.tv_nsec = timeslice * 1000; //Nanoseconds to milliseconds
    tthread->_timerspec.it_interval.tv_sec = 0;
    tthread->_timerspec.it_interval.tv_nsec = timeslice * 1000;
    tthread->_sev.sigev_notify = SIGEV_SIGNAL;
    tthread->_sev.sigev_signo = SIGVTALRM;
    tthread->_sev.sigev_value.sival_ptr = &tthread->_timer;

    if(timer_create(CLOCK_THREAD_CPUTIME_ID, &tthread->_sev, &tthread->_timer) == -1){
        perror("Error creating timer");
        exit(1);
    }

    return tthread;
}

void tthread_destroy(struct tthread_t *tthread) {
    destroy(tthread->_waiting_threads);
    free(tthread->_waiting_threads);
    if(tthread->_watchdog_args != NULL)
        free(tthread->_watchdog_args);
    free(tthread->_context.uc_stack.ss_sp);
    timer_delete(&tthread->_timer);
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
