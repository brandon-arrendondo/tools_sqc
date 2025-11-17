/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Pass Case: safe_struct_initialization.c
 *
 * This case demonstrates compliant struct initialization patterns
 * that ensure all members are properly initialized.
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

/* COMPLIANT: Complete struct initialization using designated initializers */
void safe_employee_operations(void) {
    /* C99 designated initializers ensure all fields are set */
    Employee emp = {
        .id = 1001,
        .name = "John Doe",
        .salary = 75000.0,
        .active = 1
    };

    printf("Employee ID: %d\n", emp.id);
    printf("Name: %s\n", emp.name);
    printf("Salary: %.2f\n", emp.salary);
    printf("Active: %d\n", emp.active);

    /* Safe calculation using initialized data */
    double annual_bonus = emp.salary * 0.1;
    printf("Annual bonus: %.2f\n", annual_bonus);
}

/* COMPLIANT: Zero-initialization with explicit field setting */
void safe_path_processing(void) {
    Path route = {0};  /* Zero-initialize entire struct */

    /* Explicitly set required fields */
    route.count = 3;
    route.total_distance = 0.0;

    /* Initialize array elements explicitly */
    route.points[0] = (Point){.x = 0, .y = 0, .color = "red"};
    route.points[1] = (Point){.x = 10, .y = 5, .color = "green"};
    route.points[2] = (Point){.x = 20, .y = 10, .color = "blue"};

    /* Safe access to all initialized data */
    for (int i = 0; i < route.count; i++) {
        printf("Point %d: (%d, %d) - %s\n",
               i, route.points[i].x, route.points[i].y, route.points[i].color);
    }

    printf("Total distance: %.2f\n", route.total_distance);
}

/* COMPLIANT: Function with fully initialized struct parameter */
void print_employee_info(Employee emp) {
    printf("Processing employee %d: %s\n", emp.id, emp.name);

    if (emp.active) {
        printf("Employee is active with salary %.2f\n", emp.salary);
    } else {
        printf("Employee is inactive\n");
    }
}

void safe_struct_parameter_passing(void) {
    /* Initialize struct completely before passing */
    Employee temp_emp = {
        .id = 2002,
        .name = "Jane Smith",
        .salary = 82000.0,
        .active = 1
    };

    print_employee_info(temp_emp);
}

/* COMPLIANT: Array of structs with complete initialization */
void safe_employee_array_processing(void) {
    /* Initialize array with complete struct literals */
    Employee team[] = {
        {.id = 3001, .name = "Alice", .salary = 75000.0, .active = 1},
        {.id = 3002, .name = "Bob", .salary = 80000.0, .active = 1},
        {.id = 3003, .name = "Carol", .salary = 85000.0, .active = 0},
        {.id = 3004, .name = "Dave", .salary = 78000.0, .active = 1},
        {.id = 3005, .name = "Eve", .salary = 82000.0, .active = 1}
    };
    int team_size = sizeof(team) / sizeof(team[0]);

    /* Safe processing of fully initialized array */
    double total_payroll = 0.0;
    int active_count = 0;

    for (int i = 0; i < team_size; i++) {
        if (team[i].active) {
            total_payroll += team[i].salary;
            active_count++;
            printf("Employee %d (%s): $%.2f\n",
                   team[i].id, team[i].name, team[i].salary);
        }
    }

    printf("Total payroll: $%.2f for %d active employees\n",
           total_payroll, active_count);
}

/* COMPLIANT: Dynamic struct allocation with proper initialization */
void safe_dynamic_struct_allocation(void) {
    Employee *emp_ptr = malloc(sizeof(Employee));

    if (emp_ptr == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    /* Initialize all fields explicitly */
    emp_ptr->id = 4001;
    strncpy(emp_ptr->name, "Dynamic Employee", sizeof(emp_ptr->name) - 1);
    emp_ptr->name[sizeof(emp_ptr->name) - 1] = '\0';
    emp_ptr->salary = 90000.0;
    emp_ptr->active = 1;

    /* Safe access to initialized fields */
    printf("Dynamic employee ID: %d\n", emp_ptr->id);
    printf("Name: %s\n", emp_ptr->name);
    printf("Salary: %.2f\n", emp_ptr->salary);

    if (emp_ptr->active) {
        printf("Employee is currently active\n");
    }

    free(emp_ptr);
}

/* COMPLIANT: Struct assignment with properly initialized source */
void safe_struct_assignment(void) {
    /* Initialize source struct completely */
    Employee source = {
        .id = 5001,
        .name = "Source Employee",
        .salary = 95000.0,
        .active = 1
    };

    /* Safe assignment - all data is initialized */
    Employee destination = source;

    /* Safe access to copied data */
    printf("Copied employee - ID: %d, Name: %s\n",
           destination.id, destination.name);
    printf("Salary: %.2f, Active: %d\n",
           destination.salary, destination.active);
}

/* COMPLIANT: Nested struct initialization */
typedef struct {
    Employee manager;
    Employee team_members[3];
    int team_size;
    char department[30];
} Department;

void safe_nested_struct_initialization(void) {
    Department dept = {
        .manager = {
            .id = 9001,
            .name = "Manager Smith",
            .salary = 120000.0,
            .active = 1
        },
        .team_members = {
            {.id = 9002, .name = "Dev A", .salary = 85000.0, .active = 1},
            {.id = 9003, .name = "Dev B", .salary = 87000.0, .active = 1},
            {.id = 9004, .name = "Dev C", .salary = 83000.0, .active = 0}
        },
        .team_size = 3,
        .department = "Engineering"
    };

    printf("Department: %s\n", dept.department);
    printf("Manager: %s (ID: %d)\n", dept.manager.name, dept.manager.id);
    printf("Team members:\n");

    for (int i = 0; i < dept.team_size; i++) {
        printf("  %s (ID: %d) - %s\n",
               dept.team_members[i].name,
               dept.team_members[i].id,
               dept.team_members[i].active ? "Active" : "Inactive");
    }
}

int main(void) {
    printf("=== Safe Struct Initialization Demo ===\n");

    printf("1. Safe employee operations:\n");
    safe_employee_operations();

    printf("\n2. Safe path processing:\n");
    safe_path_processing();

    printf("\n3. Safe struct parameter passing:\n");
    safe_struct_parameter_passing();

    printf("\n4. Safe employee array processing:\n");
    safe_employee_array_processing();

    printf("\n5. Safe dynamic struct allocation:\n");
    safe_dynamic_struct_allocation();

    printf("\n6. Safe struct assignment:\n");
    safe_struct_assignment();

    printf("\n7. Safe nested struct initialization:\n");
    safe_nested_struct_initialization();

    return 0;
}