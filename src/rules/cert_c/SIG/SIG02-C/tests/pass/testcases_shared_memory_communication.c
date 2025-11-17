/*
 * Rule: SIG02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG02-C violation
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>

typedef struct {
    int data_ready;
    int process_complete;
    char message[256];
} shared_data_t;

int main() {
    printf("Using shared memory for normal inter-process communication (GOOD)\n");

    // Create shared memory segment
    int shm_id = shmget(IPC_PRIVATE, sizeof(shared_data_t), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach shared memory
    shared_data_t *shared_data = (shared_data_t *)shmat(shm_id, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Initialize shared data
    shared_data->data_ready = 0;
    shared_data->process_complete = 0;
    strcpy(shared_data->message, "");

    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // Child process
        sleep(1);
        printf("Child: Preparing data in shared memory\n");
        strcpy(shared_data->message, "Data from child process");
        shared_data->data_ready = 1;

        sleep(2);
        printf("Child: Marking process as complete\n");
        shared_data->process_complete = 1;
        exit(0);
    } else {
        // Parent process
        printf("Parent: Waiting for data in shared memory\n");
        while (!shared_data->data_ready) {
            usleep(100000);  // Poll every 100ms
        }

        printf("Parent: Received message: %s\n", shared_data->message);
        printf("Parent: Processing data...\n");
        sleep(1);

        printf("Parent: Waiting for completion flag\n");
        while (!shared_data->process_complete) {
            usleep(100000);  // Poll every 100ms
        }

        printf("Parent: Process completed successfully\n");
        wait(NULL);

        // Cleanup
        shmdt(shared_data);
        shmctl(shm_id, IPC_RMID, NULL);
    }

    return 0;
}