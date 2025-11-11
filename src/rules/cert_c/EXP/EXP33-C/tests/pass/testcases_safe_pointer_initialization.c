/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Pass Case: safe_pointer_initialization.c
 *
 * This case demonstrates compliant pointer initialization patterns
 * that prevent dereferencing uninitialized pointers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* COMPLIANT: Safe pointer initialization to NULL */
void safe_pointer_usage(void) {
    char *buffer = NULL;        /* Initialize to NULL */
    int *numbers = NULL;        /* Initialize to NULL */
    FILE *file = NULL;          /* Initialize to NULL */

    /* Safe pointer allocation */
    buffer = malloc(100);
    if (buffer != NULL) {       /* Check before use */
        strcpy(buffer, "Safe string");
        printf("Buffer: %s\n", buffer);
        free(buffer);
        buffer = NULL;          /* Reset to NULL after free */
    }

    /* Safe array allocation */
    numbers = calloc(10, sizeof(int));
    if (numbers != NULL) {      /* Check before use */
        for (int i = 0; i < 10; i++) {
            numbers[i] = i * i;
        }

        printf("Numbers: ");
        for (int i = 0; i < 10; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n");

        free(numbers);
        numbers = NULL;         /* Reset to NULL after free */
    }

    /* Safe file pointer usage */
    file = fopen("test.txt", "w");
    if (file != NULL) {         /* Check before use */
        fprintf(file, "Test data\n");
        fclose(file);
        file = NULL;            /* Reset to NULL after close */
    }
}

/* COMPLIANT: Safe function pointer initialization */
typedef int (*MathOperation)(int a, int b);

int safe_add(int a, int b) { return a + b; }
int safe_multiply(int a, int b) { return a * b; }

void safe_function_pointer_usage(void) {
    MathOperation op = NULL;    /* Initialize to NULL */

    /* Safe assignment and usage */
    op = safe_add;
    if (op != NULL) {           /* Check before calling */
        int result = op(5, 3);
        printf("Addition: 5 + 3 = %d\n", result);
    }

    /* Change operation safely */
    op = safe_multiply;
    if (op != NULL) {           /* Check before calling */
        int result = op(5, 3);
        printf("Multiplication: 5 * 3 = %d\n", result);
    }

    /* Demonstrate safe NULL handling */
    op = NULL;
    if (op != NULL) {
        int result = op(5, 3);  /* This won't execute */
        printf("This won't print: %d\n", result);
    } else {
        printf("Function pointer is NULL - operation skipped\n");
    }
}

/* COMPLIANT: Safe array of pointers initialization */
void safe_pointer_array_usage(void) {
    const char *fruits[5] = {NULL};  /* Initialize all to NULL */

    /* Assign string literals safely */
    fruits[0] = "apple";
    fruits[1] = "banana";
    fruits[2] = "cherry";
    /* fruits[3] and fruits[4] remain NULL */

    printf("Fruit list:\n");
    for (int i = 0; i < 5; i++) {
        if (fruits[i] != NULL) {     /* Check before use */
            printf("  %d: %s\n", i, fruits[i]);
        } else {
            printf("  %d: (empty)\n", i);
        }
    }
}

/* COMPLIANT: Safe dynamic string array */
void safe_dynamic_string_array(void) {
    int count = 4;
    char **strings = calloc(count, sizeof(char*));  /* Initialize to NULL */

    if (strings == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    /* Allocate individual strings safely */
    for (int i = 0; i < count; i++) {
        strings[i] = malloc(50);
        if (strings[i] != NULL) {
            snprintf(strings[i], 50, "String_%d", i);
        }
    }

    /* Use strings safely */
    printf("Dynamic string array:\n");
    for (int i = 0; i < count; i++) {
        if (strings[i] != NULL) {    /* Check before use */
            printf("  %d: %s\n", i, strings[i]);
        } else {
            printf("  %d: (allocation failed)\n", i);
        }
    }

    /* Safe cleanup */
    for (int i = 0; i < count; i++) {
        if (strings[i] != NULL) {    /* Check before free */
            free(strings[i]);
            strings[i] = NULL;       /* Reset to NULL */
        }
    }
    free(strings);
    strings = NULL;                  /* Reset to NULL */
}

/* COMPLIANT: Safe linked list with pointer initialization */
typedef struct Node {
    int data;
    struct Node *next;
} Node;

void safe_linked_list_operations(void) {
    Node *head = NULL;           /* Initialize to NULL */
    Node *current = NULL;        /* Initialize to NULL */

    /* Create nodes safely */
    for (int i = 1; i <= 5; i++) {
        Node *new_node = malloc(sizeof(Node));
        if (new_node == NULL) {
            printf("Memory allocation failed for node %d\n", i);
            break;
        }

        new_node->data = i * 10;
        new_node->next = NULL;   /* Initialize next pointer */

        if (head == NULL) {      /* First node */
            head = new_node;
            current = new_node;
        } else {                 /* Subsequent nodes */
            current->next = new_node;
            current = new_node;
        }
    }

    /* Traverse list safely */
    printf("Linked list contents:\n");
    current = head;
    while (current != NULL) {    /* Check before dereferencing */
        printf("  Data: %d\n", current->data);
        current = current->next;
    }

    /* Safe cleanup */
    current = head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;                 /* Reset to NULL */
}

/* COMPLIANT: Safe double pointer usage */
void safe_allocate_array(int **array, int size) {
    if (array == NULL || size <= 0) {
        return;  /* Invalid parameters */
    }

    *array = NULL;               /* Initialize output pointer */

    int *new_array = calloc(size, sizeof(int));
    if (new_array == NULL) {
        return;  /* Allocation failed - output remains NULL */
    }

    /* Initialize array values */
    for (int i = 0; i < size; i++) {
        new_array[i] = i + 1;
    }

    *array = new_array;          /* Set output pointer only on success */
}

void safe_double_pointer_usage(void) {
    int *my_array = NULL;        /* Initialize to NULL */

    safe_allocate_array(&my_array, 8);

    if (my_array != NULL) {      /* Check before use */
        printf("Allocated array contents: ");
        for (int i = 0; i < 8; i++) {
            printf("%d ", my_array[i]);
        }
        printf("\n");

        free(my_array);
        my_array = NULL;         /* Reset to NULL */
    } else {
        printf("Array allocation failed\n");
    }
}

/* COMPLIANT: Safe callback with NULL pointer handling */
typedef void (*Callback)(int value);

void safe_callback_example(int value) {
    printf("Callback called with value: %d\n", value);
}

void safe_process_with_callback(int *data, int size, Callback callback) {
    if (data == NULL || size <= 0) {
        return;  /* Invalid parameters */
    }

    for (int i = 0; i < size; i++) {
        if (callback != NULL) {  /* Check function pointer before calling */
            callback(data[i]);
        } else {
            printf("No callback - processing value: %d\n", data[i]);
        }
    }
}

void safe_callback_usage(void) {
    int values[] = {10, 20, 30, 40, 50};
    int count = sizeof(values) / sizeof(values[0]);

    printf("With callback:\n");
    safe_process_with_callback(values, count, safe_callback_example);

    printf("\nWithout callback (NULL):\n");
    safe_process_with_callback(values, count, NULL);
}

int main(void) {
    printf("=== Safe Pointer Initialization Demo ===\n");

    printf("1. Basic pointer usage:\n");
    safe_pointer_usage();

    printf("\n2. Function pointer usage:\n");
    safe_function_pointer_usage();

    printf("\n3. Pointer array usage:\n");
    safe_pointer_array_usage();

    printf("\n4. Dynamic string array:\n");
    safe_dynamic_string_array();

    printf("\n5. Linked list operations:\n");
    safe_linked_list_operations();

    printf("\n6. Double pointer usage:\n");
    safe_double_pointer_usage();

    printf("\n7. Callback usage:\n");
    safe_callback_usage();

    return 0;
}