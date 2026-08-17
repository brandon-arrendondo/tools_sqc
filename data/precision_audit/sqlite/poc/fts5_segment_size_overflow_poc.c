#include <stdio.h>
#include <limits.h>

/* verbatim from ext/fts5/fts5_index.c: fts5SegmentSize() */
struct Fts5StructureSegment { int iSegid; int pgnoFirst; int pgnoLast; };
static int fts5SegmentSize(struct Fts5StructureSegment *pSeg){
  return 1 + pSeg->pgnoLast - pSeg->pgnoFirst;
}

int main(void){
  /* fts5StructureDecode() only rejects pgnoLast < pgnoFirst (FTS5_CORRUPT);
  ** it never bounds how large pgnoLast itself can be. Both fields are
  ** populated straight from an on-disk varint via fts5GetVarint32() into
  ** a plain `int`, so a crafted/corrupted fts5 %_data structure record can
  ** set pgnoLast = INT_MAX with pgnoFirst = 0 and still pass that check. */
  struct Fts5StructureSegment seg = { 0, 0, INT_MAX };
  int sz = fts5SegmentSize(&seg);
  printf("pgnoFirst=%d pgnoLast=%d -> fts5SegmentSize=%d (INT_MAX=%d)\n",
         seg.pgnoFirst, seg.pgnoLast, sz, INT_MAX);
  return 0;
}
