/*
 * Rule: DCL06-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL06-C violation
 */

#ifndef PORTNUMBER     /* Might be passed on compile line */
#  define PORTNUMBER 1234
#endif

#ifndef HOSTNAME       /* Might be passed on compile line */
#  define HOSTNAME "localhost"
#endif

/* ... */

LDAP *ld = ldap_init(HOSTNAME, PORTNUMBER);
if (ld == NULL) {
  perror("ldap_init");
  return(1);
}