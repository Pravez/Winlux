#ifndef FRED_CIE_CONTEXT_H
#define FRED_CIE_CONTEXT_H

#include <ucontext.h>

struct context_t{
    ucontext_t _context;
    void** _retval;
};

#endif //FRED_CIE_CONTEXT_H
