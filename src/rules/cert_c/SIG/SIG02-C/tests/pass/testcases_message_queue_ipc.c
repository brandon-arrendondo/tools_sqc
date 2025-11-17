/*
 * Rule: SIG02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG02-C violation
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define MSG_KEY 12345

typedef struct {
    long msg_type;
    char msg_text[256];
    int priority;
} message_t;

// Signal handler only for emergency system shutdown
void emergency_handler(int sig) {
    if (sig == SIGINT) {
        printf("EMERGENCY: Interrupt signal received - cleaning up message queue\n");
        int msgid = msgget(MSG_KEY, 0);
        if (msgid != -1) {
            msgctl(msgid, IPC_RMID, NULL);
        }
        exit(0);
    }
}

int main() {
    printf("Using message queues for normal communication, signals only for emergencies (GOOD)\n");

    // Set up signal handler only for emergency conditions
    signal(SIGINT, emergency_handler);

    // Create message queue
    int msgid = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    pid_t child = fork();
    if (child == 0) {
        // Child process - message sender
        printf("Sender: Starting message transmission\n");

        message_t msg;

        sleep(1);
        // Send info message
        msg.msg_type = 1;
        msg.priority = 1;
        strcpy(msg.msg_text, "System startup completed successfully");
        if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
        printf("Sender: Sent info message\n");

        sleep(2);
        // Send warning message
        msg.msg_type = 2;
        msg.priority = 2;
        strcpy(msg.msg_text, "High memory usage detected");
        if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
        printf("Sender: Sent warning message\n");

        sleep(1);
        // Send status message
        msg.msg_type = 3;
        msg.priority = 3;
        strcpy(msg.msg_text, "Daily backup completed");
        if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
        printf("Sender: Sent status message\n");

        exit(0);
    } else {
        // Parent process - message receiver
        printf("Receiver: Starting message processing\n");

        message_t received_msg;
        int messages_received = 0;

        while (messages_received < 3) {
            // Receive messages in order (FIFO)
            if (msgrcv(msgid, &received_msg, sizeof(received_msg) - sizeof(long), 0, 0) != -1) {
                printf("Receiver: Received message (type %ld, priority %d): %s\n",
                       received_msg.msg_type, received_msg.priority, received_msg.msg_text);

                // Process message based on type
                switch (received_msg.msg_type) {
                    case 1:
                        printf("Receiver: Processing info message - logging to system\n");
                        break;
                    case 2:
                        printf("Receiver: Processing warning message - triggering alert\n");
                        break;
                    case 3:
                        printf("Receiver: Processing status message - updating dashboard\n");
                        break;
                }

                messages_received++;
            } else {
                perror("msgrcv");
                break;
            }
        }

        wait(NULL);

        // Clean up message queue
        if (msgctl(msgid, IPC_RMID, NULL) == -1) {
            perror("msgctl");
        }

        printf("Message queue communication completed successfully\n");
    }

    return 0;
}