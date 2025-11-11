/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: PASS
 * Reason: Linked list node pointers are checked before traversal
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

void print_list(Node *head) {
    Node *current = head;

    if (current == NULL) {
        printf("Empty list\n");
        return;
    }

    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

int main() {
    Node *head = malloc(sizeof(Node));

    if (head != NULL) {
        head->data = 1;
        head->next = NULL;
        print_list(head);
        free(head);
    }

    print_list(NULL);  // Safe - function handles NULL
    return 0;
}