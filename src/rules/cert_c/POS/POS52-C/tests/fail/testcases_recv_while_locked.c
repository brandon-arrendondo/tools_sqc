/*
 * Rule: POS52-C
 * Source: testcases
 * Status: FAIL - Should trigger POS52-C violation
 *
 * Blocking recv() while holding pthread mutex
 */

#include <pthread.h>
#include <sys/socket.h>

pthread_mutex_t mutex;

void locked_recv(int sockfd) {
    char buf[256];
    pthread_mutex_lock(&mutex);
    /* VIOLATION: blocking I/O while holding mutex */
    recv(sockfd, buf, sizeof(buf), 0);
    pthread_mutex_unlock(&mutex);
}
