/*
 * Rule: PRE11-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE11-C violation
 */

#define FOR_LOOP(n)  for(i=0; i<(n); i++)

int i;
FOR_LOOP(3)
{
  puts("Inside for loop\n");
}