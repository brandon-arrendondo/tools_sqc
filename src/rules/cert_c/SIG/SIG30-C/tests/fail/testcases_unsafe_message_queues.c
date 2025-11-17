/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <mqueue.h>
#include <unistd.h>

struct msgbuf {
    long mtype;
    char mtext[100];
};

void mq_handler(int sig) {
    // VIOLATION: System V message queue functions are not async-safe
    key_t key = ftok("/tmp", 'M');

    // VIOLATION: msgget() is not async-safe
    int msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid != -1) {
        struct msgbuf message;
        message.mtype = 1;
        sprintf(message.mtext, "Signal %d received", sig);

        // VIOLATION: msgsnd() is not async-safe
        msgsnd(msqid, &message, strlen(message.mtext) + 1, IPC_NOWAIT);

        // VIOLATION: msgrcv() is not async-safe
        struct msgbuf received;
        msgrcv(msqid, &received, sizeof(received.mtext), 1, IPC_NOWAIT);

        // VIOLATION: msgctl() is not async-safe
        msgctl(msqid, IPC_RMID, NULL);
    }

    // VIOLATION: POSIX message queue functions
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 100;
    attr.mq_curmsgs = 0;

    // VIOLATION: mq_open() is not async-safe
    mqd_t mq = mq_open("/signal_mq", O_CREAT | O_WRONLY, 0644, &attr);
    if (mq != (mqd_t)-1) {
        char msg[100];
        sprintf(msg, "POSIX signal %d", sig);

        // VIOLATION: mq_send() is not async-safe
        mq_send(mq, msg, strlen(msg), 0);

        // VIOLATION: mq_close() is not async-safe
        mq_close(mq);

        // VIOLATION: mq_unlink() is not async-safe
        mq_unlink("/signal_mq");
    }
}

int main() {
    printf("Demonstrating unsafe message queue functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, mq_handler);

    printf("Send SIGUSR1 to trigger unsafe message queue operations\n");

    while (1) {
        pause();
    }

    return 0;
}