// #include "types.h"
// #include "stat.h"
#include "kernel/types.h"
#include "user/user.h"

int main() {
    int generate_id = sem_create(1);
    int consume_id = sem_create(0);
    int pid = fork();

    if (pid == 0) {
        // child
        for (int i = 0; i < 10; i++) {
            sem_p(consume_id);
            printf("consume ...\n");
            sem_v(generate_id);
        }

        exit(0);
    }

    for (int i = 0; i < 10; i++) {
        sem_p(generate_id);
        printf("produce ...\n");
        sem_v(consume_id);
    }

    wait(0);
    sem_free(generate_id);
    sem_free(consume_id);

    exit(0);
}