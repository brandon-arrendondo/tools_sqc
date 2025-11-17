/*
 * Rule: STR04-C
 * Source: wiki
 * Status: FAIL - Should trigger STR04-C violation
 */

size_t len;
char cstr[] = "char string";
signed char scstr[] = "signed char string";
unsigned char ucstr[] = "unsigned char string";

len = strlen(cstr);
len = strlen(scstr);  /* Warns when char is unsigned */
len = strlen(ucstr);  /* Warns when char is signed */