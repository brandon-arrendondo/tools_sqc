/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: struct_member_char_types.c
 *
 * This case demonstrates a violation of STR00-C by inconsistently
 * using different character types for structure members, leading to
 * type compatibility issues and assignment warnings.
 */

#include <stdio.h>
#include <string.h>

/* VIOLATION: Structure with mixed character pointer types */
struct mixed_char_struct {
    char *plain_string;
    signed char *signed_string;
    unsigned char *unsigned_string;
    int id;
};

/* VIOLATION: Structure using signed char for string data */
struct signed_char_data {
    signed char name[50];
    signed char description[100];
    int age;
};

/* VIOLATION: Structure using unsigned char for string data */
struct unsigned_char_data {
    unsigned char title[30];
    unsigned char content[200];
    float value;
};

/* VIOLATION: Structure with character arrays of different types */
struct inconsistent_chars {
    char field1[20];
    signed char field2[20];
    unsigned char field3[20];
};

int main(void) {
    /* VIOLATION: Initializing structures with type mismatches */
    struct mixed_char_struct mixed = {
        .plain_string = "Plain string",
        .signed_string = "Signed string",      /* Warning */
        .unsigned_string = "Unsigned string",  /* Warning */
        .id = 1
    };

    printf("Mixed character structure:\n");
    printf("Plain: %s\n", mixed.plain_string);
    printf("Signed: %s\n", mixed.signed_string);      /* Warning */
    printf("Unsigned: %s\n", mixed.unsigned_string);  /* Warning */

    /* VIOLATION: Assignment between different character types */
    mixed.plain_string = mixed.signed_string;         /* Warning */
    mixed.signed_string = mixed.unsigned_string;      /* Warning */
    mixed.unsigned_string = mixed.plain_string;       /* Warning */

    /* VIOLATION: String operations on structure members */
    struct signed_char_data signed_data;
    strcpy(signed_data.name, "John Doe");              /* Warning */
    strcpy(signed_data.description, "A test person");  /* Warning */
    signed_data.age = 30;

    printf("\nSigned character data:\n");
    printf("Name: %s\n", signed_data.name);            /* Warning */
    printf("Description: %s\n", signed_data.description); /* Warning */
    printf("Age: %d\n", signed_data.age);

    /* VIOLATION: Cross-type copying between structure members */
    struct unsigned_char_data unsigned_data;
    strcpy(unsigned_data.title, signed_data.name);     /* Warning */
    strcpy(unsigned_data.content, signed_data.description); /* Warning */
    unsigned_data.value = 3.14f;

    printf("\nUnsigned character data:\n");
    printf("Title: %s\n", unsigned_data.title);        /* Warning */
    printf("Content: %s\n", unsigned_data.content);    /* Warning */
    printf("Value: %.2f\n", unsigned_data.value);

    /* VIOLATION: Structure member comparison */
    if (strcmp(signed_data.name, unsigned_data.title) == 0) {  /* Warning */
        printf("Names match\n");
    }

    /* VIOLATION: Inconsistent character type usage */
    struct inconsistent_chars inconsistent;
    strcpy(inconsistent.field1, "Field 1");            /* OK */
    strcpy(inconsistent.field2, "Field 2");            /* Warning */
    strcpy(inconsistent.field3, "Field 3");            /* Warning */

    printf("\nInconsistent structure:\n");
    printf("Field 1: %s\n", inconsistent.field1);
    printf("Field 2: %s\n", inconsistent.field2);      /* Warning */
    printf("Field 3: %s\n", inconsistent.field3);      /* Warning */

    /* VIOLATION: Cross-field assignment */
    strcpy(inconsistent.field1, inconsistent.field2);  /* Warning */
    strcpy(inconsistent.field2, inconsistent.field3);  /* Warning */
    strcpy(inconsistent.field3, inconsistent.field1);  /* Warning */

    /* VIOLATION: Structure array with character type issues */
    struct mixed_char_struct array[3] = {
        {"First", "Signed First", "Unsigned First", 1},    /* Warnings */
        {"Second", "Signed Second", "Unsigned Second", 2}, /* Warnings */
        {"Third", "Signed Third", "Unsigned Third", 3}     /* Warnings */
    };

    printf("\nStructure array:\n");
    for (int i = 0; i < 3; i++) {
        printf("Entry %d:\n", i);
        printf("  Plain: %s\n", array[i].plain_string);
        printf("  Signed: %s\n", array[i].signed_string);      /* Warning */
        printf("  Unsigned: %s\n", array[i].unsigned_string);  /* Warning */
    }

    /* VIOLATION: Structure member address operations */
    char *plain_ptr = inconsistent.field1;           /* OK */
    signed char *signed_ptr = inconsistent.field2;   /* OK */
    unsigned char *unsigned_ptr = inconsistent.field3; /* OK */

    /* Cross-type assignments */
    plain_ptr = inconsistent.field2;      /* Warning */
    signed_ptr = inconsistent.field3;     /* Warning */
    unsigned_ptr = inconsistent.field1;   /* Warning */

    /* VIOLATION: Function calls with structure member types */
    size_t len1 = strlen(signed_data.name);         /* Warning */
    size_t len2 = strlen(unsigned_data.title);      /* Warning */

    printf("String lengths: %zu, %zu\n", len1, len2);

    /* VIOLATION: Pointer arithmetic on structure members */
    signed_char *name_end = signed_data.name + strlen((char*)signed_data.name);
    unsigned_char *title_end = unsigned_data.title + strlen((char*)unsigned_data.title);

    printf("Last characters: %c, %c\n", *(name_end - 1), *(title_end - 1));

    /* VIOLATION: Memory operations between structure members */
    memcpy(inconsistent.field1, inconsistent.field2, 10);  /* Warning */
    memcpy(inconsistent.field2, inconsistent.field3, 10);  /* Warning */

    /* VIOLATION: Structure initialization with string literals */
    struct mixed_char_struct dynamic;
    dynamic.plain_string = malloc(50);
    dynamic.signed_string = malloc(50);    /* Should be signed char* */
    dynamic.unsigned_string = malloc(50);  /* Should be unsigned char* */

    if (dynamic.plain_string && dynamic.signed_string && dynamic.unsigned_string) {
        strcpy(dynamic.plain_string, "Dynamic plain");
        strcpy(dynamic.signed_string, "Dynamic signed");    /* Warning */
        strcpy(dynamic.unsigned_string, "Dynamic unsigned"); /* Warning */

        printf("\nDynamic structure:\n");
        printf("Plain: %s\n", dynamic.plain_string);
        printf("Signed: %s\n", dynamic.signed_string);      /* Warning */
        printf("Unsigned: %s\n", dynamic.unsigned_string);  /* Warning */

        free(dynamic.plain_string);
        free(dynamic.signed_string);
        free(dynamic.unsigned_string);
    }

    return 0;
}