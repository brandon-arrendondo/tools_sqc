/*
 * Rule: DCL30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL30-C violation
 */

char local[10];
 
void squirrel_away(char **ptr_param) {
  /* Initialize array */
  *ptr_param = local;
}

void rodent(void) {
  char *ptr;
  squirrel_away(&ptr);
  /* ptr is valid in this scope */
}