/*
 * Rule: INT34-C
 * Status: PASS - Unsigned type inferred from parameter declaration AST
 */

/* Right shift on unsigned params is safe regardless of naming convention */
void param_unsigned_int(unsigned int val, unsigned int n) {
    unsigned int result = val >> n;
}

void param_unsigned_long(unsigned long data, unsigned int shift) {
    unsigned long result = data >> shift;
}

void param_unsigned_short(unsigned short val, unsigned int n) {
    unsigned short result = val >> n;
}

/* Local unsigned variable declarations */
void local_unsigned_var(int n) {
    unsigned int val = 42;
    unsigned int result = val >> n;
}
