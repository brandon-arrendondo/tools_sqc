/*
 * Rule: EXP19-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP19-C violation
 */

int login;

if (invalid_login())
  login = 0;
else
  login = 1;