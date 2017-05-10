#ifndef FRED_CIE_KERNEL_H
#define FRED_CIE_KERNEL_H

#include <sched.h>

#include "../context.h"
#include "../queue/queue.h"

//VOIR http://man7.org/linux/man-pages/man2/sched_setaffinity.2.html

cpu_set_t cpu_set;
struct tthread_t_kernel_queue *kernel_threads;

struct tthread_t_kernel_item {
    struct tthread_t *thread;
    int _kernel_id;
    TAILQ_ENTRY(tthread_t_kernel_item) _entries;
};

struct tthread_t_kernel_queue {
    int _kernel_id;
    TAILQ_HEAD(tthreads, tthread_t_kernel_item) _kernel_thread_head;
};

void kernel__init_queue(struct tthread_t_kernel_queue *klist, int kernel);

int kernel__queue_push_back(struct tthread_t_kernel_queue *klist, struct tthread_t *thread);

struct tthread_t *kernel__queue_pop(struct tthread_t_kernel_queue *klist);

struct tthread_t *kernel__queue_second(struct tthread_t_kernel_queue *klist);

struct tthread_t *kernel__queue_first(struct tthread_t_kernel_queue *klist);

int kernel__queue_empty(struct tthread_t_kernel_queue *klist);


#endif //FRED_CIE_KERNEL_H
