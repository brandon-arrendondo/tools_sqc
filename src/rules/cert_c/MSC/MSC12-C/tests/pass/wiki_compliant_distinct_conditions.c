/*
 * Rule: MSC12-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MSC12-C violation
 * Pattern: Distinct conditions in if/else-if chain
 */

void openWindow(void);
void closeWindow(void);
void moveWindowToTheBackground(void);

void func(int param) {
    if (param == 1)
        openWindow();
    else if (param == 2)
        closeWindow();
    else if (param == 3)  /* Compliant: unique condition */
        moveWindowToTheBackground();
}
