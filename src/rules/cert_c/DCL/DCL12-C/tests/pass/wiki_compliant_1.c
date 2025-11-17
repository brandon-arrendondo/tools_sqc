/*
 * Rule: DCL12-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL12-C violation
 */

struct string_mx;
typedef struct string_mx string_mx;

/* Function declarations */
extern errno_t strcpy_m(string_mx *s1, const string_mx *s2);
extern errno_t strcat_m(string_mx *s1, const string_mx *s2);
/* ... */