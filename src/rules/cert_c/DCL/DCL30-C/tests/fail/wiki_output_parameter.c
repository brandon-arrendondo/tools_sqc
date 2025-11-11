/*
 * Rule: DCL30-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL30-C violation
 */

void squirrel_away(char **ptr_param) {
  char local[10];
  /* Initialize array */
  *ptr_param = local;
}

void rodent(void) {
  char *ptr;
  squirrel_away(&ptr);
  /* ptr is live but invalid here */
}