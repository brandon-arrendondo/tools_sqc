/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Struct string field too small for assigned value
 */

#include <stdio.h>
#include <string.h>

struct person {
    char name[10];
    int age;
};

int main() {
    struct person p;
    char full_name[] = "Alexander Hamilton";

    strcpy(p.name, full_name);  // 18 chars don't fit in 10-byte field
    p.age = 30;

    printf("Person: %s, %d\n", p.name, p.age);

    return 0;
}