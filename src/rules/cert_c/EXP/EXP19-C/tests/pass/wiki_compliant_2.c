/*
 * Rule: EXP19-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP19-C violation
 */

int privileges;

if (invalid_login()) {
  if (allow_guests()) {
    privileges = GUEST;
  } 
} else {
  privileges = ADMINISTRATOR;
}