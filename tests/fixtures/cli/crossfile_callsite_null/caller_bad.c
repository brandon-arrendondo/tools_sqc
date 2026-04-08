/*
 * Cross-file callsite null test — bad caller.
 * Calls process_data(NULL) from another translation unit.
 * With -d, prescan should propagate the NULL argument and EXP34-C
 * should flag the dereference inside process_data() as a null pointer
 * dereference (callsite_param_null_states for param 0 = DefinitelyNull).
 */

void process_data(int *ptr);

void bad_caller(void) {
    process_data(NULL);
}
