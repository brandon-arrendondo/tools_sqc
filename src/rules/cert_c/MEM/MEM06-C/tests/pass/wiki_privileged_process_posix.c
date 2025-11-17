/*
 * Rule: MEM06-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM06-C violation
 */

#include <sys/resource.h>
/* ... */
struct rlimit limit;
limit.rlim_cur = 0;
limit.rlim_max = 0;
if (setrlimit(RLIMIT_CORE, &limit) != 0) {
    /* Handle error */
}

long pagesize = sysconf(_SC_PAGESIZE);
if (pagesize == -1) {
  /* Handle error */
}

char *secret_buf;
char *secret;

secret_buf = (char *)malloc(size+1+pagesize);
if (!secret_buf) {
  /* Handle error */
}

/* mlock() may require that address be a multiple of PAGESIZE */
secret = (char *)((((intptr_t)secret_buf + pagesize - 1) / pagesize) * pagesize);

if (mlock(secret, size+1) != 0) {
    /* Handle error */
}

/* Perform operations using secret... */

if (munlock(secret, size+1) != 0) {
    /* Handle error */
}
secret = NULL;

memset_s(secret_buf, '\0', size+1+pagesize);
free(secret_buf);
secret_buf = NULL;