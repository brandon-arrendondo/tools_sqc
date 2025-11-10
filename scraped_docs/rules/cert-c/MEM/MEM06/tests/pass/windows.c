char *secret;

secret = (char *)VirtualAlloc(0, size + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
if (!secret) {
  /* Handle error */
}

if (!VirtualLock(secret, size+1)) {
    /* Handle error */
}

/* Perform operations using secret... */

SecureZeroMemory(secret, size + 1);
VirtualUnlock(secret, size + 1);
VirtualFree(secret, 0, MEM_RELEASE);
secret = NULL;