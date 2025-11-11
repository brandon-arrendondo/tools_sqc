/*
 * Rule: MEM01-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM01-C violation
 */

char *message;
int message_type;

/* Initialize message and message_type */

if (message_type == value_1) {
  /* Process message type 1 */
  free(message);
  message = NULL;
}
/* ... */
if (message_type == value_2) {
  /* Process message type 2 */
  free(message);
  message = NULL;
}