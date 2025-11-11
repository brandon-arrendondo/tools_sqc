/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int age;
    char *email;
} person_t;

person_t* create_person(const char *name, int age, const char *email) {
    person_t *person = malloc(sizeof(person_t));
    if (!person) {
        return NULL;
    }

    // Initialize to safe state
    person->name = NULL;
    person->age = age;
    person->email = NULL;

    // Allocate and copy name
    if (name) {
        person->name = malloc(strlen(name) + 1);
        if (person->name) {
            strcpy(person->name, name);
        } else {
            free(person);  // Cleanup on partial failure
            return NULL;
        }
    }

    // Allocate and copy email
    if (email) {
        person->email = malloc(strlen(email) + 1);
        if (person->email) {
            strcpy(person->email, email);
        } else {
            // Cleanup on partial failure
            free(person->name);
            free(person);
            return NULL;
        }
    }

    return person;
}

void destroy_person(person_t **person) {
    if (person && *person) {
        // Free each allocated field exactly once
        if ((*person)->name) {
            free((*person)->name);
            (*person)->name = NULL;
        }

        if ((*person)->email) {
            free((*person)->email);
            (*person)->email = NULL;
        }

        // Free the struct itself exactly once
        free(*person);
        *person = NULL;
    }
}

int main() {
    // Create person with all fields
    person_t *person1 = create_person("John Doe", 30, "john@example.com");
    if (person1) {
        printf("Created person: %s, age %d, email %s\n",
               person1->name, person1->age, person1->email);
        destroy_person(&person1);  // Free exactly once
        printf("Person1 destroyed\n");
    }

    // Create person with minimal fields
    person_t *person2 = create_person("Jane", 25, NULL);
    if (person2) {
        printf("Created person: %s, age %d\n", person2->name, person2->age);
        destroy_person(&person2);  // Free exactly once
        printf("Person2 destroyed\n");
    }

    return 0;
}