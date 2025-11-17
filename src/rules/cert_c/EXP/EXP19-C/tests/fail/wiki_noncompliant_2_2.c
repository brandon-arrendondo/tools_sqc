/*
 * Rule: EXP19-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP19-C violation
 */

int login;

if (invalid_login())
  login = 0;
else
  printf("Login is valid\n");  /* Debugging line added here */
  login = 1;                   /* This line always gets executed
                               /* regardless of a valid login! */