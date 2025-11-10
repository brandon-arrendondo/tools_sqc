/* THREAD 1 */
int do_work(void) {
  /* ... */
  kill(THR2_PID, SIGUSR1);
}

/* THREAD 2 */
volatile sig_atomic_t flag;

void sigusr1_handler(int signum) {
  flag = 1;
}

int wait_and_work(void) {
  flag = 0;
  while (!flag) {}
  /* ... */
}