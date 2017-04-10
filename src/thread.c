#include <errno.h>
#include "queue.h"
#include "o_queue.h"
//push_back, pop, get


thread_t thread_self(void)
{
  tthread_t * current = queue__get();
  return ((thread_t) current);
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

  makecontext(&(args->_thread->_context), args->_func, args->_func_arg);

  // check if the main has been put in a thread
}


int thread_yield(void){
	tthread_t * actual = pop();
	push_back(actual);
	swap_context(second()._context, first()._context);
}


int thread_join(thread_t thread, void **retval){
	if (thread == NULL) { //doesn't exist --> error, invalid
		perror("Error : thread doesn't exist in thread_join");	
		return 0;
	}
	
	if (thread.status == ACTIVE) {
		thread.nb_join++; //increment the number of thread that wait the thread
		thread_yield(); //give the hand
		int finished = 0;
		while(finished == 0) {			
			//attente du signal de fin
			if (signal) {
				finished = 1;
			}
		}
		thread.nb_join--;
	}
	else {		
		return 1;
	}
}


void thread_exit(void *retval) __attribute__ ((__noreturn__)) {

}
