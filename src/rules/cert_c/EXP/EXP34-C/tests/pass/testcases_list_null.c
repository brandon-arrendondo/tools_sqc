/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - Known limitation: params assumed non-null without call-site data.
 *         This IS a real null deref (main passes NULL), but requires intra-file
 *         call-site analysis to detect. Move to fail/ when implemented.
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