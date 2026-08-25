# PRE08-C delta-adjudication (task 543) — COMPLETE

Part of task 532's breakdown (17,952 unlabeled findings across 205 rules,
run 187 v0.4.258 vs the v0.4.120 baseline). This tracks the delta pass for
**PRE08-C** ("guarantee that header file names are unique") — 150 raw
unlabeled findings across mosquitto (148) and curl (2).

## Scope

- **curl**: both findings are in `lib/vtls/schannel.c`/`schannel_verify.c`
  — Windows-only, excluded per the established WIN_MAC scope split. **0
  in-scope.**
- **mosquitto**: 146 of 148 findings are in `libmosquitto.h`/`libcommon.h`
  (mosquitto's public API and shared-utility header groups); 2 are in
  `fuzzing/broker/fuzz_packet_read_base.{c,h}`, out of scope (fuzz
  harness, same exclusion as tasks 536/538/539). **146 in-scope.**

## Method: this rule is fully mechanically verifiable — no source reading needed

PRE08-C's finding is purely a **filename-string fact**: "header X shares
its first 8 characters (case-insensitive) with header Y." This is not a
code-semantics judgment call — it's directly checkable by comparing the
literal filenames. Rather than dispatching subagents to read source, every
finding's message was parsed (`Header file 'X' has the same first 8
characters ... as 'Y'`) and mechanically verified against mosquitto's real
`include/`/`lib/` file tree:

```
libmosquitto_auth.h, libmosquitto_callbacks.h, libmosquitto_connect.h,
libmosquitto_create_delete.h, libmosquitto.h, libmosquitto_helpers.h,
libmosquitto_loop.h, libmosquitto_message.h, libmosquitto_options.h,
libmosquittopp.h, libmosquitto_publish.h, libmosquitto_socks.h,
libmosquitto_subscribe.h, libmosquitto_tls.h, libmosquitto_unsubscribe.h,
libmosquitto_will.h   -- all share the literal 8-char prefix "libmosqu"

libcommon.h, libcommon_base64.h, libcommon_cjson.h, libcommon_file.h,
libcommon_memory.h, libcommon_password.h, libcommon_properties.h,
libcommon_random.h, libcommon_string.h, libcommon_time.h,
libcommon_topic.h, libcommon_utf8.h   -- all share "libcommo"
```

**All 146 in-scope findings check out as factually true**: every claimed
pair genuinely exists on disk and genuinely shares its first 8 characters
case-insensitively. Per this repo's surface-the-letter-of-the-rule
philosophy (sqc's job is to correctly surface every genuine violation as
written; whether an 8.3-filename collision matters on a modern filesystem
is a noise/applicability judgment for the user via suppression, not the
rule), all 146 are marked **TP**. mosquitto genuinely adopted a
`libmosquitto_*`/`libcommon_*` header-splitting convention (task 157's
file-at-a-time refactor) that happens to violate PRE08-C's literal
8-character-uniqueness requirement across the board.

## Outcome

23 unique `(file, line)` label rows after same-line consolidation (the 146
raw pairwise-collision messages collapse onto a smaller set of physical
lines — `ground_truth` keys on `(project, file, line)`, not per-pairwise
message), all **TP**.

Post-import measured precision for PRE08-C over the full labeled set
(`bench realworld-score 187`): **37.3%** (107 TP / 287 labeled), **100%
recall**. 40 findings remain unlabeled.

## Follow-up

None needed — no rule-logic gap. This is a case where the checker is
functioning exactly as designed and the codebase genuinely violates the
rule's letter at scale via a real naming convention. Worth noting for the
paper/README narrative if PRE08-C's precision is ever cited standalone:
its TP rate is driven almost entirely by one project's (mosquitto's)
`lib*_topic.h`-style multi-file API-splitting convention, not a
representative cross-project signal.

CSV: `data/precision_audit/mosquitto/import_delta_pre08_task543.csv`.
