/* EXP33-C: Constant condition initialization — should NOT flag.
 * Variables initialized under static const or literal constant conditions
 * that are always true. Dead-branch elimination should recognize these. */

/* Literal constant: if(1) */
void good_literal_true(void) {
    int data;
    if (1) {
        data = 5;
    }
    if (1) {
        (void)data;
    }
}

/* Constant expression: if(5==5) */
void good_const_expr(void) {
    int data;
    if (5 == 5) {
        data = 10;
    }
    if (5 == 5) {
        (void)data;
    }
}

/* Static const: if(STATIC_CONST_TRUE) */
static const int CONST_TRUE = 1;
static const int CONST_FIVE = 5;

void good_static_const_true(void) {
    int data;
    if (CONST_TRUE) {
        data = 42;
    }
    if (CONST_TRUE) {
        (void)data;
    }
}

/* Static const comparison: if(CONST_FIVE==5) */
void good_static_const_compare(void) {
    int data;
    if (CONST_FIVE == 5) {
        data = 99;
    }
    if (CONST_FIVE == 5) {
        (void)data;
    }
}

/* Static variable with known value */
static int staticTrue = 1;

void good_static_var(void) {
    int data;
    if (staticTrue) {
        data = 7;
    }
    if (staticTrue) {
        (void)data;
    }
}
