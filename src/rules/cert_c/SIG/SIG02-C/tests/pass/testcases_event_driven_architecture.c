/*
 * Rule: SIG02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG02-C violation
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <signal.h>
#include <string.h>

#define MAX_EVENTS 10

typedef enum {
    EVENT_USER_ACTION = 1,
    EVENT_TIMER_EXPIRED = 2,
    EVENT_DATA_READY = 3
} event_type_t;

typedef struct {
    event_type_t type;
    int data;
} event_data_t;

// Signal handler only for critical error conditions
void error_handler(int sig) {
    if (sig == SIGSEGV) {
        printf("CRITICAL ERROR: Segmentation fault detected - emergency shutdown\n");
        exit(EXIT_FAILURE);
    }
}

void process_user_event(int data) {
    printf("Processing user action event (data: %d)\n", data);
    printf("Updating user interface state\n");
}

void process_timer_event(int data) {
    printf("Processing timer event (timer_id: %d)\n", data);
    printf("Executing scheduled maintenance task\n");
}

void process_data_event(int data) {
    printf("Processing data ready event (bytes: %d)\n", data);
    printf("Reading and processing incoming data\n");
}

int main() {
    printf("Using proper event-driven architecture without signals for normal events (GOOD)\n");

    // Set up signal handler only for critical errors
    signal(SIGSEGV, error_handler);

    // Create epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // Create event file descriptors for different event types
    int user_event_fd = eventfd(0, EFD_CLOEXEC);
    int timer_event_fd = eventfd(0, EFD_CLOEXEC);
    int data_event_fd = eventfd(0, EFD_CLOEXEC);

    if (user_event_fd == -1 || timer_event_fd == -1 || data_event_fd == -1) {
        perror("eventfd");
        exit(EXIT_FAILURE);
    }

    // Add event file descriptors to epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;

    ev.data.fd = user_event_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, user_event_fd, &ev);

    ev.data.fd = timer_event_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timer_event_fd, &ev);

    ev.data.fd = data_event_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, data_event_fd, &ev);

    // Fork event generator process
    pid_t child = fork();
    if (child == 0) {
        // Child process generates events using proper mechanisms
        printf("Event Generator: Starting event generation\n");

        sleep(1);
        printf("Event Generator: Generating user event\n");
        uint64_t user_data = 42;
        write(user_event_fd, &user_data, sizeof(user_data));

        sleep(2);
        printf("Event Generator: Generating timer event\n");
        uint64_t timer_data = 100;
        write(timer_event_fd, &timer_data, sizeof(timer_data));

        sleep(1);
        printf("Event Generator: Generating data event\n");
        uint64_t data_bytes = 1024;
        write(data_event_fd, &data_bytes, sizeof(data_bytes));

        exit(0);
    } else {
        // Parent process handles events
        printf("Event Processor: Starting event processing loop\n");

        struct epoll_event events[MAX_EVENTS];
        int events_processed = 0;

        while (events_processed < 3) {
            int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
            if (nfds == -1) {
                perror("epoll_wait");
                break;
            }

            for (int i = 0; i < nfds; i++) {
                uint64_t event_data;
                ssize_t bytes_read = read(events[i].data.fd, &event_data, sizeof(event_data));

                if (bytes_read == sizeof(event_data)) {
                    if (events[i].data.fd == user_event_fd) {
                        process_user_event((int)event_data);
                    } else if (events[i].data.fd == timer_event_fd) {
                        process_timer_event((int)event_data);
                    } else if (events[i].data.fd == data_event_fd) {
                        process_data_event((int)event_data);
                    }
                    events_processed++;
                }
            }
        }

        wait(NULL);
        printf("Event processing completed using proper event mechanisms\n");

        // Cleanup
        close(user_event_fd);
        close(timer_event_fd);
        close(data_event_fd);
        close(epoll_fd);
    }

    return 0;
}