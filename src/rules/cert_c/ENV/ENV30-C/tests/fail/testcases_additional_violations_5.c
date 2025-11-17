/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: additional_violations_5.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: SSH configuration modification */
void unsafe_ssh_config(void) {
    char *ssh_config = getenv("SSH_CONFIG");
    if (ssh_config) {
        strcat(ssh_config, " -o StrictHostKeyChecking=no");  /* Undefined behavior */
        printf("SSH config: %s\n", ssh_config);
    }
}

/* NON-COMPLIANT: Git configuration modification */
void unsafe_git_config(void) {
    char *git_author = getenv("GIT_AUTHOR_NAME");
    if (git_author) {
        strcat(git_author, " (Bot)");  /* Undefined behavior */
        printf("Git author: %s\n", git_author);
    }
}

/* NON-COMPLIANT: AWS configuration modification */
void unsafe_aws_config(void) {
    char *aws_region = getenv("AWS_DEFAULT_REGION");
    if (aws_region) {
        strcpy(aws_region, "us-east-1");  /* Undefined behavior */
        printf("AWS region: %s\n", aws_region);
    }
}

int main(void) {
    setenv("SSH_CONFIG", "/etc/ssh/ssh_config", 1);
    setenv("GIT_AUTHOR_NAME", "John Doe", 1);
    setenv("AWS_DEFAULT_REGION", "us-west-2", 1);

    unsafe_ssh_config();
    unsafe_git_config();
    unsafe_aws_config();
    return 0;
}