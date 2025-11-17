/*
 * Rule: EXP19-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP19-C violation
 */

int privileges;

if (invalid_login())
  if (allow_guests())
    privileges = GUEST;
  else
    privileges = ADMINISTRATOR;