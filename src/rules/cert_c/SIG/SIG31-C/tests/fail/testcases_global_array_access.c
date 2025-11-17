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

#define ARRAY_SIZE 1000

int shared_array[ARRAY_SIZE];
int array_index = 0;

void dangerous_handler(int sig) {
    for (int i = 0; i < 10; i++) {
        if (array_index < ARRAY_SIZE) {
            shared_array[array_index] = sig * 100 + i;
            array_index++;
        }
    }

    printf("Handler modified array, index now: %d\n", array_index);
}

int main() {
    printf("Demonstrating unsafe global array access in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, dangerous_handler);

    for (int i = 0; i < ARRAY_SIZE; i++) {
        shared_array[i] = i;
        array_index = i + 1;

        if (i % 100 == 0) {
            printf("Main: filled %d elements\n", i);
        }

        usleep(1000);
    }

    printf("Final array state:\n");
    for (int i = 0; i < 20; i++) {
        printf("array[%d] = %d\n", i, shared_array[i]);
    }

    return 0;
}