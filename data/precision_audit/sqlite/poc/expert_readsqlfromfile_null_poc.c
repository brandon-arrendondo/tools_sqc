/*
** Demonstrates that fread() does not detect/report a NULL destination
** buffer -- it crashes inside libc rather than returning nRead!=1, which
** is what readSqlFromFile() in ext/expert/expert.c relies on to catch a
** failed sqlite3_malloc64() and bail out cleanly.
**
** This driver reproduces the exact call shape from readSqlFromFile():
**   pBuf = sqlite3_malloc64( nIn+1 );      -- forced to NULL here, as if OOM
**   nRead = fread(pBuf, nIn, 1, in);
** against a real, nonzero-length file, so nIn>0 exactly as it would be for
** any real input to `sqlite3_expert -file FILE`.
*/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
  const char *zFile = (argc>1) ? argv[1] : __FILE__;
  FILE *in = fopen(zFile, "rb");
  long nIn;
  size_t nRead;
  char *pBuf;

  if( in==0 ){ perror("fopen"); return 1; }
  fseek(in, 0, SEEK_END);
  nIn = ftell(in);
  rewind(in);
  printf("nIn = %ld\n", nIn);

  pBuf = NULL;  /* simulates sqlite3_malloc64(nIn+1) returning NULL on OOM */
  nRead = fread(pBuf, nIn, 1, in);

  /* readSqlFromFile() expects to reach here and take the nRead!=1 branch;
  ** it never does -- fread() crashes first. */
  printf("nRead = %zu (unreachable if fread() crashed)\n", nRead);
  fclose(in);
  return 0;
}
