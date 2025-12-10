/*
 * Rule: STR31-C
 * Source: wiki
 * Status: FAIL - Should trigger STR31-C violation
 *
 * This demonstrates copying from argv[1] directly to a fixed buffer.
 */

#include <string.h>

int main(int argc, char *argv[]) {
  char program_name[128];
  /* VIOLATION: argv[1] can be arbitrarily long */
  if (argc > 1) {
    strcpy(program_name, argv[1]);
  }
  return 0;
}