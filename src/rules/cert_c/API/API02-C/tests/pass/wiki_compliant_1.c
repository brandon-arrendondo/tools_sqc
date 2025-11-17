/*
 * Rule: API02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger API02-C violation
 */

char *improved_strncpy(char * restrict s1, size_t s1count, const char * restrict s2, size_t s2count, size_t n);
char *improved_strncat(char * restrict s1, size_t s1count, const char * restrict s2, size_t s2count, size_t n);