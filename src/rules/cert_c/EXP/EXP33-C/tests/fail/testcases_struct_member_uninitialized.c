/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: struct_member_uninitialized.c
 *
 * This case demonstrates violations involving uninitialized struct
 * members and complex data structures.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Structure definitions */
typedef struct {
    int id;
    char name[50];
    double salary;
    int active;
} Employee;

typedef struct {
    int x, y;
    char color[20];
} Point;

typedef struct {
    Point points[10];
    int count;
    double total_distance;
} Path;

/* NON-COMPLIANT: Partial struct initialization */
void employee_operations(void) {
    Employee emp;  /* Uninitialized struct */

    /* Partial initialization */
    emp.id = 1001;
    strcpy(emp.name, "John Doe");
    /* salary and active members uninitialized */

    /* Reading uninitialized members */
    printf("Employee ID: %d\n", emp.id);
    printf("Name: %s\n", emp.name);
    printf("Salary: %.2f\n", emp.salary);    /* Undefined behavior */
    printf("Active: %d\n", emp.active);      /* Undefined behavior */

    /* Using uninitialized data in calculations */
    double annual_bonus = emp.salary * 0.1;  /* Undefined behavior */
    printf("Annual bonus: %.2f\n", annual_bonus);
}

/* NON-COMPLIANT: Nested struct initialization issues */
void path_processing(void) {
    Path route;  /* Uninitialized complex struct */

    /* Only initialize some fields */
    route.count = 3;
    route.points[0].x = 0;
    route.points[0].y = 0;
    strcpy(route.points[0].color, "red");

    /* points[1] and points[2] members uninitialized */
    /* total_distance uninitialized */

    /* Reading uninitialized nested struct members */
    for (int i = 0; i < route.count; i++) {
        printf("Point %d: (%d, %d) - %s\n",
               i, route.points[i].x, route.points[i].y, route.points[i].color);
        /* Undefined behavior for i > 0 */
    }

    /* Using uninitialized member in calculation */
    double avg_distance = route.total_distance / route.count;  /* Undefined behavior */
    printf("Average distance: %.2f\n", avg_distance);
}

/* NON-COMPLIANT: Struct passed to function uninitialized */
void print_employee_info(Employee emp) {
    /* Function receives potentially uninitialized struct */
    printf("Processing employee %d: %s\n", emp.id, emp.name);

    if (emp.active) {  /* Reading potentially uninitialized field */
        printf("Employee is active with salary %.2f\n", emp.salary);
    } else {
        printf("Employee is inactive\n");
    }
}

void test_struct_parameter(void) {
    Employee temp_emp;  /* Uninitialized */

    /* Partial initialization */
    temp_emp.id = 2002;
    strcpy(temp_emp.name, "Jane Smith");
    /* salary and active uninitialized */

    print_employee_info(temp_emp);  /* Passing partially initialized struct */
}

/* NON-COMPLIANT: Array of structs with initialization issues */
void process_employee_array(void) {
    Employee team[5];  /* Array of uninitialized structs */

    /* Initialize only first employee */
    team[0].id = 3001;
    strcpy(team[0].name, "Alice");
    team[0].salary = 75000.0;
    team[0].active = 1;

    /* team[1] through team[4] completely uninitialized */

    /* Processing entire array */
    double total_payroll = 0.0;
    for (int i = 0; i < 5; i++) {
        if (team[i].active) {  /* Reading uninitialized active field */
            total_payroll += team[i].salary;  /* Reading uninitialized salary */
            printf("Employee %d (%s): $%.2f\n",
                   team[i].id, team[i].name, team[i].salary);
        }
    }
    printf("Total payroll: $%.2f\n", total_payroll);
}

/* NON-COMPLIANT: Dynamic struct allocation without initialization */
void dynamic_struct_issues(void) {
    Employee *emp_ptr = malloc(sizeof(Employee));  /* Uninitialized memory */

    if (emp_ptr == NULL) {
        return;
    }

    /* Partial initialization of dynamically allocated struct */
    emp_ptr->id = 4001;
    /* name, salary, active uninitialized */

    /* Reading uninitialized fields */
    printf("Dynamic employee ID: %d\n", emp_ptr->id);
    printf("Name: %s\n", emp_ptr->name);         /* Undefined behavior */
    printf("Salary: %.2f\n", emp_ptr->salary);   /* Undefined behavior */

    /* Using uninitialized data in logic */
    if (emp_ptr->active) {  /* Undefined behavior */
        printf("Employee is currently active\n");
    }

    free(emp_ptr);
}

/* NON-COMPLIANT: Struct assignment with uninitialized source */
void struct_assignment_issues(void) {
    Employee source, destination;  /* Both uninitialized */

    /* Partial initialization of source */
    source.id = 5001;
    strcpy(source.name, "Bob Wilson");
    /* salary and active uninitialized in source */

    /* Assignment copies uninitialized data */
    destination = source;  /* Copies uninitialized members */

    /* Reading copied uninitialized data */
    printf("Copied employee - ID: %d, Name: %s\n",
           destination.id, destination.name);
    printf("Salary: %.2f, Active: %d\n",
           destination.salary, destination.active);  /* Undefined behavior */
}

int main(void) {
    printf("=== Struct Member Uninitialized Demo ===\n");

    /* Test 1: Basic struct operations */
    printf("1. Employee operations:\n");
    employee_operations();

    /* Test 2: Complex nested structs */
    printf("\n2. Path processing:\n");
    path_processing();

    /* Test 3: Struct parameter passing */
    printf("\n3. Struct parameter test:\n");
    test_struct_parameter();

    /* Test 4: Array of structs */
    printf("\n4. Employee array processing:\n");
    process_employee_array();

    /* Test 5: Dynamic allocation */
    printf("\n5. Dynamic struct issues:\n");
    dynamic_struct_issues();

    /* Test 6: Struct assignment */
    printf("\n6. Struct assignment issues:\n");
    struct_assignment_issues();

    return 0;
}