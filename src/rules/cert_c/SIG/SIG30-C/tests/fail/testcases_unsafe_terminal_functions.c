/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios original_termios;

void terminal_handler(int sig) {
    // VIOLATION: tcgetattr() is not async-safe
    struct termios current_termios;
    tcgetattr(STDIN_FILENO, &current_termios);

    // VIOLATION: tcsetattr() is not async-safe
    current_termios.c_lflag &= ~ECHO;  // Disable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &current_termios);

    // VIOLATION: tcdrain(), tcflush(), tcflow() are not async-safe
    tcdrain(STDOUT_FILENO);
    tcflush(STDIN_FILENO, TCIFLUSH);
    tcflow(STDOUT_FILENO, TCOOFF);

    // VIOLATION: cfgetispeed(), cfgetospeed() are not async-safe
    speed_t input_speed = cfgetispeed(&current_termios);
    speed_t output_speed = cfgetospeed(&current_termios);

    // VIOLATION: cfsetispeed(), cfsetospeed() are not async-safe
    cfsetispeed(&current_termios, B9600);
    cfsetospeed(&current_termios, B9600);

    // VIOLATION: ttyname() is not async-safe
    char *tty_name = ttyname(STDIN_FILENO);
    if (tty_name) {
        printf("Terminal: %s\n", tty_name);
    }

    // VIOLATION: isatty() results may be cached
    if (isatty(STDOUT_FILENO)) {
        // Terminal-specific operations
    }
}

int main() {
    printf("Demonstrating unsafe terminal functions in signal handler\n");
    printf("PID: %d\n", getpid());

    // Save original terminal settings
    tcgetattr(STDIN_FILENO, &original_termios);

    signal(SIGUSR1, terminal_handler);

    printf("Send SIGUSR1 to trigger unsafe terminal operations\n");

    while (1) {
        pause();
    }

    return 0;
}