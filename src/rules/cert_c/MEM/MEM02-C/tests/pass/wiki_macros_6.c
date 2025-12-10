/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM02-C violation
 * Description: Various types with macro allocation
 */

#include <stdlib.h>
#include <stddef.h>

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

enum month { Jan, Feb /* ... */ };
typedef enum month month;

typedef struct date date;
struct date {
  unsigned char dd;
  month mm;
  unsigned yy;
};

typedef struct string string;
struct string {
  size_t length;
  char text[];
};

void testcase_compliant_various_types(void) {
    date *d, *week, *fortnight;
    string *name;

    d = MALLOC(date);
    week = MALLOC_ARRAY(7, date);
    name = MALLOC_FLEX(string, 16, char);
    fortnight = CALLOC(14, date);

    (void)d;
    (void)week;
    (void)name;
    (void)fortnight;
}
