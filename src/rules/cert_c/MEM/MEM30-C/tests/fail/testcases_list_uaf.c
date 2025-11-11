/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Frees node in linked list but continues traversal
 */

#include <stdlib.h>
#include <stdio.h>

typedef struct node {
    int data;
    struct node *next;
} node_t;

int main() {
    node_t *head = malloc(sizeof(node_t));
    head->data = 1;
    head->next = malloc(sizeof(node_t));
    head->next->data = 2;
    head->next->next = NULL;

    node_t *current = head;
    while (current != NULL) {
        printf("Data: %d\n", current->data);

        node_t *to_free = current;
        current = current->next;
        free(to_free);

        // BUG: Continue loop with potentially freed 'current'
        if (current != NULL) {
            printf("Next data: %d\n", current->data);
        }
    }

    return 0;
}