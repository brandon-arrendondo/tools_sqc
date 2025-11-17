/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

void resource_handler(int sig) {
    struct rlimit limit;

    // VIOLATION: getrlimit() is not async-safe
    getrlimit(RLIMIT_CPU, &limit);
    getrlimit(RLIMIT_FSIZE, &limit);
    getrlimit(RLIMIT_DATA, &limit);
    getrlimit(RLIMIT_STACK, &limit);
    getrlimit(RLIMIT_NOFILE, &limit);

    // VIOLATION: setrlimit() is not async-safe
    limit.rlim_cur = limit.rlim_max / 2;
    setrlimit(RLIMIT_CPU, &limit);

    // VIOLATION: getrusage() is not async-safe
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    getrusage(RUSAGE_CHILDREN, &usage);

    // VIOLATION: Processing resource usage information
    printf("User time: %ld.%06ld\n",
           usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
    printf("System time: %ld.%06ld\n",
           usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
    printf("Max RSS: %ld\n", usage.ru_maxrss);

    // VIOLATION: getpriority() and setpriority() are not async-safe
    int priority = getpriority(PRIO_PROCESS, 0);
    setpriority(PRIO_PROCESS, 0, priority + 1);

    // VIOLATION: nice() is not async-safe
    nice(1);
}

int main() {
    printf("Demonstrating unsafe resource limit functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, resource_handler);

    printf("Send SIGUSR1 to trigger unsafe resource operations\n");

    while (1) {
        pause();
    }

    return 0;
}