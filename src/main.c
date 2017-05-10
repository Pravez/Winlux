#define _GNU_SOURCE
#include "thread.h"
#include <stdio.h>
#include <assert.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define TO_TTHREAD(void_ptr) ((struct tthread_t*)void_ptr)

int final_value = 0;
thread_mutex_t mutex;

static void *threadfunc(void *arg) {
    /*char *name = arg;
    printf("je suis le thread %p, lancé avec l'argument %s\n",
           (void *) thread_self(), name);
    //thread_yield();
    printf("je suis encore le thread %p, lancé avec l'argument %s\n",
           (void *) thread_self(), name);*/

    thread_mutex_lock(&mutex);
    thread_yield();
    final_value += 1;
    thread_mutex_unlock(&mutex);

    thread_exit(arg);
}

int main(int argc, char *argv[]) {
    /*thread_t thread1, thread2;
    void *retval1, *retval2;
    int err;

    printf("le main lance 2 threads...\n");
    err = thread_create(&thread1, threadfunc, "thread1");
    assert(!err);
    err = thread_create(&thread2, threadfunc, "thread2");
    assert(!err);
    //thread_yield();
    printf("le main a lancé les threads %p et %p\n",
           (void *) thread1, (void *) thread2);

    printf("le main attend les threads\n");
    err = thread_join(thread2, &retval2);
    assert(!err);
    err = thread_join(thread1, &retval1);
    assert(!err);
    printf("les threads ont terminé en renvoyant '%s' and '%s'\n",
           (char *) retval1, (char *) retval2);*/

    thread_mutex_init(&mutex);

    thread_t threads[10];

    for(int i = 0;i < 10;i ++){
        thread_create(&threads[i], threadfunc, NULL);
    }

    for(int i = 0;i < 10;i ++){
        thread_join(threads[i], NULL);
    }

    printf("Valeur finale : %d", final_value);

    cpu_set_t set;

    CPU_ZERO(&set);
    printf("%ld\n", sysconf(_SC_NPROCESSORS_ONLN));


    return 0;
}
