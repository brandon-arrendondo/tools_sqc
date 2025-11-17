/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int id;
    char name[64];
    double value;
} data_record_t;

data_record_t *global_record = NULL;

void unsafe_handler(int sig) {
    /* Violation: Accessing malloc'd memory in signal handler */
    if (global_record != NULL) {
        global_record->id = sig;
        sprintf(global_record->name, "Signal_%d", sig);
        global_record->value += 1.0;
        printf("Handler: Record id=%d, name=%s, value=%.1f\n",
               global_record->id, global_record->name, global_record->value);
    }
}

int main() {
    printf("Demonstrating unsafe dynamic memory access in signal handler\n");
    printf("PID: %d\n", getpid());

    global_record = malloc(sizeof(data_record_t));
    if (!global_record) {
        perror("malloc");
        exit(1);
    }

    signal(SIGUSR1, unsafe_handler);

    for (int i = 0; i < 30; i++) {
        global_record->id = i;
        sprintf(global_record->name, "Main_%d", i);
        global_record->value = i * 3.14;
        printf("Main: Record id=%d, name=%s, value=%.1f\n",
               global_record->id, global_record->name, global_record->value);
        usleep(100000);
    }

    free(global_record);
    return 0;
}