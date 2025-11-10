#include <stdbool.h>
#include <stdio.h>
 
bool get_data(char *buffer, int size) {
  if (fgets(buffer, size, stdin)) {
    return true;
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