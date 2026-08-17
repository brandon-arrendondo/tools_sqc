# SQLite forum disclosure — signed integer overflow in fts5SegmentSize()

Filed by: Brandon Arrendondo (brandon.arrendondo@bissell.com)
Date drafted: 2026-08-17
Target: SQLite User Forum (sqlite.org/forum)
Status at drafting: LIVE at trunk HEAD (13bcd6f6b26a9eb3656ba8f51cad7ee29e260475);
UBSan-confirmed; checked against the forum search and not found (apparently
unreported)
Prior-art check: searched sqlite.org/forum directly for `fts5SegmentSize`
and related terms — not found
Artifacts: poc/fts5_segment_size_overflow_poc.c, poc/fts5_segment_size_overflow_ubsan.txt

---

## Title

Signed integer overflow in fts5SegmentSize() (ext/fts5/fts5_index.c)

---

## Body

Hello,

I believe there is a signed integer overflow (undefined behavior) in
`fts5SegmentSize()` in `ext/fts5/fts5_index.c`, present on current trunk.

## The defect

    static int fts5SegmentSize(Fts5StructureSegment *pSeg){
      return 1 + pSeg->pgnoLast - pSeg->pgnoFirst;
    }

`pgnoFirst` and `pgnoLast` are both plain `int` fields on
`Fts5StructureSegment`, populated in `fts5StructureDecode()` straight from
on-disk varints via `fts5GetVarint32()`:

    i += fts5GetVarint32(&pData[i], pSeg->pgnoFirst);
    i += fts5GetVarint32(&pData[i], pSeg->pgnoLast);
    ...
    if( pSeg->pgnoLast<pSeg->pgnoFirst ){
      rc = FTS5_CORRUPT;
      break;
    }

That check only rejects the case `pgnoLast < pgnoFirst`. It does not bound
how large `pgnoLast` itself can be. A crafted or corrupted fts5 shadow-table
`%_data` structure record can therefore set `pgnoFirst = 0` and
`pgnoLast = INT_MAX` (or any value close to it) and still decode
successfully — `fts5SegmentSize()` then computes `1 + INT_MAX - 0`, which
overflows a signed `int`.

## Reproducer (UBSan)

The struct/function below are copied verbatim (fields/logic unchanged) from
`ext/fts5/fts5_index.c`:

----------------------------------------------------------------------
#include <stdio.h>
#include <limits.h>

/* verbatim from ext/fts5/fts5_index.c: fts5SegmentSize() */
struct Fts5StructureSegment { int iSegid; int pgnoFirst; int pgnoLast; };
static int fts5SegmentSize(struct Fts5StructureSegment *pSeg){
  return 1 + pSeg->pgnoLast - pSeg->pgnoFirst;
}

int main(void){
  struct Fts5StructureSegment seg = { 0, 0, INT_MAX };
  int sz = fts5SegmentSize(&seg);
  printf("pgnoFirst=%d pgnoLast=%d -> fts5SegmentSize=%d (INT_MAX=%d)\n",
         seg.pgnoFirst, seg.pgnoLast, sz, INT_MAX);
  return 0;
}
----------------------------------------------------------------------

    $ gcc -O0 -g -fsanitize=undefined -o repro fts5_segment_size_overflow_poc.c
    $ ./repro
    fts5_segment_size_overflow_poc.c:7:12: runtime error: signed integer
    overflow: 2147483647 + 1 cannot be represented in type 'int'
    pgnoFirst=0 pgnoLast=2147483647 -> fts5SegmentSize=-2147483648 (INT_MAX=2147483647)

## Impact

`fts5StructureDecode()`'s own `pgnoLast<pgnoFirst` check means the *value*
returned can't be produced by taking a difference of out-of-order pages —
the overflow instead comes from `pgnoLast` alone being large. The one call
site (`fts5StructurePromoteTo()`) uses the result only as `if( sz>szPromote )
return;` — a segment-promotion size heuristic, not a buffer size or loop
bound — so I don't have evidence this reaches a memory-safety bug on its
own; a wrapped-to-negative `sz` just short-circuits that comparison to
false and lets promotion proceed regardless of true size. I'm reporting it
as a hardening item consistent with how the project already treats UBSan
findings in fuzzed/corrupt-database input, not as a claimed exploit.

## Suggested fix

Compute the difference in a wider or unsigned type before narrowing, e.g.:

    static int fts5SegmentSize(Fts5StructureSegment *pSeg){
      return (int)(1 + (i64)pSeg->pgnoLast - (i64)pSeg->pgnoFirst);
    }

or clamp/validate `pgnoLast` against a sane maximum page count during
`fts5StructureDecode()`, alongside the existing `pgnoLast<pgnoFirst` check.

## Disclosure

This was found during a static-analysis study of the SQLite source and the
analysis/triage was assisted by an AI tool; I have manually verified the
code against current trunk and reproduced the overflow under UBSan before
reporting. Happy to provide any further detail.

Thanks,
Brandon Arrendondo
