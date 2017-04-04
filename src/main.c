#include <stdio.h>
#include <stdlib.h>
#include "context.h"
#include "queue/o_queue.h"


int main(int argc, char* argv[]){
    printf("Il n'y a rien ici ...\n");

    queue__init();

    int a, b, c, d;
    a = 5;b = 2;c = 3;d = 4;

    queue__push_back(&a);
    queue__push_back(&c);
    queue__push_back(&d);
    queue__push_back(&b);
    queue__pop();


    printf("%d\n", *(int*)queue__first());

    return EXIT_SUCCESS;
}
