/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

void user_handler(int sig) {
    // VIOLATION: getpwuid() is not async-safe
    uid_t uid = getuid();
    struct passwd *pwd = getpwuid(uid);

    // VIOLATION: getpwnam() is not async-safe
    struct passwd *root_pwd = getpwnam("root");

    // VIOLATION: getgrgid() is not async-safe
    gid_t gid = getgid();
    struct group *grp = getgrgid(gid);

    // VIOLATION: getgrnam() is not async-safe
    struct group *wheel_grp = getgrnam("wheel");

    // VIOLATION: getpwent(), setpwent(), endpwent() are not async-safe
    setpwent();
    struct passwd *pwd_entry = getpwent();
    endpwent();

    // VIOLATION: getgrent(), setgrent(), endgrent() are not async-safe
    setgrent();
    struct group *grp_entry = getgrent();
    endgrent();

    // VIOLATION: Processing user/group information
    if (pwd != NULL) {
        printf("User: %s, Home: %s\n", pwd->pw_name, pwd->pw_dir);
    }

    if (grp != NULL) {
        printf("Group: %s\n", grp->gr_name);
    }
}

int main() {
    printf("Demonstrating unsafe user/group functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, user_handler);

    printf("Send SIGUSR1 to trigger unsafe user/group operations\n");

    while (1) {
        pause();
    }

    return 0;
}