/*
 * Rule: EXP09-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP09-C violation
 */

int f(void) {
  size_t i;
  int **matrix = (int **)calloc(100, sizeof(*matrix));
  if (matrix == NULL) {
    return -1; /* Indicate calloc() failure */
  }

  for (i = 0; i < 100; i++) {
    matrix[i] = (int *)calloc(i, sizeof(**matrix));
    if (matrix[i] == NULL) {
      return -1; /* Indicate calloc() failure */
    }
  }

  return 0;
}