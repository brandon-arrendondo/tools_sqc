/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

volatile sig_atomic_t dir_operations = 0;

void directory_handler(int sig) {
    dir_operations++;
    char dirname[256];
    char filename[512];

    printf("Handler: Signal %d performing directory operations\n", sig);

    // Violation: Directory operations without proper signal masking
    // mkdir, opendir, readdir are not async-safe
    snprintf(dirname, sizeof(dirname), "/tmp/signal_%d_dir_%d",
             sig, dir_operations);

    printf("Handler: Creating directory %s\n", dirname);

    // Create directory (not async-safe)
    if (mkdir(dirname, 0755) != 0) {
        perror("Handler: mkdir failed");
        return;
    }

    // Create some files in the directory
    for (int i = 0; i < 3; i++) {
        snprintf(filename, sizeof(filename), "%s/file_%d.txt", dirname, i);

        FILE* fp = fopen(filename, "w");
        if (fp != NULL) {
            fprintf(fp, "Created by signal %d, iteration %d\n", sig, i);
            fclose(fp);

            // Create vulnerability window
            usleep(100000);
        }
    }

    // Read directory contents (not async-safe)
    DIR* dir = opendir(dirname);
    if (dir != NULL) {
        struct dirent* entry;
        int file_count = 0;

        printf("Handler: Directory contents:\n");
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                printf("Handler:   %s\n", entry->d_name);
                file_count++;

                // Vulnerability window during readdir
                usleep(50000);
            }
        }

        closedir(dir);
        printf("Handler: Found %d files\n", file_count);
    }

    printf("Handler: Directory operations complete\n");
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = directory_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Directory I/O functions not async-safe
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send signals to trigger directory operations\n");

    while (1) {
        printf("Main: Directory operations performed: %d\n", dir_operations);

        // Main thread also does directory operations
        DIR* tmp_dir = opendir("/tmp");
        if (tmp_dir != NULL) {
            struct dirent* entry;
            int signal_dirs = 0;

            while ((entry = readdir(tmp_dir)) != NULL) {
                if (strncmp(entry->d_name, "signal_", 7) == 0) {
                    signal_dirs++;
                }
            }

            closedir(tmp_dir);
            printf("Main: Found %d signal-created directories in /tmp\n", signal_dirs);
        }

        sleep(4);
    }

    return 0;
}