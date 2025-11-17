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

struct shared_data {
    int counter;
    char message[100];
    double value;
};

struct shared_data global_data = {0, "Initial", 0.0};

void unsafe_handler(int sig) {
    global_data.counter++;

    sprintf(global_data.message, "Signal %d received %d times",
            sig, global_data.counter);

    global_data.value = (double)global_data.counter * 3.14;

    printf("Handler: %s, value = %.2f\n",
           global_data.message, global_data.value);
}

int main() {
    printf("Demonstrating unsafe shared data access in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, unsafe_handler);

    for (int i = 0; i < 100; i++) {
        global_data.counter = i;
        sprintf(global_data.message, "Main loop iteration %d", i);
        global_data.value = (double)i * 2.71;

        printf("Main: %s, value = %.2f\n",
               global_data.message, global_data.value);

        usleep(100000);
    }

    return 0;
}