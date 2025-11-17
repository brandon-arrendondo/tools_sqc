/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Dynamically allocated struct is never freed
 */

#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[50];
    double value;
} Record;

void create_record() {
    Record *rec = malloc(sizeof(Record));
    if (rec == NULL) {
        return;
    }

    rec->id = 100;
    strcpy(rec->name, "Test Record");
    rec->value = 42.5;

    printf("Created record: %s\n", rec->name);

    // struct is never freed - MEMORY LEAK
}