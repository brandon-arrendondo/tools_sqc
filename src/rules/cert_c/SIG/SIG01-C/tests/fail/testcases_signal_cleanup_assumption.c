/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t cleanup_count = 0;
int* global_resource = NULL;

void cleanup_handler(int sig) {
    cleanup_count++;
    printf("Cleanup handler: %d\n", cleanup_count);

    /* Assumes handler can safely clean up resources */
    if (global_resource) {
        printf("Cleaning up global resource\n");
        free(global_resource);  /* Unsafe in signal handler */
        global_resource = NULL;
    }
}

int main() {
    printf("FAIL: Signal handler cleanup assumptions\n");

    signal(SIGTERM, cleanup_handler);

    printf("PID: %d\n", getpid());

    /* Allocate resource */
    global_resource = malloc(100 * sizeof(int));
    if (!global_resource) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    printf("Resource allocated, send SIGTERM for cleanup\n");

    /* Assumes signal handler will safely clean up */
    raise(SIGTERM);

    sleep(1);

    /* Code assumes cleanup happened safely */
    if (!global_resource) {
        printf("Resource cleaned up by signal handler\n");
    } else {
        printf("Resource still allocated\n");
        free(global_resource);
    }

    printf("Cleanup count: %d\n", cleanup_count);
    printf("Assumes signal handlers can safely manage resources\n");

    return 0;
}