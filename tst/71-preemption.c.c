#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "thread.h"

/* fibonacci.
 *
 * la durée doit être proportionnel à la valeur du résultat.
 * valgrind doit être content.
 * jusqu'à quelle valeur cela fonctionne-t-il ?
 *
 * support nécessaire:
 * - thread_create()
 * - thread_join() avec récupération de la valeur de retour
 * - retour sans thread_exit()
 */

long final_value;
thread_mutex_t mutex;

static void *heavy(void *_value) {
    int add = 0;
    for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < 10000; j++){
            final_value++;
            add++;
        }
    }

    assert(add == 10000*10000);

    thread_exit(NULL);
}

int main(int argc, char *argv[]) {
    final_value = 0;
    int i, err, nb;
    thread_t *th;

    thread_mutex_init(&mutex);

    if (argc < 2) {
        printf("argument manquant: entier x pour lequel calculer la somme\n");
        return -1;
    }

    nb = atoi(argv[1]);
    th = malloc(nb * sizeof(*th));
    if (!th) {
        perror("malloc");
        return -1;
    }

    for (i = 0; i < nb; i++) {
        err = thread_create(&th[i], heavy, NULL);
        assert(!err);
    }

    for(i=0; i<nb; i++) {
        err = thread_join(th[i], NULL);
        assert(!err);
    }

    free(th);

    assert(final_value == nb*10000*10000);

    return 0;
}
