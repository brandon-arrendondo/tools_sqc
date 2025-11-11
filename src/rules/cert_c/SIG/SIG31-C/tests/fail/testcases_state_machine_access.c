/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef enum {
    STATE_IDLE,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_PROCESSING,
    STATE_ERROR,
    STATE_DISCONNECTING
} connection_state_t;

typedef struct {
    connection_state_t current_state;
    connection_state_t previous_state;
    int state_change_count;
    char status_message[128];
} state_machine_t;

state_machine_t global_state_machine = {
    .current_state = STATE_IDLE,
    .previous_state = STATE_IDLE,
    .state_change_count = 0,
    .status_message = "Initialized"
};

const char* state_names[] = {
    "IDLE", "CONNECTING", "CONNECTED", "PROCESSING", "ERROR", "DISCONNECTING"
};

void unsafe_handler(int sig) {
    /* Violation: Accessing shared state machine in signal handler */
    global_state_machine.previous_state = global_state_machine.current_state;

    if (sig == SIGUSR1) {
        global_state_machine.current_state = STATE_ERROR;
        sprintf(global_state_machine.status_message, "Emergency state due to signal %d", sig);
    } else if (sig == SIGUSR2) {
        global_state_machine.current_state = STATE_DISCONNECTING;
        sprintf(global_state_machine.status_message, "Forced disconnect by signal %d", sig);
    }

    global_state_machine.state_change_count++;

    printf("Handler: %s -> %s (count=%d) msg=%s\n",
           state_names[global_state_machine.previous_state],
           state_names[global_state_machine.current_state],
           global_state_machine.state_change_count,
           global_state_machine.status_message);
}

int main() {
    printf("Demonstrating unsafe state machine access in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, unsafe_handler);
    signal(SIGUSR2, unsafe_handler);

    connection_state_t states[] = {STATE_CONNECTING, STATE_CONNECTED, STATE_PROCESSING};
    int num_states = sizeof(states) / sizeof(states[0]);

    for (int i = 0; i < 30; i++) {
        global_state_machine.previous_state = global_state_machine.current_state;
        global_state_machine.current_state = states[i % num_states];
        global_state_machine.state_change_count++;
        sprintf(global_state_machine.status_message, "Main loop iteration %d", i);

        printf("Main: %s -> %s (count=%d) msg=%s\n",
               state_names[global_state_machine.previous_state],
               state_names[global_state_machine.current_state],
               global_state_machine.state_change_count,
               global_state_machine.status_message);

        usleep(100000);
    }

    return 0;
}