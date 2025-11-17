/*
 * Rule: POS04-C
 * Source: wiki
 * Status: FAIL - Should trigger POS04-C violation
 */

pthread_mutexattr_t attr;
pthread_mutex_t mutex;
size_t const shared_var = 0;

int main(void) {
  int result;

  if ((result = pthread_mutexattr_init(&attr)) != 0) {
    /* Handle Error */
  }
  if ((result = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL)) != 0) {
    /* Handle Error */
  }
  if ((result = pthread_mutex_init(&mutex, &attr)) != 0) {
    /* Handle Error */
  }
  if ((result = pthread_mutex_lock(&mutex)) != 0) {
    /* Handle Error */
  }

  /* Critical Region*/

  if ((result = pthread_mutex_unlock(&mutex)) != 0) {
    /* Handle Error */
  }

  return 0;
}