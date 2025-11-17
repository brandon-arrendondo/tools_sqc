/*
 * Rule: MEM07-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM07-C violation
 */

size_t num_elements;

long *buffer = (long *)calloc(num_elements, sizeof(long));
if (buffer == NULL) {
  /* Handle error condition */
}
/* ... */
free(buffer);
buffer = NULL;