#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

static FILE *target = NULL;
static FILE *(*real_fopen)(const char*, const char*) = NULL;
static FILE *(*real_fopen64)(const char*, const char*) = NULL;
static size_t (*real_fwrite)(const void*, size_t, size_t, FILE*) = NULL;

static void track(FILE *f, const char *path){
    if(f && strstr(path, ".new")){ target = f; fprintf(stderr, "[inject] tracking %s fp=%p\n", path, (void*)f); }
}
FILE *fopen(const char *path, const char *mode){
    if(!real_fopen) real_fopen = dlsym(RTLD_NEXT, "fopen");
    FILE *f = real_fopen(path, mode); track(f, path); return f;
}
FILE *fopen64(const char *path, const char *mode){
    if(!real_fopen64) real_fopen64 = dlsym(RTLD_NEXT, "fopen64");
    FILE *f = real_fopen64(path, mode); track(f, path); return f;
}
size_t fwrite(const void *ptr, size_t size, size_t n, FILE *stream){
    if(!real_fwrite) real_fwrite = dlsym(RTLD_NEXT, "fwrite");
    if(stream && stream == target){ fprintf(stderr, "[inject] failing fwrite to persistence temp\n"); errno = ENOSPC; return 0; }
    return real_fwrite(ptr, size, n, stream);
}
