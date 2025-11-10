#include <Windows.h>
 
void *log_fn = EncodePointer(printf);
/* ... */
int (*fn)(const char *, ...) = (int (*)(const char *, ...))DecodePointer(log_fn);

fn("foo");