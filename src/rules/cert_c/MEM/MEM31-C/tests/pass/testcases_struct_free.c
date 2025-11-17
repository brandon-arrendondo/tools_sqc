/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: PASS
 * Reason: Dynamically allocated struct is properly freed after use
 */

#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[50];
    double salary;
} Employee;

void create_employee() {
    Employee *emp = malloc(sizeof(Employee));
    if (emp == NULL) {
        return;
    }

    // Initialize the employee
    emp->id = 12345;
    strcpy(emp->name, "John Doe");
    emp->salary = 55000.0;

    // Use the employee data
    printf("Employee: %s (ID: %d) - Salary: %.2f\n",
           emp->name, emp->id, emp->salary);

    // Properly free the struct
    free(emp);
}