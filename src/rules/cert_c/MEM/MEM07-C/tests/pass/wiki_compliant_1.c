/*
 * Rule: MEM07-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM07-C violation
 */

long *buffer;
size_t num_elements;

if (num_elements > SIZE_MAX/sizeof(long)) {
  /* Handle error condition */
}
buffer = (long *)calloc(num_elements, sizeof(long));
if (buffer == NULL) {
  /* Handle error condition */
}