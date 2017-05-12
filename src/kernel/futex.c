#define _GNU_SOURCE

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <sys/time.h>

#include "futex.h"
#include "../context.h"

int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
    return (int) syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr, val3);
}

void futex__wait(int *futexp) {
    int s;

    /* __sync_bool_compare_and_swap(ptr, oldval, newval) is a gcc
       built-in function.  It atomically performs the equivalent of:

           if (*ptr == oldval)
               *ptr = newval;

       It returns true if the test yielded true and *ptr was updated.
       The alternative here would be to employ the equivalent atomic
       machine-language instructions.  For further information, see
       the GCC Manual. */

    while (1) {

        /* Is the futex available? */

        if (__sync_bool_compare_and_swap(futexp, 1, 0))
            break;      /* Yes */

        /* Futex is not available; wait */

        s = futex(futexp, FUTEX_WAIT, 0, NULL, NULL, 0);
        if (s == -1 && errno != EAGAIN)
            ERROR("futex-FUTEX_WAIT");
    }
}

void futex__post(int *futexp) {
    int s;

    if (__sync_bool_compare_and_swap(futexp, 0, 1)) {

        s = futex(futexp, FUTEX_WAKE, 1, NULL, NULL, 0);
        if (s  == -1)
            ERROR("futex-FUTEX_WAKE");
    }
}