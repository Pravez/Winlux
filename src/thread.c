#include <stdio.h>
#include <ucontext.h>

#include "queue/o_queue.h"
#include "queue/o_list.h"
#include "thread.h"
#include "thread_private.h"

#define TO_TTHREAD(void_ptr) ((struct tthread_t*)void_ptr)
#define ERROR(msg) printf("\x1b[31;1mError:\x1b[0m %s\n", msg)

/*
 * Récupère l'identifiant du thread courant.
 */
thread_t thread_self(void) {
    if (thread_main_is_thread())
        thread_main_to_thread();

    struct tthread_t *current = queue__first();
    return (thread_t) current;
}

/*
 * Créée un nouveau thread qui va exécuter la fonction func avec l'argument funcarg.
 * Renvoie 0 en cas de succès, -1 en cas d'erreur.
 */
int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg) {
    if (thread_main_is_thread())
        thread_main_to_thread();

    struct watchdog_args args;
    struct tthread_t *current = thread_self();

    args._thread = tthread_init();

    int res = getcontext(&args._thread->_context);
    if (res == -1) {
        ERROR("impossible get current context");
        tthread_destroy(args._thread);
		return FAILED;
	}

    args._thread->_context.uc_link = &current->_context;
    args._thread->_context.uc_stack.ss_size = STACK_SIZE;
    args._thread->_context.uc_stack.ss_sp = malloc(STACK_SIZE);
    args._func = func;
    args._func_arg = funcarg;

    // need to add the current thread to the list of waiting threads
    queue__push_back(args._thread);
    /*
    add(args._thread->_context.uc_link, args._thread->_waiting_threads);
    args._thread->_waiting_thread_nbr++;
    */

    args._thread->_state = ACTIVE;

    makecontext(&(args._thread->_context), (void (*)(void)) cxt_watchdog, 1, &args);

    *newthread = args._thread;

    //setcontext(&args._thread->_context);

    return SUCCESS;
}


/*
 * Passe la main à un autre thread.
 */
int thread_yield(void) {
    if (thread_main_is_thread())
        thread_main_to_thread();

    struct tthread_t *actual = queue__pop();
    queue__push_back(actual);
    swapcontext(&TO_TTHREAD(queue__second())->_context, &TO_TTHREAD(queue__first())->_context);

    return 0;
}


/*
 * Attend la fin d'exécution d'un thread.
 * La valeur renvoyée par le thread est placée dans *retval.
 * Si retval est NULL, la valeur de retour est ignorée.
 */
int thread_join(thread_t thread, void **retval) {
    if (thread == NULL) { //doesn't exist --> error, invalid
        ERROR("Error : thread doesn't exist in thread_join");
        return 0;
    }
    if (thread == queue__first()){
        ERROR("Error : Try to wait itself, forbidden");
        return 0;
    }

    struct tthread_t *tthread = TO_TTHREAD(thread);
    struct tthread_t *self = TO_TTHREAD(thread_self());

    if (find(&TO_TTHREAD(queue__first())->_waiting_threads, thread) == 0){
        ERROR("Error : Thread already wait for this thread, can't wait it");
        return 0;
    }


    if (tthread->_state == ACTIVE) {
        tthread->_waiting_thread_nbr++; //increment the number of thread that wait the thread
        self->_state = SLEEPING;
        add(thread_self(), tthread->_waiting_threads);
        while(self->_state == SLEEPING)
            thread_yield(); //give the hand
    }

    delete(thread_self(), tthread->_waiting_threads);
    tthread->_waiting_thread_nbr--;

    *retval = tthread->_retval;

    if (tthread->_waiting_thread_nbr <= 0) {
        destroy(tthread->_waiting_threads);
        //desallouer le contexte lol
        free(tthread);
    }

    return 0;
}

/*
 * Termine le thread courant en renvoyant la valeur de retour retval.
 * Cette fonction ne retourne jamais.
 */
void thread_exit(void *retval) {
    struct tthread_t *current = TO_TTHREAD(queue__pop());
    current->_retval = retval; //pass function's retval to calling thread
    struct node *current_node = current->_waiting_threads->head;
    while (has_next(current_node)) {
        ((struct tthread_t *) (current_node->data))->_state = ACTIVE;
        current_node = current_node->next;
    }

    setcontext(&(TO_TTHREAD(queue__first()))->_context);
    while (1);
}

void thread_main_to_thread() {
    queue__init();
    struct tthread_t *main_thread = tthread_init();

    int res = getcontext(&(main_thread->_context));
    if (res == -1)
        ERROR("impossible get main context");

    main_thread->_context.uc_link = &(main_thread->_context);
    main_thread->_context.uc_stack.ss_size = STACK_SIZE;
    main_thread->_context.uc_stack.ss_sp = malloc(STACK_SIZE);

    queue__push_back(main_thread);
}


int thread_main_is_thread() {
    return queue__empty();
}
