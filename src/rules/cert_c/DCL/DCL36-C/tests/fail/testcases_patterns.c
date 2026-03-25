/* Rule: DCL36-C
 * Source: testcases
 * Status: FAIL - Identifiers with conflicting linkage classifications
 */

/* Case 1: static then non-static redeclaration */
static int counter = 0;
int counter;

/* Case 2: non-static then static redeclaration */
int mode;
static int mode = 1;

/* Case 3: static function-like variable then external */
static int result = 42;
int result;

int main(void) {
    return 0;
}
