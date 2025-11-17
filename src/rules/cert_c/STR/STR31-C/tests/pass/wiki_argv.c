/*
 * Rule: STR31-C
 * Source: wiki
 * Status: PASS - Should NOT trigger STR31-C violation
 */

int main(int argc, char *argv[]) {
  /* Ensure argv[0] is not null */
  const char * const prog_name = (argc && argv[0]) ? argv[0] : "";
  /* ... */
  return 0;
}