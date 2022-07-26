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
        int pid = thread_join(i + 1);
        printf("join thread %d, it's pid is %d\n", i, pid);
    }

    printf("Test create thread pass!\n");
}

struct {
    int consumer;
    int producer;
} sem_ids;

void consumeFunc(void* args) {
    for (int i = 0; i < 10; i++) {
        sem_p(sem_ids.consumer);
        printf("consume thread...\n");
        sem_v(sem_ids.producer);
    }

    exit(0);
}

void testThreadSync() {
    sem_ids.consumer = sem_create(0);
    sem_ids.producer = sem_create(1);
    thread_create(consumeFunc, 0);

    for (int i = 0; i < 10; i++) {
        sem_p(sem_ids.producer);
        printf("produce thread...\n");
        sem_v(sem_ids.consumer);
    }

    thread_join(1);

    printf("Pass thread sync\n");
}

void testThreadMallocFree() {
    sem_ids.consumer = sem_create(0);
    sem_ids.producer = sem_create(1);
    thread_create(consumeFunc, 0);

    for (int i = 0; i < 10; i++) {
        sem_p(sem_ids.producer);
        printf("produce thread...\n");
        sem_v(sem_ids.consumer);
    }

    thread_join(1);

    printf("Pass thread sync\n");
}

// void testThreadMallocFree() {
//     sem_ids.consumer = sem_create(0);
//     sem_ids.producer = sem_create(1);
//     thread_create(consumeFunc, 0);

//     for (int i = 0; i < 10; i++) {
//         sem_p(sem_ids.producer);
//         printf("produce thread...\n");
//         sem_v(sem_ids.consumer);
//     }

//     thread_join(1);

//     printf("Pass thread sync\n");
// }

int
main(int argc, char *argv[]) {
    testCreateThread();

    testThreadSync();

    exit(0);
}
