/*
 * Rule: EXP09-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP09-C violation
 */

int f(void) { /* Assuming 32-bit pointer, 32-bit integer */
  size_t i;
  int **matrix = (int **)calloc(100, 4);
  if (matrix == NULL) {
    return -1; /* Indicate calloc() failure */
  }

  for (i = 0; i < 100; i++) {
    matrix[i] = (int *)calloc(i, 4);
    if (matrix[i] == NULL) {
      return -1; /* Indicate calloc() failure */
    }
  }
 return 0;
}