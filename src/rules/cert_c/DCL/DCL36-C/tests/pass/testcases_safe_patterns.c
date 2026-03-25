/* Rule: DCL36-C
 * Source: testcases
 * Status: PASS - Identifiers with consistent linkage classifications
 */

/* Case 1: All external linkage, consistent */
int counter = 0;
int counter;

/* Case 2: All internal linkage, consistent */
static int mode = 1;
static int mode;

/* Case 3: Separate identifiers, no conflict */
int alpha = 10;
static int beta = 20;
int gamma;

/* Case 4: Only local variables inside function (no file-scope linkage issue) */
int main(void) {
    int x = 1;
    static int y = 2;
    return x + y;
}
