thread_t thread_self(void)
{

}

int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg) {
  // create watchdog_args
  // check if the main has been put in a thread
}

int thread_yield(void){

}


int thread_join(thread_t thread, void **retval){

}

void thread_exit(void *retval) __attribute__ ((__noreturn__)) {

}
