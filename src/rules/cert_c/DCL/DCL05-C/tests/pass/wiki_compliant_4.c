/*
 * Rule: DCL05-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL05-C violation
 */

typedef void SighandlerType(int signum);
extern SighandlerType *signal(
  int signum,
  SighandlerType *handler
);