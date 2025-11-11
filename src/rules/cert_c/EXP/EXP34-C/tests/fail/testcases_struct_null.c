/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Accessing structure member through NULL pointer
 */

#include <stdio.h>

typedef struct {
    int id;
    char name[50];
} Person;

int main() {
    Person *person = NULL;

    // Accessing structure member through NULL pointer
    person->id = 1;
    strcpy(person->name, "John");

    printf("ID: %d\n", person->id);

    return 0;
}