/*
 * Rule: DCL13-C
 * Source: testcases
 * Status: PASS - Modification through local pointer alias
 */

/* Alias pointer used to write */
void modify_via_alias(int *data, int len) {
    int *cur = data;
    for (int i = 0; i < len; i++) {
        *cur = i;
        cur++;
    }
}

/* Cast alias used to modify */
void modify_via_cast_alias(void *raw) {
    int *p = (int *)raw;
    *p = 100;
}

/* Nested struct modification via alias */
struct Node { int val; struct Node *next; };
void update_via_alias(struct Node *head) {
    struct Node *cur = head;
    cur->val = 0;
}
