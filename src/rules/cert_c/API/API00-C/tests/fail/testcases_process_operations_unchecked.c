/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: process_operations_unchecked.c
 *
 * This case demonstrates violations where process management functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

/* Process information structure */
typedef struct {
    pid_t pid;
    char *command;
    int status;
    time_t start_time;
} ProcessInfo;

/* NON-COMPLIANT: No validation of command execution parameters */
int execute_command(const char *command, char *const argv[], char *const envp[]) {
    /* No validation of command or argv */
    pid_t pid = fork();

    if (pid == 0) {
        /* Child process - no validation of parameters */
        execve(command, argv, envp);  /* command could be NULL */
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        /* Parent process */
        int status;
        waitpid(pid, &status, 0);
        return status;
    }

    return -1;
}

/* NON-COMPLIANT: No validation of process creation parameters */
ProcessInfo *create_process(const char *program_path, char *const args[]) {
    ProcessInfo *proc_info = malloc(sizeof(ProcessInfo));

    /* No validation of program_path or args */
    proc_info->command = malloc(strlen(program_path) + 1);  /* program_path could be NULL */
    strcpy(proc_info->command, program_path);

    pid_t pid = fork();
    if (pid == 0) {
        execv(program_path, args);  /* Both could be NULL */
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        proc_info->pid = pid;
        proc_info->status = 0;
        proc_info->start_time = time(NULL);
    } else {
        free(proc_info->command);
        free(proc_info);
        return NULL;
    }

    return proc_info;
}

/* NON-COMPLIANT: No validation of process termination parameters */
int terminate_process(ProcessInfo *proc_info, int signal_num) {
    /* No validation of proc_info or signal_num */
    return kill(proc_info->pid, signal_num);  /* proc_info could be NULL */
}

/* NON-COMPLIANT: No validation of process monitoring */
int wait_for_process(ProcessInfo *proc_info, int *exit_status) {
    /* No validation of proc_info */
    int status;
    pid_t result = waitpid(proc_info->pid, &status, 0);  /* proc_info could be NULL */

    if (result > 0) {
        if (exit_status) {
            *exit_status = WEXITSTATUS(status);
        }
        proc_info->status = status;
        return 0;
    }

    return -1;
}

/* NON-COMPLIANT: No validation of pipe creation */
int create_pipe_process(const char *command, int *read_fd, int *write_fd) {
    /* No validation of command or file descriptor pointers */
    int pipe_in[2], pipe_out[2];

    if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0) {
        return -1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        /* Child process */
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);
        close(pipe_in[1]);
        close(pipe_out[0]);

        /* No validation of command */
        execlp("/bin/sh", "sh", "-c", command, NULL);  /* command could be NULL */
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        /* Parent process */
        close(pipe_in[0]);
        close(pipe_out[1]);

        /* No validation of output pointers */
        *read_fd = pipe_out[0];  /* read_fd could be NULL */
        *write_fd = pipe_in[1];  /* write_fd could be NULL */
        return pid;
    }

    return -1;
}

/* NON-COMPLIANT: No validation of environment setting */
void set_process_environment(char *const envp[], const char *key, const char *value) {
    /* No validation of any parameters */
    char *env_string = malloc(strlen(key) + strlen(value) + 2);  /* key and value could be NULL */
    sprintf(env_string, "%s=%s", key, value);

    /* Mock environment setting - in reality this would modify envp */
    printf("Setting environment: %s\n", env_string);  /* For demonstration */
    free(env_string);
}

/* NON-COMPLIANT: No validation of process group operations */
int set_process_group(pid_t pid, pid_t pgid) {
    /* No validation of pid or pgid */
    return setpgid(pid, pgid);  /* Both could be invalid */
}

/* NON-COMPLIANT: No validation of process priority */
int set_process_priority(pid_t pid, int priority) {
    /* No validation of pid or priority range */
    return setpriority(PRIO_PROCESS, pid, priority);  /* priority could be out of range */
}

/* NON-COMPLIANT: No validation of process limits */
void set_resource_limit(int resource, long limit) {
    /* No validation of resource type or limit value */
    struct rlimit rlim;
    rlim.rlim_cur = limit;  /* limit could be negative or excessive */
    rlim.rlim_max = limit;
    setrlimit(resource, &rlim);  /* resource could be invalid */
}

/* NON-COMPLIANT: No validation of process status checking */
ProcessInfo *get_process_status(pid_t pid) {
    ProcessInfo *info = malloc(sizeof(ProcessInfo));

    /* No validation of pid */
    info->pid = pid;  /* pid could be invalid */
    info->command = NULL;
    info->status = 0;
    info->start_time = 0;

    /* Mock status retrieval */
    char proc_path[256];
    sprintf(proc_path, "/proc/%d/cmdline", pid);  /* pid could be invalid */

    FILE *cmdline_file = fopen(proc_path, "r");
    if (cmdline_file) {
        char command_buffer[1024];
        if (fgets(command_buffer, sizeof(command_buffer), cmdline_file)) {
            info->command = malloc(strlen(command_buffer) + 1);
            strcpy(info->command, command_buffer);
        }
        fclose(cmdline_file);
    }

    return info;
}

int main(void) {
    ProcessInfo *null_proc = NULL;
    char *null_command = NULL;
    char **null_argv = NULL;
    int *null_fd = NULL;

    /* Examples of dangerous process operations */
    // execute_command(null_command, null_argv, NULL);  /* NULL parameters */
    // create_process(null_command, null_argv);  /* NULL parameters */
    // terminate_process(null_proc, -1);  /* NULL process and invalid signal */
    // wait_for_process(null_proc, NULL);  /* NULL process */
    // create_pipe_process(null_command, null_fd, null_fd);  /* NULL parameters */
    // set_process_environment(NULL, null_command, null_command);  /* NULL parameters */
    // set_process_group(-1, -1);  /* Invalid PIDs */
    // set_process_priority(-1, 1000);  /* Invalid PID and priority */
    // set_resource_limit(-1, -1000);  /* Invalid resource and limit */
    // get_process_status(-1);  /* Invalid PID */

    printf("Process functions compiled but lack parameter validation\n");
    return 0;
}