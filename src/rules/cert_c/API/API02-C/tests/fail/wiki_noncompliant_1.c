/*
 * Rule: API02-C
 * Source: wiki
 * Status: FAIL - Should trigger API02-C violation
 */

char *strncpy(char * restrict s1, const char * restrict s2, size_t n);
char *strncat(char * restrict s1, const char * restrict s2, size_t n);