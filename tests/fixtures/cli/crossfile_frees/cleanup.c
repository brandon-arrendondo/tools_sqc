/*
 * Cross-file frees_params test — module that frees memory.
 * Defines a function that frees its first parameter.
 * Prescan should compute frees_params = {0} for reclaim_buffer().
 *
 * Note: function name intentionally avoids deallocation heuristic prefixes
 * (destroy_, free_, delete_, cleanup_, release_, close_) to ensure the
 * test exercises prescan frees_params detection, not name-based heuristics.
 */

#include <stdlib.h>

void reclaim_buffer(void *buf) {
    free(buf);
}
