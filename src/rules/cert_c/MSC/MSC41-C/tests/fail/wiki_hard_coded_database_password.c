/*
 * Rule: MSC41-C
 * Source: wiki
 * Status: FAIL - Should trigger MSC41-C violation
 */

/* Returns nonzero if authenticated */
int authenticate(const char* code);

int main() {
  if (!authenticate("correct code")) {
    printf("Authentication error\n");
    return -1;
  }

  printf("Authentication successful\n");
  // ...Work with system...
  return 0;
}