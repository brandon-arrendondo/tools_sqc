/*
 * Rule: API05-C
 * Source: wiki
 * Status: PASS - Should NOT trigger API05-C violation
 */

void my_memset(size_t n; char p[n], size_t n, char v)
{
  memset(p, v, n);
}