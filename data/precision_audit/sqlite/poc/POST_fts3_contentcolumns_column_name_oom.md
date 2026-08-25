# SQLite forum disclosure — unchecked sqlite3_column_name() NULL in fts3ContentColumns() (ext/fts3/fts3.c)

Filed by: Brandon Arrendondo (brandon.arrendondo@bissell.com)
Date drafted: 2026-08-25
Target: SQLite User Forum (sqlite.org/forum)
Status at drafting: LIVE at trunk HEAD (b1df30c735d18477630116e3d14360fd4293d6b6);
confirmed by direct inspection; checked against the forum search and not
found (apparently unreported)
Prior-art check: searched sqlite.org/forum directly for `fts3ContentColumns`
and related terms — not found
Artifacts: none (defect is visible directly in ext/fts3/fts3.c)

---

## Title

Unchecked NULL from sqlite3_column_name() strlen'd in fts3ContentColumns() (ext/fts3/fts3.c)

---

## Body

Hello,

I believe there is an unchecked-NULL-return bug in `fts3ContentColumns()` in
`ext/fts3/fts3.c`, present on current trunk, at two sibling call sites in the
same function.

## The defect

    nCol = sqlite3_column_count(pStmt);
    for(i=0; i<nCol; i++){
      const char *zCol = sqlite3_column_name(pStmt, i);
      nStr += strlen(zCol) + 1;              /* site 1 */
    }

    azCol = (const char **)sqlite3_malloc64(sizeof(char *) * nCol + nStr);
    if( azCol==0 ){
      rc = SQLITE_NOMEM;
    }else{
      char *p = (char *)&azCol[nCol];
      for(i=0; i<nCol; i++){
        const char *zCol = sqlite3_column_name(pStmt, i);
        int n = (int)strlen(zCol)+1;          /* site 2 */
        memcpy(p, zCol, n);
        azCol[i] = p;
        p += n;
      }
    }

Both sites call `strlen(zCol)` on the result of `sqlite3_column_name()` with
no NULL check.

## Reachability

`sqlite3_column_name()`'s own documented contract is: "if there is no
memory available to encode the column name into UTF-8, then NULL is
returned." This is a documented OOM-triggered return, not a theoretical
one — several other call sites elsewhere in the codebase already guard it
(e.g. `sqlite3_stdio.c` output paths after this report's other sites are
fixed, and shell.c's column-name printing). Here, `azCol==0` is checked for
the surrounding array allocation but the individual column-name calls are
not, in either the sizing loop or the population loop.

## Suggested fix

Guard both sites, e.g.:

    const char *zCol = sqlite3_column_name(pStmt, i);
    if( zCol==0 ){ rc = SQLITE_NOMEM; break; }
    nStr += strlen(zCol) + 1;

and equivalently in the second loop.

## Disclosure

This was found during a static-analysis study of the SQLite source and the
analysis/triage was assisted by an AI tool; I have manually verified the
code against current trunk before reporting. Happy to provide any further
detail.

Thanks,
Brandon Arrendondo
