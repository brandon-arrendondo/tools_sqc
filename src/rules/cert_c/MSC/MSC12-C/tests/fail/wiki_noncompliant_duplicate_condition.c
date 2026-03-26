/*
 * Rule: MSC12-C
 * Source: wiki
 * Status: FAIL - Should trigger MSC12-C violation
 * Pattern: Duplicate condition in if/else-if chain
 */

void openWindow(void);
void closeWindow(void);
void moveWindowToTheBackground(void);

void func(int param) {
    if (param == 1)
        openWindow();
    else if (param == 2)
        closeWindow();
    else if (param == 1)  /* Noncompliant: duplicate condition, dead branch */
        moveWindowToTheBackground();
}
