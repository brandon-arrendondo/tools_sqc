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

typedef struct node {
    int data;
    struct node *next;
} node_t;

typedef struct {
    int count;
    char status[64];
    node_t *head;
    double metrics[10];
} complex_state_t;

complex_state_t global_state = {0};

void unsafe_handler(int sig) {
    /* Violation: Accessing complex data structures in signal handler */
    global_state.count++;
    sprintf(global_state.status, "Signal %d handled", sig);

    /* Modifying linked list in signal handler - very dangerous */
    node_t *new_node = malloc(sizeof(node_t));
    if (new_node) {
        new_node->data = sig;
        new_node->next = global_state.head;
        global_state.head = new_node;
    }

    /* Modifying array */
    for (int i = 0; i < 10; i++) {
        global_state.metrics[i] += 0.1;
    }

    printf("Handler: count=%d, status=%s\n", global_state.count, global_state.status);
}

int main() {
    printf("Demonstrating unsafe complex structure access in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, unsafe_handler);

    for (int i = 0; i < 20; i++) {
        global_state.count = i * 10;
        sprintf(global_state.status, "Main processing %d", i);

        /* Initialize metrics array */
        for (int j = 0; j < 10; j++) {
            global_state.metrics[j] = i + j;
        }

        printf("Main: count=%d, status=%s\n", global_state.count, global_state.status);
        usleep(150000);
    }

    /* Cleanup linked list */
    node_t *current = global_state.head;
    while (current) {
        node_t *temp = current;
        current = current->next;
        free(temp);
    }

    return 0;
}