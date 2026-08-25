# SQLite forum disclosure — unchecked NULL SCANSTAT_EXPLAIN string in qrfEqpStats() (ext/qrf/qrf.c)

Filed by: Brandon Arrendondo (brandon.arrendondo@bissell.com)
Date drafted: 2026-08-25
Target: SQLite User Forum (sqlite.org/forum)
Status at drafting: LIVE at trunk HEAD (b1df30c735d18477630116e3d14360fd4293d6b6);
confirmed by direct inspection; checked against the forum search and not
found (apparently unreported)
Prior-art check: searched sqlite.org/forum directly for `qrfEqpStats` and
related terms — not found
Artifacts: none (defect is visible directly in ext/qrf/qrf.c and src/vdbeapi.c)

---

## Title

NULL pointer dereference on a scan with no addrExplain in qrfEqpStats() (ext/qrf/qrf.c)

---

## Body

Hello,

I believe there is a NULL pointer dereference in `qrfEqpStats()` in
`ext/qrf/qrf.c` (the `qrf` tool's `.scanstatus est`-style EQP output),
present on current trunk, reachable via a normal, non-error return from
`sqlite3_stmt_scanstatus_v2()`.

## The defect

    for(i=0; 1; i++){
      const char *z = 0;
      int n = 0;
      if( sqlite3_stmt_scanstatus_v2(pS,i,SQLITE_SCANSTAT_EXPLAIN,f,(void*)&z) ){
        break;
      }
      n = (int)strlen(z) + qrfStatsHeight(pS,i)*3;
      if( n>nWidth ) nWidth = n;
    }

`z` is passed as the output pointer for `SQLITE_SCANSTAT_EXPLAIN`, and
`strlen(z)` is called on it immediately with no NULL check, once the call
returns success (0).

## Reachability

`sqlite3_stmt_scanstatus_v2()`'s own implementation
(`src/vdbeapi.c`, `SQLITE_SCANSTAT_EXPLAIN` case) is:

    case SQLITE_SCANSTAT_EXPLAIN: {
      if( pScan->addrExplain ){
        *(const char**)pOut = aOp[ pScan->addrExplain ].p4.z;
      }else{
        *(const char**)pOut = 0;
      }
      break;
    }

When the scan's `addrExplain` is 0, this branch sets the output to NULL and
still returns 0 (success) from the outer function — the loop's `if(...)
break;` guard only catches the "no such scan index" error case, not this
one. So a query plan containing a scan step with no explain address (this
is a normal, data-dependent condition of the query plan, not corruption or
an error) makes the very next line, `strlen(z)`, dereference NULL.

## Suggested fix

Guard the NULL case before calling `strlen()`, e.g.:

    if( sqlite3_stmt_scanstatus_v2(pS,i,SQLITE_SCANSTAT_EXPLAIN,f,(void*)&z) ){
      break;
    }
    n = (int)(z ? strlen(z) : 0) + qrfStatsHeight(pS,i)*3;

## Disclosure

This was found during a static-analysis study of the SQLite source and the
analysis/triage was assisted by an AI tool; I have manually verified the
code (including the `SQLITE_SCANSTAT_EXPLAIN` implementation in
`src/vdbeapi.c` that produces the NULL) against current trunk before
reporting. Happy to provide any further detail.

Thanks,
Brandon Arrendondo
