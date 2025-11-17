/*
 * Rule: STR04-C
 * Source: wiki
 * Status: PASS - Should NOT trigger STR04-C violation
 */

size_t len;
char cstr[] = "char string";

len = strlen(cstr);