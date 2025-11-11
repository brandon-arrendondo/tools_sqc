/*
 * Rule: EXP08-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP08-C violation
 */

struct big {
  unsigned long long ull_1; /* Typically 8 bytes */
  unsigned long long ull_2; /* Typically 8 bytes */
  unsigned long long ull_3; /* Typically 8 bytes */
  int si_4; /* Typically 4 bytes */
  int si_5; /* Typically 4 bytes */
};
/* ... */
 
int f(void) {
  size_t skip = offsetof(struct big, ull_2);
  struct big *s = (struct big *)malloc(sizeof(struct big));
  if (!s) {
   return -1; /* Indicate malloc() failure */
  }

  memset(s + skip, 0, sizeof(struct big) - skip);
  /* ... */
  free(s);
  s = NULL;
  
  return 0;
}