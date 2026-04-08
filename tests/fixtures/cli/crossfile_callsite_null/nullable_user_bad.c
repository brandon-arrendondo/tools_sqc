/*
 * Cross-file can_return_null test — bad user.
 * Calls get_buffer() without checking for NULL before dereference.
 * With -d, prescan knows get_buffer() can return NULL, so EXP34-C
 * should flag the dereference.
 */

int *get_buffer(int size);

void use_buffer_bad(void) {
    int *buf = get_buffer(10);
    /* Missing NULL check — EXP34-C violation */
    buf[0] = 42;
}
