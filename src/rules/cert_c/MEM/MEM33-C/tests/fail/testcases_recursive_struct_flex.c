/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: recursive_struct_flex.c
 *
 * This case demonstrates a violation of MEM33-C by attempting to create
 * recursive structures involving flexible array members, which creates
 * complex memory management issues and violates proper usage patterns.
 */

#include <stdio.h>
#include <stdlib.h>

struct recursive_flex {
    size_t count;
    struct recursive_flex *next;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct recursive_flex *head, *current;
    size_t array_size = 3;

    /* Create first node */
    head = malloc(sizeof(struct recursive_flex) + sizeof(int) * array_size);
    if (head == NULL) return 1;

    head->count = array_size;
    head->next = NULL;
    for (size_t i = 0; i < array_size; i++) {
        head->data[i] = (int)(i + 1);
    }

    /* VIOLATION: Attempting to copy recursive structure with assignment */
    struct recursive_flex second_node = *head;  /* Only copies fixed members, not flexible array */

    second_node.next = NULL;  /* Breaks the link anyway */

    printf("Original head data: ");
    for (size_t i = 0; i < head->count; i++) {
        printf("%d ", head->data[i]);
    }
    printf("\n");

    printf("Copied node data: ");
    for (size_t i = 0; i < second_node.count; i++) {
        printf("%d ", second_node.data[i]);  /* Garbage values */
    }
    printf("\n");

    /* VIOLATION: Creating second node with wrong allocation */
    current = malloc(sizeof(struct recursive_flex));  /* Missing space for flexible array */
    if (current == NULL) {
        free(head);
        return 1;
    }

    current->count = 2;
    current->next = head;

    /* VIOLATION: Writing to flexible array without proper space */
    current->data[0] = 100;  /* Buffer overflow */
    current->data[1] = 200;  /* Buffer overflow */

    printf("Second node data (buffer overflow): ");
    for (size_t i = 0; i < current->count; i++) {
        printf("%d ", current->data[i]);
    }
    printf("\n");

    /* VIOLATION: Trying to use in automatic storage */
    struct recursive_flex local_recursive;
    local_recursive.count = 1;
    local_recursive.next = head;
    local_recursive.data[0] = 999;  /* Undefined behavior */

    printf("Local recursive data: %d\n", local_recursive.data[0]);

    free(head);
    free(current);
    return 0;
}