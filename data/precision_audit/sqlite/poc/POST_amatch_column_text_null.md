# SQLite forum disclosure — unchecked sqlite3_column_text() NULL in amatchNext() (ext/misc/amatch.c)

Filed by: Brandon Arrendondo (brandon.arrendondo@bissell.com)
Date drafted: 2026-08-25
Target: SQLite User Forum (sqlite.org/forum)
Status at drafting: LIVE at trunk HEAD (b1df30c735d18477630116e3d14360fd4293d6b6);
confirmed by direct inspection; checked against the forum search and not
found (apparently unreported)
Prior-art check: searched sqlite.org/forum directly for `amatchNext` and
related terms — not found
Artifacts: none (defect is visible directly in ext/misc/amatch.c)

---

## Title

NULL pointer dereference on a SQL NULL vocabulary entry in amatchNext() (ext/misc/amatch.c)

---

## Body

Hello,

I believe there is a NULL pointer dereference in `amatchNext()` in
`ext/misc/amatch.c` (the `approximate_match` virtual table's cursor
advance), present on current trunk, that is triggerable by ordinary SQL
data (a NULL value), not just by OOM.

## The defect

    if( zNextIn[0] && zNextIn[0]!='*' ){
      ...
      rc = sqlite3_step(p->pVCheck);
      if( rc==SQLITE_ROW ){
        zW = (const char*)sqlite3_column_text(p->pVCheck, 0);
        if( strncmp(zBuf, zW, nWord+nNextIn)==0 ){       /* site 1 */
          ...
        }
      }
      ...
    }

    while( 1 ){
      ...
      rc = sqlite3_step(p->pVCheck);
      if( rc!=SQLITE_ROW ) break;
      zW = (const char*)sqlite3_column_text(p->pVCheck, 0);
      ...
      if( strncmp(zW, zBuf, nWord)!=0 ) break;             /* site 2 */
      ...
    }

`p->pVCheck` is a statement compiled earlier in the same function as
`SELECT "<zVocabWord>" FROM "<zVocabTab>" WHERE "<zVocabWord>">=?1 ...`,
where `zVocabTab`/`zVocabWord` are the backing vocabulary table/column
supplied when the `approximate_match` virtual table is created. If that
row's selected column value is SQL NULL, `sqlite3_column_text()` returns
NULL, and both sites pass it straight to `strncmp()` with no check.

## Reachability

`sqlite3_column_text()` returns NULL either on OOM or, per its documented
contract, "if the result is NULL". Nothing in this function's query
(`WHERE "word">=?1`) excludes NULL — a NULL value in the vocabulary word
column sorts and compares under SQLite's normal NULL-ordering rules and can
be selected as a row here. No special encoding is needed to reach this: any
`approximate_match` virtual table built over a vocabulary table containing
a NULL word entry crashes on the first lookup that reaches that row.

## Suggested fix

Guard both sites, e.g.:

    zW = (const char*)sqlite3_column_text(p->pVCheck, 0);
    if( zW && strncmp(zBuf, zW, nWord+nNextIn)==0 ){
      ...
    }

and equivalently for the second site (treating a NULL `zW` as "no match",
consistent with how the rest of the loop treats a non-matching row).

## Disclosure

This was found during a static-analysis study of the SQLite source and the
analysis/triage was assisted by an AI tool; I have manually verified the
code against current trunk before reporting. Happy to provide any further
detail.

Thanks,
Brandon Arrendondo
