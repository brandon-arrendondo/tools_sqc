/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Pass Case: safe_advanced_patterns.c
 *
 * This case demonstrates advanced compliant patterns for memory
 * initialization in complex scenarios.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

/* COMPLIANT: Safe RAII-style resource management */
typedef struct {
    FILE *file;
    char *buffer;
    int *array;
    int is_initialized;
} ResourceManager;

int safe_resource_init(ResourceManager *rm, const char *filename, size_t buffer_size, int array_size) {
    if (rm == NULL) {
        return -1;
    }

    /* Initialize all fields to safe defaults immediately */
    rm->file = NULL;
    rm->buffer = NULL;
    rm->array = NULL;
    rm->is_initialized = 0;

    /* Allocate resources with proper error handling */
    rm->buffer = calloc(buffer_size, sizeof(char));
    if (rm->buffer == NULL) {
        return -1;
    }

    rm->array = calloc(array_size, sizeof(int));
    if (rm->array == NULL) {
        free(rm->buffer);
        rm->buffer = NULL;
        return -1;
    }

    if (filename != NULL) {
        rm->file = fopen(filename, "w+");
        if (rm->file == NULL) {
            free(rm->buffer);
            free(rm->array);
            rm->buffer = NULL;
            rm->array = NULL;
            return -1;
        }
    }

    rm->is_initialized = 1;
    return 0;
}

void safe_resource_cleanup(ResourceManager *rm) {
    if (rm == NULL) {
        return;
    }

    if (rm->file != NULL) {
        fclose(rm->file);
        rm->file = NULL;
    }

    if (rm->buffer != NULL) {
        /* Clear sensitive data before freeing */
        memset(rm->buffer, 0, strlen(rm->buffer));
        free(rm->buffer);
        rm->buffer = NULL;
    }

    if (rm->array != NULL) {
        free(rm->array);
        rm->array = NULL;
    }

    rm->is_initialized = 0;
}

void safe_resource_manager_demo(void) {
    ResourceManager rm;  /* Will be initialized by function */

    if (safe_resource_init(&rm, "test_output.txt", 256, 10) == 0) {
        printf("Resource manager initialized successfully\n");

        /* Use resources safely */
        strcpy(rm.buffer, "Safe resource usage");
        for (int i = 0; i < 10; i++) {
            rm.array[i] = i * i;
        }

        fprintf(rm.file, "Buffer: %s\n", rm.buffer);
        fprintf(rm.file, "Array: ");
        for (int i = 0; i < 10; i++) {
            fprintf(rm.file, "%d ", rm.array[i]);
        }
        fprintf(rm.file, "\n");

        printf("Data written to file successfully\n");
    } else {
        printf("Resource manager initialization failed\n");
    }

    safe_resource_cleanup(&rm);
    printf("Resources cleaned up\n");
}

/* COMPLIANT: Safe exception handling with setjmp/longjmp */
jmp_buf safe_error_handler;
int error_occurred = 0;

typedef enum {
    NO_ERROR = 0,
    NULL_POINTER_ERROR,
    ALLOCATION_ERROR,
    CALCULATION_ERROR
} SafeErrorType;

void safe_complex_operation_with_exceptions(int *input, int size, int **output, int *output_size) {
    SafeErrorType error = NO_ERROR;

    /* Initialize outputs immediately */
    if (output != NULL) *output = NULL;
    if (output_size != NULL) *output_size = 0;

    /* Set up exception handling */
    if (setjmp(safe_error_handler) != 0) {
        /* Exception occurred - cleanup and return */
        if (output != NULL && *output != NULL) {
            free(*output);
            *output = NULL;
        }
        if (output_size != NULL) {
            *output_size = 0;
        }
        return;
    }

    /* Validate parameters */
    if (input == NULL || output == NULL || output_size == NULL || size <= 0) {
        error_occurred = 1;
        longjmp(safe_error_handler, NULL_POINTER_ERROR);
    }

    /* Allocate output array */
    int *result = malloc(size * sizeof(int));
    if (result == NULL) {
        error_occurred = 1;
        longjmp(safe_error_handler, ALLOCATION_ERROR);
    }

    /* Initialize all elements */
    for (int i = 0; i < size; i++) {
        result[i] = 0;
    }

    /* Perform calculations with error checking */
    for (int i = 0; i < size; i++) {
        if (input[i] > INT_MAX / 2) {
            free(result);
            error_occurred = 1;
            longjmp(safe_error_handler, CALCULATION_ERROR);
        }
        result[i] = input[i] * 2;
    }

    /* Success - commit results */
    *output = result;
    *output_size = size;
    error_occurred = 0;
}

