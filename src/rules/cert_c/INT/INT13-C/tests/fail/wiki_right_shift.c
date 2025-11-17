/*
 * Rule: INT13-C
 * Source: wiki
 * Status: FAIL - Should trigger INT13-C violation
 */

int rc = 0;
int stringify = 0x80000000;
char buf[sizeof("256")];
rc = snprintf(buf, sizeof(buf), "%u", stringify >> 24);
if (rc == -1 || rc >= sizeof(buf)) {
  /* Handle error */
}