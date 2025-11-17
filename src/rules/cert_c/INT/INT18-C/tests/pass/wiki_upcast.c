/*
 * Rule: INT18-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT18-C violation
 */

#include <stdlib.h>
#include <stdint.h>


enum { BLOCK_HEADER_SIZE = 16 };
 
void *AllocateBlock(size_t length) {
  struct memBlock *mBlock;

  if ((unsigned long long)length + BLOCK_HEADER_SIZE > SIZE_MAX) {
    return NULL;
  }
  mBlock = (struct memBlock *)malloc(
    length + BLOCK_HEADER_SIZE
  );
  if (!mBlock) { return NULL; }
  /* Fill in block header and return data portion */

  return mBlock;
}