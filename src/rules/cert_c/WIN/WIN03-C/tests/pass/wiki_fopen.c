/*
 * Rule: WIN03-C
 * Source: wiki
 * Status: PASS - Should NOT trigger WIN03-C violation
 */

#include <stdio.h>
#include <stdlib.h>
 
int main(void) {
  FILE *fp = fopen("SomeFile.txt", "rwN");
  if (!fp) {
    return -1;
  }
  
  system("SomeProcess.exe");
 
  fclose(fp);
  return 0;
}