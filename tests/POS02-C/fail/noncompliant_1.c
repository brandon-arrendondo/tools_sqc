// POS02-C: Noncompliant - privileged operation without privilege drop
#include <unistd.h>

void test_pos02c_fail() {
    // VIOLATION: Perform privileged operation without dropping privileges
    chown("/tmp/file", 0, 0);  // Privileged operation
    // Continue with elevated privileges - no setuid/setgid call
}
