#define _GNU_SOURCE

#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>
#include <sched.h>

#include "queue/o_list.h"
#include "thread.h"
#include "kernel/kernel.h"

ucontext_t end_context;
char ssp[STACK_SIZE];

/*
 * Récupère l'identifiant du thread courant.
 */
thread_t thread_self(void) {
    struct tthread_t *current = kwatcher__queue_first(&k_watcher, kwatcher__get_current_kernel(&k_watcher));
    return (thread_t) current;
}

/*
 * Créée un nouveau thread qui va exécuter la fonction func avec l'argument funcarg.
 * Renvoie 0 en cas de succès, -1 en cas d'erreur.
 */
int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg) {
    struct watchdog_args *args = malloc(sizeof(struct watchdog_args));
    struct tthread_t *current = thread_self();

    args->_thread = tthread_init();

    int res = getcontext(&args->_thread->_context);
    if (res == -1) {
        ERROR("impossible get current context");
        tthread_destroy(args->_thread);
        return FAILED;
    }

    args->_thread->_context.uc_link = &current->_context;
    args->_thread->_context.uc_stack.ss_size = STACK_SIZE;
    args->_thread->_context.uc_stack.ss_sp = malloc(STACK_SIZE);
    args->_thread->_valgrind_stackid = VALGRIND_STACK_REGISTER(args->_thread->_context.uc_stack.ss_sp,
                                                               args->_thread->_context.uc_stack.ss_sp +
                                                               args->_thread->_context.uc_stack.ss_size);
    args->_func = func;
    args->_func_arg = funcarg;
    args->_thread->_watchdog_args = args;
    args->_thread->_priority = 1;

    //args->_thread->name = name;

    makecontext(&(args->_thread->_context), (void (*)(void)) cxt_watchdog, 1, args);

    *newthread = args->_thread;

    // need to add the current thread to the list of waiting threads
    kwatcher__add_thread(&k_watcher, args->_thread);

    /*
    add(args._thread->_context.uc_link, args._thread->_waiting_threads);
    args._thread->_waiting_thread_nbr++;
    */

    //setcontext(&args._thread->_context);

    return SUCCESS;
}

void thread_yield_handler(int signum) {
    if (signum == SIGVTALRM) {
        //Basically we only do a thread_yield
        thread_yield();
    }
}

/*
 * Passe la main à un autre thread.
 */
int thread_yield(void) {
    //Disabling signals
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    struct tthread_t *actual = kwatcher__remove_thread(&k_watcher, kwatcher__get_current_kernel(&k_watcher));

    if (actual->_state != SLEEPING)
        kwatcher__add_thread(&k_watcher, actual);

    struct tthread_t *first = NULL;

    do {
        if (first != NULL)
            kwatcher__add_thread(&k_watcher,
                                 kwatcher__remove_thread(&k_watcher, kwatcher__get_current_kernel(&k_watcher)));

        first = kwatcher__queue_first(&k_watcher, kwatcher__get_current_kernel(&k_watcher));
        //TODO file avec les deads
    } while (first->_state == DEAD);

    int timeslice = first->_priority * TIMESLICE;

    //We do only one iteration of the timer, only decrementing when process executes
    first->_timer.it_value.tv_sec = 0;
    first->_timer.it_value.tv_usec = timeslice * 1000; //Nanoseconds to milliseconds
    first->_timer.it_interval.tv_sec = 0;
    first->_timer.it_interval.tv_usec = timeslice * 1000;
    setitimer(ITIMER_VIRTUAL, &first->_timer, NULL);

    //Enable signals
    sigemptyset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    //Signal the sig for timer
    signal(SIGVTALRM, thread_yield_handler);

    swapcontext(&actual->_context, &first->_context);

    return 0;
}


/*
 * Attend la fin d'exécution d'un thread.
 * La valeur renvoyée par le thread est placée dans *retval.
 * Si retval est NULL, la valeur de retour est ignorée.
 */
int thread_join(thread_t thread, void **retval) {
    if (thread == NULL) { //doesn't exist --> error, invalid
        ERROR("thread doesn't exist in thread_join");
        return ERR_INVALID_THREAD;
    }
    if (thread == kwatcher__queue_first(&k_watcher, kwatcher__get_current_kernel(&k_watcher))) {
        ERROR("try to wait itself, forbidden");
        return ERR_JOIN_ITSELF;
    }

    struct tthread_t *tthread = TO_TTHREAD(thread);
    struct tthread_t *self = TO_TTHREAD(thread_self());

    if (find(kwatcher__queue_first(&k_watcher, kwatcher__get_current_kernel(&k_watcher))->_waiting_threads,
             thread) == 0) {
        ERROR("another thread is already waiting for this thread, can't wait it");
        return ERR_EXISTING_JOIN;
    }


    if (tthread->_state == ACTIVE) {
        tthread->_waiting_thread_nbr++; //increment the number of thread that wait the thread
        self->_state = SLEEPING;
        add(self, tthread->_waiting_threads);

        thread_yield(); //give the hand

        delete(self, tthread->_waiting_threads);
        tthread->_waiting_thread_nbr--;
    }

    if (retval != NULL)
        *retval = tthread->_retval;

    if (tthread->_waiting_thread_nbr <= 0)
        tthread_destroy(tthread);

    return 0;
}


