#include "kernel/types.h"
#include "user/user.h"

void func(void* arg) {
    uint64 addr = *(uint64*)arg;
    uint64 x = 100;
    addr += x;
    printf("this func is called in thread : arg : %ld\n", addr);
}

int
main(int argc, char *argv[]) {
    int cnt = argc;
    int pid = thread_create(func, &cnt);

    if (pid == -1) {
        printf("create thread error\n");
        exit(0);
    }
    thread_join(pid);

    exit(0);
}
