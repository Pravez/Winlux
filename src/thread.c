#include "queue.h"
#include "o_queue.h"
//push_back, pop, get


thread_t thread_self(void)
{

}

int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg){

}

int thread_yield(void){
	tthread_t actual = pop();
	push_back(actual);
	swap_context(second()._context, first()._context);		
}


int thread_join(thread_t thread, void **retval){

}

void thread_exit(void *retval) __attribute__ ((__noreturn__)) {

}
