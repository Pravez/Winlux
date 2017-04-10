#include <errno.h>
#include <tclDecls.h>
#include <stdio.h>
#include "queue/o_queue.h"
#include "thread.h"
#include "context.h"
#include <ucontext.h>
//push_back, pop, get

#define TO_TTHREAD(void_ptr) ((struct tthread_t*)void_ptr)


thread_t thread_self(void)
{
    struct tthread_t * current = queue__first();
    return (thread_t) current;
}


int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg) {
  struct watchdog_args args;

  int res = getcontext(args._thread->_context);
  if (res == -1)
    ERROR("impossible get current context");

  int res = getcontext(&(args._thread->_context.uc_link));
  if (res == -1)
    ERROR("impossible get current context");

  args._thread->_context.uc_stack.ss_size = STACK_SIZE;
  args._thread->_context.uc_stack.ss_sp = malloc(STACK_SIZE);
  args._func = func;
  args._func_arg = funcargs;

  makecontext(&(args->_thread->_context), cxt_watchdog, args);

  // check if the main has been put in a thread
}


int thread_yield(void){
    struct tthread_t * actual = queue__pop();
	queue__push_back(actual);
	swapcontext(&TO_TTHREAD(queue__second())->_context, &TO_TTHREAD(queue__first())->_context);
}


int thread_join(thread_t thread, void **retval){
	if (thread == NULL) { //doesn't exist --> error, invalid
		perror("Error : thread doesn't exist in thread_join");
		return 0;
	}

  struct tthread_t* tthread = TO_TTHREAD(thread);

	if (tthread->_state == ACTIVE) {
        tthread->_join_wait++; //increment the number of thread that wait the thread
		thread_yield(); //give the hand


    int i =0;
    while(i < tthread.nb_waiting) {
      if (tthread.waiting[i] == (&TO_TTHREAD(queue__first()))) {
          tthread.waiting[i] = tthread.waiting[nb_waiting-1];
          tthread.nb_waiting--;
      }
      ++i;
    }

    if (tthread.nb_waiting == 0) {
        retval = tthread._retval;
    }
}


void thread_exit(void *retval) __attribute__ ((__noreturn__)) {
  struct tthread_t * current = TO_TTHREAD(queue__first());
  current->retval = &retval; //pass function's retval to calling thread
  exit(0);
}
