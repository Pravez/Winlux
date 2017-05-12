#ifndef FRED_CIE_FUTEX_H
#define FRED_CIE_FUTEX_H

int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3);
void futex__wait(int *futexp);
void futex__post(int *futexp);

#endif //FRED_CIE_FUTEX_H
