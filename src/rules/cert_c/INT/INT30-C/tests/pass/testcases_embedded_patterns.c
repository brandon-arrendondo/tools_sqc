/*
 * Rule: INT30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT30-C violation
 *
 * Covers three embedded-C idioms:
 *   1. Plain ++/-- of a wide-unsigned struct/union field counter
 *      (monotonic tick / sequence counter — wrap at 2^32 is benign).
 *   2. A thin calloc wrapper that forwards its parameters directly to
 *      calloc, delegating overflow detection to C11 §7.22.3.2.
 *   3. An `if (a < b) return/break/continue` early-exit guard placing
 *      subsequent `a - b` style subtractions in the implicit else-path.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint32_t tick;
    uint64_t seq;
} Ctx;

/* Pattern 1: wide-unsigned struct-field increments. */
void advance(Ctx *ctx) {
    ++ctx->tick;
    ctx->seq++;
    --ctx->tick;
    ctx->seq--;
}

/* Pattern 2: thin calloc wrapper. Both args are function parameters. */
void *my_calloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0) {
        return NULL;
    }
    return calloc(nmemb, size);
}

/* Pattern 3: implicit-else from early-exit if. */
uint16_t positive_delta(uint16_t time, uint16_t half) {
    if (time < half) {
        return 0;
    }
    /* `time >= half` holds here, so `time - half` cannot underflow. */
    return time - half;
}

uint16_t positive_delta_compound(uint16_t a, uint16_t b) {
    if (a <= b) {
        return 0;
    }
    /* `a > b` holds here, so `a - b` cannot underflow. */
    return a - b;
}
