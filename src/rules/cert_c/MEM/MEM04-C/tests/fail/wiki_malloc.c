/*
 * Rule: MEM04-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM04-C violation
 */

size_t size;

/* Initialize size, possibly by user-controlled input */

int *list = (int *)malloc(size);
if (list == NULL) {
  /* Handle allocation error */
}
else {
/* Continue processing list */
}