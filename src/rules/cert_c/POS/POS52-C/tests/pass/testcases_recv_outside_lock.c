/*
 * Rule: POS52-C
 * Source: testcases
 * Status: PASS - Should NOT trigger POS52-C violation
 *
 * recv() performed outside the locked region
 */

#include <pthread.h>
#include <sys/socket.h>

pthread_mutex_t mutex;
int shared_data;

void recv_outside_lock(int sockfd) {
    char buf[256];
    /* COMPLIANT: blocking I/O outside lock */
    recv(sockfd, buf, sizeof(buf), 0);
    pthread_mutex_lock(&mutex);
    shared_data = buf[0];
    pthread_mutex_unlock(&mutex);
}
