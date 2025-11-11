/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct node {
    int data;
    struct node* next;
} node_t;

node_t* head = NULL;
volatile sig_atomic_t list_operations = 0;

void list_handler(int sig) {
    list_operations++;

    printf("Handler: Signal %d modifying linked list\n", sig);

    // Violation: Linked list operations without proper signal masking
    // can cause pointer corruption and memory leaks
    node_t* new_node = malloc(sizeof(node_t));
    if (new_node == NULL) {
        printf("Handler: malloc failed\n");
        return;
    }

    new_node->data = sig * 100 + list_operations;

    // Vulnerable insertion at head
    new_node->next = head;

    // Create vulnerability window
    usleep(100000);

    head = new_node;

    printf("Handler: Added node with data %d\n", new_node->data);

    // Traverse list (vulnerable to corruption)
    node_t* current = head;
    int count = 0;

    printf("Handler: List contents: ");
    while (current != NULL && count < 10) {
        printf("%d ", current->data);
        current = current->next;
        count++;

        // Vulnerability window during traversal
        usleep(10000);

        // Detect corruption
        if (count > 1000) {
            printf("LOOP DETECTED!");
            break;
        }
    }
    printf("\n");

    printf("Handler: List operation %d complete\n", list_operations);
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = list_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Pointer operations vulnerable to interruption
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send signals to corrupt linked list operations\n");

    while (1) {
        printf("Main: List operations: %d\n", list_operations);

        // Count nodes in main thread (also vulnerable)
        node_t* current = head;
        int node_count = 0;

        while (current != NULL && node_count < 1000) {
            node_count++;
            current = current->next;

            // Check for obvious corruption
            if (current == head && node_count > 1) {
                printf("Main: ERROR - Circular reference detected!\n");
                break;
            }
        }

        printf("Main: List has %d nodes\n", node_count);

        if (node_count >= 1000) {
            printf("Main: ERROR - List appears corrupted (too many nodes)\n");
        }

        // Occasionally clean up some nodes
        static int cleanup_counter = 0;
        cleanup_counter++;
        if (cleanup_counter % 5 == 0 && head != NULL) {
            node_t* to_delete = head;
            head = head->next;
            free(to_delete);
            printf("Main: Cleaned up one node\n");
        }

        sleep(2);
    }

    return 0;
}