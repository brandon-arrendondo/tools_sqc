/*
 * Rule: ENV02-C
 * Source: wiki
 * Status: FAIL - Should trigger ENV02-C violation
 */

if (putenv("TEST_ENV=foo") != 0) {
  /* Handle error */
}
if (putenv("Test_ENV=bar") != 0) {
  /* Handle error */
}

const char *temp = getenv("TEST_ENV");

if (temp == NULL) {
  /* Handle error */
}

printf("%s\n", temp);