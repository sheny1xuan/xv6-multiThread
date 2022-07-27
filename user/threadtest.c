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

static struct {
    int provider;
    int consumer;
    void* mm;
} mallocFree;

void workerMallocFree(void* arg) {
    // malloc a mm
    sem_p(mallocFree.consumer);
    if (*(int*)(mallocFree.mm) != 100) {
        printf("Test Malloc error.\n");
        exit(-1);
    }
    *(int*)(mallocFree.mm) = 110;
    sem_v(mallocFree.provider);

    // free a mm
    sem_p(mallocFree.consumer);
    // still can access, but is destory
    if (*(int*)(mallocFree.mm) == 110) {
        printf("Test Free error.\n");
        exit(-1);
    }
    sem_v(mallocFree.provider);

    exit(0);
}

void workerDestoryMallocFree(void* arg) {
    for (int i = 0; i < 10; i++) {
        void* mm = malloc(sizeof(int[100]));
        memset(mm, 0, sizeof(int[100]));
        free(mm);
    }

    exit(0);
}

void testMallocFree() {
    mallocFree.provider = sem_create(1);
    mallocFree.consumer = sem_create(0);

    int tid = thread_create(workerMallocFree, 0);

    // malloc a mm
    sem_p(mallocFree.provider);
    mallocFree.mm = malloc(sizeof(int));
    *(int*)(mallocFree.mm) = 100;
    sem_v(mallocFree.consumer);

    // free a mm
    sem_p(mallocFree.provider);
    if (*(int*)(mallocFree.mm) != 110) {
        printf("Test Shard memory error.\n");
        exit(1);
    }
    free(mallocFree.mm);
    // begin destory memeory
    int tid2 = thread_create(workerDestoryMallocFree, 0);
    sleep(10);
    sem_v(mallocFree.consumer);
 
    thread_join(tid);
    thread_join(tid2);

    sem_free(mallocFree.consumer);
    sem_free(mallocFree.provider);

    printf("Pass malloc free test.\n");
}

void threadMalloc(void* arg) {
    // malloc a mm
    sem_p(mallocFree.provider);
    // More than one page
    mallocFree.mm = malloc(sizeof(int[2048]));
    ((int*)mallocFree.mm)[0] = 100;
    sem_v(mallocFree.consumer);

    // free a mm
    sem_p(mallocFree.provider);
    if (((int*)mallocFree.mm)[0] != 110) {
        printf("Test Shard memory error.\n");
        exit(1);
    }
    free(mallocFree.mm);
    sem_v(mallocFree.consumer);

    exit(0);
}

void threadFree(void* arg) {
    // malloc a mm
    sem_p(mallocFree.consumer);
    if (((int*)mallocFree.mm)[0] != 100) {
        printf("Test Malloc error.\n");
        exit(-1);
    }
    ((int*)mallocFree.mm)[0] = 110;
    sem_v(mallocFree.provider);

    // free a mm
    sem_p(mallocFree.consumer);
    // still can access, but is destory
    if (((int*)mallocFree.mm)[0] != 110) {
        printf("Test Free error.\n");
        exit(-1);
    }
    sem_v(mallocFree.provider);

    exit(0);
}

void threadMallocFree() {
    void* mm[100];
    for (int i = 0; i < 100; i++) {
        mm[i] = malloc(100);
        if (i % 2 == 0) {
            free(mm[i]);
        } 
    }

    for (int i = 0; i < 100; i++) {
        if (i % 2 != 0) free(mm[i]);
    }

    exit(0);
}

void testThreadMallocFree() {
    mallocFree.provider = sem_create(1);
    mallocFree.consumer = sem_create(0);

    int tid3 = thread_create(threadMallocFree, 0);
    int tid1 = thread_create(threadMalloc, 0);
    int tid2 = thread_create(threadFree, 0);

    thread_join(tid1);
    thread_join(tid2);
    thread_join(tid3);

    printf("Test child thread malloc/free ok\n");
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

    testMallocFree();

    testThreadMallocFree();

    exit(0);
}
