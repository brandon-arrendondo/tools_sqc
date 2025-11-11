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

/* Violation: Accessing linked lists in signal handler */
typedef struct node {
    int data;
    char label[32];
    struct node *next;
    struct node *prev;
} node_t;

typedef struct {
    node_t *head;
    node_t *tail;
    int count;
    char list_name[64];
} linked_list_t;

linked_list_t global_list = {NULL, NULL, 0, "global_data_list"};

node_t* create_node(int value, const char *label) {
    node_t *new_node = malloc(sizeof(node_t));
    if (new_node) {
        new_node->data = value;
        strcpy(new_node->label, label);
        new_node->next = NULL;
        new_node->prev = NULL;
    }
    return new_node;
}

void add_node(linked_list_t *list, int value, const char *label) {
    node_t *new_node = create_node(value, label);
    if (!new_node) return;

    if (list->head == NULL) {
        list->head = list->tail = new_node;
    } else {
        new_node->prev = list->tail;
        list->tail->next = new_node;
        list->tail = new_node;
    }
    list->count++;
}

void unsafe_handler(int sig) {
    /* Violation: Modifying linked list structure in signal handler */
    char signal_label[32];
    sprintf(signal_label, "signal_%d", sig);

    /* Adding nodes to linked list - very dangerous */
    add_node(&global_list, sig * 100, signal_label);

    /* Traversing and modifying linked list */
    node_t *current = global_list.head;
    int traverse_count = 0;
    while (current && traverse_count < 5) {
        current->data += sig;
        sprintf(current->label, "modified_by_%d", sig);
        current = current->next;
        traverse_count++;
    }

    /* Updating list metadata */
    sprintf(global_list.list_name, "signal_modified_%d", sig);

    printf("Handler: list_count=%d, name=%s, signal=%d\n",
           global_list.count, global_list.list_name, sig);
}

int main() {
    printf("Demonstrating unsafe linked list access in signal handler\n");
    printf("PID: %d\n", getpid());

    /* Initialize list with some nodes */
    add_node(&global_list, 1, "initial_1");
    add_node(&global_list, 2, "initial_2");
    add_node(&global_list, 3, "initial_3");

    signal(SIGUSR1, unsafe_handler);

    for (int i = 0; i < 25; i++) {
        /* Main program also modifies the linked list */
        char main_label[32];
        sprintf(main_label, "main_%d", i);
        add_node(&global_list, i * 10, main_label);

        /* Traverse and display list */
        node_t *current = global_list.head;
        int display_count = 0;
        printf("Main[%d]: List contents: ", i);
        while (current && display_count < 3) {
            printf("(%d:%s) ", current->data, current->label);
            current = current->next;
            display_count++;
        }
        printf("... total_nodes=%d\n", global_list.count);

        usleep(120000);
    }

    /* Cleanup */
    node_t *current = global_list.head;
    while (current) {
        node_t *temp = current;
        current = current->next;
        free(temp);
    }

    return 0;
}