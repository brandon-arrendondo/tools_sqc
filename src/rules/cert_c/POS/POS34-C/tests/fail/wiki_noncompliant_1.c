/*
 * Rule: POS34-C
 * Source: wiki
 * Status: FAIL - Should trigger POS34-C violation
 */

int func(const char *var) {
  char env[1024];
  int retval = snprintf(env, sizeof(env),"TEST=%s", var);
  if (retval < 0 || (size_t)retval >= sizeof(env)) {
    /* Handle error */
  }

  return putenv(env);
}