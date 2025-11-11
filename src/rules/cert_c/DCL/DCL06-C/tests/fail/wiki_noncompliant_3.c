/*
 * Rule: DCL06-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL06-C violation
 */

LDAP *ld = ldap_init("localhost", 1234);
if (ld == NULL) {
  perror("ldap_init");
  return(1);
}