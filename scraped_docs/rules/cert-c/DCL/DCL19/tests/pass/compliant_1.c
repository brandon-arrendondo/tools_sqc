void counter() {
  static unsigned int count = 0;
  if (count++ > MAX_COUNT) return;
  /* ... */

}