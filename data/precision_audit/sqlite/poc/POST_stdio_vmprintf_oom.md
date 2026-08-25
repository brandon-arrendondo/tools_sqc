# SQLite forum disclosure — unchecked sqlite3_vmprintf() NULL in sqlite3_fprintf()/sqlite3_vfprintf() (ext/misc/sqlite3_stdio.c)

Filed by: Brandon Arrendondo (brandon.arrendondo@bissell.com)
Date drafted: 2026-08-25
Target: SQLite User Forum (sqlite.org/forum)
Status at drafting: LIVE at trunk HEAD (b1df30c735d18477630116e3d14360fd4293d6b6);
confirmed by direct inspection; checked against the forum search and not
found (apparently unreported)
Prior-art check: searched sqlite.org/forum directly for `sqlite3_vfprintf`
and `sqlite3_fprintf` — not found
Artifacts: none (defect is visible directly in ext/misc/sqlite3_stdio.c)

---

## Title

Unchecked NULL from sqlite3_vmprintf() in sqlite3_fprintf() and sqlite3_vfprintf() (ext/misc/sqlite3_stdio.c)

---

## Body

Hello,

I believe there is an unchecked-allocation NULL pointer dereference,
present at two nearly-identical sites in current trunk, in
`ext/misc/sqlite3_stdio.c`'s `sqlite3_fprintf()` and `sqlite3_vfprintf()`
work-alikes for the standard `fprintf()`/`vfprintf()`.

## The defect

    int sqlite3_fprintf(FILE *out, const char *zFormat, ...){
      int rc;
      if( UseWtextForOutput(out) ){
        char *z;
        va_list ap;
        va_start(ap, zFormat);
        z = sqlite3_vmprintf(zFormat, ap);
        va_end(ap);
        sqlite3_fputs(z, out);
        rc = (int)strlen(z);
        sqlite3_free(z);
      }else{
        ...
      }
      return rc;
    }
    int sqlite3_vfprintf(FILE *out, const char *zFormat, va_list ap){
      int rc;
      if( UseWtextForOutput(out) ){
        char *z;
        z = sqlite3_vmprintf(zFormat, ap);
        sqlite3_fputs(z, out);
        rc = (int)strlen(z);
        sqlite3_free(z);
      }else{
        ...
      }
      return rc;
    }

In both functions, `z = sqlite3_vmprintf(...)` is used directly in
`sqlite3_fputs(z, out)` and `strlen(z)` with no NULL check. `UseWtextForOutput`
gates this branch to the case of writing to the Windows command prompt in
UTF-16 mode.

## Reachability

`sqlite3_vmprintf()` is documented to return NULL on allocation failure,
exactly like `sqlite3_mprintf()`/`sqlite3_malloc()`. Any caller of these
work-alike functions writing to the Windows console under memory pressure
(a long/complex format producing a large formatted string, or a
low-memory environment) hits this. The non-Windows-console branch in both
functions correctly has no such issue because it calls libc's `vfprintf()`
directly rather than through an intermediate allocation.

## Suggested fix

Guard both sites, e.g.:

    z = sqlite3_vmprintf(zFormat, ap);
    if( z==0 ){
      rc = -1;
    }else{
      sqlite3_fputs(z, out);
      rc = (int)strlen(z);
      sqlite3_free(z);
    }

applied identically in `sqlite3_fprintf()` and `sqlite3_vfprintf()`.

## Disclosure

This was found during a static-analysis study of the SQLite source and the
analysis/triage was assisted by an AI tool; I have manually verified the
code against current trunk before reporting. Happy to provide any further
detail.

Thanks,
Brandon Arrendondo
