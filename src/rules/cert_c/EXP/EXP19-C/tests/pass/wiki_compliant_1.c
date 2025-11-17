/*
 * Rule: EXP19-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP19-C violation
 */

int login;

if (invalid_login()) {
  login = 0;
} else {
  login = 1;
}