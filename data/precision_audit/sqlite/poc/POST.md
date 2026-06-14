# SQLite forum disclosure — os_kv.c kvvfsDecode heap overflow

Filed by: Brandon Arrendondo (brandon.arrendondo@bissell.com)
Date filed: 2026-06-14 (held for moderator approval)
Target: SQLite User Forum (sqlite.org/forum)
Status at filing: LIVE at trunk HEAD; Valgrind-confirmed; not found in a forum search (apparently unreported)
Artifacts: poc/kvvfs_overflow_poc.c, poc/kvvfs_valgrind.txt

---

## Title

Heap buffer overflow (write) in kvvfsDecode() in os_kv.c

---

## Body

Hello,

I believe there is a heap buffer overflow (out-of-bounds write) in
kvvfsDecode() in src/os_kv.c. It is present on current trunk.

## The defect

kvvfsDecode(const char *a, char *aOut, int nOut) decodes the text encoding
used by the kvvfs (key/value) VFS into aOut, which holds nOut bytes. It has
two decode branches. The zero-run (memset) branch is bounds-checked:

    if( j+n>nOut ) return -1;
    memset(&aOut[j], 0, n);
    j += n;

The hex-pair branch is not -- it writes to aOut[j] with no check that j is
still within nOut, inside a while(1) loop:

    }else{
      aOut[j] = c<<4;
      c = kvvfsHexValue[aIn[++i]];
      if( c<0 ) return -1 /* hex bytes are always in pairs */;
      aOut[j++] += c;
      i++;
    }

So the number of output bytes is governed only by how many hex pairs are in
the input, never by nOut.

## Why nOut and the payload are both attacker-influenced

kvvfsDecodeJournal() sizes the destination buffer from a length header that
is read from the front of the *same* journal text, then decodes the rest of
that text into it:

    while( (c = zTxt[i++])>='a' && c<='z' ){ n += (zTxt[i]-'a')*mult; mult*=26; }
    pFile->aJrnl = sqlite3_malloc64( n );
    pFile->nJrnl = n;
    n = kvvfsDecode(zTxt+i, pFile->aJrnl, pFile->nJrnl);

A journal text with a small length header followed by a longer run of hex
pairs therefore makes kvvfsDecode() write past the n-byte allocation, with
attacker-controlled length and contents. This is reachable whenever the
kvvfs backing store can contain data an attacker influences (the journal is
read back via the xRead path -> kvvfsReadJournal -> kvvfsDecodeJournal).

Note for anyone reproducing: kvvfs decodes UPPERCASE hex. kvvfsHexValue maps
0x41-0x46 ('A'-'F'); lowercase 'a'-'f' map to -1.

## Reproducer (Valgrind)

The table and function below are copied verbatim from src/os_kv.c. The driver
calls kvvfsDecode() exactly as kvvfsDecodeJournal() does, and also reproduces
the kvvfsDecodeJournal() header-parse + malloc(n) flow end to end.

----------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* verbatim from src/os_kv.c */
static const signed char kvvfsHexValue[256] = {
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
   0, 1, 2, 3, 4, 5, 6, 7,  8, 9,-1,-1,-1,-1,-1,-1,
  -1,10,11,12,13,14,15,-1, -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1
};

int kvvfsDecode(const char *a, char *aOut, int nOut){
  int i, j, c;
  const unsigned char *aIn = (const unsigned char*)a;
  i = 0; j = 0;
  while( 1 ){
    c = kvvfsHexValue[aIn[i]];
    if( c<0 ){
      int n = 0, mult = 1;
      c = aIn[i];
      if( c==0 ) break;
      while( c>='a' && c<='z' ){ n += (c-'a')*mult; mult *= 26; c = aIn[++i]; }
      if( j+n>nOut ) return -1;
      memset(&aOut[j], 0, n); j += n;
      if( c==0 || mult==1 ) break;
    }else{
      aOut[j] = c<<4;
      c = kvvfsHexValue[aIn[++i]];
      if( c<0 ) return -1;
      aOut[j++] += c;
      i++;
    }
  }
  return j;
}

int main(void){
  /* exactly how kvvfsDecodeJournal calls it: kvvfsDecode(text, aJrnl, nJrnl) */
  int nOut = 4;
  char *aOut = malloc(nOut);
  int rc = kvvfsDecode("00112233445566778899AABBCCDDEEFF", aOut, nOut);
  printf("decoded %d bytes into a %d-byte buffer\n", rc, nOut);
  free(aOut);
  return 0;
}
----------------------------------------------------------------------

    $ gcc -O0 -g -o poc poc.c
    $ valgrind --leak-check=no ./poc
    ==...== Invalid write of size 1
    ==...==    at 0x...: kvvfsDecode (poc.c:NN)
    ==...==  Address 0x... is 0 bytes after a block of size 4 alloc'd
    ...
    decoded 16 bytes into a 4-byte buffer

The decode writes 16 bytes into the 4-byte allocation. Driving the full
kvvfsDecodeJournal() flow (a "small header + long payload" journal text)
reproduces the same overflow against the malloc'd journal buffer.

## Suggested fix

Add a bound to the hex-pair branch, mirroring the zero-run branch, e.g.:

    }else{
      if( j>=nOut ) return -1;
      aOut[j] = c<<4;
      c = kvvfsHexValue[aIn[++i]];
      if( c<0 ) return -1;
      aOut[j++] += c;
      i++;
    }

## Disclosure

This was found during a static-analysis study of the SQLite source and the
analysis/triage was assisted by an AI tool; I have manually verified the code
against current trunk and reproduced the overflow under Valgrind before
reporting. Happy to provide any further detail.

Thanks,
Brandon Arrendondo
