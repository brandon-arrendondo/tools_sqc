/*
 * Rule: POS38-C
 * Source: wiki (variation)
 * Status: PASS - Should NOT trigger POS38-C violation
 *
 * This is compliant because the child process closes the inherited
 * file descriptor and doesn't use it, avoiding the race condition.
 */

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

char c;
pid_t pid;

int fd = open("data.txt", O_RDWR);
if (fd == -1) {
  /* Handle error */
}

read(fd, &c, 1);

pid = fork();
if (pid == -1) {
  /* Handle error */
}

if (pid == 0) { /*child*/
  close(fd);
  /* Child doesn't use the file descriptor */
}
else { /*parent*/
  read(fd, &c, 1);
  close(fd);
}
