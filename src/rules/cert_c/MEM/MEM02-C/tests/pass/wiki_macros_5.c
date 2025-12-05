/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM02-C violation
 * Description: Comprehensive allocation macros with proper casts
 */

#include <stdlib.h>

/* Allocates a single object using malloc() */
#define MALLOC(type) ((type *)malloc(sizeof(type)))

/* Allocates an array of objects using malloc() */
#define MALLOC_ARRAY(number, type) \
    ((type *)malloc((number) * sizeof(type)))

/*
 * Allocates a single object with a flexible
 * array member using malloc().
 */
#define MALLOC_FLEX(stype, number, etype) \
    ((stype *)malloc(sizeof(stype) \
    + (number) * sizeof(etype)))

/* Allocates an array of objects using calloc() */
#define CALLOC(number, type) \
    ((type *)calloc(number, sizeof(type)))

/* Reallocates an array of objects using realloc() */
#define REALLOC_ARRAY(pointer, number, type) \
    ((type *)realloc(pointer, (number) * sizeof(type)))

/*
 * Reallocates a single object with a flexible
 * array member using realloc().
 */
#define REALLOC_FLEX(pointer, stype, number, etype) \
    ((stype *)realloc(pointer, sizeof(stype) \
    + (number) * sizeof(etype)))

typedef struct widget widget;
struct widget {
    int i;
    double d;
};

void testcase_compliant_all_macros(void) {
    widget *p = MALLOC(widget);
    widget *arr = MALLOC_ARRAY(10, widget);
    widget *arr2 = CALLOC(10, widget);
    (void)p;
    (void)arr;
    (void)arr2;
}
