/*
 * Rule: POS47-C
 * Source: wiki
 * Status: PASS - Should NOT trigger POS47-C violation
 */

void* worker_thread(void* dummy) {
  int c;
  int result;

  while (1) {
    if ((result = pthread_mutex_lock(&global_lock)) != 0) {
      /* handle error */
    }
    c = b;
    b = a;
    a = c;
    if ((result = pthread_mutex_unlock(&global_lock)) != 0) {
      /* handle error */
    }

    /* now we're safe to cancel, creating cancel point */
    pthread_testcancel();
  }
  return NULL;
}