# SQLite forum disclosure — unchecked sqlite3_mprintf() NULL passed to libc printf("%s") in main() (ext/session/changeset.c)

Filed by: Brandon Arrendondo (brandon.arrendondo@bissell.com)
Date drafted: 2026-08-25
Target: SQLite User Forum (sqlite.org/forum)
Status at drafting: LIVE at trunk HEAD (b1df30c735d18477630116e3d14360fd4293d6b6);
confirmed by direct inspection; checked against the forum search and not
found (apparently unreported)
Prior-art check: searched sqlite.org/forum directly for `zSQLTabName` and
related terms — not found
Artifacts: none (defect is visible directly in ext/session/changeset.c)

---

## Title

Unchecked sqlite3_mprintf() NULL fed to libc printf("%s") for zSQLTabName in main() (ext/session/changeset.c)

---

## Body

Hello,

I believe there is an unchecked-allocation NULL pointer dereference in
`main()` in `ext/session/changeset.c` (the `changeset` command-line tool
that renders a changeset as SQL), present on current trunk.

## The defect

    if( zPrevTab==0 || strcmp(zPrevTab,zTab)!=0 ){
      sqlite3_free(zPrevTab);
      sqlite3_free(zSQLTabName);
      zPrevTab = sqlite3_mprintf("%s", zTab);
      if( !isalnum(zTab[0]) || sqlite3_strglob("*[^a-zA-Z0-9]*",zTab)==0 ){
        zSQLTabName = sqlite3_mprintf("\"%w\"", zTab);
      }else{
        zSQLTabName = sqlite3_mprintf("%s", zTab);
      }
      printf("/****** Changes for table %s ***************/\n", zSQLTabName);
    }

`zSQLTabName` is assigned from `sqlite3_mprintf()` and then passed directly
as a `%s` argument to the standard C library's `printf()` — not SQLite's
own NULL-safe `%s` handling in `sqlite3_mprintf`/`sqlite3_vmprintf`, which
tolerates a NULL string argument (prints "(NULL)") — with no check that the
allocation succeeded.

## Reachability

`sqlite3_mprintf()` returns NULL on allocation failure, exactly like
`sqlite3_malloc()`. This is the tool's per-changeset-record table-name
printer, called once per distinct table name encountered while iterating a
changeset with `sqlite3changeset_next()`; a large or attacker-supplied
changeset blob together with memory pressure at the point this line runs
makes the allocation fail, and the very next statement dereferences the
resulting NULL through plain libc `printf("%s", zSQLTabName)` — glibc's
`printf("%s", NULL)` is undefined behavior and commonly segfaults (unlike
SQLite's own `%s` formatting, which explicitly handles a NULL argument).

## Suggested fix

Check both allocations before use, e.g.:

    zPrevTab = sqlite3_mprintf("%s", zTab);
    if( zPrevTab==0 ){ fprintf(stderr, "out of memory\n"); exit(1); }
    if( !isalnum(zTab[0]) || sqlite3_strglob("*[^a-zA-Z0-9]*",zTab)==0 ){
      zSQLTabName = sqlite3_mprintf("\"%w\"", zTab);
    }else{
      zSQLTabName = sqlite3_mprintf("%s", zTab);
    }
    if( zSQLTabName==0 ){ fprintf(stderr, "out of memory\n"); exit(1); }
    printf("/****** Changes for table %s ***************/\n", zSQLTabName);

## Disclosure

This was found during a static-analysis study of the SQLite source and the
analysis/triage was assisted by an AI tool; I have manually verified the
code against current trunk before reporting. Happy to provide any further
detail.

Thanks,
Brandon Arrendondo
