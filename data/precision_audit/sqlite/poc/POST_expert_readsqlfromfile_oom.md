# SQLite forum disclosure — NULL pointer write in readSqlFromFile() (ext/expert/expert.c)

Filed by: Brandon Arrendondo (brandon.arrendondo@bissell.com)
Date drafted: 2026-08-25
Target: SQLite User Forum (sqlite.org/forum)
Status at drafting: LIVE at trunk HEAD (b1df30c735d18477630116e3d14360fd4293d6b6);
confirmed by direct inspection; checked against the forum search and not
found (apparently unreported)
Prior-art check: searched sqlite.org/forum directly for `readSqlFromFile` and
related terms — not found
Artifacts: poc/expert_readsqlfromfile_null_poc.c, poc/expert_readsqlfromfile_null_gdb.txt

---

## Title

NULL pointer write on allocation failure in readSqlFromFile() (ext/expert/expert.c)

---

## Body

Hello,

I believe there is an unchecked-allocation NULL pointer dereference in
`readSqlFromFile()` in `ext/expert/expert.c`, present on current trunk. This
is the file-reading helper used by the `sqlite3_expert` command-line tool's
`-file FILE` option.

## The defect

    static int readSqlFromFile(sqlite3expert *p, const char *zFile, char **pzErr){
      FILE *in = fopen(zFile, "rb");
      long nIn;
      size_t nRead;
      char *pBuf;
      int rc;
      if( in==0 ){
        *pzErr = sqlite3_mprintf("failed to open file %s\n", zFile);
        return SQLITE_ERROR;
      }
      fseek(in, 0, SEEK_END);
      nIn = ftell(in);
      rewind(in);
      pBuf = sqlite3_malloc64( nIn+1 );
      nRead = fread(pBuf, nIn, 1, in);
      fclose(in);
      if( nRead!=1 ){
        sqlite3_free(pBuf);
        *pzErr = sqlite3_mprintf("failed to read file %s\n", zFile);
        return SQLITE_ERROR;
      }
      pBuf[nIn] = 0;
      ...
    }

`pBuf` is used directly as the destination buffer for `fread()`, and then
written to again at `pBuf[nIn] = 0`, with no check that `sqlite3_malloc64()`
actually succeeded. `fopen()`'s return value is checked two lines above this
code, but the allocation's is not.

## Reachability

`sqlite3_malloc64()` returns NULL on allocation failure exactly like
`malloc()`, and every other allocation call site in this same file (e.g. in
`main()`) is guarded. `-file FILE` accepts an arbitrarily large input file —
`nIn` comes straight from `ftell()` on that file, so a large-enough input
(or a low-memory environment) makes the allocation fail, at which point
`fread(pBuf, nIn, 1, in)` and `pBuf[nIn] = 0` both dereference NULL.

This isn't a case where `fread()` fails gracefully and lets the existing
`if( nRead!=1 )` check catch it. glibc's `fread()` does not validate the
destination pointer before use -- it forwards straight into `memcpy()`
against the NULL buffer, and the process crashes inside libc before
`fread()` ever returns.

## Reproducer (GDB backtrace)

The driver below reproduces the exact call shape from `readSqlFromFile()`
against a real, nonzero-length file (so `nIn>0`, exactly as for any real
input to `-file FILE`), with `pBuf` forced to NULL as it would be on a
failed `sqlite3_malloc64()`:

----------------------------------------------------------------------
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

  pBuf = NULL;  /* simulates sqlite3_malloc64(nIn+1) returning NULL on OOM */
  nRead = fread(pBuf, nIn, 1, in);

  printf("nRead = %zu (unreachable if fread() crashed)\n", nRead);
  fclose(in);
  return 0;
}
----------------------------------------------------------------------

    $ gcc -O0 -g -o repro poc.c
    $ gdb -batch -ex run -ex bt --args ./repro poc.c
    Program received signal SIGSEGV, Segmentation fault.
    __memcpy_sse2_unaligned_erms () at ../sysdeps/x86_64/.../memmove-vec-unaligned-erms.S:496
    #0  __memcpy_sse2_unaligned_erms ()
    #1  0x00007ffff7e532bb in __GI__IO_file_xsgetn (fp=..., data=<optimized out>, n=1259) at ./libio/fileops.c:1295
    #2  0x00007ffff7e48715 in __GI__IO_fread (buf=0x0, size=1259, count=1, fp=...) at ./libio/iofread.c:38
    #3  0x000055555555527e in main (...) at poc.c:30

`fread()` crashes inside glibc's own `memcpy()` against the NULL buffer,
before it ever gets a chance to return -- confirming the `if( nRead!=1 )`
check in `readSqlFromFile()` is never reached on this path.

## Suggested fix

Add the missing check, consistent with the rest of the file:

    pBuf = sqlite3_malloc64( nIn+1 );
    if( pBuf==0 ){
      fclose(in);
      *pzErr = sqlite3_mprintf("out of memory\n");
      return SQLITE_NOMEM;
    }
    nRead = fread(pBuf, nIn, 1, in);

## Disclosure

This was found during a static-analysis study of the SQLite source and the
analysis/triage was assisted by an AI tool; I have manually verified the
code against current trunk and reproduced the crash under GDB before
reporting. Happy to provide any further detail.

Thanks,
Brandon Arrendondo
