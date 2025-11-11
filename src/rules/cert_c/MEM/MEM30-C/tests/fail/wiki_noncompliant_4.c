/*
 * Rule: MEM30-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM30-C violation
 */

void gdClipSetAdd(gdImagePtr im, gdClipRectanglePtr rect) {
  gdClipRectanglePtr more;
  if (im->clip == 0) {
   /* ... */
  }
  if (im->clip->count == im->clip->max) {
    more = gdRealloc (im->clip->list,(im->clip->max + 8) *
                      sizeof (gdClipRectangle));
    /*
     * If the realloc fails, then we have not lost the
     * im->clip->list value.
     */
    if (more == 0) return; 
    im->clip->max += 8;
  }
  im->clip->list[im->clip->count] = *rect;
  im->clip->count++;

}