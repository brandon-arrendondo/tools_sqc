/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Pass Case: comprehensive_initialization.c
 *
 * This case demonstrates a comprehensive collection of compliant
 * initialization patterns covering all major scenarios.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

/* COMPLIANT: Comprehensive initialization patterns */

/* 1. Basic variable initialization */
void comprehensive_basic_initialization(void) {
    /* Immediate initialization at declaration */
    int counter = 0;
    double value = 0.0;
    char grade = 'A';
    int *ptr = NULL;

    /* Boolean-style initialization */
    int is_active = 1;
    int is_complete = 0;

    /* Array initializations */
    int numbers[5] = {0};                    /* Zero-initialize all */
    int values[3] = {1, 2, 3};              /* Complete initialization */
    char name[50] = {0};                     /* Zero-initialize string buffer */
    char message[] = "Hello, World!";        /* String literal initialization */

    printf("Basic initialization successful\n");
    printf("Counter: %d, Value: %.1f, Grade: %c\n", counter, value, grade);
    printf("Active: %d, Complete: %d\n", is_active, is_complete);
    printf("Message: %s\n", message);
}

/* 2. Structure initialization patterns */
typedef struct {
    int id;
    char name[32];
    double salary;
    int department_id;
    time_t hire_date;
} Employee;

typedef struct {
    int employees[100];
    int count;
    double total_payroll;
    char department_name[64];
} Department;

void comprehensive_struct_initialization(void) {
    /* Zero initialization */
    Employee emp1 = {0};

    /* Designated initializers (C99) */
    Employee emp2 = {
        .id = 1001,
        .name = "John Doe",
        .salary = 75000.0,
        .department_id = 5,
        .hire_date = time(NULL)
    };

    /* Partial initialization with designated initializers */
    Employee emp3 = {
        .id = 1002,
        .name = "Jane Smith",
        .salary = 82000.0
        /* Other fields zero-initialized */
    };

    /* Complex nested structure */
    Department dept = {
        .employees = {1001, 1002},
        .count = 2,
        .total_payroll = emp2.salary + emp3.salary,
        .department_name = "Engineering"
    };

    printf("Struct initialization successful\n");
    printf("Employee %d: %s, Salary: %.2f\n", emp2.id, emp2.name, emp2.salary);
    printf("Department: %s, Employees: %d, Payroll: %.2f\n",
           dept.department_name, dept.count, dept.total_payroll);
}

/* 3. Dynamic memory initialization */
void comprehensive_dynamic_memory(void) {
    /* Safe malloc with explicit initialization */
    int *array1 = malloc(10 * sizeof(int));
    if (array1 != NULL) {
        for (int i = 0; i < 10; i++) {
            array1[i] = i + 1;  /* Explicit initialization */
        }
    }

    /* Zero-initialized allocation with calloc */
    double *array2 = calloc(5, sizeof(double));

    /* String allocation and initialization */
    char *buffer = malloc(256);
    if (buffer != NULL) {
        memset(buffer, 0, 256);  /* Zero-initialize */
        strcpy(buffer, "Dynamic string");
    }

    /* Structure allocation */
    Employee *emp = malloc(sizeof(Employee));
    if (emp != NULL) {
        memset(emp, 0, sizeof(Employee));  /* Zero-initialize first */
        emp->id = 2001;
        strncpy(emp->name, "Dynamic Employee", sizeof(emp->name) - 1);
        emp->salary = 90000.0;
    }

    printf("Dynamic memory initialization successful\n");

    /* Cleanup */
    free(array1);
    free(array2);
    free(buffer);
    free(emp);
}

/* 4. Function parameter and return value initialization */
typedef struct {
    int success;
    int error_code;
    char message[128];
} Result;

Result comprehensive_function_result(int input) {
    Result result = {0};  /* Zero-initialize return value */

    if (input < 0) {
        result.success = 0;
        result.error_code = -1;
        strcpy(result.message, "Invalid input: negative value");
    } else if (input > 1000) {
        result.success = 0;
        result.error_code = -2;
        strcpy(result.message, "Invalid input: value too large");
    } else {
        result.success = 1;
        result.error_code = 0;
        snprintf(result.message, sizeof(result.message),
                "Successfully processed value: %d", input);
    }

    return result;  /* Always returns fully initialized structure */
}

int comprehensive_output_parameters(const int *input, int input_size,
                                   int **output, int *output_size) {
    /* Initialize output parameters immediately */
    if (output != NULL) *output = NULL;
    if (output_size != NULL) *output_size = 0;

    /* Validate inputs */
    if (input == NULL || input_size <= 0 || output == NULL || output_size == NULL) {
        return -1;
    }

    /* Allocate and initialize output */
    int *result = calloc(input_size, sizeof(int));
    if (result == NULL) {
        return -1;
    }

    /* Process data */
    for (int i = 0; i < input_size; i++) {
        result[i] = input[i] * 2;
    }

    /* Set outputs only on success */
    *output = result;
    *output_size = input_size;
    return 0;
}

