/*
 * Rule: EXP12-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP12-C violation
 */

void func(char* name) {
  char* s = NULL;
  asprintf(&s,"Hello, %s!\n", name);
  (void) puts(s);
  free(s);
}