#ifndef FRED_CIE_CONTEXT_H
#define FRED_CIE_CONTEXT_H

#include <ucontext.h>
#include <signal.h>
#include <valgrind/valgrind.h>

#include "queue/o_list.h"

#define STACK_SIZE 64 * 102
#define SUCCESS 0
#define FAILED -1
mlkj

enum TTHREAD_STATE {
    ACTIVE, SLEEPING, DEAD
};

struct tthread_t {
    ucontext_t _context;
    void *_retval;

    enum TTHREAD_STATE _state;
    struct list *_waiting_threads;
    int _waiting_thread_nbr;

    unsigned short int _priority;

    struct watchdog_args * _watchdog_args;

    //For memory purposes
    int _valgrind_stackid;

    //For debug purposes
    char * name;
};


struct watchdog_args {
    struct tthread_t *_thread;

    void *(*_func)(void *);

    void *_func_arg;
};


struct tthread_t *tthread_init();

void tthread_destroy(struct tthread_t *tthread);

int cxt_watchdog(void *args);

void tthread__end_program(void *last_address);

#endif //FRED_CIE_CONTEXT_H
