#include "kernel/types.h"
#include "user/user.h"

void func(void* arg) {
    int x = *(int*)arg;
    printf("this func is called in thread, thread : %d\n", x);

    exit(0);
}

int tid[4] = {1, 2, 3, 4}; 

void testCreateThread() {
    for (int i = 0; i < 4; i++) {
        int pid = thread_create(func, &tid[i]);

        if (pid == -1) {
            printf("create thread error\n");
            exit(0);
        }
        // thread_join(pid);
    }

    for (int i = 0; i < 4; i++) {
        int pid = thread_join(i);
        printf("join thread %d, it's pid is %d\n", i, pid);
    }

    printf("Test create thread pass!\n");
}

int
main(int argc, char *argv[]) {
    testCreateThread();
    sleep(10);

    exit(0);
}
