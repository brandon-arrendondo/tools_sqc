/*
 * Rule: DCL13-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL13-C violation
 */

char *strcat_nc(char *s1, char *s2);

char *c_str1 = "c_str1";
const char *c_str2 = "c_str2";
char c_str3[9] = "c_str3";
const char c_str4[9] = "c_str4";

strcat_nc(c_str3, c_str2);  /* Compiler warns that c_str2 is const */
strcat_nc(c_str1, c_str3);  /* Attempts to overwrite string literal! */
strcat_nc(c_str4, c_str3);  /* Compiler warns that c_str4 is const */