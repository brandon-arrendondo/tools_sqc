/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Pass Case: const_data_structures.c
 *
 * This case demonstrates compliant code that properly uses const
 * qualification with complex data structures and algorithms.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* COMPLIANT: Const-qualified complex structures */
typedef struct {
    const char *name;
    const int id;
    const double salary;
    const char *department;
} Employee;

typedef struct {
    const char *title;
    const char *author;
    const int year;
    const double price;
    const char *isbn;
} Book;

/* COMPLIANT: Node structure for demonstration */
typedef struct Node {
    const int data;
    const struct Node *next;
} Node;

/* COMPLIANT: Global const data structures */
static const Employee COMPANY_EMPLOYEES[] = {
    {"Alice Johnson", 1001, 75000.0, "Engineering"},
    {"Bob Smith", 1002, 82000.0, "Engineering"},
    {"Carol Davis", 1003, 68000.0, "Marketing"},
    {"David Wilson", 1004, 71000.0, "Sales"},
    {"Eve Brown", 1005, 95000.0, "Management"}
};

static const Book LIBRARY_BOOKS[] = {
    {"The C Programming Language", "Kernighan & Ritchie", 1988, 45.99, "978-0131103627"},
    {"Clean Code", "Robert Martin", 2008, 42.99, "978-0132350884"},
    {"Design Patterns", "Gang of Four", 1994, 54.99, "978-0201633610"},
    {"Refactoring", "Martin Fowler", 1999, 49.99, "978-0201485677"},
    {"Code Complete", "Steve McConnell", 2004, 39.99, "978-0735619678"}
};

/* COMPLIANT: Const data structure sizes */
static const size_t EMPLOYEE_COUNT = sizeof(COMPANY_EMPLOYEES) / sizeof(COMPANY_EMPLOYEES[0]);
static const size_t BOOK_COUNT = sizeof(LIBRARY_BOOKS) / sizeof(LIBRARY_BOOKS[0]);

/* COMPLIANT: Function processing const array of structures */
void display_employees(const Employee employees[], const size_t count,
                      const char *title) {
    if (!employees || !title) {
        return;
    }

    /* COMPLIANT: Local const for formatting */
    const char * const HEADER_FORMAT = "\\n=== %s ===\\n";
    const char * const EMPLOYEE_FORMAT = "ID: %d, Name: %-15s, Dept: %-12s, Salary: $%.2f\\n";

    printf(HEADER_FORMAT, title);

    for (size_t i = 0; i < count; i++) {
        printf(EMPLOYEE_FORMAT,
               employees[i].id,
               employees[i].name,
               employees[i].department,
               employees[i].salary);
    }
}

/* COMPLIANT: Function for searching const structures */
const Employee *find_employee_by_id(const Employee employees[], const size_t count,
                                   const int target_id) {
    if (!employees) {
        return NULL;
    }

    for (size_t i = 0; i < count; i++) {
        if (employees[i].id == target_id) {
            return &employees[i];
        }
    }

    return NULL;
}

/* COMPLIANT: Function calculating statistics from const data */
void calculate_salary_statistics(const Employee employees[], const size_t count) {
    if (!employees || count == 0) {
        return;
    }

    /* COMPLIANT: Local const for calculations */
    const char * const STATS_HEADER = "\\nSalary Statistics:";

    printf("%s\\n", STATS_HEADER);

    double total_salary = 0.0;
    double min_salary = employees[0].salary;
    double max_salary = employees[0].salary;
    const Employee *min_employee = &employees[0];
    const Employee *max_employee = &employees[0];

    for (size_t i = 0; i < count; i++) {
        const double current_salary = employees[i].salary;
        total_salary += current_salary;

        if (current_salary < min_salary) {
            min_salary = current_salary;
            min_employee = &employees[i];
        }

        if (current_salary > max_salary) {
            max_salary = current_salary;
            max_employee = &employees[i];
        }
    }

    const double average_salary = total_salary / (double)count;

    printf("  Total employees: %zu\\n", count);
    printf("  Average salary: $%.2f\\n", average_salary);
    printf("  Minimum salary: $%.2f (%s)\\n", min_salary, min_employee->name);
    printf("  Maximum salary: $%.2f (%s)\\n", max_salary, max_employee->name);
    printf("  Total payroll: $%.2f\\n", total_salary);
}

/* COMPLIANT: Function processing const book structures */
void display_books_by_year_range(const Book books[], const size_t count,
                                const int start_year, const int end_year) {
    if (!books) {
        return;
    }

    /* COMPLIANT: Local const for formatting */
    const char * const RANGE_HEADER = "\\nBooks published between %d and %d:\\n";
    const char * const BOOK_FORMAT = "  '%s' by %s (%d) - $%.2f [%s]\\n";

    printf(RANGE_HEADER, start_year, end_year);

    int found_count = 0;
    for (size_t i = 0; i < count; i++) {
        const int book_year = books[i].year;
        if (book_year >= start_year && book_year <= end_year) {
            printf(BOOK_FORMAT,
                   books[i].title,
                   books[i].author,
                   books[i].year,
                   books[i].price,
                   books[i].isbn);
            found_count++;
        }
    }

    if (found_count == 0) {
        printf("  No books found in the specified year range.\\n");
    } else {
        printf("  Total books found: %d\\n", found_count);
    }
}

