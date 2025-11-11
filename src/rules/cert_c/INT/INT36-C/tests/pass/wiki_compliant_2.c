/*
 * Rule: INT36-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT36-C violation
 */

struct ptrflag {
  char *pointer;
  unsigned int flag : 9;
} ptrflag;
 
void func(unsigned int flag) {
  char *ptr;
  /* ... */
  ptrflag.pointer = ptr;
  ptrflag.flag = flag;
}