# SQLite forum disclosure — non-functional include guard in pcache.h

Filed by: Brandon Arrendondo (brandon.arrendondo@bissell.com)
Date drafted: 2026-08-17
Target: SQLite User Forum (sqlite.org/forum)
Status at drafting: LIVE at trunk HEAD (13bcd6f6b26a9eb3656ba8f51cad7ee29e260475);
confirmed by direct inspection; checked against the forum search and not
found (apparently unreported)
Prior-art check: searched sqlite.org/forum directly for `_PCACHE_H_` and
related terms — not found
Artifacts: none (defect is visible directly in src/pcache.h)

---

## Title

Include guard in src/pcache.h never defines its own macro

---

## Body

Hello,

`src/pcache.h` opens a standard include guard but never actually defines the
guard macro, so the guard is non-functional:

    #ifndef _PCACHE_H_
    ...
    #endif /* _PCACHE_H_ */

There is no `#define _PCACHE_H_` anywhere in the file — I checked the whole
file for `_PCACHE_H_`/`#define` and the macro is only ever referenced by the
opening `#ifndef` and the closing `#endif` comment, never defined. As
written, every `#include "pcache.h"` re-enters and re-processes the entire
body of the header, because the guard condition is always true.

## Why this is currently harmless

In the amalgamation build (the way most consumers build SQLite) this header
is only textually included once, so the missing `#define` has no observable
effect today. I'm reporting it because the guard *reads* as protecting
against double-inclusion but doesn't, which could surprise anyone building
against the split (non-amalgamation) source tree who includes `pcache.h`
from more than one translation unit's shared header, or refactors code
under the assumption the guard actually works.

## Suggested fix

Add the missing definition, mirroring every other header in the tree:

    #ifndef _PCACHE_H_
    #define _PCACHE_H_
    ...
    #endif /* _PCACHE_H_ */

## Disclosure

This was found during a static-analysis study of the SQLite source and the
analysis/triage was assisted by an AI tool; I have manually verified the
code against current trunk before reporting. Happy to provide any further
detail.

Thanks,
Brandon Arrendondo
