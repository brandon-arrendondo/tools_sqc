/* In a.c */
extern int *a;   /* UB 14 */

int f(unsigned int i, int x) {
  int tmp = a[i];   /* UB 36: read access */
  a[i] = x;         /* UB 36: write access */
  return tmp;
}

/* In b.c */
int a[] = { 1, 2, 3, 4 };   /* UB 14 */