#ifndef FRED_CIE_KERNEL_H
#define FRED_CIE_KERNEL_H

#include <sched.h>

#include "../context.h"
#include "../queue/queue.h"


#define KTHREAD_STACK_SIZE 64*1024

#ifndef MAX_CPU
#define MAX_CPU _SC_NPROCESSORS_ONLN
#endif

struct tthread_t_kernel_queue *kernel_threads;
int available_cpus;

struct tthread_t_kernel_item {
    struct tthread_t *thread;
    int _kernel_id;
    TAILQ_ENTRY(tthread_t_kernel_item) _entries;
};

struct tthread_t_kernel_queue {
    int _kernel_id;
    TAILQ_HEAD(tthreads, tthread_t_kernel_item) _kernel_thread_head;
    int _tailq_size;
    pid_t _pid;
};

struct multikernel_watcher {
    int _taken_cpus;
    int _premain_initialized;
    struct tthread_t_kernel_queue* _kernel_queues;
};

void kwatcher__init(struct multikernel_watcher *kw);
int kwatcher__get_cpuid_to_add(struct multikernel_watcher* kw);
int kwatcher__add_thread(struct multikernel_watcher* kw, struct tthread_t *thread);
struct tthread_t* kwatcher__remove_thread(struct multikernel_watcher* kw, int kernel_queue);
struct tthread_t* kwatcher__queue_first(struct multikernel_watcher* kw, int kernel_queue);
int kwatcher__queue_empty(struct multikernel_watcher* kw, int kernel_queue);
int kwatcher__get_current_kernel(struct multikernel_watcher* kw);

struct multikernel_watcher k_watcher;


void kernel__init_queue(struct tthread_t_kernel_queue *klist, int kernel);
int kernel__queue_push_back(struct tthread_t_kernel_queue *klist, struct tthread_t *thread);
struct tthread_t *kernel__queue_pop(struct tthread_t_kernel_queue *klist);
struct tthread_t *kernel__queue_second(struct tthread_t_kernel_queue *klist);
struct tthread_t *kernel__queue_first(struct tthread_t_kernel_queue *klist);
int kernel__queue_empty(struct tthread_t_kernel_queue *klist);


#endif //FRED_CIE_KERNEL_H
