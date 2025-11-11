/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Accesses struct members after the struct has been freed
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char *name;
    int id;
} person_t;

int main() {
    person_t *person = malloc(sizeof(person_t));
    if (person == NULL) {
        return -1;
    }

    person->name = malloc(20);
    strcpy(person->name, "John");
    person->id = 123;

    free(person->name);
    free(person);

    // BUG: Access freed struct
    printf("ID: %d\n", person->id);

    return 0;
}