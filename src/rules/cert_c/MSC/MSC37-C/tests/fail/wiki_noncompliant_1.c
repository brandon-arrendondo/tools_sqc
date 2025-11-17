/*
 * Rule: MSC37-C
 * Source: wiki
 * Status: FAIL - Should trigger MSC37-C violation
 */

#include <string.h>
#include <stdio.h>
 
int checkpass(const char *password) {
  if (strcmp(password, "pass") == 0) {
    return 1;
  }
}

void func(const char *userinput) {
  if (checkpass(userinput)) {
    printf("Success\n");
  }
}