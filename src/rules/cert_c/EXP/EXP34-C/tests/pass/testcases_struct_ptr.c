/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: PASS
 * Reason: Structure pointer is validated before accessing members
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char name[50];
} Person;

void print_person(Person *p) {
    if (p == NULL) {
        printf("Invalid person object\n");
        return;
    }

    printf("ID: %d, Name: %s\n", p->id, p->name);
}

int main() {
    Person *person = malloc(sizeof(Person));

    if (person != NULL) {
        person->id = 1;
        strcpy(person->name, "John Doe");
        print_person(person);
        free(person);
    }

    print_person(NULL);  // Safe - function handles NULL
    return 0;
}