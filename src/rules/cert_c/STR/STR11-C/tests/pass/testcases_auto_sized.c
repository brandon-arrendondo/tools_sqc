/*
 * Rule: STR11-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR11-C violation
 * Description: Auto-sized arrays or adequate explicit bounds
 */

const char greeting[] = "hello";    /* Compiler sizes to 6 */
const char msg[] = "test";          /* Compiler sizes to 5 */
char code[10] = "US";              /* Explicit size with room */
char buffer[256] = "initial";      /* Plenty of room */
