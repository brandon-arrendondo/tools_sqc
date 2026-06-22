# Lua audit — categorical pattern brief (READ FIRST)

Lua 5.5 @ `40b76de2`, sqc v0.4.59, manifest `conf/realworld/lua-rules.toml`.
3332 findings / 60 files. Lua is a small (~19K SLOC), mature, heavily-fuzzed
interpreter. Expect **very low precision** (curl/sqlite/mosquitto ran 0.9–6%);
the enabled rules fire overwhelmingly on advisory/style items, not defects.

## What Lua is / isn't
- **Portability-C99, not structural-C99**: no designated initializers, compound
  literals, `_Bool`, `restrict`, flexible array members, or conformant array
  parameters. So `API05-C` (conformant-array-parameter advisory, 69 findings)
  is ~100% FP here — Lua never adopts that syntax. Don't credit it as a TP.
- Error handling is **longjmp/setjmp** (`luaD_throw`, `LUAI_THROW`), not
  return-code cleanup ladders. "Leak on error path" intuition from C usually
  does NOT apply — the protected-call boundary or GC reclaims.
- Allocation goes through **`luaM_*`** (`lmem.c`), which **longjmp on OOM**
  (raises `LUA_ERRMEM`). A missing NULL-check after `luaM_new`/`luaM_malloc` is
  a FP. Only raw libc `malloc`/`realloc` (rare, e.g. `lauxlib.c` default alloc,
  `loadlib.c`) warrants the unchecked-return concern.

## Expected high-volume FP rules (verify, don't rubber-stamp)
- `API00-C` (511) — non-NULL deref on internal `TValue*`/stack slots the VM
  guarantees live; ~all FP.
- `DCL02-C` (206), `DCL15-C` (149), `DCL00-C` (80) — declaration/scope/const
  advisories; static-linkage findings are FP when the symbol is the internal
  cross-TU API (declared in `l*.h`).
- `PRE00-C`(190)/`PRE12-C`(183)/`PRE01-C`(107)/`PRE02-C` — macro multiple-eval /
  parenthesization on Lua's macro-heavy headers; TP only with a real
  side-effecting call-site argument.
- `STR34-C` (124) — `char`/`signed char` sign-extension; Lua uses `lua_Byte`/
  casts deliberately. Check `lctype.h`/`ljumptab.h` context.
- `MSC37-C`(136)/`MSC04-C`(123) — fall-off-end / inconsistent style.
- `EXP33-C`(106)/`EXP34-C`(89)/`MEM30-C`(77) — the semantic mem-safety rules;
  these are where any *genuine* TP would live. Read carefully.

## Where genuine TPs / FNs actually hide (spend effort here)
Untrusted-input decode paths — give these files a real read:
- `lundump.c` — bytecode loader (attacker-controllable; classic OOB/overflow site)
- `llex.c` / `lparser.c` / `lcode.c` — source lexer/parser, jump/const-table math
- `lstrlib.c` — `str_format`, `str_pack`/`str_unpack` (size/format-driven)
- `lvm.c` — interpreter loop, arithmetic, table/string ops
- `lstring.c` / `lobject.c` — string interning, `luaO_*` numeric conversion
- stdlib C fns taking sizes/indices: `liolib.c`, `loslib.c`, `lutf8lib.c`

## Verdict discipline
- TP = a competent CERT-C reviewer would change the code. Advisory-but-correct
  idiom = FP. When the rule is technically firing but the construct is safe &
  intentional in context, that's FP with confidence high.
- Record confidence honestly; most TPs here will be low-confidence advisory
  (const-param, recursion, unchecked `fprintf`/`fwrite` return). High-confidence
  TPs should be rare and memory-relevant.