/*
 * Termine le thread courant en renvoyant la valeur de retour retval.
 * Cette fonction ne retourne jamais.
 */
void thread_exit(void *retval) {
    struct tthread_t *current = kwatcher__remove_thread(&k_watcher, kwatcher__get_current_kernel(&k_watcher));
    current->_retval = retval; //pass function's retval to calling thread
    current->_state = DEAD;

    struct node *current_node = current->_waiting_threads->head;
    while (current_node != NULL) {
        ((struct tthread_t *) (current_node->data))->_state = ACTIVE;
        kwatcher__add_thread(&k_watcher, current_node->data);
        current_node = current_node->next;
    }

    if (kwatcher__queue_first(&k_watcher, kwatcher__get_current_kernel(&k_watcher)) == NULL) {
        makecontext(&end_context, (void (*)(void)) tthread__end_program, 1, current);
        swapcontext(&current->_context, &end_context);
    } else {
        swapcontext(&current->_context, &(
                kwatcher__queue_first(&k_watcher, kwatcher__get_current_kernel(
                        &k_watcher)))->_context); //TODO : Pas forcément le premier de la queue mais chercher le premier qui est ACTIF ?
    }

    while (1);
}

/*
 * Initialise le mutex.
 * Renvoie 0 en cas de succès, -1 en cas d'erreur.
 */
int thread_mutex_init(thread_mutex_t *mutex) {
    struct tthread_mutex_t *new_mutex = TO_TTHREAD_MUTEX(mutex);

    if (new_mutex->_initialized) {
        ERROR("mutex already initialized");
        return FAILED;
    }

    new_mutex->_lock = 0;
    new_mutex->_initialized = 1;
    TAILQ_INIT(&(new_mutex->_queue_head));

    return SUCCESS;
}

/*
 * Tente d'entrer en section critique.
 * Si le verrou est déjà pris, le thread courant attend jusqu'à ce que le
 * verrou soit libre.
 */
int thread_mutex_lock(thread_mutex_t *mutex) {
    struct tthread_mutex_t *mutex_lock = TO_TTHREAD_MUTEX(mutex);
    struct tthread_mutex_list_item *item = malloc(sizeof(struct tthread_mutex_list_item));

    item->_thread = TO_TTHREAD(thread_self());

    if (mutex_lock->_lock) {
        //le verrou est déjà pris, attendre, à ce moment là on se position en fin de queue
        TAILQ_INSERT_TAIL(&(mutex_lock->_queue_head), item, _entries);
        while (mutex_lock->_lock) {
            item->_is_waiting = 1;
            while (item->_is_waiting) {
                thread_yield();
            }
        }
    } else {
        //S'il n'est pas pris, on se position en début de queue
        TAILQ_INSERT_HEAD(&(mutex_lock->_queue_head), item, _entries);
    }

    mutex_lock->_lock = 1;

    return SUCCESS;
}

int has_waiter(struct tthread_mutex_t *t) {
    return !TAILQ_EMPTY(&t->_queue_head);
}

int thread_mutex_unlock(thread_mutex_t *mutex) {
    struct tthread_mutex_t *mutex_t = TO_TTHREAD_MUTEX(mutex);

    mutex_t->_lock = 0;
    struct tthread_mutex_list_item *item = TAILQ_FIRST(&mutex_t->_queue_head);
    struct tthread_mutex_list_item *next = TAILQ_NEXT(item, _entries);
    if (next != NULL) {
        next->_is_waiting = 0;
    }

    TAILQ_REMOVE(&mutex_t->_queue_head, item, _entries);

    return SUCCESS;
}

int thread_mutex_destroy(thread_mutex_t *mutex) {
    struct tthread_mutex_t *mutex_t = TO_TTHREAD_MUTEX(mutex);

    while (has_waiter(mutex_t)) {}

    free(mutex_t);

    return SUCCESS;
}

void __attribute__((constructor)) premain() {

    //Init kernel threads
    kwatcher__init(&k_watcher);

    struct tthread_t *main_thread = tthread_init();

    int res = getcontext(&main_thread->_context);
    if (res == -1)
        ERROR("impossible get main context");

    main_thread->_context.uc_link = &main_thread->_context;
    main_thread->_context.uc_stack.ss_size = STACK_SIZE;
    main_thread->_context.uc_stack.ss_sp = malloc(STACK_SIZE);
    main_thread->_watchdog_args = NULL;

    main_thread->name = "main";

    getcontext(&end_context);
    end_context.uc_link = &end_context;
    end_context.uc_stack.ss_flags = 0;
    end_context.uc_stack.ss_size = STACK_SIZE;
    end_context.uc_stack.ss_sp = &ssp;

    kwatcher__add_thread(&k_watcher, main_thread);
    k_watcher._premain_initialized = 1;
    k_watcher._kernel_queues[0]._pid = getpid();
}


void __attribute__((destructor)) postmain() {
    int main_was_last = 0;
    struct tthread_t *thread;
    for (int i = 0; i < k_watcher._taken_cpus; i++) {
        if (!kwatcher__queue_empty(&k_watcher, i)) {
            thread = kwatcher__remove_thread(&k_watcher, i);
            tthread_destroy(thread);
            main_was_last = 1;
        }
    }

    if (main_was_last)
        free(k_watcher._kernel_queues);
}

//postmain_watchdog_args
