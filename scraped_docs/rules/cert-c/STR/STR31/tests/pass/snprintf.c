#include <stdio.h>
 
void func(const char *name) {
  char filename[128];
  int result = snprintf(filename, sizeof(filename), "%s.txt", name);
  if (result != strlen(filename) {
    /* truncation occurred */
  }
}