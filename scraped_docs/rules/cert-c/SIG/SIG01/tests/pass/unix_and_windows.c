void handler(int signum) {
#ifndef WINDOWS
  if (signal(signum, SIG_DFL) == SIG_ERR) {
    /* Handler error */
  }
#endif
  /* Handle signal */
}