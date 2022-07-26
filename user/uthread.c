#include "kernel/types.h"
#include "user/user.h"

int thread_create(void (*start_routine)(void*), void* args) {
    int tid = clone(start_routine, args, 0);
    return tid;
}

int thread_join(int tid) {
    return join(tid);
}