void safe_exception_handling_demo(void) {
    printf("Exception handling demo:\n");

    int input_data[] = {1, 2, 3, 4, 5};
    int input_size = sizeof(input_data) / sizeof(input_data[0]);
    int *output_data = NULL;
    int output_size = 0;

    error_occurred = 0;
    safe_complex_operation_with_exceptions(input_data, input_size, &output_data, &output_size);

    if (!error_occurred && output_data != NULL) {
        printf("Operation successful, output: ");
        for (int i = 0; i < output_size; i++) {
            printf("%d ", output_data[i]);
        }
        printf("\n");
        free(output_data);
    } else {
        printf("Operation failed with error\n");
    }

    /* Test error case */
    error_occurred = 0;
    safe_complex_operation_with_exceptions(NULL, 5, &output_data, &output_size);
    if (error_occurred) {
        printf("NULL input correctly handled\n");
    }
}

/* COMPLIANT: Safe memory pool management */
typedef struct MemoryBlock {
    void *data;
    size_t size;
    int is_free;
    struct MemoryBlock *next;
} MemoryBlock;

typedef struct {
    MemoryBlock *blocks;
    size_t total_size;
    size_t used_size;
    int is_initialized;
} MemoryPool;

int safe_memory_pool_init(MemoryPool *pool, size_t initial_size) {
    if (pool == NULL || initial_size == 0) {
        return -1;
    }

    /* Initialize pool structure */
    pool->blocks = NULL;
    pool->total_size = 0;
    pool->used_size = 0;
    pool->is_initialized = 0;

    /* Allocate initial block */
    MemoryBlock *first_block = malloc(sizeof(MemoryBlock));
    if (first_block == NULL) {
        return -1;
    }

    first_block->data = calloc(1, initial_size);  /* Zero-initialized */
    if (first_block->data == NULL) {
        free(first_block);
        return -1;
    }

    first_block->size = initial_size;
    first_block->is_free = 1;
    first_block->next = NULL;

    pool->blocks = first_block;
    pool->total_size = initial_size;
    pool->used_size = 0;
    pool->is_initialized = 1;

    return 0;
}

void *safe_memory_pool_alloc(MemoryPool *pool, size_t size) {
    if (pool == NULL || !pool->is_initialized || size == 0) {
        return NULL;
    }

    /* Find suitable free block */
    MemoryBlock *current = pool->blocks;
    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            current->is_free = 0;
            pool->used_size += current->size;

            /* Return zero-initialized memory */
            memset(current->data, 0, current->size);
            return current->data;
        }
        current = current->next;
    }

    return NULL;  /* No suitable block found */
}

void safe_memory_pool_cleanup(MemoryPool *pool) {
    if (pool == NULL || !pool->is_initialized) {
        return;
    }

    MemoryBlock *current = pool->blocks;
    while (current != NULL) {
        MemoryBlock *next = current->next;

        /* Clear memory before freeing */
        if (current->data != NULL) {
            memset(current->data, 0, current->size);
            free(current->data);
        }
        free(current);

        current = next;
    }

    pool->blocks = NULL;
    pool->total_size = 0;
    pool->used_size = 0;
    pool->is_initialized = 0;
}

