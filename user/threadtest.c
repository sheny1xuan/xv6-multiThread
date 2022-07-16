#include "kernel/types.h"
#include "user/user.h"

void func(void* arg) {
    int n = *(int*)arg;
    printf("This is a thread, pass %d\n", n);
}

int main() {
    // int cnt = 10;
    // int pid = thread_create(func, &cnt);

    // if (pid == -1) {
    //     // printf("create thread error\n");
    //     return 0;
    // }
    // thread_join(pid);

    return 0;
}