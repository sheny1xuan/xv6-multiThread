#include "kernel/types.h"
#include "user/user.h"

#define NTHREAD 3
#define PAGESIZE 4096

struct 
{
    int pid;
    void* userstk;
    int used;
} threads[NTHREAD];

void add_thread(int* pid, void* ustk) {
    for (int i = 0; i < NTHREAD; i++) {
        if (threads[i].used == 0) {
            threads[i].pid = *pid;
            threads[i].userstk = ustk;
            threads[i].used = i;
            break;
        }
    }
}

void remove_thread(int* pid) {
    for (int i = 0; i < NTHREAD; i++) {
        if (threads[i].used && threads[i].pid == *pid) {
            free(threads[i].userstk);
            threads[i].pid = 0;
            threads[i].userstk = 0;
            threads[i].used = 0;
            break;
        }
    }
}

int thread_create(void (*start_routine)(void*), void* args) {
    static int first = 1;
    if (first) {
        first = 0;
        for (int i = 0; i < NTHREAD; i++) {
            threads[i].pid = 0;
            threads[i].userstk = 0;
            threads[i].used = 0;
        }
    }

    // void *stk = malloc(PAGESIZE);
    int pid = clone(start_routine, args, 0);
    add_thread(&pid, 0);
    return pid;
}

int thread_join(int tid) {
    for (int i = 0; i < NTHREAD; i++) {
        if (threads[i].used == 1 && threads[i].pid == tid) {
            int pid = join(&threads[i].userstk);

            if (pid > 0) {
                remove_thread(&pid);
                return pid;
            }
        }
    }

    return -1;
}

void printTCB() {
    for (int i = 0; i < NTHREAD; i++) {
        printf("TCB %d: %d \n", i, threads[i].used);
    }
}