void safe_memory_pool_demo(void) {
    MemoryPool pool;  /* Will be initialized by function */

    if (safe_memory_pool_init(&pool, 1024) == 0) {
        printf("Memory pool initialized with 1024 bytes\n");

        /* Allocate some memory blocks */
        char *buffer1 = (char *)safe_memory_pool_alloc(&pool, 100);
        int *array1 = (int *)safe_memory_pool_alloc(&pool, 10 * sizeof(int));

        if (buffer1 != NULL && array1 != NULL) {
            /* Use allocated memory safely */
            strcpy(buffer1, "Pool allocation test");
            for (int i = 0; i < 10; i++) {
                array1[i] = i * 3;
            }

            printf("Buffer: %s\n", buffer1);
            printf("Array: ");
            for (int i = 0; i < 10; i++) {
                printf("%d ", array1[i]);
            }
            printf("\n");

            printf("Pool usage: %zu/%zu bytes\n", pool.used_size, pool.total_size);
        } else {
            printf("Memory pool allocation failed\n");
        }
    } else {
        printf("Memory pool initialization failed\n");
    }

    safe_memory_pool_cleanup(&pool);
    printf("Memory pool cleaned up\n");
}

/* COMPLIANT: Safe state machine implementation */
typedef enum {
    STATE_IDLE,
    STATE_PROCESSING,
    STATE_COMPLETE,
    STATE_ERROR
} MachineState;

typedef struct {
    MachineState current_state;
    MachineState previous_state;
    int data_value;
    char status_message[100];
    int is_initialized;
} StateMachine;

int safe_state_machine_init(StateMachine *sm) {
    if (sm == NULL) {
        return -1;
    }

    /* Initialize all fields explicitly */
    sm->current_state = STATE_IDLE;
    sm->previous_state = STATE_IDLE;
    sm->data_value = 0;
    memset(sm->status_message, 0, sizeof(sm->status_message));
    strcpy(sm->status_message, "Initialized");
    sm->is_initialized = 1;

    return 0;
}

int safe_state_machine_transition(StateMachine *sm, MachineState new_state) {
    if (sm == NULL || !sm->is_initialized) {
        return -1;
    }

    sm->previous_state = sm->current_state;
    sm->current_state = new_state;

    /* Update status based on new state */
    switch (new_state) {
        case STATE_IDLE:
            strcpy(sm->status_message, "Machine is idle");
            break;
        case STATE_PROCESSING:
            strcpy(sm->status_message, "Processing data");
            break;
        case STATE_COMPLETE:
            strcpy(sm->status_message, "Processing complete");
            break;
        case STATE_ERROR:
            strcpy(sm->status_message, "Error occurred");
            break;
        default:
            strcpy(sm->status_message, "Unknown state");
            return -1;
    }

    return 0;
}

void safe_state_machine_demo(void) {
    StateMachine sm;  /* Will be initialized by function */

    if (safe_state_machine_init(&sm) == 0) {
        printf("State machine initialized: %s\n", sm.status_message);

        /* Simulate state transitions */
        safe_state_machine_transition(&sm, STATE_PROCESSING);
        printf("Transitioned to processing: %s\n", sm.status_message);

        sm.data_value = 42;  /* Process some data */

        safe_state_machine_transition(&sm, STATE_COMPLETE);
        printf("Transitioned to complete: %s (data: %d)\n", sm.status_message, sm.data_value);

        safe_state_machine_transition(&sm, STATE_IDLE);
        printf("Transitioned to idle: %s\n", sm.status_message);
    } else {
        printf("State machine initialization failed\n");
    }
}

int main(void) {
    printf("=== Safe Advanced Patterns Demo ===\n");

    printf("1. Resource manager:\n");
    safe_resource_manager_demo();

    printf("\n2. Exception handling:\n");
    safe_exception_handling_demo();

    printf("\n3. Memory pool management:\n");
    safe_memory_pool_demo();

    printf("\n4. State machine:\n");
    safe_state_machine_demo();

    return 0;
}