void comprehensive_function_patterns(void) {
    printf("Function patterns demonstration:\n");

    /* Test return value initialization */
    Result r1 = comprehensive_function_result(50);
    Result r2 = comprehensive_function_result(-10);
    Result r3 = comprehensive_function_result(2000);

    printf("Result 1: %s (code: %d)\n", r1.message, r1.error_code);
    printf("Result 2: %s (code: %d)\n", r2.message, r2.error_code);
    printf("Result 3: %s (code: %d)\n", r3.message, r3.error_code);

    /* Test output parameter initialization */
    int input_data[] = {1, 2, 3, 4, 5};
    int *output_data = NULL;
    int output_size = 0;

    if (comprehensive_output_parameters(input_data, 5, &output_data, &output_size) == 0) {
        printf("Output: ");
        for (int i = 0; i < output_size; i++) {
            printf("%d ", output_data[i]);
        }
        printf("\n");
        free(output_data);
    }
}

/* 5. Control flow initialization patterns */
void comprehensive_control_flow(void) {
    printf("Control flow patterns:\n");

    /* Safe loop initialization */
    int total = 0;
    for (int i = 0; i < 5; i++) {  /* i initialized in for statement */
        total += i;
    }

    /* Safe conditional initialization */
    int score = 85;
    char letter_grade = 'F';  /* Default initialization */
    const char *description = "Unknown";

    if (score >= 90) {
        letter_grade = 'A';
        description = "Excellent";
    } else if (score >= 80) {
        letter_grade = 'B';
        description = "Good";
    } else if (score >= 70) {
        letter_grade = 'C';
        description = "Satisfactory";
    } else if (score >= 60) {
        letter_grade = 'D';
        description = "Poor";
    } else {
        letter_grade = 'F';
        description = "Failing";
    }

    /* Safe switch with default */
    int day = 3;
    const char *day_name = "Unknown";  /* Default initialization */

    switch (day) {
        case 1: day_name = "Monday"; break;
        case 2: day_name = "Tuesday"; break;
        case 3: day_name = "Wednesday"; break;
        case 4: day_name = "Thursday"; break;
        case 5: day_name = "Friday"; break;
        case 6: day_name = "Saturday"; break;
        case 7: day_name = "Sunday"; break;
        default: day_name = "Invalid day"; break;
    }

    printf("Total: %d, Grade: %c (%s), Day: %s\n",
           total, letter_grade, description, day_name);
}

/* 6. Advanced initialization patterns */
void comprehensive_advanced_patterns(void) {
    printf("Advanced patterns:\n");

    /* Union safe initialization */
    union {
        int i;
        float f;
        char c[4];
    } data = {0};  /* Initialize first member */

    data.i = 42;
    printf("Union value: %d\n", data.i);

    /* Bit field initialization */
    struct {
        unsigned int flag1 : 1;
        unsigned int flag2 : 1;
        unsigned int value : 6;
    } flags = {0};  /* Zero-initialize all bit fields */

    flags.flag1 = 1;
    flags.value = 15;
    printf("Flags: flag1=%u, flag2=%u, value=%u\n",
           flags.flag1, flags.flag2, flags.value);

    /* Function pointer initialization */
    int (*operation)(int, int) = NULL;  /* Initialize to NULL */

    /* Variadic function safe usage */
    printf("Variadic test with initialized arguments: ");
    int a = 1, b = 2, c = 3;  /* All initialized */
    printf("%d %d %d\n", a, b, c);
}

/* 7. Error handling with initialization */
void comprehensive_error_handling(void) {
    printf("Error handling patterns:\n");

    /* Resource allocation with cleanup */
    FILE *file = NULL;
    char *buffer = NULL;
    int *data = NULL;
    int success = 0;

    do {  /* Use do-while for cleanup pattern */
        file = fopen("test_comprehensive.txt", "w");
        if (file == NULL) break;

        buffer = malloc(256);
        if (buffer == NULL) break;
        memset(buffer, 0, 256);

        data = calloc(10, sizeof(int));
        if (data == NULL) break;

        /* All allocations successful */
        strcpy(buffer, "Test data");
        for (int i = 0; i < 10; i++) {
            data[i] = i * i;
        }

        fprintf(file, "Buffer: %s\n", buffer);
        fprintf(file, "Data: ");
        for (int i = 0; i < 10; i++) {
            fprintf(file, "%d ", data[i]);
        }
        fprintf(file, "\n");

        success = 1;
    } while (0);

    /* Cleanup (safe even if allocations failed) */
    if (file != NULL) {
        fclose(file);
        unlink("test_comprehensive.txt");  /* Remove test file */
    }
    if (buffer != NULL) {
        memset(buffer, 0, 256);  /* Clear before free */
        free(buffer);
    }
    if (data != NULL) {
        free(data);
    }

    printf("Error handling test: %s\n", success ? "Success" : "Failed");
}

int main(void) {
    printf("=== Comprehensive Initialization Demo ===\n");

    printf("\n1. Basic initialization:\n");
    comprehensive_basic_initialization();

    printf("\n2. Struct initialization:\n");
    comprehensive_struct_initialization();

    printf("\n3. Dynamic memory:\n");
    comprehensive_dynamic_memory();

    printf("\n4. Function patterns:\n");
    comprehensive_function_patterns();

    printf("\n5. Control flow:\n");
    comprehensive_control_flow();

    printf("\n6. Advanced patterns:\n");
    comprehensive_advanced_patterns();

    printf("\n7. Error handling:\n");
    comprehensive_error_handling();

    printf("\n=== All initialization patterns completed successfully ===\n");
    return 0;
}