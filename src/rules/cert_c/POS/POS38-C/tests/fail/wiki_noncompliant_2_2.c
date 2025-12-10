/*
 * Rule: POS38-C
 * Source: wiki (variation)
 * Status: FAIL - Should trigger POS38-C violation
 *
 * This is a variation using write operations instead of read operations.
 * The race condition still exists because both parent and child write to
 * the same file descriptor, leading to nondeterministic output order.
 */

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

void func(void) {
    char data[] = "test";
    pid_t pid;

    int fd = open("output.txt", O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
      /* Handle error */
    }

    pid = fork();
    if (pid == -1) {
      /* Handle error */
    }

    if (pid == 0) { /*child*/
      write(fd, "child\n", 6);
    }
    else { /*parent*/
      write(fd, "parent\n", 7);
    }
}
