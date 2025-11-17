/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    char *data;
    struct node *next;
} node_t;

typedef struct {
    node_t *head;
    size_t count;
} linked_list_t;

linked_list_t* create_list(void) {
    linked_list_t *list = malloc(sizeof(linked_list_t));
    if (list) {
        list->head = NULL;
        list->count = 0;
    }
    return list;
}

int add_node(linked_list_t *list, const char *data) {
    if (!list || !data) {
        return -1;
    }

    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) {
        return -1;
    }

    new_node->data = malloc(strlen(data) + 1);
    if (!new_node->data) {
        free(new_node);  // Clean up on failure
        return -1;
    }

    strcpy(new_node->data, data);
    new_node->next = list->head;
    list->head = new_node;
    list->count++;

    return 0;
}

void destroy_list(linked_list_t **list) {
    if (!list || !*list) {
        return;
    }

    node_t *current = (*list)->head;
    while (current) {
        node_t *next = current->next;

        // Free node data exactly once
        if (current->data) {
            free(current->data);
            current->data = NULL;
        }

        // Free node exactly once
        free(current);
        current = next;
    }

    // Free list structure exactly once
    free(*list);
    *list = NULL;

    printf("Linked list destroyed\n");
}

void print_list(const linked_list_t *list) {
    if (!list) {
        return;
    }

    printf("List contents (%zu nodes):\n", list->count);
    node_t *current = list->head;
    int index = 0;
    while (current) {
        printf("  Node %d: %s\n", index++, current->data);
        current = current->next;
    }
}

int main() {
    linked_list_t *list = create_list();
    if (!list) {
        printf("Failed to create list\n");
        return 1;
    }

    // Add some nodes
    add_node(list, "First");
    add_node(list, "Second");
    add_node(list, "Third");

    print_list(list);

    // Clean up - all memory freed exactly once
    destroy_list(&list);

    // Safe to call again - does nothing
    destroy_list(&list);

    printf("Program completed successfully\n");
    return 0;
}