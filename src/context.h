#ifndef FRED_CIE_CONTEXT_H
#define FRED_CIE_CONTEXT_H

#include <ucontext.h>
#include <valgrind/valgrind.h>

enum TTHREAD_STATE{
    ACTIVE, SLEEPING
};


struct tthread_t{
    ucontext_t _context;
    void** _retval;

    enum TTHREAD_STATE _state;
    int _join_wait;

    //For memory purposes
    int _valgrind_stackid;
};

#endif //FRED_CIE_CONTEXT_H