/* COMPLIANT: Function for sorting const data (creates sorted indices) */
void display_books_sorted_by_price(const Book books[], const size_t count) {
    if (!books || count == 0) {
        return;
    }

    /* COMPLIANT: Create array of indices instead of modifying const data */
    size_t *indices = malloc(count * sizeof(size_t));
    if (!indices) {
        printf("Error: Cannot allocate memory for sorting\\n");
        return;
    }

    /* Initialize indices */
    for (size_t i = 0; i < count; i++) {
        indices[i] = i;
    }

    /* Sort indices based on book prices (bubble sort for simplicity) */
    for (size_t i = 0; i < count - 1; i++) {
        for (size_t j = 0; j < count - i - 1; j++) {
            if (books[indices[j]].price > books[indices[j + 1]].price) {
                /* Swap indices */
                const size_t temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }

    /* COMPLIANT: Display sorted results using const data */
    const char * const SORTED_HEADER = "\\nBooks sorted by price (ascending):\\n";
    printf("%s", SORTED_HEADER);

    for (size_t i = 0; i < count; i++) {
        const size_t book_index = indices[i];
        printf("  %zu. '%s' - $%.2f\\n",
               i + 1, books[book_index].title, books[book_index].price);
    }

    free(indices);
}

/* COMPLIANT: Function working with const linked list nodes */
void display_linked_list(const Node *head, const char *list_name) {
    if (!list_name) {
        return;
    }

    /* COMPLIANT: Local const for list processing */
    const char * const LIST_HEADER = "\\n%s:\\n";
    const char * const EMPTY_LIST = "  (empty list)\\n";
    const char * const NODE_FORMAT = "  Node: %d\\n";

    printf(LIST_HEADER, list_name);

    if (!head) {
        printf("%s", EMPTY_LIST);
        return;
    }

    const Node *current = head;
    int node_count = 0;

    while (current) {
        printf(NODE_FORMAT, current->data);
        current = current->next;
        node_count++;
    }

    printf("  Total nodes: %d\\n", node_count);
}

/* COMPLIANT: Function for creating const-friendly linked list */
Node *create_sample_list(void) {
    /* COMPLIANT: Const data for list creation */
    const int list_values[] = {10, 20, 30, 40, 50};
    const size_t value_count = sizeof(list_values) / sizeof(list_values[0]);

    Node *head = NULL;
    Node *tail = NULL;

    for (size_t i = 0; i < value_count; i++) {
        Node *new_node = malloc(sizeof(Node));
        if (!new_node) {
            printf("Error: Cannot allocate memory for node\\n");
            break;
        }

        /* Initialize const fields using compound literal */
        *new_node = (Node){.data = list_values[i], .next = NULL};

        if (!head) {
            head = new_node;
            tail = new_node;
        } else {
            /* We need to modify next pointer, but data remains const */
            Node *mutable_tail = (Node *)tail;  /* Cast away const for next pointer only */
            mutable_tail->next = new_node;
            tail = new_node;
        }
    }

    return head;
}

/* COMPLIANT: Function to free linked list */
void free_list(Node *head) {
    while (head) {
        Node *temp = (Node *)head;  /* Cast to allow freeing */
        head = (Node *)head->next;
        free(temp);
    }
}

/* COMPLIANT: Matrix operations with const data */
void demonstrate_const_matrix_operations(void) {
    /* COMPLIANT: Const matrix data */
    const int matrix1[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    const int matrix2[3][3] = {
        {9, 8, 7},
        {6, 5, 4},
        {3, 2, 1}
    };

    const char * const MATRIX_HEADER = "\\nMatrix Operations:\\n";
    printf("%s", MATRIX_HEADER);

    /* Display matrices using const data */
    printf("Matrix 1:\\n");
    for (int i = 0; i < 3; i++) {
        printf("  ");
        for (int j = 0; j < 3; j++) {
            printf("%d ", matrix1[i][j]);
        }
        printf("\\n");
    }

    printf("Matrix 2:\\n");
    for (int i = 0; i < 3; i++) {
        printf("  ");
        for (int j = 0; j < 3; j++) {
            printf("%d ", matrix2[i][j]);
        }
        printf("\\n");
    }

    /* Calculate sum using const matrices */
    printf("Matrix Sum:\\n");
    for (int i = 0; i < 3; i++) {
        printf("  ");
        for (int j = 0; j < 3; j++) {
            const int sum = matrix1[i][j] + matrix2[i][j];
            printf("%d ", sum);
        }
        printf("\\n");
    }
}

int main(void) {
    /* COMPLIANT: Main function const declarations */
    const char * const PROGRAM_TITLE = "Const Data Structures Demonstration";

    printf("=== %s ===\\n", PROGRAM_TITLE);

    /* Display employee data */
    display_employees(COMPANY_EMPLOYEES, EMPLOYEE_COUNT, "Company Employees");

    /* Calculate salary statistics */
    calculate_salary_statistics(COMPANY_EMPLOYEES, EMPLOYEE_COUNT);

    /* Search for specific employee */
    const int search_id = 1003;
    const Employee *found_employee = find_employee_by_id(COMPANY_EMPLOYEES, EMPLOYEE_COUNT, search_id);
    if (found_employee) {
        printf("\\nEmployee found: %s (ID: %d)\\n", found_employee->name, found_employee->id);
    }

    /* Display books in date range */
    display_books_by_year_range(LIBRARY_BOOKS, BOOK_COUNT, 1990, 2000);

    /* Display books sorted by price */
    display_books_sorted_by_price(LIBRARY_BOOKS, BOOK_COUNT);

    /* Demonstrate linked list operations */
    Node *sample_list = create_sample_list();
    display_linked_list(sample_list, "Sample Linked List");
    free_list(sample_list);

    /* Demonstrate matrix operations */
    demonstrate_const_matrix_operations();

    printf("\\n=== Data structures demonstration completed ===\\n");

    return 0;
}