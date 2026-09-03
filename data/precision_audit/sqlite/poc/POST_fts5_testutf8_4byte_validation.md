# SQLite forum disclosure — fts5TestUtf8() 4-byte branch: dead re-check and short advance

Filed by: Brandon Arrendondo (brandon.arrendondo@bissell.com)
Date drafted: 2026-09-03
Target: SQLite User Forum (sqlite.org/forum)
Status at drafting: LIVE at trunk HEAD (e6082d077e7e96c91fa366219ae07a1c5ab0ce70,
2026-09-02); confirmed by direct inspection of trunk and of our pinned
2026-02-24 snapshot, byte-identical in both
Prior-art check: TODO before filing — search sqlite.org/forum for
`fts5TestUtf8` and for `fts5TestTerm`
Artifacts: none (defect is visible directly in ext/fts5/fts5_index.c; no
runtime repro, see "Why the impact is small")

---

## Title

fts5TestUtf8(): 4-byte branch never validates z[i+3] and advances i by 3

---

## Body

Hello,

In `ext/fts5/fts5_index.c`, `fts5TestUtf8()`'s 4-byte branch (lines 8322-8325
at the SHA above) reads:

    }else
    if( (z[i] & 0xF8)==0xF0 ){
      if( i+3>=n || (z[i+1] & 0xC0)!=0x80 || (z[i+2] & 0xC0)!=0x80 ) return 1;
      if( (z[i+2] & 0xC0)!=0x80 ) return 1;
      i += 3;
    }else{

Two things look off, both consistent with a copy-paste from the 3-byte branch
immediately above it:

1. `z[i+3]` is never validated, while `z[i+2]` is tested twice. The second
   test is dead — it is identical to a disjunct of the condition on the line
   above, which already returned when it held.

2. `i` advances by 3 rather than 4, so the 4th byte of a valid 4-byte
   sequence is re-entered as a lead byte on the next iteration.

Item 2 has an observable effect. A continuation byte re-entered as a lead byte
matches none of the branches — `(b & 0x80)` is nonzero so not the ASCII
branch, `(b & 0xE0)` is `0x80` not `0xC0`, `(b & 0xF0)` is `0x80` not `0xE0`,
and `(b & 0xF8)` is `0x80` not `0xF0` — so it falls through to the final
`return 1`. The net result is that `fts5TestUtf8()` reports every valid 4-byte
UTF-8 sequence (U+10000 and above, e.g. emoji) as invalid.

There is no out-of-bounds read. The `i+3>=n` guard precedes the accesses, so
`z[i+1]` through `z[i+3]` are all in bounds — item 1 is a missed check, not an
unsafe one.

## Why the impact is small

`fts5TestUtf8()` gates rather than asserts. Its only caller, `fts5TestTerm()`,
uses it as a precondition for two extra checksum comparisons:

    if( p->nPendingData==0 && 0==fts5TestUtf8(zTerm, nTerm) ){
      /* ... two FTS5INDEX_QUERY_TEST_NOIDX prefix-query checksum checks ... */
    }

So a spurious "invalid" does not fail anything — it silently *skips* those two
checks. The comment above that call explains why the gate exists: a buffer
ending in a truncated sequence might be a prefix of a valid character in the
main index, "which will cause the test to fail". The bug makes the gate more
conservative than intended, so the no-index prefix checks are also skipped for
terms that contain perfectly valid 4-byte UTF-8.

That is lost coverage in a SQLITE_TEST-only path, not a correctness or safety
problem in anything shipped — `fts5TestTerm()`'s own comment notes it "is also
purely an internal test. It does not contribute to FTS functionality, or even
the integrity-check, in any way." I'm reporting it because the effect is
silent: nothing signals that the check was skipped, so the reduced coverage
would not show up as a failure.

## Suggested fix

Validate the fourth byte and advance by 4, matching the shape of the 2- and
3-byte branches:

    if( (z[i] & 0xF8)==0xF0 ){
      if( i+3>=n || (z[i+1] & 0xC0)!=0x80 || (z[i+2] & 0xC0)!=0x80
       || (z[i+3] & 0xC0)!=0x80 ) return 1;
      i += 4;
    }else{

## Disclosure

This was found during a static-analysis study of the SQLite source and the
analysis/triage was assisted by an AI tool; I have manually verified the code
against current trunk before reporting. The duplicated subcondition was
noticed by reading the function to confirm a separate analyzer finding
elsewhere in it, not reported by the tool itself. Happy to provide any further
detail.

Thanks,
Brandon Arrendondo
