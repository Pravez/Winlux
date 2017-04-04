#ifndef FRED_CIE_CONTEXT_H
#define FRED_CIE_CONTEXT_H

#include <ucontext.h>
#include <valgrind/valgrind.h>


struct tthread_t{
    ucontext_t _context;
    void** _retval;

    //For memory purposes
    int _valgrind_stackid;
};

#endif //FRED_CIE_CONTEXT_H
