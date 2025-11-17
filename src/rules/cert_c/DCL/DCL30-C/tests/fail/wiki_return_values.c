/*
 * Rule: DCL30-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL30-C violation
 */

char *init_array(void) {
  char array[10];
  /* Initialize array */
  return array;
}