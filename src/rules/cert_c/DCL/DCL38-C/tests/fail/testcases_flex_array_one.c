/*
 * Rule: DCL38-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL38-C violation
 * Description: Flexible array member declared with [1] instead of []
 */

#include <stdlib.h>

struct message {
    int type;
    int length;
    char payload[1];  /* Violation: should be char payload[] */
};

struct message *create_msg(int type, int len) {
    struct message *msg = malloc(
        sizeof(struct message) + sizeof(char) * (len - 1)
    );
    if (msg == NULL) return NULL;
    msg->type = type;
    msg->length = len;
    return msg;
}
