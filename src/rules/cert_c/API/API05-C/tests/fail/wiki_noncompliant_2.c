/*
 * Rule: API05-C
 * Source: wiki
 * Status: FAIL - Should trigger API05-C violation
 */

void my_memset(char p[n], size_t n, char v)
{
  memset( p, v, n);
}