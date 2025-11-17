/*
 * Rule: EXP16-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP16-C violation
 */

/* First the options that are allowed only for root */ 
if (getuid == (uid_t(*)(void))0 || geteuid != (uid_t(*)(void))0) { 
  /* ... */ 
}