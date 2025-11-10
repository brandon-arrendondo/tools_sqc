(void) execl(LOGIN_PROGRAM, "login",
  "-p",
  "-d", slavename,
  "-h", host,
  "-s", pam_svc_name,
  (AuthenticatingUser != NULL ? AuthenticatingUser :
  getenv("USER")),
  0);