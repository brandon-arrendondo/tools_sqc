/*
 * Rule: DCL38-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL38-C violation
 * Description: Correct C99 flexible array member syntax
 */

#include <stdlib.h>

struct message {
    int type;
    int length;
    char payload[];  /* Correct: C99 flexible array member */
};

struct message *create_msg(int type, int len) {
    struct message *msg = malloc(
        sizeof(struct message) + sizeof(char) * len
    );
    if (msg == NULL) return NULL;
    msg->type = type;
    msg->length = len;
    return msg;
}

struct fixed_struct {
    int a;
    int b[1];  /* Not last member, not flagged */
    int c;
};
