#define _GNU_SOURCE

#include <sys/wait.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include "kernel.h"

void kwatcher__init(struct multikernel_watcher *kw) {
    available_cpus = (int) sysconf(_SC_NPROCESSORS_ONLN);
    kw->_taken_cpus = 0;
    kw->_kernel_queues = malloc(sizeof(struct tthread_t_kernel_queue) * available_cpus);
    kw->_premain_initialized = 0;

    signal(SIGCHLD, SIG_IGN);

    for (int i = 0; i < available_cpus; i++) {
        kernel__init_queue(&kw->_kernel_queues[i], i);
    }
}

int kwatcher__get_cpuid_to_add(struct multikernel_watcher *kw) {
    int min = kw->_kernel_queues[0]._tailq_size;
    int min_position = 0;


    for (int i = 0; i < available_cpus; i++) {
        if(i < kw->_taken_cpus) {
            if (kw->_kernel_queues[i]._tailq_size <= min) {
                min = kw->_kernel_queues[i]._tailq_size;
                min_position = i;
            }
        }else{
            min_position = i;
            break;
        }
    }

    return min_position;
}

int k_launcher(void *arg) {
    struct tthread_t_kernel_queue* kqueue = (struct tthread_t_kernel_queue*) arg;
    kqueue->_pid = getpid();

    setcontext(&kwatcher__queue_first(&k_watcher, kqueue->_kernel_id)->_context);

    return 1;
}

int kwatcher__add_thread(struct multikernel_watcher *kw, struct tthread_t *thread) {
    int cpu = kwatcher__get_cpuid_to_add(kw);
    int retval = kernel__queue_push_back(&kw->_kernel_queues[cpu], thread);

    //If we added on a new kernel list, we clone to create a new real thread
    if (cpu == kw->_taken_cpus && kw->_premain_initialized) {
        kw->_taken_cpus++;
        void *new_stack = malloc(KTHREAD_STACK_SIZE);
        int clone_val = clone(k_launcher, new_stack, CLONE_FS | CLONE_VM | CLONE_FILES | CLONE_SIGHAND | CLONE_IO,
                              &kw->_kernel_queues[cpu]);
        if (clone_val == FAILED) {
            ERROR("impossible to clone");
        }
    }else if(!kw->_premain_initialized){
        kw->_taken_cpus++;
    }

    return retval;
}

struct tthread_t *kwatcher__remove_thread(struct multikernel_watcher *kw, int kernel_queue) {
    return kernel__queue_pop(&kw->_kernel_queues[kernel_queue]);
}

struct tthread_t *kwatcher__queue_first(struct multikernel_watcher *kw, int kernel_queue) {
    return kernel__queue_first(&kw->_kernel_queues[kernel_queue]);
}

int kwatcher__queue_empty(struct multikernel_watcher *kw, int kernel_queue) {
    return kernel__queue_empty(&kw->_kernel_queues[kernel_queue]);
}

int kwatcher__get_current_kernel(struct multikernel_watcher* kw){
    pid_t pid = getpid();
    for(int i=0;i < kw->_taken_cpus;i++){
        if(kw->_kernel_queues[i]._pid == pid)
            return i;
    }

    ERROR("pid not found");
    return -1;
}

void kernel__init_queue(struct tthread_t_kernel_queue *klist, int kernel) {
    TAILQ_INIT(&klist->_kernel_thread_head);
    klist->_kernel_id = kernel;
    klist->_tailq_size = 0;
}

int kernel__queue_push_back(struct tthread_t_kernel_queue *klist, struct tthread_t *thread) {
    struct tthread_t_kernel_item *item = malloc(sizeof(struct tthread_t_kernel_item));
    item->_kernel_id = klist->_kernel_id;
    item->thread = thread;
    thread->_kernel_id = klist->_kernel_id;
    TAILQ_INSERT_TAIL(&klist->_kernel_thread_head, item, _entries);

    klist->_tailq_size++;

    return 1;
}

struct tthread_t *kernel__queue_pop(struct tthread_t_kernel_queue *klist) {
    struct tthread_t_kernel_item *item = TAILQ_FIRST(&klist->_kernel_thread_head);
    struct tthread_t *thread = item->thread;
    TAILQ_REMOVE(&klist->_kernel_thread_head, item, _entries);
    free(item);

    klist->_tailq_size--;

    return thread;
}

struct tthread_t *kernel__queue_second(struct tthread_t_kernel_queue *klist) {
    return TAILQ_NEXT(TAILQ_FIRST(&klist->_kernel_thread_head), _entries) == NULL ? NULL :
           TAILQ_NEXT(TAILQ_FIRST(&klist->_kernel_thread_head), _entries)->thread;
}

struct tthread_t *kernel__queue_first(struct tthread_t_kernel_queue *klist) {
    return TAILQ_FIRST(&klist->_kernel_thread_head) == NULL ? NULL : TAILQ_FIRST(&klist->_kernel_thread_head)->thread;
}

int kernel__queue_empty(struct tthread_t_kernel_queue *klist) {
    return TAILQ_EMPTY(&klist->_kernel_thread_head);
}