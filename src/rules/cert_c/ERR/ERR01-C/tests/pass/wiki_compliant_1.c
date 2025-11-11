/*
 * Rule: ERR01-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ERR01-C violation
 */

printf("This\n");
printf("is\n");
printf("a\n");
printf("test.\n");
if (ferror(stdout)) {
  fprintf(stderr, "printf failed\n");
}