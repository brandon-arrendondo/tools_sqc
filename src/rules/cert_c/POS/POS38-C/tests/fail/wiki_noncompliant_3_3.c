/*
 * Rule: POS38-C
 * Source: wiki (variation)
 * Status: FAIL - Should trigger POS38-C violation
 *
 * This is a variation using lseek operations.
 * The race condition exists because both parent and child seek and read
 * from the same file descriptor, with shared file position.
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

pid = fork();
if (pid == -1) {
  /* Handle error */
}

if (pid == 0) { /*child*/
  lseek(fd, 0, SEEK_SET);
  read(fd, &c, 1);
}
else { /*parent*/
  lseek(fd, 10, SEEK_SET);
  read(fd, &c, 1);
}
