/**
 * Compliant validation patterns that should NOT trigger API00-C.
 *
 * Covers patterns where validation is present but in forms not previously
 * recognized: return-expression null checks, if/else chains, and helper fn calls.
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

/* -----------------------------------------------------------------------
 * Pattern A: return expression with null check (short-circuit validation)
 * ----------------------------------------------------------------------- */
bool string_is_null_or_empty(const char *str)
{
    return (str == NULL || str[0] == '\0');
}

bool string_is_null_reversed(const char *str)
{
    return (NULL == str || str[0] == '\0');
}

/* -----------------------------------------------------------------------
 * Pattern B: if/else chain — negative null check, result accumulation
 * ----------------------------------------------------------------------- */
typedef int result_e;
typedef struct { int *buffer; int bufferSize; int readIndex; int writeIndex; } rbuf_t;
#define RESULT_INVALID_ARGS (-1)
#define RESULT_SUCCESS (0)

result_e rbuf_initialize(rbuf_t *ptrBuf, int *ptrBuffer, int bufferSize)
{
    result_e result = RESULT_SUCCESS;

    if (NULL == ptrBuf) {
        result = RESULT_INVALID_ARGS;
    }
    else if (0 == bufferSize) {
        result = RESULT_INVALID_ARGS;
    }
    else if (NULL == ptrBuffer) {
        result = RESULT_INVALID_ARGS;
    }
    else {
        ptrBuf->bufferSize = bufferSize;
        ptrBuf->buffer = ptrBuffer;
        ptrBuf->readIndex = 0;
        ptrBuf->writeIndex = 0;
    }

    return result;
}

result_e rbuf_msg_available(const rbuf_t *ptrBuf)
{
    result_e result;

    if (NULL == ptrBuf) {
        result = RESULT_INVALID_ARGS;
    }
    else if (ptrBuf->readIndex == ptrBuf->writeIndex) {
        result = RESULT_INVALID_ARGS;
    }
    else {
        result = RESULT_SUCCESS;
    }

    return result;
}

/* -----------------------------------------------------------------------
 * Pattern C: early-return via helper validation function
 * ----------------------------------------------------------------------- */
bool my_is_null_or_empty(const char *str)
{
    return (str == NULL || str[0] == '\0');
}

bool string_is_whitespace_only(const char *str)
{
    if (my_is_null_or_empty(str)) {
        return false;
    }
    for (size_t i = 0; i < strlen(str); i++) {
        if (!isspace((unsigned char)str[i])) {
            return false;
        }
    }
    return true;
}

/* -----------------------------------------------------------------------
 * Pattern D: unsigned char parameter — no overflow validation needed
 * ----------------------------------------------------------------------- */
uint8_t hex_char_to_int(unsigned char c)
{
    if (c >= '0' && c <= '9') {
        return (uint8_t)(c - '0');
    }
    if (c >= 'a' && c <= 'f') {
        return (uint8_t)(c - 'a' + 10);
    }
    return (uint8_t)(c - 'A' + 10);
}
