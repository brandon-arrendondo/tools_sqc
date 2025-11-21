// POS02-C: Compliant - drop privileges after setup
#include <unistd.h>

void test_pos02c_pass() {
    chown("/tmp/file", 0, 0);  // Privileged operation
    // OK: Drop privileges immediately after
    setuid(getuid());
}
