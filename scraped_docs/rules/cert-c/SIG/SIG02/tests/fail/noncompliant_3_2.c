void dologout(status) {
 /*
  * Prevent reception of SIGURG from resulting in a resumption
  * back to the main program loop.
  */ 
  transflag = 0;
  if (logged_in) {
    (void) seteuid((uid_t)0);
    logwtmp(ttyline, "", "");
    /* ... */
  }
  _exit(status);
}