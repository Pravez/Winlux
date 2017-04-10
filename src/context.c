#include <stdlib.h>
#include <stdio.h>
#include <valgrind/valgrind.h>

#include "context.h"

#define ERROR(msg) printf("\x1b[31;1mError:\x1b[0m %s\n", msg)
#define SUCCESS 1
#define FAILED 0


struct tthread_t* tthread_init(){
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
  int res = getcontext(&args->_thread->_context);
  if (res == -1)
    ERROR("impossible to create");

  args->_thread->_context.uc_stack.ss_size = STACK_SIZE;
  args->_thread->_context.uc_stack.ss_sp = malloc(STACK_SIZE);
  args->_thread->_context.uc_link = &(args->_calling->_context);

  makecontext(&(args->_thread->_context), (void (*)(void)) args->_func, (int) args->_func_arg);

  // get return value;

  return SUCCESS;
}
