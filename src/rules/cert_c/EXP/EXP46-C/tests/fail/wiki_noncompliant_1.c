/*
 * Rule: EXP46-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP46-C violation
 */

if (getuid() == 0 & getgid() == 0) { 
  /* ... */ 
}