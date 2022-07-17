#include "kernel/types.h"
#include "user/user.h"

void func(void* arg) {
    int x = *(int*)arg;
    printf("this func is called in thread, arg : %d\n", x);

    exit(0);
}

int
main(int argc, char *argv[]) {
    int cnt = argc;
    int pid = thread_create(func, &cnt);

    if (pid == -1) {
        printf("create thread error\n");
        exit(0);
    }
    // thread_join(pid);

    printf("clone ret\n");

    sleep(10);

    exit(0);
}
