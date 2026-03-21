/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - No violation without call-site data (params assumed non-null)
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Traversing linked list without NULL checks
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

void print_list(Node *head) {
    Node *current = head;

    // No NULL check before accessing node
    while (current->next != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
}

int main() {
    print_list(NULL);
    return 0;
}