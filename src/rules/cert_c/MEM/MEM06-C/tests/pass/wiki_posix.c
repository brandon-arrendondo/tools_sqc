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

char *secret;

secret = (char *)malloc(size+1);
if (!secret) {
  /* Handle error */
}

/* Perform operations using secret... */

memset_s(secret, '\0', size+1);
free(secret);
secret = NULL;