void dologout(status) {
  if (logged_in) {
    (void) seteuid((uid_t)0);
    logwtmp(ttyline, "", "");
    /* ... */
  }
  _exit(status);
}

static void lostconn(int signo) {
  if (debug)
    syslog(LOG_DEBUG, "lost connection");
  dologout(-1);
}

static void myoob(signo) {
  if (!transflag)
    return;
  /* ... */
  if (strcmp(cp, "ABOR\r\n") == 0) {
    tmpline[0] = '\0';
    reply(426, "Transfer aborted. Data connection closed.");
    reply(226, "Abort successful");
    longjmp(urgcatch, 1);
  }
  /* ... */
}

/* ... */

signal(SIGPIPE, lostconn);
signal(SIGURG, myoob);