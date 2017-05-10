#include "kernel.h"

void kernel__init_queue(struct tthread_t_kernel_queue *klist, int kernel) {
    TAILQ_INIT(&klist->_kernel_thread_head);
    klist->_kernel_id = kernel;
}

int kernel__queue_push_back(struct tthread_t_kernel_queue *klist, struct tthread_t *thread) {
    struct tthread_t_kernel_item *item = malloc(sizeof(struct tthread_t_kernel_item));
    item->_kernel_id = klist->_kernel_id;
    item->thread = thread;
    TAILQ_INSERT_TAIL(&klist->_kernel_thread_head, item, _entries);

    return 1;
}

struct tthread_t *kernel__queue_pop(struct tthread_t_kernel_queue *klist) {
    struct tthread_t_kernel_item *item = TAILQ_FIRST(&klist->_kernel_thread_head);
    struct tthread_t *thread = item->thread;
    TAILQ_REMOVE(&klist->_kernel_thread_head, item, _entries);
    free(item);

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