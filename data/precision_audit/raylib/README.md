# raylib ground-truth audit — sqc v0.4.63, codebase commit 962bbfc

raylib (registry key `raylib`, checked out at `~/toolchain/raylib`) is the
suite's **structural-C99 oracle**, adopted via task 217 and adjudicated to
completion under task 227. It is the one codebase in the suite that leans
hard on the C99 constructs a tree-sitter-based analyzer is most likely to
mis-parse — compound literals (`(Vector2){ ... }`), designated initializers,
flexible array members, `restrict` — so it exists to positively validate
that sqc handles those idioms rather than to harvest bugs. Pinned to commit
`962bbfc6bfbd7a5acd08e21314fcfa161003a589`.

## Scope: raylib's own code only (`src/*.c|*.h` + `src/platforms/*.c`)

**23 files**, and the audit covered all 23 (100% coverage — see task 227's
final line). The machine-readable mirror in `data/benchmark_repos.json` is:

```json
"scope_include": ["src/*.c", "src/*.h", "src/platforms/*.c"],
"scope_exclude": ["src/external/*"]
```

which resolves at the pinned commit to:

| Pattern             | Files |
|---------------------|------:|
| `src/*.c`           |     7 |
| `src/*.h`           |     6 |
| `src/platforms/*.c` |    10 |
| **Total**           | **23** |

`ground_truth` holds 6,062 labels across 22 of these; the 23rd
(`src/config.h`) is in scope but produced no findings.

**Why the `scope_exclude` is not optional.** `bench/corpus.py:in_scope`
matches with `fnmatch`, where `*` crosses `/`. Without the exclude,
`src/*.c` alone also matches `src/external/glfw/src/init.c` and the rest of
the bundled third-party tree, giving **121** files instead of 23. The
exclude is written so that a consumer re-implementing the predicate with
true recursive-glob semantics (`*` stopping at `/`) still lands on 23 — it
is a no-op under the stricter reading and load-bearing under fnmatch.

**What is deliberately out of scope**, and was never audited:

- `src/external/` — 98 files of vendored third-party code (miniaudio, GLFW,
  stb, RGFW, cgltf, tinyobj, m3d). Not raylib's code. Task 227's
  loader-trust-boundary finding makes the split meaningful rather than
  merely conventional: raylib's *own* IQM parser does no header validation,
  so its `memcpy`/alloc sites are genuine TPs, while the OBJ/glTF/M3D paths
  delegate to these vendored parsers, which validate internally.
- `examples/` — ~230 standalone demo programs, not library code.

Sweeping either in would multiply the coverage denominator by ~16 against a
labeled corpus that never touched them.

## Result: 2.6% precision, 87.3% recall (frozen oracle `raylib-v1.0`, run #66)

23/23 files, **5,263 labels at freeze**: 137 TP / 5,126 FP / 20 FN.
Adjudicated in 9 batches, each primary pass challenged by an independent
adversarial agent before import — the same two-pass workflow as the
sqlite/curl/lua audits. Per-batch detail, calibration decisions, and the
adversary's flips are recorded in full in `todo-sqlite-cli show 227`.

The current label count (6,062) exceeds the 5,263 at freeze because of the
per-rule delta-adjudication passes that followed (`import_delta_*.csv` in
this directory, and `data/precision_audit/DELTA_*` for their write-ups).

**TP rules**: DCL37-C 100% (22), DCL00-C 29% (34), ARR38-C 56% (19, all
IQM), INT32-C 37% (17, all IQM), DCL13-C (9), CON34-C 75% (9), plus
CON33-C / ENV33-C / MSC30-C / POS30-C at 100%, FIO11-C / FIO14-C at 75%,
MEM05-C (2), INT30-C (2). Every semantic advisory scored 0%.

**The conformance question raylib was adopted to answer came back clean**:
sqc parses raymath.h's compound-literal/designated-initializer centerpiece
without crashing and raises nothing spurious *on the literals themselves*
(task 227 batch 2). The 317 FPs in that file are float-vs-integer rule
misfires, a different defect entirely.

**Dominant false-positive drivers**, each filed as its own tool-improvement
task from this audit: INT33-C on float division (228), C++-only preproc
blocks parsed as live C (229), FLP06-C misreading float arithmetic as
integer (230), DCL00-C not crediting reassignment (231), MEM30-C
free-tracking across mutually-exclusive branches (232), ARR00-C array-size
misparse from braced initializers (234). The warning-without-return FN class
is task 235; the IQM parser findings were filed upstream as task 233.

See `data/precision_audit/DELTA_*` for how to delta-adjudicate if any of
those rules change (per `CLAUDE.md`'s benchmark protocol §6) before citing a
precision/recall change for a rule this audit covers.

## History

The `## Scope` section above was written under task 709, which found that
`data/benchmark_repos.json` declared no `scope_include` for raylib at all —
so the corpus predicate read it as unrestricted and derived 368 in-scope
files against the hand-audited 23. The recorded 23 was right and the
declaration was simply absent; this file is the human-readable rationale it
was supposed to mirror, which had never been written down.
