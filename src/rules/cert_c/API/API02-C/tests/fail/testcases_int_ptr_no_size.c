/*
 * Rule: API02-C
 * Source: testcases
 * Status: FAIL - Should trigger API02-C violation
 *
 * Function declaration with writable pointer parameter missing size
 */

/* VIOLATION: writable char pointer without corresponding size parameter */
void fill_array(char * restrict dest, const char * restrict src, int count);
