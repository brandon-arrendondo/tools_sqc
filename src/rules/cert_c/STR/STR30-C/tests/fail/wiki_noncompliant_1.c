/*
 * Rule: STR30-C
 * Source: wiki
 * Status: FAIL - Should trigger STR30-C violation
 */

char *str  = "string literal";
str[0] = 'S';