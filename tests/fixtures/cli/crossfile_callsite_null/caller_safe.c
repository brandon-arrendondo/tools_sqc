/*
 * Cross-file callsite null test — safe caller.
 * Calls process_data() with a valid (non-NULL) pointer.
 * With -d, prescan should see param 0 = NotNull, and EXP34-C
 * should NOT flag the dereference.
 */

void process_data(int *ptr);

void safe_caller(void) {
    int value = 10;
    process_data(&value);
}
