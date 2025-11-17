/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>

typedef struct {
    int sequence_number;
    char data[256];
    int checksum;
} shared_message_t;

shared_message_t* shared_memory = NULL;
volatile sig_atomic_t update_count = 0;

void shm_handler(int sig) {
    update_count++;

    if (shared_memory == NULL) {
        printf("Handler: Shared memory not initialized\n");
        return;
    }

    printf("Handler: Signal %d updating shared memory\n", sig);

    // Violation: Shared memory access without proper signal masking
    // can cause data races and corruption
    shared_memory->sequence_number = update_count;

    snprintf(shared_memory->data, sizeof(shared_memory->data),
             "Message from signal %d, update #%d", sig, update_count);

    // Simulate complex update with vulnerability window
    int temp_checksum = 0;
    for (int i = 0; shared_memory->data[i] != '\0'; i++) {
        temp_checksum += shared_memory->data[i];

        // Create race condition opportunity
        if (i % 10 == 0) {
            usleep(1000);
        }
    }

    shared_memory->checksum = temp_checksum + shared_memory->sequence_number;

    printf("Handler: Update complete (seq=%d, checksum=%d)\n",
           shared_memory->sequence_number, shared_memory->checksum);
}

int main() {
    struct sigaction sa;
    int shm_id;

    // Create shared memory segment
    shm_id = shmget(IPC_PRIVATE, sizeof(shared_message_t), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shared_memory = (shared_message_t*)shmat(shm_id, NULL, 0);
    if (shared_memory == (void*)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory
    memset(shared_memory, 0, sizeof(shared_message_t));

    // Install handler without masking
    sa.sa_handler = shm_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Shared memory vulnerable to concurrent access
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Shared memory ID: %d\n", shm_id);
    printf("Send signals to corrupt shared memory\n");

    while (1) {
        // Read and verify shared memory in main thread
        int seq = shared_memory->sequence_number;
        char data_copy[256];
        strncpy(data_copy, shared_memory->data, sizeof(data_copy) - 1);
        data_copy[255] = '\0';
        int checksum = shared_memory->checksum;

        // Verify checksum
        int calculated_checksum = seq;
        for (int i = 0; data_copy[i] != '\0'; i++) {
            calculated_checksum += data_copy[i];
        }

        printf("Main: seq=%d, data='%.50s...', checksum=%d\n",
               seq, data_copy, checksum);

        if (checksum != calculated_checksum) {
            printf("Main: ERROR - Checksum mismatch! Expected %d, got %d\n",
                   calculated_checksum, checksum);
        }

        sleep(2);
    }

    // Cleanup
    shmdt(shared_memory);
    shmctl(shm_id, IPC_RMID, NULL);
    return 0;
}