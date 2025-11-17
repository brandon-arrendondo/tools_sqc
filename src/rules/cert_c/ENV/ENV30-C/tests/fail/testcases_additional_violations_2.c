/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: additional_violations_2.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Java options modification */
void unsafe_java_opts(void) {
    char *java_opts = getenv("JAVA_OPTS");
    if (java_opts) {
        strcat(java_opts, " -Xmx2g");  /* Undefined behavior */
        printf("Enhanced JAVA_OPTS: %s\n", java_opts);
    }
}

/* NON-COMPLIANT: Node options modification */
void unsafe_node_opts(void) {
    char *node_opts = getenv("NODE_OPTIONS");
    if (node_opts) {
        strcat(node_opts, " --max-old-space-size=4096");  /* Undefined behavior */
        printf("Enhanced NODE_OPTIONS: %s\n", node_opts);
    }
}

/* NON-COMPLIANT: Go path modification */
void unsafe_go_path(void) {
    char *gopath = getenv("GOPATH");
    if (gopath) {
        strcat(gopath, ":/opt/go");  /* Undefined behavior */
        printf("Enhanced GOPATH: %s\n", gopath);
    }
}

int main(void) {
    setenv("JAVA_OPTS", "-Xms1g", 1);
    setenv("NODE_OPTIONS", "--inspect", 1);
    setenv("GOPATH", "/home/user/go", 1);

    unsafe_java_opts();
    unsafe_node_opts();
    unsafe_go_path();
    return 0;
}