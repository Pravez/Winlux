#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define _SCHED_H 1
#define __USE_GNU 1
#include <bits/sched.h>
#include <sys/wait.h>
#include "src/kernel/futex.h"

#define STACK_SIZE 4096

int futexes[2];

int func(void *arg) {
    futex__wait(&futexes[0]);
    printf("child cpu : %d\n", sched_getcpu());
    printf("pid fils : %d\n", getpid());
    printf("Inside func.\n");
    printf("Terminating func...\n");
    futex__post(&futexes[1]);

    printf("Fini\n");

    return 0;
}

int main() {
    futexes[1] = 1;
    futexes[0] = 0;
    printf("This process pid: %u\n", getpid());
    void *child_stack = malloc(STACK_SIZE);
    signal(SIGCHLD, SIG_IGN);


    printf("main cpu : %d\n", sched_getcpu());

    futex__wait(&futexes[1]);
    printf("Creating new thread...\n");
    clone(&func, child_stack+STACK_SIZE, CLONE_SIGHAND|CLONE_FS|CLONE_VM|CLONE_FILES, NULL);
    printf("Done! Thread pid: %d\n", getpid());

    //futex__post(&futexes[0]);

    waitpid(-1, NULL, 0);

    return 0;
}