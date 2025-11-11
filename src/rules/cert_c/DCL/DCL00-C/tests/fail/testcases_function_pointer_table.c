/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: function_pointer_table.c
 *
 * This case demonstrates violations where function pointer tables
 * that don't change are not const-qualified.
 */

#include <stdio.h>

/* Sample functions for the table */
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) { return b != 0 ? a / b : 0; }

void arithmetic_operations(void) {
    /* NON-COMPLIANT: Function pointer table should be const */
    int (*operations[])(int, int) = {
        add,
        subtract,
        multiply,
        divide
    };
    
    /* NON-COMPLIANT: Operation names should be const */
    char *op_names[] = {"Add", "Subtract", "Multiply", "Divide"};
    
    int a = 10, b = 5;
    
    for (int i = 0; i < 4; i++) {
        int result = operations[i](a, b);
        printf("%s(%d, %d) = %d\n", op_names[i], a, b, result);
    }
}

/* State machine example */
void state_init(void) { printf("Initializing...\n"); }
void state_running(void) { printf("Running...\n"); }
void state_paused(void) { printf("Paused...\n"); }
void state_stopped(void) { printf("Stopped...\n"); }

void state_machine(void) {
    /* NON-COMPLIANT: State function table should be const */
    void (*states[])(void) = {
        state_init,
        state_running,
        state_paused,
        state_stopped
    };
    
    /* NON-COMPLIANT: State names should be const */
    char state_names[][10] = {"INIT", "RUNNING", "PAUSED", "STOPPED"};
    
    printf("\nState Machine Demo:\n");
    for (int i = 0; i < 4; i++) {
        printf("State: %s - ", state_names[i]);
        states[i]();
    }
}

int main(void) {
    /* NON-COMPLIANT: Menu function pointers should be const */
    void (*menu_functions[])(void) = {
        arithmetic_operations,
        state_machine
    };
    
    printf("Function Pointer Table Demo\n\n");
    
    for (int i = 0; i < 2; i++) {
        menu_functions[i]();
    }
    
    return 0;
}