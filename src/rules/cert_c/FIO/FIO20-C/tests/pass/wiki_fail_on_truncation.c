/*
 * Rule: FIO20-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO20-C violation
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
 
bool get_data(char *buffer, int size) {
  if (fgets(buffer, size, stdin)) {
    size_t len = strlen(buffer);
    return feof(stdin) || (len != 0 && buffer[len-1] == '\n');
  }
  return false;
}
 
void func(void) {
  char buf[8];
  if (get_data(buf, sizeof(buf))) {
    printf("The user input %s\n", buf);
  } else {
    printf("Error getting data from the user\n");
  }
}