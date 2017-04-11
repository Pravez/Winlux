#include <errno.h>
//#include <tclDecls.h>
#include <stdio.h>
#include <ucontext.h>
#include "queue/o_queue.h"
#include "queue/o_list.h"
#include "thread.h"
#include "context.h"

#define TO_TTHREAD(void_ptr) ((struct tthread_t*)void_ptr)
#define ERROR(msg) printf("\x1b[31;1mError:\x1b[0m %s\n", msg)

/*
 * Récupère l'identifiant du thread courant.
 */
thread_t thread_self(void) {
    struct tthread_t *current = queue__first();
    return (thread_t) current;
}

/*
 * Créée un nouveau thread qui va exécuter la fonction func avec l'argument funcarg.
 * Renvoie 0 en cas de succès, -1 en cas d'erreur.
 */
int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg) {
    struct watchdog_args args;

    int res = getcontext(&args._thread->_context);
    if (res == -1)
        ERROR("impossible get current context");

    res = getcontext(args._thread->_context.uc_link);
    if (res == -1) {
        ERROR("impossible get current context");
        return FAILED;
    }

    args._thread->_context.uc_stack.ss_size = STACK_SIZE;
    args._thread->_context.uc_stack.ss_sp = malloc(STACK_SIZE);
    args._func = func;
    args._func_arg = funcarg;

    // need to add the current thread to the list of waiting threads
    queue__push_back(args._thread);
    add(args._thread->_context.uc_link, args._thread->_waiting_threads);
    args._thread->_waiting_thread_nbr++;

    makecontext(&(args._thread->_context), (void (*)(void)) cxt_watchdog, 1, &args);

    // check if the main has been put in a thread

    return SUCCESS;
}

/*
 * Passe la main à un autre thread.
 */
int thread_yield(void) {
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

    struct tthread_t *tthread = TO_TTHREAD(thread);

    if (tthread->_state == ACTIVE) {
        tthread->_waiting_thread_nbr++; //increment the number of thread that wait the thread
        add(thread_self(), tthread->_waiting_threads);
        thread_yield(); //give the hand
    }

    delete(thread_self(), tthread->_waiting_threads);
    tthread->_waiting_thread_nbr--;

    *retval = tthread->_retval;

    if(tthread->_waiting_thread_nbr <= 0){
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
__attribute__ ((__noreturn__)) void thread_exit(void *retval) {
    struct tthread_t *current = TO_TTHREAD(queue__first());
    current->_retval = retval; //pass function's retval to calling thread
    struct node *current_node = current->_waiting_threads->head;
    while (hasNext(current_node)) {
        ((struct tthread_t *) (current_node->data))->_state = ACTIVE;
        current_node = current_node->next;
    }
}
