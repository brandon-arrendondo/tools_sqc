#include <stdio.h>
 
void func(void) {
  FILE *fp = tmpfile();
  if (fp == NULL) {
    /* Handle error */
  }
}