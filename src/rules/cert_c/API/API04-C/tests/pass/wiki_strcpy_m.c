/*
 * Rule: API04-C
 * Source: wiki
 * Status: PASS - Should NOT trigger API04-C violation
 */

errno_t retValue; 
string_m dest, source;  

/* ... */

if (retValue = strcpy_m(dest, source)) { 
  fprintf(stderr, "Error %d from strcpy_m.\n", retValue);
}