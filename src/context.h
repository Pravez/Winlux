#ifndef FRED_CIE_CONTEXT_H
#define FRED_CIE_CONTEXT_H

#include <ucontext.h>
#include <signal.h>
#include <valgrind/valgrind.h>

#define STACK_SIZE 64 * 1024


struct tthread_t {
  ucontext_t _context;
  void ** _retval;
  
  //For memory purposes
  int _valgrind_stackid;
};


struct watchdog_args {
  tthread_t * _thread;
  tthread_t * _calling;
  void * (*_func)(void *);
  void * _func_arg;
};


struct tthread_t* tthread_init(); 
void tthread_destroy(struct tthread_t * tthread);


#endif //FRED_CIE_CONTEXT_H
