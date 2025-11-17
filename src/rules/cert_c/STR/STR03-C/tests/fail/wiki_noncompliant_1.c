/*
 * Rule: STR03-C
 * Source: wiki
 * Status: FAIL - Should trigger STR03-C violation
 */

char *string_data;
char a[16];
/* ... */
strncpy(a, string_data, sizeof(a));