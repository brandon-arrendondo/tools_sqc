/*
 * Rule: POS38-C
 * Source: wiki
 * Status: FAIL - Should trigger POS38-C violation
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

void func(void) {
    char c;
    pid_t pid;

    int fd = open("filename", O_RDWR);
    if (fd == -1) {
      /* Handle error */
    }
    read(fd, &c, 1);
    printf("root process:%c\n",c);

    pid = fork();
    if (pid == -1) {
      /* Handle error */
    }

    if (pid == 0) { /*child*/
      read(fd, &c, 1);
      printf("child:%c\n",c);
    }
    else { /*parent*/
      read(fd, &c, 1);
      printf("parent:%c\n",c);
    }
}
