/*
 * Rule: WIN04-C
 * Source: wiki
 * Status: PASS - Should NOT trigger WIN04-C violation
 * Description: Function pointer encrypted with EncodePointer
 */

/* Mock Windows.h declarations for analysis */
typedef int (*FARPROC)(void);
void *EncodePointer(void *ptr);
void *DecodePointer(void *ptr);
int printf(const char *, ...);

void testcase_compliant_encrypted_fnptr(void) {
    void *log_fn = EncodePointer((void *)printf);  /* Compliant: encrypted */
    /* ... */
    int (*fn)(const char *, ...) = (int (*)(const char *, ...))DecodePointer(log_fn);

    fn("foo");
}
