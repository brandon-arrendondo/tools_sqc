/*
 * Rule: DCL40-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL40-C violation
 */

/* In bashline.h */
extern char * bash_groupname_completion(const char *, int);   

/* In a.c */
#include "bashline.h"

void f(const char *s, int i) {
  bash_groupname_completion(s, i);  
}

/* In b.c */
int bash_groupname_completion_funct;