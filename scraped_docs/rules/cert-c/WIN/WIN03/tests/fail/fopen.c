#include <stdio.h>
#include <stdlib.h>
 
int main(void) {
  FILE *fp = fopen("SomeFile.txt", "rw");
  if (!fp) {
    return -1;
  }
  
  system("SomeProcess.exe");
 
  fclose(fp);
  return 0;
}