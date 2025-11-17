/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <unistd.h>

void shm_handler(int sig) {
    // VIOLATION: System V shared memory functions are not async-safe
    key_t key = ftok("/tmp", 'S');

    // VIOLATION: shmget() is not async-safe
    int shmid = shmget(key, 1024, IPC_CREAT | 0666);
    if (shmid != -1) {
        // VIOLATION: shmat() is not async-safe
        void *shmaddr = shmat(shmid, NULL, 0);

        if (shmaddr != (void *)-1) {
            // Write to shared memory
            sprintf((char *)shmaddr, "Signal %d received", sig);

            // VIOLATION: shmdt() is not async-safe
            shmdt(shmaddr);
        }

        // VIOLATION: shmctl() is not async-safe
        shmctl(shmid, IPC_RMID, NULL);
    }

    // VIOLATION: POSIX shared memory functions
    int fd = shm_open("/signal_shm", O_CREAT | O_RDWR, 0666);
    if (fd != -1) {
        ftruncate(fd, 1024);

        // VIOLATION: mmap() is not async-safe
        void *addr = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        if (addr != MAP_FAILED) {
            sprintf((char *)addr, "POSIX signal %d", sig);

            // VIOLATION: munmap() is not async-safe
            munmap(addr, 1024);
        }

        close(fd);
        shm_unlink("/signal_shm");
    }
}

int main() {
    printf("Demonstrating unsafe shared memory functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, shm_handler);

    printf("Send SIGUSR1 to trigger unsafe shared memory operations\n");

    while (1) {
        pause();
    }

    return 0;
}