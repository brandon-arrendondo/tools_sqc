/*
 * Cross-file can_return_null test — safe user.
 * Calls get_buffer() and checks for NULL before dereference.
 * EXP34-C should NOT flag this.
 */

int *get_buffer(int size);

void use_buffer_safe(void) {
    int *buf = get_buffer(10);
    if (buf != NULL) {
        buf[0] = 42;
    }
}
