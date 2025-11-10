#include <stdlib.h>
#include <stdint.h>
 
enum { BLOCK_HEADER_SIZE = 16 };

void *AllocateBlock(size_t length) {
  struct memBlock *mBlock;

  if (SIZE_MAX - length < BLOCK_HEADER_SIZE) return NULL;
  mBlock = (struct memBlock *)malloc(
    length + BLOCK_HEADER_SIZE
  );
  if (!mBlock) { return NULL; }
  /* Fill in block header and return data portion */

  return mBlock;
}