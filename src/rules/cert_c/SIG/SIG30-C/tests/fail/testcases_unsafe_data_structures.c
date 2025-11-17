/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <unistd.h>

struct node {
    int data;
    struct node *next;
};

struct node *head = NULL;

void data_handler(int sig) {
    // VIOLATION: Dynamic list operations using malloc/free
    struct node *new_node = malloc(sizeof(struct node));
    if (new_node != NULL) {
        new_node->data = sig;
        new_node->next = head;
        head = new_node;
    }

    // VIOLATION: Hash table operations (hsearch family)
    ENTRY item, *found_item;
    item.key = "signal";
    item.data = &sig;

    // hcreate() would be called in main, but hsearch is not async-safe
    found_item = hsearch(item, ENTER);

    // VIOLATION: Binary tree operations (tsearch family)
    static void *tree_root = NULL;
    int *signal_copy = malloc(sizeof(int));
    if (signal_copy) {
        *signal_copy = sig;
        tsearch(signal_copy, &tree_root,
               (int(*)(const void*, const void*))strcmp);
    }

    // VIOLATION: Queue operations using dynamic memory
    struct node *temp = head;
    while (temp && temp->next) {
        temp = temp->next;
    }

    // VIOLATION: Complex data manipulation
    qsort(&sig, 1, sizeof(int),
          (int(*)(const void*, const void*))strcmp);
}

int compare_int(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int main() {
    printf("Demonstrating unsafe data structure operations in signal handler\n");
    printf("PID: %d\n", getpid());

    // Initialize hash table
    hcreate(100);

    signal(SIGUSR1, data_handler);

    printf("Send SIGUSR1 to trigger unsafe data operations\n");

    while (1) {
        pause();
    }

    return 0;
}