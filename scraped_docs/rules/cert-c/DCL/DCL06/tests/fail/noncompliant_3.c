LDAP *ld = ldap_init("localhost", 1234);
if (ld == NULL) {
  perror("ldap_init");
  return(1);
}