/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: data_structure_unchecked.c
 *
 * This case demonstrates violations where data structure operations
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simple linked list node */
typedef struct Node {
    int data;
    struct Node *next;
} Node;

/* Simple stack structure */
typedef struct {
    int *items;
    int top;
    int capacity;
} Stack;

/* NON-COMPLIANT: No validation of list parameter */
void append_to_list(Node **head, int value) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = value;
    new_node->next = NULL;

    /* No check if head is NULL */
    if (*head == NULL) {  /* Could dereference NULL pointer */
        *head = new_node;
    } else {
        Node *current = *head;
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;
    }
}

/* NON-COMPLIANT: No validation of node to remove */
void remove_node(Node **head, Node *node_to_remove) {
    /* No validation of parameters */
    if (*head == node_to_remove) {  /* head could be NULL */
        *head = (*head)->next;
        free(node_to_remove);
        return;
    }

    Node *current = *head;
    while (current->next != node_to_remove) {  /* No check if node exists */
        current = current->next;
    }
    current->next = node_to_remove->next;
    free(node_to_remove);
}

/* NON-COMPLIANT: No validation of stack state */
void push_stack(Stack *stack, int value) {
    /* No check if stack is NULL or full */
    stack->items[++stack->top] = value;  /* Could overflow */
}

/* NON-COMPLIANT: No validation for empty stack */
int pop_stack(Stack *stack) {
    /* No check if stack is NULL or empty */
    return stack->items[stack->top--];  /* Could underflow */
}

/* NON-COMPLIANT: No validation of index */
int get_at_index(Node *head, int index) {
    Node *current = head;
    /* No validation of head or index */
    for (int i = 0; i < index; i++) {
        current = current->next;  /* Could dereference NULL */
    }
    return current->data;
}

/* NON-COMPLIANT: No validation of tree node */
typedef struct TreeNode {
    int value;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

int get_tree_height(TreeNode *root) {
    /* No NULL check for root */
    if (!root->left && !root->right) {  /* Could dereference NULL */
        return 0;
    }
    int left_height = get_tree_height(root->left) + 1;
    int right_height = get_tree_height(root->right) + 1;
    return (left_height > right_height) ? left_height : right_height;
}

/* NON-COMPLIANT: No validation of queue parameters */
typedef struct {
    int *items;
    int front;
    int rear;
    int size;
    int capacity;
} Queue;

void enqueue(Queue *queue, int value) {
    /* No validation of queue state */
    queue->rear = (queue->rear + 1) % queue->capacity;  /* No NULL or full check */
    queue->items[queue->rear] = value;
    queue->size++;
}

int main(void) {
    Node *null_list = NULL;
    Stack *null_stack = NULL;
    Queue *null_queue = NULL;

    /* Examples of dangerous data structure operations */
    // append_to_list(NULL, 42);  /* NULL pointer dereference */
    // remove_node(&null_list, (Node *)0x1234);  /* Invalid node */
    // push_stack(null_stack, 10);  /* NULL stack */
    // pop_stack(null_stack);  /* NULL stack */
    // get_at_index(null_list, 5);  /* NULL list or out of bounds */
    // get_tree_height(NULL);  /* NULL tree */
    // enqueue(null_queue, 20);  /* NULL queue */

    printf("Data structure functions compiled but lack parameter validation\n");
    return 0;
}