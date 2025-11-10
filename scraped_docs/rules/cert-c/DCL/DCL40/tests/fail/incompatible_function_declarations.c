/* In a.c */
extern int f(int a);   /* UB 14 */

int g(int a) {
  return f(a);   /* UB 37 */
}

/* In b.c */
long f(long a) {   /* UB 14 */
  return a * 2;
}