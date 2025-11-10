/* In a.c */
extern int i;   /* UB 14 */

int f(void) {
  return ++i;   /* UB 36 */
}

/* In b.c */
short i;   /* UB 14 */