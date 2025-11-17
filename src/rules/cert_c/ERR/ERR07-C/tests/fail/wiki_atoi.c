/*
 * Rule: ERR07-C
 * Source: wiki
 * Status: FAIL - Should trigger ERR07-C violation
 */

int si;

if (argc > 1) {
  si = atoi(argv[1]);
}