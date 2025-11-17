/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void directory_handler(int sig) {
    // VIOLATION: opendir() is not async-safe
    DIR *dir = opendir("/tmp");
    if (dir != NULL) {
        // VIOLATION: readdir() is not async-safe
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            // Process directory entries
            if (entry->d_name[0] != '.') {
                break;  // Just process first non-hidden entry
            }
        }

        // VIOLATION: closedir() is not async-safe
        closedir(dir);
    }

    // VIOLATION: mkdir() is not async-safe
    mkdir("/tmp/signal_test", 0755);

    // VIOLATION: rmdir() is not async-safe
    rmdir("/tmp/signal_test");

    // VIOLATION: stat() family functions are not async-safe
    struct stat file_stat;
    stat("/tmp", &file_stat);
}

int main() {
    printf("Demonstrating unsafe directory operations in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, directory_handler);

    printf("Send SIGUSR1 to trigger unsafe directory operations\n");

    while (1) {
        pause();
    }

    return 0;
}