# CERT C Rules Test Summary

## Overview

- **Total Rules:** 283
- **Implemented Rules:** 50 (17.7%)
- **Total Test Cases:** 2710
- **Average Tests per Rule:** 9.6

## Table of Contents

- [API](#category-api) (5 implemented / 9 total)
  - 🔶 [API10-C](#rule-api10c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [API05-C](#rule-api05c) - Implemented: Pass 0/4 (0.0%) [4 not run]
  - ✅ [API02-C](#rule-api02c) - Implemented: Pass 0/2 (0.0%) [2 not run]
  - 🔶 [API07-C](#rule-api07c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [API00-C](#rule-api00c) - Implemented: Pass 0/42 (0.0%) [42 not run]
  - ✅ [API01-C](#rule-api01c) - Implemented: Pass 0/3 (0.0%) [3 not run]
  - 🔶 [API09-C](#rule-api09c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [API04-C](#rule-api04c) - Implemented: Pass 0/2 (0.0%) [2 not run]
  - 🔶 [API03-C](#rule-api03c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
- [ARR](#category-arr) (8 implemented / 9 total)
  - ✅ [ARR32-C](#rule-arr32c) - Implemented: Pass 0/62 (0.0%) [62 not run]
  - ✅ [ARR36-C](#rule-arr36c) - Implemented: Pass 0/42 (0.0%) [42 not run]
  - ✅ [ARR01-C](#rule-arr01c) - Implemented: Pass 0/65 (0.0%) [65 not run]
  - ✅ [ARR30-C](#rule-arr30c) - Implemented: Pass 0/61 (0.0%) [61 not run]
  - ✅ [ARR39-C](#rule-arr39c) - Implemented: Pass 0/46 (0.0%) [46 not run]
  - ✅ [ARR38-C](#rule-arr38c) - Implemented: Pass 0/50 (0.0%) [50 not run]
  - ✅ [ARR37-C](#rule-arr37c) - Implemented: Pass 0/43 (0.0%) [43 not run]
  - 🔶 [ARR02-C](#rule-arr02c) - Not Implemented (has tests): Pass 0/82 (0.0%) [82 not run]
  - ✅ [ARR00-C](#rule-arr00c) - Implemented: Pass 0/39 (0.0%) [39 not run]
- [CON](#category-con) (0 implemented / 23 total)
  - 🔶 [CON03-C](#rule-con03c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [CON35-C](#rule-con35c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [CON08-C](#rule-con08c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - 🔶 [CON38-C](#rule-con38c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [CON37-C](#rule-con37c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [CON39-C](#rule-con39c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [CON04-C](#rule-con04c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [CON31-C](#rule-con31c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [CON36-C](#rule-con36c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [CON01-C](#rule-con01c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [CON41-C](#rule-con41c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [CON32-C](#rule-con32c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [CON34-C](#rule-con34c) - Not Implemented (has tests): Pass 0/8 (0.0%) [8 not run]
  - 🔶 [CON50-C](#rule-con50c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [CON09-C](#rule-con09c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - ⚫ [CON06-C](#rule-con06c) - Not Implemented (no tests): Pass 0/0 (N/A)
  - 🔶 [CON05-C](#rule-con05c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [CON33-C](#rule-con33c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [CON43-C](#rule-con43c) - Not Implemented (has tests): Pass 0/7 (0.0%) [7 not run]
  - 🔶 [CON02-C](#rule-con02c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [CON40-C](#rule-con40c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [CON30-C](#rule-con30c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [CON07-C](#rule-con07c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
- [DCL](#category-dcl) (4 implemented / 31 total)
  - 🔶 [DCL23-C](#rule-dcl23c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - ✅ [DCL03-C](#rule-dcl03c) - Implemented: Pass 0/3 (0.0%) [3 not run]
  - 🔶 [DCL02-C](#rule-dcl02c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - ✅ [DCL13-C](#rule-dcl13c) - Implemented: Pass 0/5 (0.0%) [5 not run]
  - 🔶 [DCL08-C](#rule-dcl08c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [DCL30-C](#rule-dcl30c) - Not Implemented (has tests): Pass 0/7 (0.0%) [7 not run]
  - 🔶 [DCL19-C](#rule-dcl19c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - 🔶 [DCL05-C](#rule-dcl05c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - 🔶 [DCL15-C](#rule-dcl15c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [DCL21-C](#rule-dcl21c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [DCL06-C](#rule-dcl06c) - Not Implemented (has tests): Pass 0/7 (0.0%) [7 not run]
  - 🔶 [DCL37-C](#rule-dcl37c) - Not Implemented (has tests): Pass 0/10 (0.0%) [10 not run]
  - 🔶 [DCL16-C](#rule-dcl16c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [DCL10-C](#rule-dcl10c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [DCL20-C](#rule-dcl20c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - ✅ [DCL01-C](#rule-dcl01c) - Implemented: Pass 0/4 (0.0%) [4 not run]
  - 🔶 [DCL11-C](#rule-dcl11c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - 🔶 [DCL39-C](#rule-dcl39c) - Not Implemented (has tests): Pass 0/8 (0.0%) [8 not run]
  - 🔶 [DCL31-C](#rule-dcl31c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - 🔶 [DCL18-C](#rule-dcl18c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [DCL22-C](#rule-dcl22c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [DCL09-C](#rule-dcl09c) - Not Implemented (has tests): Pass 0/1 (0.0%) [1 not run]
  - 🔶 [DCL12-C](#rule-dcl12c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [DCL07-C](#rule-dcl07c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - 🔶 [DCL40-C](#rule-dcl40c) - Not Implemented (has tests): Pass 0/10 (0.0%) [10 not run]
  - ✅ [DCL00-C](#rule-dcl00c) - Implemented: Pass 0/42 (0.0%) [42 not run]
  - 🔶 [DCL38-C](#rule-dcl38c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [DCL04-C](#rule-dcl04c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [DCL17-C](#rule-dcl17c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [DCL41-C](#rule-dcl41c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [DCL36-C](#rule-dcl36c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
- [ENV](#category-env) (0 implemented / 8 total)
  - 🔶 [ENV02-C](#rule-env02c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [ENV34-C](#rule-env34c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [ENV33-C](#rule-env33c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - 🔶 [ENV30-C](#rule-env30c) - Not Implemented (has tests): Pass 0/45 (0.0%) [45 not run]
  - 🔶 [ENV03-C](#rule-env03c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [ENV32-C](#rule-env32c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [ENV31-C](#rule-env31c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - 🔶 [ENV01-C](#rule-env01c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
- [ERR](#category-err) (2 implemented / 11 total)
  - 🔶 [ERR05-C](#rule-err05c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - 🔶 [ERR01-C](#rule-err01c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [ERR33-C](#rule-err33c) - Implemented: Pass 0/51 (0.0%) [51 not run]
  - 🔶 [ERR34-C](#rule-err34c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - ✅ [ERR07-C](#rule-err07c) - Implemented: Pass 0/6 (0.0%) [6 not run]
  - 🔶 [ERR02-C](#rule-err02c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - ⚫ [ERR00-C](#rule-err00c) - Not Implemented (no tests): Pass 0/0 (N/A)
  - 🔶 [ERR04-C](#rule-err04c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [ERR06-C](#rule-err06c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [ERR30-C](#rule-err30c) - Not Implemented (has tests): Pass 0/7 (0.0%) [7 not run]
  - 🔶 [ERR32-C](#rule-err32c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
- [EXP](#category-exp) (6 implemented / 31 total)
  - 🔶 [EXP36-C](#rule-exp36c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - ✅ [EXP07-C](#rule-exp07c) - Implemented: Pass 2/2 (100.0%)
  - 🔶 [EXP39-C](#rule-exp39c) - Not Implemented (has tests): Pass 0/8 (0.0%) [8 not run]
  - 🔶 [EXP46-C](#rule-exp46c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [EXP15-C](#rule-exp15c) - Implemented: Pass 0/2 (0.0%) [2 not run]
  - 🔶 [EXP47-C](#rule-exp47c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [EXP45-C](#rule-exp45c) - Not Implemented (has tests): Pass 0/8 (0.0%) [8 not run]
  - 🔶 [EXP12-C](#rule-exp12c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [EXP32-C](#rule-exp32c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [EXP00-C](#rule-exp00c) - Implemented: Pass 0/4 (0.0%) [4 not run]
  - 🔶 [EXP30-C](#rule-exp30c) - Not Implemented (has tests): Pass 0/8 (0.0%) [8 not run]
  - ✅ [EXP05-C](#rule-exp05c) - Implemented: Pass 0/4 (0.0%) [4 not run]
  - 🔶 [EXP11-C](#rule-exp11c) - Not Implemented (has tests): Pass 0/7 (0.0%) [7 not run]
  - 🔶 [EXP08-C](#rule-exp08c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - 🔶 [EXP43-C](#rule-exp43c) - Not Implemented (has tests): Pass 0/12 (0.0%) [12 not run]
  - 🔶 [EXP42-C](#rule-exp42c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [EXP09-C](#rule-exp09c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [EXP44-C](#rule-exp44c) - Not Implemented (has tests): Pass 0/8 (0.0%) [8 not run]
  - 🔶 [EXP19-C](#rule-exp19c) - Not Implemented (has tests): Pass 0/8 (0.0%) [8 not run]
  - 🔶 [EXP37-C](#rule-exp37c) - Not Implemented (has tests): Pass 0/10 (0.0%) [10 not run]
  - 🔶 [EXP20-C](#rule-exp20c) - Not Implemented (has tests): Pass 0/8 (0.0%) [8 not run]
  - 🔶 [EXP40-C](#rule-exp40c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [EXP03-C](#rule-exp03c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [EXP33-C](#rule-exp33c) - Implemented: Pass 0/50 (0.0%) [50 not run]
  - 🔶 [EXP13-C](#rule-exp13c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [EXP10-C](#rule-exp10c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [EXP02-C](#rule-exp02c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [EXP14-C](#rule-exp14c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [EXP35-C](#rule-exp35c) - Not Implemented (has tests): Pass 0/7 (0.0%) [7 not run]
  - 🔶 [EXP16-C](#rule-exp16c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - ✅ [EXP34-C](#rule-exp34c) - Implemented: Pass 0/46 (0.0%) [46 not run]
- [FIO](#category-fio) (3 implemented / 35 total)
  - 🔶 [FIO39-C](#rule-fio39c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FIO50-C](#rule-fio50c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FIO40-C](#rule-fio40c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FIO03-C](#rule-fio03c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - 🔶 [FIO42-C](#rule-fio42c) - Not Implemented (has tests): Pass 0/8 (0.0%) [8 not run]
  - ⚫ [FIO11-C](#rule-fio11c) - Not Implemented (no tests): Pass 0/0 (N/A)
  - 🔶 [FIO23-C](#rule-fio23c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [FIO46-C](#rule-fio46c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FIO41-C](#rule-fio41c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [FIO13-C](#rule-fio13c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FIO06-C](#rule-fio06c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [FIO01-C](#rule-fio01c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [FIO32-C](#rule-fio32c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [FIO08-C](#rule-fio08c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FIO38-C](#rule-fio38c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FIO44-C](#rule-fio44c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FIO21-C](#rule-fio21c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - ✅ [FIO37-C](#rule-fio37c) - Implemented: Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FIO20-C](#rule-fio20c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [FIO02-C](#rule-fio02c) - Not Implemented (has tests): Pass 0/7 (0.0%) [7 not run]
  - 🔶 [FIO10-C](#rule-fio10c) - Not Implemented (has tests): Pass 0/7 (0.0%) [7 not run]
  - 🔶 [FIO18-C](#rule-fio18c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FIO45-C](#rule-fio45c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - ⚫ [FIO14-C](#rule-fio14c) - Not Implemented (no tests): Pass 0/0 (N/A)
  - ✅ [FIO30-C](#rule-fio30c) - Implemented: Pass 0/45 (0.0%) [45 not run]
  - 🔶 [FIO09-C](#rule-fio09c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [FIO34-C](#rule-fio34c) - Implemented: Pass 0/48 (0.0%) [48 not run]
  - 🔶 [FIO24-C](#rule-fio24c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FIO19-C](#rule-fio19c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - 🔶 [FIO47-C](#rule-fio47c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FIO05-C](#rule-fio05c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - 🔶 [FIO15-C](#rule-fio15c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [FIO17-C](#rule-fio17c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FIO51-C](#rule-fio51c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [FIO22-C](#rule-fio22c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
- [FLP](#category-flp) (0 implemented / 13 total)
  - 🔶 [FLP03-C](#rule-flp03c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [FLP07-C](#rule-flp07c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [FLP34-C](#rule-flp34c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [FLP32-C](#rule-flp32c) - Not Implemented (has tests): Pass 0/8 (0.0%) [8 not run]
  - 🔶 [FLP30-C](#rule-flp30c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - ⚫ [FLP00-C](#rule-flp00c) - Not Implemented (no tests): Pass 0/0 (N/A)
  - ⚫ [FLP01-C](#rule-flp01c) - Not Implemented (no tests): Pass 0/0 (N/A)
  - 🔶 [FLP02-C](#rule-flp02c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [FLP04-C](#rule-flp04c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FLP36-C](#rule-flp36c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FLP37-C](#rule-flp37c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [FLP06-C](#rule-flp06c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [FLP05-C](#rule-flp05c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
- [INT](#category-int) (3 implemented / 23 total)
  - 🔶 [INT36-C](#rule-int36c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - 🔶 [INT09-C](#rule-int09c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - ✅ [INT30-C](#rule-int30c) - Implemented: Pass 0/47 (0.0%) [47 not run]
  - 🔶 [INT07-C](#rule-int07c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [INT17-C](#rule-int17c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [INT05-C](#rule-int05c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [INT01-C](#rule-int01c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [INT35-C](#rule-int35c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - 🔶 [INT33-C](#rule-int33c) - Not Implemented (has tests): Pass 0/44 (0.0%) [44 not run]
  - 🔶 [INT13-C](#rule-int13c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [INT04-C](#rule-int04c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - 🔶 [INT00-C](#rule-int00c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [INT08-C](#rule-int08c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [INT12-C](#rule-int12c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [INT02-C](#rule-int02c) - Not Implemented (has tests): Pass 0/9 (0.0%) [9 not run]
  - 🔶 [INT34-C](#rule-int34c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - 🔶 [INT15-C](#rule-int15c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - 🔶 [INT10-C](#rule-int10c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [INT16-C](#rule-int16c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - ✅ [INT18-C](#rule-int18c) - Implemented: Pass 0/7 (0.0%) [7 not run]
  - 🔶 [INT31-C](#rule-int31c) - Not Implemented (has tests): Pass 0/12 (0.0%) [12 not run]
  - 🔶 [INT14-C](#rule-int14c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - ✅ [INT32-C](#rule-int32c) - Implemented: Pass 0/56 (0.0%) [56 not run]
- [MEM](#category-mem) (3 implemented / 17 total)
  - ✅ [MEM33-C](#rule-mem33c) - Implemented: Pass 0/46 (0.0%) [46 not run]
  - 🔶 [MEM10-C](#rule-mem10c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [MEM31-C](#rule-mem31c) - Implemented: Pass 0/100 (0.0%) [100 not run]
  - 🔶 [MEM11-C](#rule-mem11c) - Not Implemented (has tests): Pass 0/1 (0.0%) [1 not run]
  - 🔶 [MEM01-C](#rule-mem01c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [MEM34-C](#rule-mem34c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [MEM06-C](#rule-mem06c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [MEM36-C](#rule-mem36c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [MEM04-C](#rule-mem04c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [MEM07-C](#rule-mem07c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [MEM02-C](#rule-mem02c) - Not Implemented (has tests): Pass 0/10 (0.0%) [10 not run]
  - 🔶 [MEM12-C](#rule-mem12c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [MEM00-C](#rule-mem00c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [MEM03-C](#rule-mem03c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - ✅ [MEM30-C](#rule-mem30c) - Implemented: Pass 0/48 (0.0%) [48 not run]
  - 🔶 [MEM35-C](#rule-mem35c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [MEM05-C](#rule-mem05c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
- [MSC](#category-msc) (1 implemented / 8 total)
  - 🔶 [MSC30-C](#rule-msc30c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [MSC38-C](#rule-msc38c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [MSC41-C](#rule-msc41c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [MSC33-C](#rule-msc33c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [MSC32-C](#rule-msc32c) - Implemented: Pass 0/6 (0.0%) [6 not run]
  - 🔶 [MSC37-C](#rule-msc37c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [MSC39-C](#rule-msc39c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [MSC40-C](#rule-msc40c) - Not Implemented (has tests): Pass 0/7 (0.0%) [7 not run]
- [POS](#category-pos) (4 implemented / 20 total)
  - 🔶 [POS05-C](#rule-pos05c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [POS47-C](#rule-pos47c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [POS52-C](#rule-pos52c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [POS34-C](#rule-pos34c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [POS01-C](#rule-pos01c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [POS04-C](#rule-pos04c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [POS38-C](#rule-pos38c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - ✅ [POS37-C](#rule-pos37c) - Implemented: Pass 0/3 (0.0%) [3 not run]
  - 🔶 [POS44-C](#rule-pos44c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [POS51-C](#rule-pos51c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [POS02-C](#rule-pos02c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [POS48-C](#rule-pos48c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [POS36-C](#rule-pos36c) - Implemented: Pass 0/2 (0.0%) [2 not run]
  - 🔶 [POS35-C](#rule-pos35c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - ✅ [POS54-C](#rule-pos54c) - Implemented: Pass 0/2 (0.0%) [2 not run]
  - 🔶 [POS50-C](#rule-pos50c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - 🔶 [POS39-C](#rule-pos39c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [POS53-C](#rule-pos53c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [POS30-C](#rule-pos30c) - Implemented: Pass 0/3 (0.0%) [3 not run]
  - 🔶 [POS49-C](#rule-pos49c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
- [PRE](#category-pre) (4 implemented / 16 total)
  - ✅ [PRE30-C](#rule-pre30c) - Implemented: Pass 0/42 (0.0%) [42 not run]
  - 🔶 [PRE12-C](#rule-pre12c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [PRE11-C](#rule-pre11c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - 🔶 [PRE06-C](#rule-pre06c) - Not Implemented (has tests): Pass 0/1 (0.0%) [1 not run]
  - 🔶 [PRE10-C](#rule-pre10c) - Not Implemented (has tests): Pass 0/7 (0.0%) [7 not run]
  - 🔶 [PRE02-C](#rule-pre02c) - Not Implemented (has tests): Pass 0/8 (0.0%) [8 not run]
  - 🔶 [PRE07-C](#rule-pre07c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [PRE01-C](#rule-pre01c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - ✅ [PRE31-C](#rule-pre31c) - Implemented: Pass 0/48 (0.0%) [48 not run]
  - 🔶 [PRE05-C](#rule-pre05c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - 🔶 [PRE08-C](#rule-pre08c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [PRE00-C](#rule-pre00c) - Not Implemented (has tests): Pass 0/8 (0.0%) [8 not run]
  - ✅ [PRE32-C](#rule-pre32c) - Implemented: Pass 0/42 (0.0%) [42 not run]
  - 🔶 [PRE04-C](#rule-pre04c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [PRE13-C](#rule-pre13c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - ✅ [PRE09-C](#rule-pre09c) - Implemented: Pass 0/2 (0.0%) [2 not run]
- [SIG](#category-sig) (2 implemented / 7 total)
  - 🔶 [SIG00-C](#rule-sig00c) - Not Implemented (has tests): Pass 0/44 (0.0%) [44 not run]
  - 🔶 [SIG01-C](#rule-sig01c) - Not Implemented (has tests): Pass 0/47 (0.0%) [47 not run]
  - 🔶 [SIG34-C](#rule-sig34c) - Not Implemented (has tests): Pass 0/44 (0.0%) [44 not run]
  - ✅ [SIG31-C](#rule-sig31c) - Implemented: Pass 0/43 (0.0%) [43 not run]
  - 🔶 [SIG02-C](#rule-sig02c) - Not Implemented (has tests): Pass 0/46 (0.0%) [46 not run]
  - 🔶 [SIG35-C](#rule-sig35c) - Not Implemented (has tests): Pass 0/43 (0.0%) [43 not run]
  - ✅ [SIG30-C](#rule-sig30c) - Implemented: Pass 0/47 (0.0%) [47 not run]
- [STR](#category-str) (3 implemented / 16 total)
  - 🔶 [STR05-C](#rule-str05c) - Not Implemented (has tests): Pass 0/4 (0.0%) [4 not run]
  - 🔶 [STR10-C](#rule-str10c) - Not Implemented (has tests): Pass 0/3 (0.0%) [3 not run]
  - 🔶 [STR37-C](#rule-str37c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [STR38-C](#rule-str38c) - Implemented: Pass 0/5 (0.0%) [5 not run]
  - 🔶 [STR09-C](#rule-str09c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [STR30-C](#rule-str30c) - Implemented: Pass 0/46 (0.0%) [46 not run]
  - 🔶 [STR06-C](#rule-str06c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [STR04-C](#rule-str04c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [STR11-C](#rule-str11c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [STR32-C](#rule-str32c) - Not Implemented (has tests): Pass 0/7 (0.0%) [7 not run]
  - 🔶 [STR03-C](#rule-str03c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [STR00-C](#rule-str00c) - Not Implemented (has tests): Pass 0/40 (0.0%) [40 not run]
  - 🔶 [STR02-C](#rule-str02c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - ⚫ [STR01-C](#rule-str01c) - Not Implemented (no tests): Pass 0/0 (N/A)
  - 🔶 [STR34-C](#rule-str34c) - Not Implemented (has tests): Pass 0/5 (0.0%) [5 not run]
  - ✅ [STR31-C](#rule-str31c) - Implemented: Pass 0/58 (0.0%) [58 not run]
- [WIN](#category-win) (2 implemented / 6 total)
  - 🔶 [WIN30-C](#rule-win30c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [WIN00-C](#rule-win00c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - 🔶 [WIN03-C](#rule-win03c) - Not Implemented (has tests): Pass 0/6 (0.0%) [6 not run]
  - 🔶 [WIN04-C](#rule-win04c) - Not Implemented (has tests): Pass 0/2 (0.0%) [2 not run]
  - ✅ [WIN02-C](#rule-win02c) - Implemented: Pass 0/2 (0.0%) [2 not run]
  - ✅ [WIN01-C](#rule-win01c) - Implemented: Pass 0/2 (0.0%) [2 not run]

## Category: API

<a id="category-api"></a>

**Implementation Status:** 5 / 9 rules (55.6%)

### 🔶 API10-C - Not Implemented (has tests)

<a id="rule-api10c"></a>

**Title:** APIs should have security options enabled by default

**Description:** APIS should have security options enabled by default– for example, having best
practice cipher suites enabled by default (something that changes over time)
while disabling out-of-favor cipher suites by default. When interface stability
is also a design requirement, an interface can meet both goals by providing off-
by-default options that produce stable behavior, such
asTLS_ENABLE_Y2015_BEST_PRACTICE_CIPHERS_ONLY. If the caller of this API in this
noncompliant example doesn't understand what the options mean, they will pass 0
orTLS_DEFAULT_OPTIONSand get a connection vulnerable to man-in-the-middle
attacks and using old versions of TLS. int tls_connect_by_name(const char *host,
int port, int option_bitmask); #define TLS_DEFAULT_OPTIONS 0 #define
TLS_VALIDATE_HOST 0x0001 #define TLS_DISABLE_V1_0 0x0002 #define
TLS_DISABLE_V1_1 0x0004

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_api10_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_api10_c_pass_wiki_compliant_1`

---

### ✅ API05-C - Implemented

<a id="rule-api05c"></a>

**Title:** Use conformant array parameters

**Description:** Traditionally, C arrays are declared with an index that is either a fixed
constant or empty. An array with a fixed constant index indicates to the
compiler how much space to reserve for the array. An array declaration with an
empty index is an incomplete type and indicates that the variable references a
pointer to an array of indeterminate size. The termconformant array
parametercomes from Pascal; it refers to a function argument that is an array
whose size is specified in the function declaration.Since C99, C has supported
conformant array parameters by permitting array parameter declarations to use
extended syntax.Subclause 6.7.6.2, paragraph 1, of C11 [ISO/IEC 9899:2011]
summarizes the array index syntax extensions: Consequently, an array declaration
that serves as a function argument may have an index that is a variable or an
expression. The array argument is demoted to a pointer and is consequently not a
variable length array (VLA). Conformant array parameters can be used by
developers to indicate the expected bounds of the array. This information may be
used by compilers, or it may be ignored. However, such declarations are useful
to developers because they serve to document relationships between array sizes
and pointers. This information can also be used bystatic analysistools to
diagnose potential defects.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_api05_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_api05_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_api_change.c` → `test_api05_c_pass_wiki_api_change`
- ⏭️ NOT RUN `wiki_gcc.c` → `test_api05_c_pass_wiki_gcc`

---

### ✅ API02-C - Implemented

<a id="rule-api02c"></a>

**Title:** Functions that read or write to or from an array should take an argument to specify the source or target size

**Description:** Functions that have an array as a parameter should also have an additional
parameter that indicates the maximum number of elements that can be stored in
the array. That parameter is required to ensure that the function does not
access memory outside the bounds of the array and adversely influence program
execution. It should be present for each array parameter (in other words, the
existence of each array parameter implies the existence of a complementary
parameter that represents the maximum number of elements in the array). Note
thatarrayis used in this recommendation to mean array, string, or any other
pointer to a contiguous block of memory in which one or more elements of a
particular type are (potentially) stored. These terms are all effectively
synonymous and represent the same potential for error. Also note that this
recommendation suggests the parameter accompanying array parameters indicates
the maximum number of elements that can be stored in the array, not the maximum
size, in bytes, of the array, because

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_api02_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_api02_c_pass_wiki_compliant_1`

---

### 🔶 API07-C - Not Implemented (has tests)

<a id="rule-api07c"></a>

**Title:** Enforce type safety

**Description:** Upon return, functions should guarantee that any object returned by the
function, or any modified value referenced by a pointer argument, is a valid
object of function return type or argument type. Otherwise, type errors can
occur in the program. A good example is the null-terminated byte string type in
C. If a string lacks the terminating null character, the program may be tricked
into accessing storage after the string as legitimate data. A program may, as a
result, process a string it should not process, which might be asecurity flawin
itself. It may also cause the program to abort, which might be adenial-of-
service attack. The emphasis of this recommendation is to avoid producing
unterminated strings; it does not address processing of already existing
unterminated strings. However, by preventing the creation of unterminated
strings, the need to process them is greatly lessened.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_api07_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_strncpy_s_c11_annex_k.c` → `test_api07_c_pass_wiki_strncpy_s_c11_annex_k`

---

### ✅ API00-C - Implemented

<a id="rule-api00c"></a>

**Title:** Functions should validate their parameters

**Description:** Redundant testing by caller and by callee as a style ofdefensive programmingis
largely discredited in the C and C++ communities, the main problem being
performance. The usual discipline in C and C++ is to requirevalidationon only
one side of each interface. Requiring the caller to validate arguments can
result in faster code because the caller may understand certain invariants that
prevent invalid values from being passed. Requiring the callee to validate
arguments allows the validation code to be encapsulated in one location,
reducing the size of the code and making it more likely that these checks are
performed in a consistent and correct fashion. For safety and security reasons,
this standard recommends that the called function validate its parameters.
Validity checks allow the function to survive at least some forms of improper
usage, enabling an application using the function to likewise survive. Validity
checks can also simplify the task of determining the condition that caused the
invalid parameter.

**Test Coverage:** 42 tests (31 fail, 11 pass)

**Test Results:** 0/42 passed (0.0%), 42 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_audio_processing_unchecked.c` → `test_api00_c_fail_testcases_audio_processing_unchecked`
- ⏭️ NOT RUN `testcases_cache_operations_unsafe.c` → `test_api00_c_fail_testcases_cache_operations_unsafe`
- ⏭️ NOT RUN `testcases_compression_functions_unchecked.c` → `test_api00_c_fail_testcases_compression_functions_unchecked`
- ⏭️ NOT RUN `testcases_config_parser_unchecked.c` → `test_api00_c_fail_testcases_config_parser_unchecked`
- ⏭️ NOT RUN `testcases_conversion_functions_unsafe.c` → `test_api00_c_fail_testcases_conversion_functions_unsafe`
- ⏭️ NOT RUN `testcases_crypto_operations_unsafe.c` → `test_api00_c_fail_testcases_crypto_operations_unsafe`
- ⏭️ NOT RUN `testcases_data_structure_unchecked.c` → `test_api00_c_fail_testcases_data_structure_unchecked`
- ⏭️ NOT RUN `testcases_database_operations_unsafe.c` → `test_api00_c_fail_testcases_database_operations_unsafe`
- ⏭️ NOT RUN `testcases_division_by_zero.c` → `test_api00_c_fail_testcases_division_by_zero`
- ⏭️ NOT RUN `testcases_file_operations_unchecked.c` → `test_api00_c_fail_testcases_file_operations_unchecked`
- ⏭️ NOT RUN `testcases_graphics_operations_unchecked.c` → `test_api00_c_fail_testcases_graphics_operations_unchecked`
- ⏭️ NOT RUN `testcases_image_processing_unsafe.c` → `test_api00_c_fail_testcases_image_processing_unsafe`
- ⏭️ NOT RUN `testcases_integer_overflow_unchecked.c` → `test_api00_c_fail_testcases_integer_overflow_unchecked`
- ⏭️ NOT RUN `testcases_invalid_array_bounds.c` → `test_api00_c_fail_testcases_invalid_array_bounds`
- ⏭️ NOT RUN `testcases_json_parser_unchecked.c` → `test_api00_c_fail_testcases_json_parser_unchecked`
- ⏭️ NOT RUN `testcases_log_operations_unchecked.c` → `test_api00_c_fail_testcases_log_operations_unchecked`
- ⏭️ NOT RUN `testcases_math_operations_unsafe.c` → `test_api00_c_fail_testcases_math_operations_unsafe`
- ⏭️ NOT RUN `testcases_memory_operations_unsafe.c` → `test_api00_c_fail_testcases_memory_operations_unsafe`
- ⏭️ NOT RUN `testcases_network_operations_unsafe.c` → `test_api00_c_fail_testcases_network_operations_unsafe`
- ⏭️ NOT RUN `testcases_process_operations_unchecked.c` → `test_api00_c_fail_testcases_process_operations_unchecked`
- ⏭️ NOT RUN `testcases_protocol_handlers_unchecked.c` → `test_api00_c_fail_testcases_protocol_handlers_unchecked`
- ⏭️ NOT RUN `testcases_regex_operations_unchecked.c` → `test_api00_c_fail_testcases_regex_operations_unchecked`
- ⏭️ NOT RUN `testcases_signal_handling_unsafe.c` → `test_api00_c_fail_testcases_signal_handling_unsafe`
- ⏭️ NOT RUN `testcases_sorting_algorithms_unsafe.c` → `test_api00_c_fail_testcases_sorting_algorithms_unsafe`
- ⏭️ NOT RUN `testcases_thread_operations_unchecked.c` → `test_api00_c_fail_testcases_thread_operations_unchecked`
- ⏭️ NOT RUN `testcases_time_operations_unchecked.c` → `test_api00_c_fail_testcases_time_operations_unchecked`
- ⏭️ NOT RUN `testcases_unchecked_null_pointer.c` → `test_api00_c_fail_testcases_unchecked_null_pointer`
- ⏭️ NOT RUN `testcases_unvalidated_string_operations.c` → `test_api00_c_fail_testcases_unvalidated_string_operations`
- ⏭️ NOT RUN `testcases_url_operations_unsafe.c` → `test_api00_c_fail_testcases_url_operations_unsafe`
- ⏭️ NOT RUN `testcases_xml_parser_unsafe.c` → `test_api00_c_fail_testcases_xml_parser_unsafe`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_api00_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_comprehensive_api_validation.c` → `test_api00_c_pass_testcases_comprehensive_api_validation`
- ⏭️ NOT RUN `testcases_defensive_data_structures.c` → `test_api00_c_pass_testcases_defensive_data_structures`
- ⏭️ NOT RUN `testcases_defensive_parsing_functions.c` → `test_api00_c_pass_testcases_defensive_parsing_functions`
- ⏭️ NOT RUN `testcases_error_handling_patterns.c` → `test_api00_c_pass_testcases_error_handling_patterns`
- ⏭️ NOT RUN `testcases_robust_file_operations.c` → `test_api00_c_pass_testcases_robust_file_operations`
- ⏭️ NOT RUN `testcases_safe_network_operations.c` → `test_api00_c_pass_testcases_safe_network_operations`
- ⏭️ NOT RUN `testcases_safe_pointer_validation.c` → `test_api00_c_pass_testcases_safe_pointer_validation`
- ⏭️ NOT RUN `testcases_secure_memory_management.c` → `test_api00_c_pass_testcases_secure_memory_management`
- ⏭️ NOT RUN `testcases_validated_math_operations.c` → `test_api00_c_pass_testcases_validated_math_operations`
- ⏭️ NOT RUN `testcases_validated_string_processing.c` → `test_api00_c_pass_testcases_validated_string_processing`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_api00_c_pass_wiki_compliant_1`

---

### ✅ API01-C - Implemented

<a id="rule-api01c"></a>

**Title:** Avoid laying out strings in memory directly before sensitive data

**Description:** Strings (both character and wide-character) are often subject to buffer
overflows, which will overwrite the memory immediately past the string. Many
rules warn against buffer overflows, includingSTR31-C. Guarantee that storage
for strings has sufficient space for character data and the null terminator.
Sometimes the danger of buffer overflows can be minimized by ensuring that
arranging memory such that data that might be corrupted by a buffer overflow is
not sensitive. This noncompliant code example stores a set of strings using a
linked list: const size_t String_Size = 20; struct node_s { char
name[String_Size]; struct node_s* next; }

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_api01_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_api01_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_api01_c_pass_wiki_compliant_2`

---

### 🔶 API09-C - Not Implemented (has tests)

<a id="rule-api09c"></a>

**Title:** Compatible values should have the same type

**Description:** Make sure compatible values have the same type. For example, when the return
value of one function is used as an argument to another function, make sure they
are the same type. Ensuring compatible values have the same type allows the
return value to be passed as an argument to the related function without
conversion, reducing the potential for conversion errors. A source of potential
errors may be traced to POSIX's tendency to overload return codes, using −1 to
indicate an error condition but 0 for success and positive values as a result
indicator (seeERR02-C. Avoid in-band error indicators). A good example is
theread()system call. This leads to a natural mixing of unsigned and signed
quantities, potentially leading to conversion errors. OpenSSH performs most I/O
calls through a "retry on interrupt" function,atomicio(). The following is a
slightly simplified version ofatomicio.c, v 1.12 2003/07/31. The functionf()is
eitherread()orvwrite():

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_api09_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_api09_c_pass_wiki_compliant_1`

---

### ✅ API04-C - Implemented

<a id="rule-api04c"></a>

**Title:** Provide a consistent and usable error-checking mechanism

**Description:** Functions should provide consistent and usable error-checking mechanisms.
Complex interfaces are sometimes ignored by programmers, resulting in code that
is not error checked. Inconsistent interfaces are frequently misused and
difficult to use, resulting in lower-quality code and higher development costs.
Thestrlcpy()function copies a null-terminated source string to a destination
array. It is designed to be a safer, more consistent, and less error-prone
replacement forstrcpy(). Thestrlcpy()function returns the total length of the
string it tried to create (the length of the source string).

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_strlcpy.c` → `test_api04_c_fail_wiki_strlcpy`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_strcpy_m.c` → `test_api04_c_pass_wiki_strcpy_m`

---

### 🔶 API03-C - Not Implemented (has tests)

<a id="rule-api03c"></a>

**Title:** Create consistent interfaces and capabilities across related functions

**Description:** Related functions, such as those that make up a library, should provide
consistent and usable interfaces. Ralph Waldo Emerson said, "A foolish
consistency is the hobgoblin of little minds," but inconsistencies in functional
interfaces or behavior can lead to erroneous use, so we understand this to be a
"wise consistency." One aspect of providing a consistent interface is to provide
a consistent and usable error-checking mechanism. For more information,
seeAPI04-C. Provide a consistent and usable error-checking mechanism. It is not
necessary to go beyond the standard C library to find examples of inconsistent
interfaces: the standard library is a fusion of multiple libraries with various
styles and levels of rigor. For example, thefputs()defined in the C Standard,
subclause 7.21.7.4, is closely related to thefprintf()defined in subclause
7.21.6.1. However,fputs()'s file handle is at the end, andfprintf()'s is at the
beginning, as shown by their function declarations: int fputs(const char *
restrict s, FILE * restrict stream); int fprintf(FILE * restrict stream, const
char * restrict format, ...);

**Test Coverage:** 3 tests (2 fail, 1 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_interface.c` → `test_api03_c_fail_wiki_interface`
- ⏭️ NOT RUN `wiki_interface_2.c` → `test_api03_c_fail_wiki_interface_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_interface.c` → `test_api03_c_pass_wiki_interface`

---

## Category: ARR

<a id="category-arr"></a>

**Implementation Status:** 8 / 9 rules (88.9%)

### ✅ ARR32-C - Implemented

<a id="rule-arr32c"></a>

**Title:** Ensure size arguments for variable length arrays are in a valid range

**Description:** Variable length arrays (VLAs), a conditionally supported language feature, are
essentially the same as traditional C arrays except that they are declared with
a size that is not a constant integer expression and can be declared only at
block scope or function prototype scope and no linkage. When supported, a
variable length array can be declared { /* Block scope */ char vla[size]; }
where the integer expressionsizeand the declaration ofvlaare both evaluated at
runtime. If the size argument supplied to a variable length array is not a
positive integer value, the behavior is undefined. (Seeundefined behavior 72.)
Additionally, if the magnitude of the argument is excessive, the program may
behave in an unexpected way. An attacker may be able to leverage this behavior
to overwrite critical program data [Griffiths 2006].The programmer must ensure
that size arguments to variable length arrays, especially those derived from
untrusted data, are in a valid range.

**Test Coverage:** 62 tests (40 fail, 22 pass)

**Test Results:** 0/62 passed (0.0%), 62 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_excessive_size_vla.c` → `test_arr32_c_fail_testcases_excessive_size_vla`
- ⏭️ NOT RUN `testcases_fail_case_1.c` → `test_arr32_c_fail_testcases_fail_case_1`
- ⏭️ NOT RUN `testcases_fail_case_10.c` → `test_arr32_c_fail_testcases_fail_case_10`
- ⏭️ NOT RUN `testcases_fail_case_11.c` → `test_arr32_c_fail_testcases_fail_case_11`
- ⏭️ NOT RUN `testcases_fail_case_12.c` → `test_arr32_c_fail_testcases_fail_case_12`
- ⏭️ NOT RUN `testcases_fail_case_13.c` → `test_arr32_c_fail_testcases_fail_case_13`
- ⏭️ NOT RUN `testcases_fail_case_14.c` → `test_arr32_c_fail_testcases_fail_case_14`
- ⏭️ NOT RUN `testcases_fail_case_15.c` → `test_arr32_c_fail_testcases_fail_case_15`
- ⏭️ NOT RUN `testcases_fail_case_16.c` → `test_arr32_c_fail_testcases_fail_case_16`
- ⏭️ NOT RUN `testcases_fail_case_17.c` → `test_arr32_c_fail_testcases_fail_case_17`
- ⏭️ NOT RUN `testcases_fail_case_18.c` → `test_arr32_c_fail_testcases_fail_case_18`
- ⏭️ NOT RUN `testcases_fail_case_19.c` → `test_arr32_c_fail_testcases_fail_case_19`
- ⏭️ NOT RUN `testcases_fail_case_2.c` → `test_arr32_c_fail_testcases_fail_case_2`
- ⏭️ NOT RUN `testcases_fail_case_20.c` → `test_arr32_c_fail_testcases_fail_case_20`
- ⏭️ NOT RUN `testcases_fail_case_21.c` → `test_arr32_c_fail_testcases_fail_case_21`
- ⏭️ NOT RUN `testcases_fail_case_22.c` → `test_arr32_c_fail_testcases_fail_case_22`
- ⏭️ NOT RUN `testcases_fail_case_23.c` → `test_arr32_c_fail_testcases_fail_case_23`
- ⏭️ NOT RUN `testcases_fail_case_24.c` → `test_arr32_c_fail_testcases_fail_case_24`
- ⏭️ NOT RUN `testcases_fail_case_25.c` → `test_arr32_c_fail_testcases_fail_case_25`
- ⏭️ NOT RUN `testcases_fail_case_26.c` → `test_arr32_c_fail_testcases_fail_case_26`
- ⏭️ NOT RUN `testcases_fail_case_27.c` → `test_arr32_c_fail_testcases_fail_case_27`
- ⏭️ NOT RUN `testcases_fail_case_28.c` → `test_arr32_c_fail_testcases_fail_case_28`
- ⏭️ NOT RUN `testcases_fail_case_29.c` → `test_arr32_c_fail_testcases_fail_case_29`
- ⏭️ NOT RUN `testcases_fail_case_3.c` → `test_arr32_c_fail_testcases_fail_case_3`
- ⏭️ NOT RUN `testcases_fail_case_30.c` → `test_arr32_c_fail_testcases_fail_case_30`
- ⏭️ NOT RUN `testcases_fail_case_4.c` → `test_arr32_c_fail_testcases_fail_case_4`
- ⏭️ NOT RUN `testcases_fail_case_5.c` → `test_arr32_c_fail_testcases_fail_case_5`
- ⏭️ NOT RUN `testcases_fail_case_6.c` → `test_arr32_c_fail_testcases_fail_case_6`
- ⏭️ NOT RUN `testcases_fail_case_7.c` → `test_arr32_c_fail_testcases_fail_case_7`
- ⏭️ NOT RUN `testcases_fail_case_8.c` → `test_arr32_c_fail_testcases_fail_case_8`
- ⏭️ NOT RUN `testcases_fail_case_9.c` → `test_arr32_c_fail_testcases_fail_case_9`
- ⏭️ NOT RUN `testcases_integer_overflow_size.c` → `test_arr32_c_fail_testcases_integer_overflow_size`
- ⏭️ NOT RUN `testcases_negative_size_vla.c` → `test_arr32_c_fail_testcases_negative_size_vla`
- ⏭️ NOT RUN `testcases_no_bounds_checking.c` → `test_arr32_c_fail_testcases_no_bounds_checking`
- ⏭️ NOT RUN `testcases_stack_exhaustion.c` → `test_arr32_c_fail_testcases_stack_exhaustion`
- ⏭️ NOT RUN `testcases_unvalidated_calculation.c` → `test_arr32_c_fail_testcases_unvalidated_calculation`
- ⏭️ NOT RUN `testcases_unvalidated_user_input.c` → `test_arr32_c_fail_testcases_unvalidated_user_input`
- ⏭️ NOT RUN `testcases_zero_size_vla.c` → `test_arr32_c_fail_testcases_zero_size_vla`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_arr32_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_sizeof.c` → `test_arr32_c_fail_wiki_sizeof`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_bounds_checking_before_vla.c` → `test_arr32_c_pass_testcases_bounds_checking_before_vla`
- ⏭️ NOT RUN `testcases_dynamic_allocation_alternative.c` → `test_arr32_c_pass_testcases_dynamic_allocation_alternative`
- ⏭️ NOT RUN `testcases_error_handling_invalid_sizes.c` → `test_arr32_c_pass_testcases_error_handling_invalid_sizes`
- ⏭️ NOT RUN `testcases_multidimensional_vla_safe.c` → `test_arr32_c_pass_testcases_multidimensional_vla_safe`
- ⏭️ NOT RUN `testcases_pass_case_1.c` → `test_arr32_c_pass_testcases_pass_case_1`
- ⏭️ NOT RUN `testcases_pass_case_10.c` → `test_arr32_c_pass_testcases_pass_case_10`
- ⏭️ NOT RUN `testcases_pass_case_2.c` → `test_arr32_c_pass_testcases_pass_case_2`
- ⏭️ NOT RUN `testcases_pass_case_3.c` → `test_arr32_c_pass_testcases_pass_case_3`
- ⏭️ NOT RUN `testcases_pass_case_4.c` → `test_arr32_c_pass_testcases_pass_case_4`
- ⏭️ NOT RUN `testcases_pass_case_5.c` → `test_arr32_c_pass_testcases_pass_case_5`
- ⏭️ NOT RUN `testcases_pass_case_6.c` → `test_arr32_c_pass_testcases_pass_case_6`
- ⏭️ NOT RUN `testcases_pass_case_7.c` → `test_arr32_c_pass_testcases_pass_case_7`
- ⏭️ NOT RUN `testcases_pass_case_8.c` → `test_arr32_c_pass_testcases_pass_case_8`
- ⏭️ NOT RUN `testcases_pass_case_9.c` → `test_arr32_c_pass_testcases_pass_case_9`
- ⏭️ NOT RUN `testcases_reasonable_size_limits.c` → `test_arr32_c_pass_testcases_reasonable_size_limits`
- ⏭️ NOT RUN `testcases_safe_calculated_sizes.c` → `test_arr32_c_pass_testcases_safe_calculated_sizes`
- ⏭️ NOT RUN `testcases_safe_vla_with_limits.c` → `test_arr32_c_pass_testcases_safe_vla_with_limits`
- ⏭️ NOT RUN `testcases_size_calculation_validation.c` → `test_arr32_c_pass_testcases_size_calculation_validation`
- ⏭️ NOT RUN `testcases_validated_size_input.c` → `test_arr32_c_pass_testcases_validated_size_input`
- ⏭️ NOT RUN `testcases_vla_parameter_validation.c` → `test_arr32_c_pass_testcases_vla_parameter_validation`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_arr32_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_sizeof.c` → `test_arr32_c_pass_wiki_sizeof`

---

### ✅ ARR36-C - Implemented

<a id="rule-arr36c"></a>

**Title:** Do not subtract or compare two pointers that do not refer to the same array

**Description:** When two pointers are subtracted, both must point to elements of the same array
object or just one past the last element of the array object (C Standard, 6.5.7
[ISO/IEC 9899:2024]); the result is the difference of the subscripts of the two
array elements. Otherwise, the operation isundefined behavior. (Seeundefined
behavior 45.) Similarly, comparing pointers using the relational
operators<,<=,>=, and>gives the positions of the pointers relative to each
other. Subtracting or comparing pointers that do not refer to the same array is
undefined behavior. (Seeundefined behavior 45andundefined behavior 50.)
Comparing pointers using the equality operators==and!=has well-defined semantics
regardless of whether or not either of the pointers is null, points into the
same object, or points one past the last element of an array object or function.

**Test Coverage:** 42 tests (31 fail, 11 pass)

**Test Results:** 0/42 passed (0.0%), 42 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_adjacent_vars.c` → `test_arr36_c_fail_testcases_adjacent_vars`
- ⏭️ NOT RUN `testcases_aligned_alloc.c` → `test_arr36_c_fail_testcases_aligned_alloc`
- ⏭️ NOT RUN `testcases_array_of_arrays.c` → `test_arr36_c_fail_testcases_array_of_arrays`
- ⏭️ NOT RUN `testcases_array_params.c` → `test_arr36_c_fail_testcases_array_params`
- ⏭️ NOT RUN `testcases_atomic_arrays.c` → `test_arr36_c_fail_testcases_atomic_arrays`
- ⏭️ NOT RUN `testcases_auto_arrays.c` → `test_arr36_c_fail_testcases_auto_arrays`
- ⏭️ NOT RUN `testcases_buffer_in_struct.c` → `test_arr36_c_fail_testcases_buffer_in_struct`
- ⏭️ NOT RUN `testcases_calloc_objects.c` → `test_arr36_c_fail_testcases_calloc_objects`
- ⏭️ NOT RUN `testcases_compare_separate.c` → `test_arr36_c_fail_testcases_compare_separate`
- ⏭️ NOT RUN `testcases_compound_literal.c` → `test_arr36_c_fail_testcases_compound_literal`
- ⏭️ NOT RUN `testcases_const_arrays.c` → `test_arr36_c_fail_testcases_const_arrays`
- ⏭️ NOT RUN `testcases_extern_arrays.c` → `test_arr36_c_fail_testcases_extern_arrays`
- ⏭️ NOT RUN `testcases_global_arrays.c` → `test_arr36_c_fail_testcases_global_arrays`
- ⏭️ NOT RUN `testcases_malloc_objects.c` → `test_arr36_c_fail_testcases_malloc_objects`
- ⏭️ NOT RUN `testcases_mixed_storage.c` → `test_arr36_c_fail_testcases_mixed_storage`
- ⏭️ NOT RUN `testcases_multidim_diff.c` → `test_arr36_c_fail_testcases_multidim_diff`
- ⏭️ NOT RUN `testcases_nested_struct.c` → `test_arr36_c_fail_testcases_nested_struct`
- ⏭️ NOT RUN `testcases_param_vars.c` → `test_arr36_c_fail_testcases_param_vars`
- ⏭️ NOT RUN `testcases_realloc_old_new.c` → `test_arr36_c_fail_testcases_realloc_old_new`
- ⏭️ NOT RUN `testcases_restrict_arrays.c` → `test_arr36_c_fail_testcases_restrict_arrays`
- ⏭️ NOT RUN `testcases_separate_arrays.c` → `test_arr36_c_fail_testcases_separate_arrays`
- ⏭️ NOT RUN `testcases_stack_heap.c` → `test_arr36_c_fail_testcases_stack_heap`
- ⏭️ NOT RUN `testcases_static_vars.c` → `test_arr36_c_fail_testcases_static_vars`
- ⏭️ NOT RUN `testcases_string_literals.c` → `test_arr36_c_fail_testcases_string_literals`
- ⏭️ NOT RUN `testcases_struct_arrays.c` → `test_arr36_c_fail_testcases_struct_arrays`
- ⏭️ NOT RUN `testcases_thread_local.c` → `test_arr36_c_fail_testcases_thread_local`
- ⏭️ NOT RUN `testcases_typedef_arrays.c` → `test_arr36_c_fail_testcases_typedef_arrays`
- ⏭️ NOT RUN `testcases_union_arrays.c` → `test_arr36_c_fail_testcases_union_arrays`
- ⏭️ NOT RUN `testcases_vla_different.c` → `test_arr36_c_fail_testcases_vla_different`
- ⏭️ NOT RUN `testcases_volatile_arrays.c` → `test_arr36_c_fail_testcases_volatile_arrays`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_arr36_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_array_param.c` → `test_arr36_c_pass_testcases_array_param`
- ⏭️ NOT RUN `testcases_compare_within_array.c` → `test_arr36_c_pass_testcases_compare_within_array`
- ⏭️ NOT RUN `testcases_dynamic_array.c` → `test_arr36_c_pass_testcases_dynamic_array`
- ⏭️ NOT RUN `testcases_equality_different.c` → `test_arr36_c_pass_testcases_equality_different`
- ⏭️ NOT RUN `testcases_multidim_same.c` → `test_arr36_c_pass_testcases_multidim_same`
- ⏭️ NOT RUN `testcases_one_past_end.c` → `test_arr36_c_pass_testcases_one_past_end`
- ⏭️ NOT RUN `testcases_same_array_subtract.c` → `test_arr36_c_pass_testcases_same_array_subtract`
- ⏭️ NOT RUN `testcases_string_within.c` → `test_arr36_c_pass_testcases_string_within`
- ⏭️ NOT RUN `testcases_struct_members.c` → `test_arr36_c_pass_testcases_struct_members`
- ⏭️ NOT RUN `testcases_vla_same.c` → `test_arr36_c_pass_testcases_vla_same`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_arr36_c_pass_wiki_compliant_1`

---

### ✅ ARR01-C - Implemented

<a id="rule-arr01c"></a>

**Title:** Do not apply the sizeof operator to a pointer when taking the size of an array

**Description:** Thesizeofoperator yields the size (in bytes) of its operand, which can be an
expression or the parenthesized name of a type. However, using thesizeofoperator
to determine the size of arrays is error prone. Thesizeofoperator is often used
in determining how much memory to allocate viamalloc(). However using an
incorrect size is a violation ofMEM35-C. Allocate sufficient memory for an
object. In this noncompliant code example, the functionclear()zeros the elements
in an array. The function has one parameter declared asint array[]and is passed
a static array consisting of 12intas the argument. The functionclear()uses the
idiomsizeof(array) / sizeof(array[0])to determine the number of elements in the
array. However,arrayhas a pointer type because it is a parameter. As a
result,sizeof(array)is equal to thesizeof(int *). For example, on an
architecture (such as IA-32) where thesizeof(int) == 4and thesizeof(int *) == 4,
the expressionsizeof(array) / sizeof(array[0])evaluates to 1, regardless of the
length of the array passed, leaving the rest of the array unaffected.

**Test Coverage:** 65 tests (43 fail, 22 pass)

**Test Results:** 0/65 passed (0.0%), 65 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_const_array_sizeof.c` → `test_arr01_c_fail_testcases_const_array_sizeof`
- ⏭️ NOT RUN `testcases_fail_case_1.c` → `test_arr01_c_fail_testcases_fail_case_1`
- ⏭️ NOT RUN `testcases_fail_case_10.c` → `test_arr01_c_fail_testcases_fail_case_10`
- ⏭️ NOT RUN `testcases_fail_case_11.c` → `test_arr01_c_fail_testcases_fail_case_11`
- ⏭️ NOT RUN `testcases_fail_case_12.c` → `test_arr01_c_fail_testcases_fail_case_12`
- ⏭️ NOT RUN `testcases_fail_case_13.c` → `test_arr01_c_fail_testcases_fail_case_13`
- ⏭️ NOT RUN `testcases_fail_case_14.c` → `test_arr01_c_fail_testcases_fail_case_14`
- ⏭️ NOT RUN `testcases_fail_case_15.c` → `test_arr01_c_fail_testcases_fail_case_15`
- ⏭️ NOT RUN `testcases_fail_case_16.c` → `test_arr01_c_fail_testcases_fail_case_16`
- ⏭️ NOT RUN `testcases_fail_case_17.c` → `test_arr01_c_fail_testcases_fail_case_17`
- ⏭️ NOT RUN `testcases_fail_case_18.c` → `test_arr01_c_fail_testcases_fail_case_18`
- ⏭️ NOT RUN `testcases_fail_case_19.c` → `test_arr01_c_fail_testcases_fail_case_19`
- ⏭️ NOT RUN `testcases_fail_case_2.c` → `test_arr01_c_fail_testcases_fail_case_2`
- ⏭️ NOT RUN `testcases_fail_case_20.c` → `test_arr01_c_fail_testcases_fail_case_20`
- ⏭️ NOT RUN `testcases_fail_case_21.c` → `test_arr01_c_fail_testcases_fail_case_21`
- ⏭️ NOT RUN `testcases_fail_case_22.c` → `test_arr01_c_fail_testcases_fail_case_22`
- ⏭️ NOT RUN `testcases_fail_case_23.c` → `test_arr01_c_fail_testcases_fail_case_23`
- ⏭️ NOT RUN `testcases_fail_case_24.c` → `test_arr01_c_fail_testcases_fail_case_24`
- ⏭️ NOT RUN `testcases_fail_case_25.c` → `test_arr01_c_fail_testcases_fail_case_25`
- ⏭️ NOT RUN `testcases_fail_case_26.c` → `test_arr01_c_fail_testcases_fail_case_26`
- ⏭️ NOT RUN `testcases_fail_case_27.c` → `test_arr01_c_fail_testcases_fail_case_27`
- ⏭️ NOT RUN `testcases_fail_case_28.c` → `test_arr01_c_fail_testcases_fail_case_28`
- ⏭️ NOT RUN `testcases_fail_case_29.c` → `test_arr01_c_fail_testcases_fail_case_29`
- ⏭️ NOT RUN `testcases_fail_case_3.c` → `test_arr01_c_fail_testcases_fail_case_3`
- ⏭️ NOT RUN `testcases_fail_case_30.c` → `test_arr01_c_fail_testcases_fail_case_30`
- ⏭️ NOT RUN `testcases_fail_case_4.c` → `test_arr01_c_fail_testcases_fail_case_4`
- ⏭️ NOT RUN `testcases_fail_case_5.c` → `test_arr01_c_fail_testcases_fail_case_5`
- ⏭️ NOT RUN `testcases_fail_case_6.c` → `test_arr01_c_fail_testcases_fail_case_6`
- ⏭️ NOT RUN `testcases_fail_case_7.c` → `test_arr01_c_fail_testcases_fail_case_7`
- ⏭️ NOT RUN `testcases_fail_case_8.c` → `test_arr01_c_fail_testcases_fail_case_8`
- ⏭️ NOT RUN `testcases_fail_case_9.c` → `test_arr01_c_fail_testcases_fail_case_9`
- ⏭️ NOT RUN `testcases_flexible_array_sizeof.c` → `test_arr01_c_fail_testcases_flexible_array_sizeof`
- ⏭️ NOT RUN `testcases_incomplete_array_sizeof.c` → `test_arr01_c_fail_testcases_incomplete_array_sizeof`
- ⏭️ NOT RUN `testcases_malloc_sizeof_error.c` → `test_arr01_c_fail_testcases_malloc_sizeof_error`
- ⏭️ NOT RUN `testcases_multidim_sizeof_error.c` → `test_arr01_c_fail_testcases_multidim_sizeof_error`
- ⏭️ NOT RUN `testcases_sizeof_array_parameter.c` → `test_arr01_c_fail_testcases_sizeof_array_parameter`
- ⏭️ NOT RUN `testcases_sizeof_pointer_confusion.c` → `test_arr01_c_fail_testcases_sizeof_pointer_confusion`
- ⏭️ NOT RUN `testcases_string_sizeof_error.c` → `test_arr01_c_fail_testcases_string_sizeof_error`
- ⏭️ NOT RUN `testcases_typedef_array_sizeof.c` → `test_arr01_c_fail_testcases_typedef_array_sizeof`
- ⏭️ NOT RUN `testcases_variadic_sizeof_error.c` → `test_arr01_c_fail_testcases_variadic_sizeof_error`
- ⏭️ NOT RUN `testcases_void_ptr_sizeof.c` → `test_arr01_c_fail_testcases_void_ptr_sizeof`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_arr01_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_arr01_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_array_length_constants.c` → `test_arr01_c_pass_testcases_array_length_constants`
- ⏭️ NOT RUN `testcases_flexible_array_proper.c` → `test_arr01_c_pass_testcases_flexible_array_proper`
- ⏭️ NOT RUN `testcases_local_size_calculation.c` → `test_arr01_c_pass_testcases_local_size_calculation`
- ⏭️ NOT RUN `testcases_multidimensional_proper.c` → `test_arr01_c_pass_testcases_multidimensional_proper`
- ⏭️ NOT RUN `testcases_pass_case_1.c` → `test_arr01_c_pass_testcases_pass_case_1`
- ⏭️ NOT RUN `testcases_pass_case_10.c` → `test_arr01_c_pass_testcases_pass_case_10`
- ⏭️ NOT RUN `testcases_pass_case_2.c` → `test_arr01_c_pass_testcases_pass_case_2`
- ⏭️ NOT RUN `testcases_pass_case_3.c` → `test_arr01_c_pass_testcases_pass_case_3`
- ⏭️ NOT RUN `testcases_pass_case_4.c` → `test_arr01_c_pass_testcases_pass_case_4`
- ⏭️ NOT RUN `testcases_pass_case_5.c` → `test_arr01_c_pass_testcases_pass_case_5`
- ⏭️ NOT RUN `testcases_pass_case_6.c` → `test_arr01_c_pass_testcases_pass_case_6`
- ⏭️ NOT RUN `testcases_pass_case_7.c` → `test_arr01_c_pass_testcases_pass_case_7`
- ⏭️ NOT RUN `testcases_pass_case_8.c` → `test_arr01_c_pass_testcases_pass_case_8`
- ⏭️ NOT RUN `testcases_pass_case_9.c` → `test_arr01_c_pass_testcases_pass_case_9`
- ⏭️ NOT RUN `testcases_pointer_arithmetic_known_bounds.c` → `test_arr01_c_pass_testcases_pointer_arithmetic_known_bounds`
- ⏭️ NOT RUN `testcases_safe_memory_allocation.c` → `test_arr01_c_pass_testcases_safe_memory_allocation`
- ⏭️ NOT RUN `testcases_size_as_parameter.c` → `test_arr01_c_pass_testcases_size_as_parameter`
- ⏭️ NOT RUN `testcases_static_array_declarator.c` → `test_arr01_c_pass_testcases_static_array_declarator`
- ⏭️ NOT RUN `testcases_string_length_safe.c` → `test_arr01_c_pass_testcases_string_length_safe`
- ⏭️ NOT RUN `testcases_vla_size_management.c` → `test_arr01_c_pass_testcases_vla_size_management`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_arr01_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_arr01_c_pass_wiki_compliant_2`

---

### ✅ ARR30-C - Implemented

<a id="rule-arr30c"></a>

**Title:** Do not form or use out-of-bounds pointers or array subscripts

**Description:** The C Standard identifies the following distinct situations in which undefined
behavior (UB) can arise as a result of invalid pointer operations:
UBDescriptionExample Code43Addition or subtraction of a pointer into, or just
beyond, an array object and an integer type produces a result that does not
point into, or just beyond, the same array object.Forming Out-of-Bounds
Pointer,Null Pointer Arithmetic44Addition or subtraction of a pointer into, or
just beyond, an array object and an integer type produces a result that points
just beyond the array object and is used as the operand of a unary*operator that
is evaluated.Dereferencing Past the End Pointer,Using Past the End Index46An
array subscript is out of range, even if an object is apparently accessible with
the given subscript, for example, in the lvalue expressiona[1][7]given the
declarationint a[4][5]).Apparently Accessible Out-of-Range Index59An attempt is
made to access, or generate a pointer to just past, a flexible array member of a
structure when the referenced object provides no elements for that array.Pointer
Past Flexible Array Member In this noncompliant code example, the
functionf()attempts to validate theindexbefore using it as an offset to the
statically allocatedtableof integers. However, the function fails to reject
negativeindexvalues. Whenindexis less than zero, the behavior of the addition
expression in the return statement of the function isundefined behavior 43. On
some implementations, the addition alone can trigger a hardware trap. On other
implementations, the addition may produce a result that when dereferenced
triggers a hardware trap. Other implementations still may produce a
dereferenceable pointer that points to an object distinct fromtable. Using such
a pointer to access the object may lead to information exposure or cause the
wrong object to be modified.

**Test Coverage:** 61 tests (43 fail, 18 pass)

**Test Results:** 0/61 passed (0.0%), 61 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_basic_overrun.c` → `test_arr30_c_fail_testcases_basic_overrun`
- ⏭️ NOT RUN `testcases_cast_over.c` → `test_arr30_c_fail_testcases_cast_over`
- ⏭️ NOT RUN `testcases_const_over.c` → `test_arr30_c_fail_testcases_const_over`
- ⏭️ NOT RUN `testcases_double_ptr.c` → `test_arr30_c_fail_testcases_double_ptr`
- ⏭️ NOT RUN `testcases_enum_over.c` → `test_arr30_c_fail_testcases_enum_over`
- ⏭️ NOT RUN `testcases_extern_over.c` → `test_arr30_c_fail_testcases_extern_over`
- ⏭️ NOT RUN `testcases_func_param.c` → `test_arr30_c_fail_testcases_func_param`
- ⏭️ NOT RUN `testcases_func_ptr.c` → `test_arr30_c_fail_testcases_func_ptr`
- ⏭️ NOT RUN `testcases_global_over.c` → `test_arr30_c_fail_testcases_global_over`
- ⏭️ NOT RUN `testcases_goto_over.c` → `test_arr30_c_fail_testcases_goto_over`
- ⏭️ NOT RUN `testcases_inline_over.c` → `test_arr30_c_fail_testcases_inline_over`
- ⏭️ NOT RUN `testcases_input_over.c` → `test_arr30_c_fail_testcases_input_over`
- ⏭️ NOT RUN `testcases_loop_overrun.c` → `test_arr30_c_fail_testcases_loop_overrun`
- ⏭️ NOT RUN `testcases_macro_over.c` → `test_arr30_c_fail_testcases_macro_over`
- ⏭️ NOT RUN `testcases_malloc_over.c` → `test_arr30_c_fail_testcases_malloc_over`
- ⏭️ NOT RUN `testcases_matrix_over.c` → `test_arr30_c_fail_testcases_matrix_over`
- ⏭️ NOT RUN `testcases_memcpy_over.c` → `test_arr30_c_fail_testcases_memcpy_over`
- ⏭️ NOT RUN `testcases_neg_index.c` → `test_arr30_c_fail_testcases_neg_index`
- ⏭️ NOT RUN `testcases_off_by_one.c` → `test_arr30_c_fail_testcases_off_by_one`
- ⏭️ NOT RUN `testcases_ptr_arith.c` → `test_arr30_c_fail_testcases_ptr_arith`
- ⏭️ NOT RUN `testcases_realloc_over.c` → `test_arr30_c_fail_testcases_realloc_over`
- ⏭️ NOT RUN `testcases_recursive.c` → `test_arr30_c_fail_testcases_recursive`
- ⏭️ NOT RUN `testcases_restrict_over.c` → `test_arr30_c_fail_testcases_restrict_over`
- ⏭️ NOT RUN `testcases_signal_over.c` → `test_arr30_c_fail_testcases_signal_over`
- ⏭️ NOT RUN `testcases_stack_over.c` → `test_arr30_c_fail_testcases_stack_over`
- ⏭️ NOT RUN `testcases_static_over.c` → `test_arr30_c_fail_testcases_static_over`
- ⏭️ NOT RUN `testcases_str_overflow.c` → `test_arr30_c_fail_testcases_str_overflow`
- ⏭️ NOT RUN `testcases_struct_over.c` → `test_arr30_c_fail_testcases_struct_over`
- ⏭️ NOT RUN `testcases_switch_over.c` → `test_arr30_c_fail_testcases_switch_over`
- ⏭️ NOT RUN `testcases_ternary_over.c` → `test_arr30_c_fail_testcases_ternary_over`
- ⏭️ NOT RUN `testcases_thread_over.c` → `test_arr30_c_fail_testcases_thread_over`
- ⏭️ NOT RUN `testcases_typedef_over.c` → `test_arr30_c_fail_testcases_typedef_over`
- ⏭️ NOT RUN `testcases_union_over.c` → `test_arr30_c_fail_testcases_union_over`
- ⏭️ NOT RUN `testcases_vla_over.c` → `test_arr30_c_fail_testcases_vla_over`
- ⏭️ NOT RUN `testcases_void_ptr.c` → `test_arr30_c_fail_testcases_void_ptr`
- ⏭️ NOT RUN `testcases_volatile_over.c` → `test_arr30_c_fail_testcases_volatile_over`
- ⏭️ NOT RUN `testcases_write_over.c` → `test_arr30_c_fail_testcases_write_over`
- ⏭️ NOT RUN `wiki_apparently_accessible_out_of_range_index.c` → `test_arr30_c_fail_wiki_apparently_accessible_out_of_range_index`
- ⏭️ NOT RUN `wiki_dereferencing_past_the_end_pointer.c` → `test_arr30_c_fail_wiki_dereferencing_past_the_end_pointer`
- ⏭️ NOT RUN `wiki_forming_out_of_bounds_pointer.c` → `test_arr30_c_fail_wiki_forming_out_of_bounds_pointer`
- ⏭️ NOT RUN `wiki_null_pointer_arithmetic.c` → `test_arr30_c_fail_wiki_null_pointer_arithmetic`
- ⏭️ NOT RUN `wiki_pointer_past_flexible_array_member.c` → `test_arr30_c_fail_wiki_pointer_past_flexible_array_member`
- ⏭️ NOT RUN `wiki_using_past_the_end_index.c` → `test_arr30_c_fail_wiki_using_past_the_end_index`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_basic_check.c` → `test_arr30_c_pass_testcases_basic_check`
- ⏭️ NOT RUN `testcases_binary_safe.c` → `test_arr30_c_pass_testcases_binary_safe`
- ⏭️ NOT RUN `testcases_bounds_check.c` → `test_arr30_c_pass_testcases_bounds_check`
- ⏭️ NOT RUN `testcases_func_param.c` → `test_arr30_c_pass_testcases_func_param`
- ⏭️ NOT RUN `testcases_loop_bounds.c` → `test_arr30_c_pass_testcases_loop_bounds`
- ⏭️ NOT RUN `testcases_malloc_safe.c` → `test_arr30_c_pass_testcases_malloc_safe`
- ⏭️ NOT RUN `testcases_matrix_safe.c` → `test_arr30_c_pass_testcases_matrix_safe`
- ⏭️ NOT RUN `testcases_ptr_check.c` → `test_arr30_c_pass_testcases_ptr_check`
- ⏭️ NOT RUN `testcases_search_safe.c` → `test_arr30_c_pass_testcases_search_safe`
- ⏭️ NOT RUN `testcases_str_bounds.c` → `test_arr30_c_pass_testcases_str_bounds`
- ⏭️ NOT RUN `testcases_struct_safe.c` → `test_arr30_c_pass_testcases_struct_safe`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_arr30_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_arr30_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_arr30_c_pass_wiki_compliant_3`
- ⏭️ NOT RUN `wiki_compliant_4.c` → `test_arr30_c_pass_wiki_compliant_4`
- ⏭️ NOT RUN `wiki_compliant_5.c` → `test_arr30_c_pass_wiki_compliant_5`
- ⏭️ NOT RUN `wiki_compliant_6.c` → `test_arr30_c_pass_wiki_compliant_6`
- ⏭️ NOT RUN `wiki_null_pointer_arithmetic.c` → `test_arr30_c_pass_wiki_null_pointer_arithmetic`

---

### ✅ ARR39-C - Implemented

<a id="rule-arr39c"></a>

**Title:** Do not add or subtract a scaled integer to a pointer

**Description:** Pointer arithmetic is appropriate only when the pointer argument refers to an
array (seeARR37-C. Do not add or subtract an integer to a pointer to a non-array
object), including an array of bytes. When performing pointer arithmetic, the
size of the value to add to or subtract from a pointer is automatically scaled
to the size of the type of the referenced array object. Adding or subtracting a
scaled integer value to or from a pointer is invalid because it may yield a
pointer that does not point to an element within or one past the end of the
array. (SeeARR30-C. Do not form or use out-of-bounds pointers or array
subscripts.) Adding a pointer to an array of a type other than character to the
result of thesizeofoperator oroffsetofmacro, which returns a size and an offset,
respectively, violates this rule. However, adding an array pointer to the number
of array elements, for example, by using
thearr[sizeof(arr)/sizeof(arr[0])])idiom, is allowed provided thatarrrefers to
an array and not a pointer. In this noncompliant code example,sizeof(buf)is
added to the arraybuf. This example is noncompliant becausesizeof(buf)is scaled
byintand then scaled again when added tobuf.

**Test Coverage:** 46 tests (33 fail, 13 pass)

**Test Results:** 0/46 passed (0.0%), 46 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_array_sizeof_index.c` → `test_arr39_c_fail_testcases_array_sizeof_index`
- ⏭️ NOT RUN `testcases_byte_offset_int_ptr.c` → `test_arr39_c_fail_testcases_byte_offset_int_ptr`
- ⏭️ NOT RUN `testcases_calloc_sizeof_ptr.c` → `test_arr39_c_fail_testcases_calloc_sizeof_ptr`
- ⏭️ NOT RUN `testcases_malloc_sizeof.c` → `test_arr39_c_fail_testcases_malloc_sizeof`
- ⏭️ NOT RUN `testcases_manual_sizeof_scale.c` → `test_arr39_c_fail_testcases_manual_sizeof_scale`
- ⏭️ NOT RUN `testcases_offsetof_array.c` → `test_arr39_c_fail_testcases_offsetof_array`
- ⏭️ NOT RUN `testcases_offsetof_struct_ptr.c` → `test_arr39_c_fail_testcases_offsetof_struct_ptr`
- ⏭️ NOT RUN `testcases_ptr_diff_sizeof.c` → `test_arr39_c_fail_testcases_ptr_diff_sizeof`
- ⏭️ NOT RUN `testcases_sizeof_array_elem.c` → `test_arr39_c_fail_testcases_sizeof_array_elem`
- ⏭️ NOT RUN `testcases_sizeof_atomic.c` → `test_arr39_c_fail_testcases_sizeof_atomic`
- ⏭️ NOT RUN `testcases_sizeof_cast_ptr.c` → `test_arr39_c_fail_testcases_sizeof_cast_ptr`
- ⏭️ NOT RUN `testcases_sizeof_comparison.c` → `test_arr39_c_fail_testcases_sizeof_comparison`
- ⏭️ NOT RUN `testcases_sizeof_complex.c` → `test_arr39_c_fail_testcases_sizeof_complex`
- ⏭️ NOT RUN `testcases_sizeof_division.c` → `test_arr39_c_fail_testcases_sizeof_division`
- ⏭️ NOT RUN `testcases_sizeof_function_param.c` → `test_arr39_c_fail_testcases_sizeof_function_param`
- ⏭️ NOT RUN `testcases_sizeof_loop_bound.c` → `test_arr39_c_fail_testcases_sizeof_loop_bound`
- ⏭️ NOT RUN `testcases_sizeof_memcpy_offset.c` → `test_arr39_c_fail_testcases_sizeof_memcpy_offset`
- ⏭️ NOT RUN `testcases_sizeof_multidim.c` → `test_arr39_c_fail_testcases_sizeof_multidim`
- ⏭️ NOT RUN `testcases_sizeof_nested_struct.c` → `test_arr39_c_fail_testcases_sizeof_nested_struct`
- ⏭️ NOT RUN `testcases_sizeof_pointer_add.c` → `test_arr39_c_fail_testcases_sizeof_pointer_add`
- ⏭️ NOT RUN `testcases_sizeof_ptr_arithmetic.c` → `test_arr39_c_fail_testcases_sizeof_ptr_arithmetic`
- ⏭️ NOT RUN `testcases_sizeof_subtract.c` → `test_arr39_c_fail_testcases_sizeof_subtract`
- ⏭️ NOT RUN `testcases_sizeof_ternary.c` → `test_arr39_c_fail_testcases_sizeof_ternary`
- ⏭️ NOT RUN `testcases_sizeof_typedef.c` → `test_arr39_c_fail_testcases_sizeof_typedef`
- ⏭️ NOT RUN `testcases_sizeof_union.c` → `test_arr39_c_fail_testcases_sizeof_union`
- ⏭️ NOT RUN `testcases_sizeof_vla.c` → `test_arr39_c_fail_testcases_sizeof_vla`
- ⏭️ NOT RUN `testcases_strlen_sizeof.c` → `test_arr39_c_fail_testcases_strlen_sizeof`
- ⏭️ NOT RUN `testcases_struct_sizeof_offset.c` → `test_arr39_c_fail_testcases_struct_sizeof_offset`
- ⏭️ NOT RUN `testcases_wchar_scaled_len.c` → `test_arr39_c_fail_testcases_wchar_scaled_len`
- ⏭️ NOT RUN `testcases_wcs_sizeof_scale.c` → `test_arr39_c_fail_testcases_wcs_sizeof_scale`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_arr39_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_arr39_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_arr39_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_array_length.c` → `test_arr39_c_pass_testcases_array_length`
- ⏭️ NOT RUN `testcases_byte_ptr_arithmetic.c` → `test_arr39_c_pass_testcases_byte_ptr_arithmetic`
- ⏭️ NOT RUN `testcases_char_ptr_offsetof.c` → `test_arr39_c_pass_testcases_char_ptr_offsetof`
- ⏭️ NOT RUN `testcases_direct_index.c` → `test_arr39_c_pass_testcases_direct_index`
- ⏭️ NOT RUN `testcases_element_count.c` → `test_arr39_c_pass_testcases_element_count`
- ⏭️ NOT RUN `testcases_element_count_compare.c` → `test_arr39_c_pass_testcases_element_count_compare`
- ⏭️ NOT RUN `testcases_sizeof_div_element.c` → `test_arr39_c_pass_testcases_sizeof_div_element`
- ⏭️ NOT RUN `testcases_strlen_direct.c` → `test_arr39_c_pass_testcases_strlen_direct`
- ⏭️ NOT RUN `testcases_struct_index.c` → `test_arr39_c_pass_testcases_struct_index`
- ⏭️ NOT RUN `testcases_unscaled_wchar_len.c` → `test_arr39_c_pass_testcases_unscaled_wchar_len`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_arr39_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_arr39_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_arr39_c_pass_wiki_compliant_3`

---

### ✅ ARR38-C - Implemented

<a id="rule-arr38c"></a>

**Title:** Guarantee that library functions do not form invalid pointers

**Description:** C library functions that make changes to arrays or objects take at least two
arguments: a pointer to the array or object and an integer indicating the number
of elements or bytes to be manipulated. For the purposes of this rule, the
element count of a pointer is the size of the object to which it points,
expressed by the number of elements that are valid to access. Supplying
arguments to such a function might cause the function to form a pointer that
does not point into or just past the end of the object, resulting inundefined
behavior. Annex J of the C Standard [ISO/IEC 9899:2024] states that it is
undefined behavior if the "pointer passed to a library function array parameter
does not have a value such that all address computations and object accesses are
valid." (Seeundefined behavior108.) In the following code,

**Test Coverage:** 50 tests (35 fail, 15 pass)

**Test Results:** 0/50 passed (0.0%), 50 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_aligned_alloc_exceed.c` → `test_arr38_c_fail_testcases_aligned_alloc_exceed`
- ⏭️ NOT RUN `testcases_bsearch_wrong_count.c` → `test_arr38_c_fail_testcases_bsearch_wrong_count`
- ⏭️ NOT RUN `testcases_calloc_memset_exceed.c` → `test_arr38_c_fail_testcases_calloc_memset_exceed`
- ⏭️ NOT RUN `testcases_fgets_oversize.c` → `test_arr38_c_fail_testcases_fgets_oversize`
- ⏭️ NOT RUN `testcases_fread_wrong_count.c` → `test_arr38_c_fail_testcases_fread_wrong_count`
- ⏭️ NOT RUN `testcases_fwrite_wrong_count.c` → `test_arr38_c_fail_testcases_fwrite_wrong_count`
- ⏭️ NOT RUN `testcases_memchr_overrun.c` → `test_arr38_c_fail_testcases_memchr_overrun`
- ⏭️ NOT RUN `testcases_memcmp_first_short.c` → `test_arr38_c_fail_testcases_memcmp_first_short`
- ⏭️ NOT RUN `testcases_memcpy_src_short.c` → `test_arr38_c_fail_testcases_memcpy_src_short`
- ⏭️ NOT RUN `testcases_memmove_dest_small.c` → `test_arr38_c_fail_testcases_memmove_dest_small`
- ⏭️ NOT RUN `testcases_memset_oversize.c` → `test_arr38_c_fail_testcases_memset_oversize`
- ⏭️ NOT RUN `testcases_pointer_offset_memcpy.c` → `test_arr38_c_fail_testcases_pointer_offset_memcpy`
- ⏭️ NOT RUN `testcases_qsort_wrong_size.c` → `test_arr38_c_fail_testcases_qsort_wrong_size`
- ⏭️ NOT RUN `testcases_realloc_old_size.c` → `test_arr38_c_fail_testcases_realloc_old_size`
- ⏭️ NOT RUN `testcases_snprintf_oversize.c` → `test_arr38_c_fail_testcases_snprintf_oversize`
- ⏭️ NOT RUN `testcases_strncat_overflow.c` → `test_arr38_c_fail_testcases_strncat_overflow`
- ⏭️ NOT RUN `testcases_strncmp_exceed.c` → `test_arr38_c_fail_testcases_strncmp_exceed`
- ⏭️ NOT RUN `testcases_strncpy_oversize.c` → `test_arr38_c_fail_testcases_strncpy_oversize`
- ⏭️ NOT RUN `testcases_struct_hardcoded_size.c` → `test_arr38_c_fail_testcases_struct_hardcoded_size`
- ⏭️ NOT RUN `testcases_swprintf_oversize.c` → `test_arr38_c_fail_testcases_swprintf_oversize`
- ⏭️ NOT RUN `testcases_user_controlled_size.c` → `test_arr38_c_fail_testcases_user_controlled_size`
- ⏭️ NOT RUN `testcases_vla_wrong_size.c` → `test_arr38_c_fail_testcases_vla_wrong_size`
- ⏭️ NOT RUN `testcases_wcsncat_overflow.c` → `test_arr38_c_fail_testcases_wcsncat_overflow`
- ⏭️ NOT RUN `testcases_wcsncmp_exceed.c` → `test_arr38_c_fail_testcases_wcsncmp_exceed`
- ⏭️ NOT RUN `testcases_wcsncpy_overflow.c` → `test_arr38_c_fail_testcases_wcsncpy_overflow`
- ⏭️ NOT RUN `testcases_wmemchr_overrun.c` → `test_arr38_c_fail_testcases_wmemchr_overrun`
- ⏭️ NOT RUN `testcases_wmemcmp_overflow.c` → `test_arr38_c_fail_testcases_wmemcmp_overflow`
- ⏭️ NOT RUN `testcases_wmemcpy_sizeof.c` → `test_arr38_c_fail_testcases_wmemcpy_sizeof`
- ⏭️ NOT RUN `testcases_wmemset_byte_count.c` → `test_arr38_c_fail_testcases_wmemset_byte_count`
- ⏭️ NOT RUN `testcases_wrong_type_scale.c` → `test_arr38_c_fail_testcases_wrong_type_scale`
- ⏭️ NOT RUN `wiki_element_count.c` → `test_arr38_c_fail_wiki_element_count`
- ⏭️ NOT RUN `wiki_heartbleed.c` → `test_arr38_c_fail_wiki_heartbleed`
- ⏭️ NOT RUN `wiki_one_pointer_two_integers.c` → `test_arr38_c_fail_wiki_one_pointer_two_integers`
- ⏭️ NOT RUN `wiki_pointer_integer.c` → `test_arr38_c_fail_wiki_pointer_integer`
- ⏭️ NOT RUN `wiki_two_pointers_one_integer.c` → `test_arr38_c_fail_wiki_two_pointers_one_integer`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_fread_correct_count.c` → `test_arr38_c_pass_testcases_fread_correct_count`
- ⏭️ NOT RUN `testcases_memcpy_correct_size.c` → `test_arr38_c_pass_testcases_memcpy_correct_size`
- ⏭️ NOT RUN `testcases_memset_allocated_size.c` → `test_arr38_c_pass_testcases_memset_allocated_size`
- ⏭️ NOT RUN `testcases_qsort_correct.c` → `test_arr38_c_pass_testcases_qsort_correct`
- ⏭️ NOT RUN `testcases_sizeof_correct.c` → `test_arr38_c_pass_testcases_sizeof_correct`
- ⏭️ NOT RUN `testcases_snprintf_correct.c` → `test_arr38_c_pass_testcases_snprintf_correct`
- ⏭️ NOT RUN `testcases_strncpy_bounded.c` → `test_arr38_c_pass_testcases_strncpy_bounded`
- ⏭️ NOT RUN `testcases_struct_sizeof.c` → `test_arr38_c_pass_testcases_struct_sizeof`
- ⏭️ NOT RUN `testcases_validated_user_input.c` → `test_arr38_c_pass_testcases_validated_user_input`
- ⏭️ NOT RUN `testcases_wmemcpy_correct.c` → `test_arr38_c_pass_testcases_wmemcpy_correct`
- ⏭️ NOT RUN `wiki_element_count.c` → `test_arr38_c_pass_wiki_element_count`
- ⏭️ NOT RUN `wiki_heartbleed.c` → `test_arr38_c_pass_wiki_heartbleed`
- ⏭️ NOT RUN `wiki_one_pointer_two_integers.c` → `test_arr38_c_pass_wiki_one_pointer_two_integers`
- ⏭️ NOT RUN `wiki_pointer_integer.c` → `test_arr38_c_pass_wiki_pointer_integer`
- ⏭️ NOT RUN `wiki_two_pointers_one_integer.c` → `test_arr38_c_pass_wiki_two_pointers_one_integer`

---

### ✅ ARR37-C - Implemented

<a id="rule-arr37c"></a>

**Title:** Do not add or subtract an integer to a pointer to a non-array object

**Description:** Pointer arithmetic must be performed only on pointers that reference elements of
array objects. The C Standard, 6.5.7 [ISO/IEC 9899:2024], states the following
about pointer arithmetic: This noncompliant code example attempts to access
structure members using pointer arithmetic. This practice is dangerous because
structure members are not guaranteed to be contiguous.

**Test Coverage:** 43 tests (31 fail, 12 pass)

**Test Results:** 0/43 passed (0.0%), 43 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_aligned_single.c` → `test_arr37_c_fail_testcases_aligned_single`
- ⏭️ NOT RUN `testcases_atomic_single.c` → `test_arr37_c_fail_testcases_atomic_single`
- ⏭️ NOT RUN `testcases_bitfield_struct.c` → `test_arr37_c_fail_testcases_bitfield_struct`
- ⏭️ NOT RUN `testcases_calloc_single.c` → `test_arr37_c_fail_testcases_calloc_single`
- ⏭️ NOT RUN `testcases_char_var.c` → `test_arr37_c_fail_testcases_char_var`
- ⏭️ NOT RUN `testcases_complex_type.c` → `test_arr37_c_fail_testcases_complex_type`
- ⏭️ NOT RUN `testcases_const_single.c` → `test_arr37_c_fail_testcases_const_single`
- ⏭️ NOT RUN `testcases_decrement_single.c` → `test_arr37_c_fail_testcases_decrement_single`
- ⏭️ NOT RUN `testcases_enum_var.c` → `test_arr37_c_fail_testcases_enum_var`
- ⏭️ NOT RUN `testcases_flexible_array_wrong.c` → `test_arr37_c_fail_testcases_flexible_array_wrong`
- ⏭️ NOT RUN `testcases_global_single.c` → `test_arr37_c_fail_testcases_global_single`
- ⏭️ NOT RUN `testcases_malloc_single.c` → `test_arr37_c_fail_testcases_malloc_single`
- ⏭️ NOT RUN `testcases_nested_struct.c` → `test_arr37_c_fail_testcases_nested_struct`
- ⏭️ NOT RUN `testcases_param_single.c` → `test_arr37_c_fail_testcases_param_single`
- ⏭️ NOT RUN `testcases_pointer_offset.c` → `test_arr37_c_fail_testcases_pointer_offset`
- ⏭️ NOT RUN `testcases_pointer_to_pointer.c` → `test_arr37_c_fail_testcases_pointer_to_pointer`
- ⏭️ NOT RUN `testcases_register_hint.c` → `test_arr37_c_fail_testcases_register_hint`
- ⏭️ NOT RUN `testcases_restrict_single.c` → `test_arr37_c_fail_testcases_restrict_single`
- ⏭️ NOT RUN `testcases_single_var_add.c` → `test_arr37_c_fail_testcases_single_var_add`
- ⏭️ NOT RUN `testcases_single_var_increment.c` → `test_arr37_c_fail_testcases_single_var_increment`
- ⏭️ NOT RUN `testcases_static_single.c` → `test_arr37_c_fail_testcases_static_single`
- ⏭️ NOT RUN `testcases_struct_iterate.c` → `test_arr37_c_fail_testcases_struct_iterate`
- ⏭️ NOT RUN `testcases_struct_members.c` → `test_arr37_c_fail_testcases_struct_members`
- ⏭️ NOT RUN `testcases_struct_padding.c` → `test_arr37_c_fail_testcases_struct_padding`
- ⏭️ NOT RUN `testcases_subtract_from_single.c` → `test_arr37_c_fail_testcases_subtract_from_single`
- ⏭️ NOT RUN `testcases_thread_local.c` → `test_arr37_c_fail_testcases_thread_local`
- ⏭️ NOT RUN `testcases_typedef_single.c` → `test_arr37_c_fail_testcases_typedef_single`
- ⏭️ NOT RUN `testcases_union_members.c` → `test_arr37_c_fail_testcases_union_members`
- ⏭️ NOT RUN `testcases_vla_single.c` → `test_arr37_c_fail_testcases_vla_single`
- ⏭️ NOT RUN `testcases_volatile_single.c` → `test_arr37_c_fail_testcases_volatile_single`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_arr37_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_array_arithmetic.c` → `test_arr37_c_pass_testcases_array_arithmetic`
- ⏭️ NOT RUN `testcases_array_parameter.c` → `test_arr37_c_pass_testcases_array_parameter`
- ⏭️ NOT RUN `testcases_calloc_array.c` → `test_arr37_c_pass_testcases_calloc_array`
- ⏭️ NOT RUN `testcases_direct_member_access.c` → `test_arr37_c_pass_testcases_direct_member_access`
- ⏭️ NOT RUN `testcases_flexible_array_member.c` → `test_arr37_c_pass_testcases_flexible_array_member`
- ⏭️ NOT RUN `testcases_malloc_array.c` → `test_arr37_c_pass_testcases_malloc_array`
- ⏭️ NOT RUN `testcases_multidim_array.c` → `test_arr37_c_pass_testcases_multidim_array`
- ⏭️ NOT RUN `testcases_string_array.c` → `test_arr37_c_pass_testcases_string_array`
- ⏭️ NOT RUN `testcases_struct_with_array.c` → `test_arr37_c_pass_testcases_struct_with_array`
- ⏭️ NOT RUN `testcases_vla_array.c` → `test_arr37_c_pass_testcases_vla_array`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_arr37_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_arr37_c_pass_wiki_compliant_2`

---

### 🔶 ARR02-C - Not Implemented (has tests)

<a id="rule-arr02c"></a>

**Title:** Explicitly specify array bounds, even if implicitly defined by an initializer

**Description:** The C Standard allows an array variable to be declared both with a bound and
with an initialization literal. The initialization literal also implies an array
bound in the number of elements specified. The size implied by an initialization
literal is usually specified by the number of elements, int array[] = {1, 2, 3};
/* 3-element array */

**Test Coverage:** 82 tests (62 fail, 20 pass)

**Test Results:** 0/82 passed (0.0%), 82 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_designated_init_implicit.c` → `test_arr02_c_fail_testcases_designated_init_implicit`
- ⏭️ NOT RUN `testcases_fail_case_1.c` → `test_arr02_c_fail_testcases_fail_case_1`
- ⏭️ NOT RUN `testcases_fail_case_10.c` → `test_arr02_c_fail_testcases_fail_case_10`
- ⏭️ NOT RUN `testcases_fail_case_11.c` → `test_arr02_c_fail_testcases_fail_case_11`
- ⏭️ NOT RUN `testcases_fail_case_12.c` → `test_arr02_c_fail_testcases_fail_case_12`
- ⏭️ NOT RUN `testcases_fail_case_13.c` → `test_arr02_c_fail_testcases_fail_case_13`
- ⏭️ NOT RUN `testcases_fail_case_14.c` → `test_arr02_c_fail_testcases_fail_case_14`
- ⏭️ NOT RUN `testcases_fail_case_15.c` → `test_arr02_c_fail_testcases_fail_case_15`
- ⏭️ NOT RUN `testcases_fail_case_16.c` → `test_arr02_c_fail_testcases_fail_case_16`
- ⏭️ NOT RUN `testcases_fail_case_17.c` → `test_arr02_c_fail_testcases_fail_case_17`
- ⏭️ NOT RUN `testcases_fail_case_18.c` → `test_arr02_c_fail_testcases_fail_case_18`
- ⏭️ NOT RUN `testcases_fail_case_19.c` → `test_arr02_c_fail_testcases_fail_case_19`
- ⏭️ NOT RUN `testcases_fail_case_2.c` → `test_arr02_c_fail_testcases_fail_case_2`
- ⏭️ NOT RUN `testcases_fail_case_20.c` → `test_arr02_c_fail_testcases_fail_case_20`
- ⏭️ NOT RUN `testcases_fail_case_21.c` → `test_arr02_c_fail_testcases_fail_case_21`
- ⏭️ NOT RUN `testcases_fail_case_22.c` → `test_arr02_c_fail_testcases_fail_case_22`
- ⏭️ NOT RUN `testcases_fail_case_23.c` → `test_arr02_c_fail_testcases_fail_case_23`
- ⏭️ NOT RUN `testcases_fail_case_24.c` → `test_arr02_c_fail_testcases_fail_case_24`
- ⏭️ NOT RUN `testcases_fail_case_25.c` → `test_arr02_c_fail_testcases_fail_case_25`
- ⏭️ NOT RUN `testcases_fail_case_26.c` → `test_arr02_c_fail_testcases_fail_case_26`
- ⏭️ NOT RUN `testcases_fail_case_27.c` → `test_arr02_c_fail_testcases_fail_case_27`
- ⏭️ NOT RUN `testcases_fail_case_28.c` → `test_arr02_c_fail_testcases_fail_case_28`
- ⏭️ NOT RUN `testcases_fail_case_29.c` → `test_arr02_c_fail_testcases_fail_case_29`
- ⏭️ NOT RUN `testcases_fail_case_3.c` → `test_arr02_c_fail_testcases_fail_case_3`
- ⏭️ NOT RUN `testcases_fail_case_30.c` → `test_arr02_c_fail_testcases_fail_case_30`
- ⏭️ NOT RUN `testcases_fail_case_4.c` → `test_arr02_c_fail_testcases_fail_case_4`
- ⏭️ NOT RUN `testcases_fail_case_5.c` → `test_arr02_c_fail_testcases_fail_case_5`
- ⏭️ NOT RUN `testcases_fail_case_6.c` → `test_arr02_c_fail_testcases_fail_case_6`
- ⏭️ NOT RUN `testcases_fail_case_7.c` → `test_arr02_c_fail_testcases_fail_case_7`
- ⏭️ NOT RUN `testcases_fail_case_8.c` → `test_arr02_c_fail_testcases_fail_case_8`
- ⏭️ NOT RUN `testcases_fail_case_9.c` → `test_arr02_c_fail_testcases_fail_case_9`
- ⏭️ NOT RUN `testcases_implicit_bounds_10.c` → `test_arr02_c_fail_testcases_implicit_bounds_10`
- ⏭️ NOT RUN `testcases_implicit_bounds_11.c` → `test_arr02_c_fail_testcases_implicit_bounds_11`
- ⏭️ NOT RUN `testcases_implicit_bounds_12.c` → `test_arr02_c_fail_testcases_implicit_bounds_12`
- ⏭️ NOT RUN `testcases_implicit_bounds_13.c` → `test_arr02_c_fail_testcases_implicit_bounds_13`
- ⏭️ NOT RUN `testcases_implicit_bounds_14.c` → `test_arr02_c_fail_testcases_implicit_bounds_14`
- ⏭️ NOT RUN `testcases_implicit_bounds_15.c` → `test_arr02_c_fail_testcases_implicit_bounds_15`
- ⏭️ NOT RUN `testcases_implicit_bounds_16.c` → `test_arr02_c_fail_testcases_implicit_bounds_16`
- ⏭️ NOT RUN `testcases_implicit_bounds_17.c` → `test_arr02_c_fail_testcases_implicit_bounds_17`
- ⏭️ NOT RUN `testcases_implicit_bounds_18.c` → `test_arr02_c_fail_testcases_implicit_bounds_18`
- ⏭️ NOT RUN `testcases_implicit_bounds_19.c` → `test_arr02_c_fail_testcases_implicit_bounds_19`
- ⏭️ NOT RUN `testcases_implicit_bounds_20.c` → `test_arr02_c_fail_testcases_implicit_bounds_20`
- ⏭️ NOT RUN `testcases_implicit_bounds_21.c` → `test_arr02_c_fail_testcases_implicit_bounds_21`
- ⏭️ NOT RUN `testcases_implicit_bounds_22.c` → `test_arr02_c_fail_testcases_implicit_bounds_22`
- ⏭️ NOT RUN `testcases_implicit_bounds_23.c` → `test_arr02_c_fail_testcases_implicit_bounds_23`
- ⏭️ NOT RUN `testcases_implicit_bounds_24.c` → `test_arr02_c_fail_testcases_implicit_bounds_24`
- ⏭️ NOT RUN `testcases_implicit_bounds_25.c` → `test_arr02_c_fail_testcases_implicit_bounds_25`
- ⏭️ NOT RUN `testcases_implicit_bounds_26.c` → `test_arr02_c_fail_testcases_implicit_bounds_26`
- ⏭️ NOT RUN `testcases_implicit_bounds_27.c` → `test_arr02_c_fail_testcases_implicit_bounds_27`
- ⏭️ NOT RUN `testcases_implicit_bounds_28.c` → `test_arr02_c_fail_testcases_implicit_bounds_28`
- ⏭️ NOT RUN `testcases_implicit_bounds_29.c` → `test_arr02_c_fail_testcases_implicit_bounds_29`
- ⏭️ NOT RUN `testcases_implicit_bounds_30.c` → `test_arr02_c_fail_testcases_implicit_bounds_30`
- ⏭️ NOT RUN `testcases_implicit_bounds_4.c` → `test_arr02_c_fail_testcases_implicit_bounds_4`
- ⏭️ NOT RUN `testcases_implicit_bounds_5.c` → `test_arr02_c_fail_testcases_implicit_bounds_5`
- ⏭️ NOT RUN `testcases_implicit_bounds_6.c` → `test_arr02_c_fail_testcases_implicit_bounds_6`
- ⏭️ NOT RUN `testcases_implicit_bounds_7.c` → `test_arr02_c_fail_testcases_implicit_bounds_7`
- ⏭️ NOT RUN `testcases_implicit_bounds_8.c` → `test_arr02_c_fail_testcases_implicit_bounds_8`
- ⏭️ NOT RUN `testcases_implicit_bounds_9.c` → `test_arr02_c_fail_testcases_implicit_bounds_9`
- ⏭️ NOT RUN `testcases_implicit_bounds_basic.c` → `test_arr02_c_fail_testcases_implicit_bounds_basic`
- ⏭️ NOT RUN `testcases_multidim_partial_bounds.c` → `test_arr02_c_fail_testcases_multidim_partial_bounds`
- ⏭️ NOT RUN `wiki_implicit_size.c` → `test_arr02_c_fail_wiki_implicit_size`
- ⏭️ NOT RUN `wiki_incorrect_size.c` → `test_arr02_c_fail_wiki_incorrect_size`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_aggregate_initialization.c` → `test_arr02_c_pass_testcases_aggregate_initialization`
- ⏭️ NOT RUN `testcases_const_arrays_explicit.c` → `test_arr02_c_pass_testcases_const_arrays_explicit`
- ⏭️ NOT RUN `testcases_designated_initializers.c` → `test_arr02_c_pass_testcases_designated_initializers`
- ⏭️ NOT RUN `testcases_enum_arrays_explicit.c` → `test_arr02_c_pass_testcases_enum_arrays_explicit`
- ⏭️ NOT RUN `testcases_explicit_bounds_basic.c` → `test_arr02_c_pass_testcases_explicit_bounds_basic`
- ⏭️ NOT RUN `testcases_large_arrays_explicit.c` → `test_arr02_c_pass_testcases_large_arrays_explicit`
- ⏭️ NOT RUN `testcases_mixed_initialization.c` → `test_arr02_c_pass_testcases_mixed_initialization`
- ⏭️ NOT RUN `testcases_multidimensional_explicit.c` → `test_arr02_c_pass_testcases_multidimensional_explicit`
- ⏭️ NOT RUN `testcases_pass_case_1.c` → `test_arr02_c_pass_testcases_pass_case_1`
- ⏭️ NOT RUN `testcases_pass_case_10.c` → `test_arr02_c_pass_testcases_pass_case_10`
- ⏭️ NOT RUN `testcases_pass_case_2.c` → `test_arr02_c_pass_testcases_pass_case_2`
- ⏭️ NOT RUN `testcases_pass_case_3.c` → `test_arr02_c_pass_testcases_pass_case_3`
- ⏭️ NOT RUN `testcases_pass_case_4.c` → `test_arr02_c_pass_testcases_pass_case_4`
- ⏭️ NOT RUN `testcases_pass_case_5.c` → `test_arr02_c_pass_testcases_pass_case_5`
- ⏭️ NOT RUN `testcases_pass_case_6.c` → `test_arr02_c_pass_testcases_pass_case_6`
- ⏭️ NOT RUN `testcases_pass_case_7.c` → `test_arr02_c_pass_testcases_pass_case_7`
- ⏭️ NOT RUN `testcases_pass_case_8.c` → `test_arr02_c_pass_testcases_pass_case_8`
- ⏭️ NOT RUN `testcases_pass_case_9.c` → `test_arr02_c_pass_testcases_pass_case_9`
- ⏭️ NOT RUN `testcases_static_arrays_explicit.c` → `test_arr02_c_pass_testcases_static_arrays_explicit`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_arr02_c_pass_wiki_compliant_1`

---

### ✅ ARR00-C - Implemented

<a id="rule-arr00c"></a>

**Title:** Understand how arrays work

**Description:** The incorrect use of arrays has traditionally been a source of
exploitablevulnerabilities. Elements referenced within an array using the
subscript operator[]are not checked unless the programmer provides adequate
bounds checking. As a result, the expressionarray [pos] = valuecan be used by an
attacker to transfer control to arbitrary code. An attacker who can control the
values of bothposandvaluein the expressionarray [pos] = valuecan perform an
arbitrary write (which is when the attacker overwrites other storage locations
with different content). The consequences range from changing a variable used to
determine what permissions the program grants to executing arbitrary code with
the permissions of the vulnerable process. Arrays are also a common source of
buffer overflows when iterators exceed the bounds of the array. An array is a
series of objects, all of which are the same size and type. Each object in an
array is called anarray element. The entire array is stored contiguously in
memory (that is, there are no gaps between elements). Arrays are commonly used
to represent a sequence of elements where random access is important but there
is little or no need to insert new elements into the sequence (which can be an
expensive operation with arrays).

**Test Coverage:** 39 tests (29 fail, 10 pass)

**Test Results:** 0/39 passed (0.0%), 39 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_array_assignment_error.c` → `test_arr00_c_fail_testcases_array_assignment_error`
- ⏭️ NOT RUN `testcases_array_decay_confusion.c` → `test_arr00_c_fail_testcases_array_decay_confusion`
- ⏭️ NOT RUN `testcases_buffer_overflow_basic.c` → `test_arr00_c_fail_testcases_buffer_overflow_basic`
- ⏭️ NOT RUN `testcases_gets_usage.c` → `test_arr00_c_fail_testcases_gets_usage`
- ⏭️ NOT RUN `testcases_incorrect_2d_access.c` → `test_arr00_c_fail_testcases_incorrect_2d_access`
- ⏭️ NOT RUN `testcases_integer_overflow_index.c` → `test_arr00_c_fail_testcases_integer_overflow_index`
- ⏭️ NOT RUN `testcases_memset_overflow.c` → `test_arr00_c_fail_testcases_memset_overflow`
- ⏭️ NOT RUN `testcases_multidim_overflow.c` → `test_arr00_c_fail_testcases_multidim_overflow`
- ⏭️ NOT RUN `testcases_negative_index.c` → `test_arr00_c_fail_testcases_negative_index`
- ⏭️ NOT RUN `testcases_no_bounds_check.c` → `test_arr00_c_fail_testcases_no_bounds_check`
- ⏭️ NOT RUN `testcases_off_by_one.c` → `test_arr00_c_fail_testcases_off_by_one`
- ⏭️ NOT RUN `testcases_out_of_bounds_read.c` → `test_arr00_c_fail_testcases_out_of_bounds_read`
- ⏭️ NOT RUN `testcases_pointer_past_end.c` → `test_arr00_c_fail_testcases_pointer_past_end`
- ⏭️ NOT RUN `testcases_pointer_subtraction_error.c` → `test_arr00_c_fail_testcases_pointer_subtraction_error`
- ⏭️ NOT RUN `testcases_realloc_misuse.c` → `test_arr00_c_fail_testcases_realloc_misuse`
- ⏭️ NOT RUN `testcases_scanf_overflow.c` → `test_arr00_c_fail_testcases_scanf_overflow`
- ⏭️ NOT RUN `testcases_sizeof_misuse.c` → `test_arr00_c_fail_testcases_sizeof_misuse`
- ⏭️ NOT RUN `testcases_sprintf_overflow.c` → `test_arr00_c_fail_testcases_sprintf_overflow`
- ⏭️ NOT RUN `testcases_stack_array_return.c` → `test_arr00_c_fail_testcases_stack_array_return`
- ⏭️ NOT RUN `testcases_strcat_overflow.c` → `test_arr00_c_fail_testcases_strcat_overflow`
- ⏭️ NOT RUN `testcases_strcpy_no_check.c` → `test_arr00_c_fail_testcases_strcpy_no_check`
- ⏭️ NOT RUN `testcases_string_overflow.c` → `test_arr00_c_fail_testcases_string_overflow`
- ⏭️ NOT RUN `testcases_uninitialized_array_access.c` → `test_arr00_c_fail_testcases_uninitialized_array_access`
- ⏭️ NOT RUN `testcases_uninitialized_size.c` → `test_arr00_c_fail_testcases_uninitialized_size`
- ⏭️ NOT RUN `testcases_unvalidated_input.c` → `test_arr00_c_fail_testcases_unvalidated_input`
- ⏭️ NOT RUN `testcases_use_after_free.c` → `test_arr00_c_fail_testcases_use_after_free`
- ⏭️ NOT RUN `testcases_vla_no_check.c` → `test_arr00_c_fail_testcases_vla_no_check`
- ⏭️ NOT RUN `testcases_wrong_size_memcpy.c` → `test_arr00_c_fail_testcases_wrong_size_memcpy`
- ⏭️ NOT RUN `testcases_zero_size_vla.c` → `test_arr00_c_fail_testcases_zero_size_vla`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_array_parameters.c` → `test_arr00_c_pass_testcases_array_parameters`
- ⏭️ NOT RUN `testcases_bounds_checking.c` → `test_arr00_c_pass_testcases_bounds_checking`
- ⏭️ NOT RUN `testcases_multidimensional_arrays.c` → `test_arr00_c_pass_testcases_multidimensional_arrays`
- ⏭️ NOT RUN `testcases_pointer_arithmetic_safe.c` → `test_arr00_c_pass_testcases_pointer_arithmetic_safe`
- ⏭️ NOT RUN `testcases_proper_initialization.c` → `test_arr00_c_pass_testcases_proper_initialization`
- ⏭️ NOT RUN `testcases_safe_array_copy.c` → `test_arr00_c_pass_testcases_safe_array_copy`
- ⏭️ NOT RUN `testcases_safe_iteration.c` → `test_arr00_c_pass_testcases_safe_iteration`
- ⏭️ NOT RUN `testcases_safe_string_operations.c` → `test_arr00_c_pass_testcases_safe_string_operations`
- ⏭️ NOT RUN `testcases_sizeof_correct_usage.c` → `test_arr00_c_pass_testcases_sizeof_correct_usage`
- ⏭️ NOT RUN `testcases_vla_safe_usage.c` → `test_arr00_c_pass_testcases_vla_safe_usage`

---

## Category: CON

<a id="category-con"></a>

**Implementation Status:** 0 / 23 rules (0.0%)

### 🔶 CON03-C - Not Implemented (has tests)

<a id="rule-con03c"></a>

**Title:** Ensure visibility when accessing shared variables

**Description:** Reading a shared primitive variable in one thread may not yield the value of the
most recent write to the variable from another thread. Consequently, the thread
may observe a stale value of the shared variable. To ensure the visibility of
the most recent update, the write to the variable musthappen beforethe read (C
Standard, subclause 5.1.2.4, paragraph 18 [ISO/IEC 9899:2011]). Atomic
operations—other than relaxed atomic operations—trivially satisfy the happens
before relationship. Where atomic operations are inappropriate, protecting both
reads and writes with a mutex also satisfies the happens before relationship.
This noncompliant code example uses ashutdown()method to set the non-
volatiledoneflag that is checked in therun()method. final class ControlledStop
implements Runnable { private boolean done = false; @Override public void run()
{ while (!done) { try { // ... Thread.currentThread().sleep(1000); // Do
something } catch(InterruptedException ie) { Thread.currentThread().interrupt();
// Reset interrupted status } } } public void shutdown() { done = true; } }

**Test Coverage:** 4 tests (1 fail, 3 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_non_volatile_flag.c` → `test_con03_c_fail_wiki_non_volatile_flag`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_atomicboolean.c` → `test_con03_c_pass_wiki_atomicboolean`
- ⏭️ NOT RUN `wiki_synchronized.c` → `test_con03_c_pass_wiki_synchronized`
- ⏭️ NOT RUN `wiki_volatile.c` → `test_con03_c_pass_wiki_volatile`

---

### 🔶 CON35-C - Not Implemented (has tests)

<a id="rule-con35c"></a>

**Title:** Avoid deadlock by locking in a predefined order

**Description:** Mutexes are used to prevent multiple threads from causing a data race by
accessing shared resources at the same time. Sometimes, when locking mutexes,
multiple threads hold each other's lock, and the program consequently deadlocks.
Four conditions are required for deadlock to occur: Deadlock needs all four
conditions, so preventing deadlock requires preventing any one of the four
conditions. One simple solution is to lock the mutexes in a predefined order,
which prevents circular wait. The behavior of this noncompliant code example
depends on the runtime environment and the platform's scheduler. The program is
susceptible to deadlock if threadthr1attempts to lockba2's mutex at the same
time threadthr2attempts to lockba1's mutex in thedeposit()function.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con35_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_con35_c_pass_wiki_compliant_1`

---

### 🔶 CON08-C - Not Implemented (has tests)

<a id="rule-con08c"></a>

**Title:** Do not assume that a group of calls to independently atomic methods is atomic

**Description:** A consistent locking policy guarantees that multiple threads cannot
simultaneously access or modify shared data. When two or more operations must be
performed as a single atomic operation, a consistent locking policy must be
implemented using some form of locking, such as a mutex. In the absence of such
a policy, the code is susceptible to race conditions.When presented with a set
of operations, where each is guaranteed to be atomic, it is tempting to assume
that a single operation consisting of individually-atomic operations is
guaranteed to be collectively atomic without additional locking. A grouping of
calls to such methods requires additional synchronization for the group.Compound
operations on shared variables are also non-atomic. SeeCON07-C. Ensure that
compound operations on shared variables are atomicfor more
information.Noncompliant Code ExampleThis noncompliant code example stores two
integers atomically. It also provides atomic methods to obtain their sum and
product. All methods are locked with the same mutex to provide their
atomicity.#include <threads.h> #include <stdio.h> #include <stdbool.h> static
int a = 0; static int b = 0; mtx_t lock; bool init_mutex(int type) { /* Validate
type */ if (thrd_success != mtx_init(&lock, type)) { return false; /* Report
error */ } return true; } void set_values(int new_a, int new_b) { if
(thrd_success != mtx_lock(&lock)) { /* Handle error */ } a = new_a; b = new_b;
if (thrd_success != mtx_unlock(&lock)) { /* Handle error */ } } int
get_sum(void) { if (thrd_success != mtx_lock(&lock)) { /* Handle error */ } int
sum = a + b; if (thrd_success != mtx_unlock(&lock)) { /* Handle error */ }
return sum; } int get_product(void) { if (thrd_success != mtx_lock(&lock)) { /*
Handle error */ } int product = a * b; if (thrd_success != mtx_unlock(&lock)) {
/* Handle error */ } return product; } /* Can be called by multiple threads */
void multiply_monomials(int x1, int x2) { printf("(x + %d)(x + %d)\n", x1, x2);
set_values( x1, x2); printf("= x^2 + %dx + %d\n", get_sum(), get_product());
}Unfortunately, themultiply_monomials()function is still subject to race
conditions, despite relying exclusively on atomic function calls. It is quite
possible forget_sum()andget_product()to work with different numbers than the
ones that were set byset_values(). It is even possible forget_sum()to operate
with different numbers thanget_product().Compliant SolutionThis compliant
solution locks themultiply_monomials()function with the same mutex lock that is
used by the other functions.For this code to work, the mutex must be recursive.
This is accomplished by making it recursive in theinit_mutex()function.#include
<threads.h> #include <stdio.h> #include <stdbool.h> extern void set_values(int,
int); extern int get_sum(void); extern int get_product(void); mtx_t lock; bool
init_mutex(int type) { /* Validate type */ if (thrd_success != mtx_init(&lock,
type | mtx_recursive)) { return false; /* Report error */ } return true; } /*
Can be called by multiple threads */ void multiply_monomials(int x1, int x2) {
if (thrd_success != mtx_lock(&lock)) { /* Handle error */ } set_values( x1, x2);
int sum = get_sum(); int product = get_product(); if (thrd_success !=
mtx_unlock(&lock)) { /* Handle error */ } printf("(x + %d)(x + %d)\n", x1, x2);
printf("= x^2 + %dx + %d\n", sum, product); }Noncompliant Code ExampleFunction
chaining is a useful design pattern for building an object and setting its
optional fields. The output of one function serves as an argument (typically the
last) in the next function. However, if accessed concurrently, a thread may
observe shared fields to contain inconsistent values. This noncompliant code
example demonstrates a race condition that can occur when multiple threads can
variables with no thread protection.#include <threads.h> #include <stdio.h>
typedef struct currency_s { int quarters; int dimes; int nickels; int pennies; }
currency_t; currency_t *set_quarters(int quantity, currency_t *currency) {
currency->quarters += quantity; return currency; } currency_t *set_dimes(int
quantity, currency_t *currency) { currency->dimes += quantity; return currency;
} currency_t *set_nickels(int quantity, currency_t *currency) {
currency->nickels += quantity; return currency; } currency_t *set_pennies(int
quantity, currency_t *currency) { currency->pennies += quantity; return
currency; } int init_45_cents(void *currency) { currency_t *c = set_quarters(1,
set_dimes(2, currency)); /* Validate values are correct */ return 0; } int
init_60_cents(void* currency) { currency_t *c = set_quarters(2, set_dimes(1,
currency)); /* Validate values are correct */ return 0; } int main(void) {
thrd_t thrd1; thrd_t thrd2; currency_t currency = {0, 0, 0, 0}; if (thrd_success
!= thrd_create(&thrd1, init_45_cents, &currency)) { /* Handle error */ } if
(thrd_success != thrd_create(&thrd2, init_60_cents, &currency)) { /* Handle
error */ } if (thrd_success != thrd_join(thrd1, NULL)) { /* Handle error */ } if
(thrd_success != thrd_join(thrd2, NULL)) { /* Handle error */ } printf("%d
quarters, %d dimes, %d nickels, %d pennies\n", currency.quarters,
currency.dimes, currency.nickels, currency.pennies); return 0; }In this
noncompliant code example, the program constructs a currencystructand starts two
threads that use method chaining to set the optional values of the structure.
This example code might result in the currencystructbeing left in an
inconsistent state, for example, with two quarters and one dime or one quarter
andtwodimes.Noncompliant Code ExampleThis code remains unsafe even if it uses a
mutex on thesetfunctions to guard modification of the currency:#include
<threads.h> #include <stdio.h> typedef struct currency_s { int quarters; int
dimes; int nickels; int pennies; mtx_t lock; } currency_t; currency_t
*set_quarters(int quantity, currency_t *currency) { if (thrd_success !=
mtx_lock(&currency->lock)) { /* Handle error */ } currency->quarters +=
quantity; if (thrd_success != mtx_unlock(&currency->lock)) { /* Handle error */
} return currency; } currency_t *set_dimes(int quantity, currency_t *currency) {
if (thrd_success != mtx_lock(&currency->lock)) { /* Handle error */ }
currency->dimes += quantity; if (thrd_success != mtx_unlock(&currency->lock)) {
/* Handle error */ } return currency; } currency_t *set_nickels(int quantity,
currency_t *currency) { if (thrd_success != mtx_lock(&currency->lock)) { /*
Handle error */ } currency->nickels += quantity; if (thrd_success !=
mtx_unlock(&currency->lock)) { /* Handle error */ } return currency; }
currency_t *set_pennies(int quantity, currency_t *currency) { if (thrd_success
!= mtx_lock(&currency->lock)) { /* Handle error */ } currency->pennies +=
quantity; if (thrd_success != mtx_unlock(&currency->lock)) { /* Handle error */
} return currency; } int init_45_cents(void *currency) { currency_t *c =
set_quarters(1, set_dimes(2, currency)); /* Validate values are correct */
return 0; } int init_60_cents(void* currency) { currency_t *c = set_quarters(2,
set_dimes(1, currency)); /* Validate values are correct */ return 0; } int
main(void) { int result; thrd_t thrd1; thrd_t thrd2; currency_t currency = {0,
0, 0, 0}; if (thrd_success != mtx_init(&currency.lock, mtx_plain)) { /* Handle
error */ } if (thrd_success != thrd_create(&thrd1, init_45_cents, &currency)) {
/* Handle error */ } if (thrd_success != thrd_create(&thrd2, init_60_cents,
&currency)) { /* Handle error */ } if (thrd_success != thrd_join(thrd1, NULL)) {
/* Handle error */ } if (thrd_success != thrd_join(thrd2, NULL)) { /* Handle
error */ } printf("%d quarters, %d dimes, %d nickels, %d pennies\n",
currency.quarters, currency.dimes, currency.nickels, currency.pennies);
mtx_destroy( &currency.lock); return 0; }Compliant SolutionThis compliant
solution uses a mutex, but instead of guarding thesetfunctions, it guards
theinitfunctions, which are invoked at thread creation.#include <threads.h>
#include <stdio.h> typedef struct currency_s { int quarters; int dimes; int
nickels; int pennies; mtx_t lock; } currency_t; currency_t *set_quarters(int
quantity, currency_t *currency) { currency->quarters += quantity; return
currency; } currency_t *set_dimes(int quantity, currency_t *currency) {
currency->dimes += quantity; return currency; } currency_t *set_nickels(int
quantity, currency_t *currency) { currency->nickels += quantity; return
currency; } currency_t *set_pennies(int quantity, currency_t *currency) {
currency->pennies += quantity; return currency; } int init_45_cents(void
*currency) { currency_t *c = (currency_t *)currency; if (thrd_success !=
mtx_lock(&c->lock)) { /* Handle error */ } set_quarters(1, set_dimes(2,
currency)); if (thrd_success != mtx_unlock(&c->lock)) { /* Handle error */ }
return 0; } int init_60_cents(void *currency) { currency_t *c = (currency_t
*)currency; if (thrd_success != mtx_lock(&c->lock)) { /* Handle error */ }
set_quarters(2, set_dimes(1, currency)); if (thrd_success !=
mtx_unlock(&c->lock)) { /* Handle error */ } return 0; } int main(void) { int
result; thrd_t thrd1; thrd_t thrd2; currency_t currency = {0, 0, 0, 0}; if
(thrd_success != mtx_init(&currency.lock, mtx_plain)) { /* Handle error */ } if
(thrd_success != thrd_create(&thrd1, init_45_cents, &currency)) { /* Handle
error */ } if (thrd_success != thrd_create(&thrd2, init_60_cents, &currency)) {
/* Handle error */ } if (thrd_success != thrd_join(thrd1, NULL)) { /* Handle
error */ } if (thrd_success != thrd_join(thrd2, NULL)) { /* Handle error */ }
printf("%d quarters, %d dimes, %d nickels, %d pennies\n", currency.quarters,
currency.dimes, currency.nickels, currency.pennies);
mtx_destroy(&currency.lock); return 0; }Consequently this compliant solution is
thread-safe, and will always print out the same number of quarters as dimes.Risk
AssessmentFailure to ensure the atomicity of two or more operations that must be
performed as a single atomic operation can result in race conditions in
multithreaded applications.RuleSeverityLikelihoodDetectableRepairablePriorityLev
elCON08-CLowProbableNoNoP2L3Related GuidelinesCERT Oracle Secure Coding Standard
for JavaVNA03-J. Do not assume that a group of calls to independently atomic
methods is atomicVNA04-J. Ensure that calls to chained methods are atomicMITRE
CWECWE-362, Concurrent execution using shared resource with improper
synchronization ("race condition")CWE-366,Race condition within a threadCWE-662,
Improper synchronizationBibliography[ISO/IEC 9899:2011]Subclause 7.26, "Threads
<threads.h>"

**Test Coverage:** 5 tests (3 fail, 2 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con08_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_con08_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_con08_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_con08_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_con08_c_pass_wiki_compliant_2`

---

### 🔶 CON38-C - Not Implemented (has tests)

<a id="rule-con38c"></a>

**Title:** Preserve thread safety and liveness when using condition variables

**Description:** Both thread safety andlivenessare concerns when using condition variables.
Thethread-safetyproperty requires that all objects maintain consistent states in
a multithreaded environment [Lea 2000]. Thelivenessproperty requires that every
operation or function invocation execute to completion without interruption; for
example, there is no deadlock. Condition variables must be used inside
awhileloop. (SeeCON36-C. Wrap functions that can spuriously wake up in a loopfor
more information.) To guarantee liveness, programs must test thewhileloop
condition before invoking thecnd_wait()function. This early test checks whether
another thread has already satisfied thecondition predicateand has sent a
notification. Invoking thecnd_wait()function after the notification has been
sent results in indefinite blocking. To guarantee thread safety, programs must
test thewhileloop condition after returning from thecnd_wait()function. When a
given thread invokes thecnd_wait()function, it will attempt to block until its
condition variable is signaled by a call tocnd_broadcast()or tocnd_signal().

**Test Coverage:** 4 tests (1 fail, 3 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_cnd_signal.c` → `test_con38_c_fail_wiki_cnd_signal`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_cnd_broadcast.c` → `test_con38_c_pass_wiki_cnd_broadcast`
- ⏭️ NOT RUN `wiki_usingcnd_signalwith_a_unique_condition_variable_per_thread.c` → `test_con38_c_pass_wiki_usingcnd_signalwith_a_unique_condition_variable_per_thread`
- ⏭️ NOT RUN `wiki_windows_condition_variables.c` → `test_con38_c_pass_wiki_windows_condition_variables`

---

### 🔶 CON37-C - Not Implemented (has tests)

<a id="rule-con37c"></a>

**Title:** Do not call signal() in a multithreaded program

**Description:** Calling thesignal()function in a multithreaded program isundefined behavior.
(Seeundefined behavior 135.) This noncompliant code example invokes
thesignal()function from a multithreaded program: #include <signal.h> #include
<stddef.h> #include <threads.h> volatile sig_atomic_t flag = 0; void handler(int
signum) { flag = 1; } /* Runs until user sends SIGUSR1 */ int func(void *data) {
while (!flag) { /* ... */ } return 0; } int main(void) { signal(SIGUSR1,
handler); /* Undefined behavior */ thrd_t tid; if (thrd_success !=
thrd_create(&tid, func, NULL)) { /* Handle error */ } /* ... */ return 0; }

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con37_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_con37_c_pass_wiki_compliant_1`

---

### 🔶 CON39-C - Not Implemented (has tests)

<a id="rule-con39c"></a>

**Title:** Do not join or detach a thread that was previously joined or detached

**Description:** The C Standard, 7.28.5.6 paragraph 2 [ISO/IEC 9899:2024], states that a thread
shall not be joined once it was previously joined or detached. Similarly,
subclause 7.28.5.3 paragraph 2 [ISO/IEC 9899:2024], states that a thread shall
not be detached once it was previously joined or detached. Violating either of
these subclauses results inundefined behavior 211.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con39_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_con39_c_pass_wiki_compliant_1`

---

### 🔶 CON04-C - Not Implemented (has tests)

<a id="rule-con04c"></a>

**Title:** Join or detach threads even if their exit status is unimportant

**Description:** Thethrd_detach()function is used to tell the underlying system that resources
allocated to a particular thread can be reclaimed once it terminates. This
function should be used when a thread's exit status is not required by other
threads (and no other thread needs to usethrd_join()to wait for it to complete).
Whenever a thread terminates without detaching, the thread's stack is
deallocated, but some other resources, including the thread ID and exit status,
are left until it is destroyed by eitherthrd_join()orthrd_detach(). These
resources can be vital for systems with limited resources and can lead to
various "resource unavailable" errors, depending on which critical resource gets
used up first. For example, if the system has a limit (either per-process or
system wide) on the number of thread IDs it can keep track of, failure to
release the thread ID of a terminated thread may lead tothrd_create() being
unable tocreate another thread. This noncompliant code example shows a pool of
threads that are not exited correctly:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con04_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_con04_c_pass_wiki_compliant_1`

---

### 🔶 CON31-C - Not Implemented (has tests)

<a id="rule-con31c"></a>

**Title:** Do not destroy a mutex while it is locked

**Description:** Mutexes are used to protect shared data structures being concurrently accessed.
If a mutex is destroyed while a thread is blocked waiting for that
mutex,critical sectionsand shared data are no longer protected. The C Standard,
7.28.4.1, paragraph 2 [ISO/IEC 9899:2024], states This statement implies that
destroying a mutex while a thread is waiting on it isundefined behavior.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con31_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_con31_c_pass_wiki_compliant_1`

---

### 🔶 CON36-C - Not Implemented (has tests)

<a id="rule-con36c"></a>

**Title:** Wrap functions that can spuriously wake up in a loop

**Description:** Thecnd_wait()andcnd_timedwait()functionstemporarily cede possession of a mutex
so that other threads that may be requesting the mutex can proceed. These
functions must always be called from code that is protected by locking a mutex.
The waiting thread resumes execution only after it has been notified, generally
as the result of the invocation of thecnd_signal()orcnd_broadcast()function
invoked by another thread. Thecnd_wait()function must be invoked from a loop
that checks whether acondition predicateholds. A condition predicate is an
expression constructed from the variables of a function that must be true for a
thread to be allowed to continue execution. The thread pauses execution,
viacnd_wait(),cnd_timedwait(), or some other mechanism, and is resumed later,
presumably when the condition predicate is true and the thread is notified.
#include <threads.h> #include <stdbool.h> extern bool until_finish(void); extern
mtx_t lock; extern cnd_t condition; void func(void) { if (thrd_success !=
mtx_lock(&lock)) { /* Handle error */ } while (until_finish()) { /* Predicate
does not hold */ if (thrd_success != cnd_wait(&condition, &lock)) { /* Handle
error */ } } /* Resume when condition holds */ if (thrd_success !=
mtx_unlock(&lock)) { /* Handle error */ } } The notification mechanism notifies
the waiting thread and allows it to check its condition predicate. The
invocation ofcnd_broadcast()in another thread cannot precisely determine which
waiting thread will be resumed. Condition predicate statements allow notified
threads to determine whether they should resume upon receiving the notification.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con36_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_con36_c_pass_wiki_compliant_1`

---

### 🔶 CON01-C - Not Implemented (has tests)

<a id="rule-con01c"></a>

**Title:** Acquire and release synchronization primitives in the same module, at the same level of abstraction

**Description:** All locking and unlocking of mutexes should be performed in the same module and
at the same level of abstraction. Failure to follow this recommendation can lead
to some lock or unlock operations not being executed by the multithreaded
program as designed, eventually resulting in deadlock, race conditions, or other
securityvulnerabilities, depending on the mutex type. A common consequence of
improper locking is for a mutex to be unlocked twice, via two calls
tomtx_unlock(). This can cause the unlock operation to return errors. In the
case of recursive mutexes, an error is returned only if the lock count is 0
(making the mutex available to other threads) and a call tomtx_unlock()is made.
In this noncompliant code example for a simplified multithreaded banking system,
imagine an account with a required minimum balance. The code would need to
verify that all debit transactions are allowable. Suppose a call is made
todebit()asking to withdraw funds that would
bringaccount_balancebelowMIN_BALANCE, which would result in two calls
tomtx_unlock(). In this example, because the mutex is defined statically, the
mutex type isimplementation-defined.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con01_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_con01_c_pass_wiki_compliant_1`

---

### 🔶 CON41-C - Not Implemented (has tests)

<a id="rule-con41c"></a>

**Title:** Wrap functions that can fail spuriously in a loop

**Description:** Functions that can fail spuriously should be wrapped in a loop. Theatomic_compar
e_exchange_weak()andatomic_compare_exchange_weak_explicit()functions both
attempt to set an atomic variable to a new value but only if it currently
possesses a known old value. Unlike the related functionsatomic_compare_exchange
_strong()andatomic_compare_exchange_strong_explicit(), these functions are
permitted tofail spuriously. This makes these functions faster on some
platforms—for example, on architectures that implement compare-and-exchange
using load-linked/store-conditional instructions, such as Alpha, ARM, MIPS, and
PowerPC. The C Standard, 7.17.7.4, paragraph 5 [ISO/IEC 9899:2024], describes
this behavior:A weak compare-and-exchange operation may fail spuriously. That
is, even when the contents of memory referred to byexpectedandobjectare equal,
it may return zero and store back toexpectedthe same memory contents that were
originally there.Noncompliant Code ExampleIn this noncompliant code
example,reorganize_data_structure()is to be used as an argument tothrd_create().
After reorganizing, the function attempts to replace the head pointer so that it
points to the new version. If no other thread has changed the head pointer since
it was originally loaded,reorganize_data_structure()is intended to exit the
thread with a result oftrue, indicating success. Otherwise, the new
reorganization attempt is discarded and the thread is exited with a result
offalse. However,atomic_compare_exchange_weak()may fail even when the head
pointer has not changed. Therefore,reorganize_data_structure()may perform the
work and then discard it unnecessarily.#include <stdatomic.h> #include
<stdbool.h> struct data { struct data *next; /* ... */ }; extern void
cleanup_data_structure(struct data *head); int reorganize_data_structure(void
*thread_arg) { struct data *_Atomic *ptr_to_head = thread_arg; struct data
*old_head = atomic_load(ptr_to_head); struct data *new_head; bool success; /*
... Reorganize the data structure ... */ success =
atomic_compare_exchange_weak(ptr_to_head, &old_head, new_head); if (!success) {
cleanup_data_structure(new_head); } return success; /* Exit the thread */
}Compliant Solution (atomic_compare_exchange_weak())To recover from spurious
failures, a loop must be used. However,atomic_compare_exchange_weak()might fail
because the head pointer changed, or the failure may be spurious. In either
case, the thread must perform the work repeatedly until the compare-and-exchange
succeeds, as shown in this compliant solution:#include <stdatomic.h> #include
<stdbool.h> #include <stddef.h> struct data { struct data *next; /* ... */ };
extern void cleanup_data_structure(struct data *head); int
reorganize_data_structure(void *thread_arg) { struct data *_Atomic *ptr_to_head
= thread_arg; struct data *old_head = atomic_load(ptr_to_head); struct data
*new_head = NULL; struct data *saved_old_head; bool success; do { if (new_head
!= NULL) { cleanup_data_structure(new_head); } saved_old_head = old_head; /* ...
Reorganize the data structure ... */ } while (!(success =
atomic_compare_exchange_weak( ptr_to_head, &old_head, new_head )) && old_head ==
saved_old_head); return success; /* Exit the thread */ }This loop could also be
part of a larger control flow; for example, the thread from the noncompliant
code example could be retried if it returnsfalse.Compliant Solution
(atomic_compare_exchange_strong())When a weak compare-and-exchange would require
a loop and a strong one would not, the strong one is preferable, as in this
compliant solution:#include <stdatomic.h> #include <stdbool.h> struct data {
struct data *next; /* ... */ }; extern void cleanup_data_structure(struct data
*head); int reorganize_data_structure(void *thread_arg) { struct data *_Atomic
*ptr_to_head = thread_arg; struct data *old_head = atomic_load(ptr_to_head);
struct data *new_head; bool success; /* ... Reorganize the data structure ... */
success = atomic_compare_exchange_strong( ptr_to_head, &old_head, new_head ); if
(!success) { cleanup_data_structure(new_head); } return success; /* Exit the
thread */ }Risk AssessmentFailing to wrap theatomic_compare_exchange_weak()andat
omic_compare_exchange_weak_explicit()functions in a loop can result in incorrect
values and control flow.RuleSeverityLikelihoodDetectableRepairablePriorityLevelC
ON41-CLowUnlikelyYesNoP2L3Automated DetectionToolVersionCheckerDescriptionCodeSo
nar9.1p0LANG.STRUCT.ICOLInappropriate Call Outside
LoopCoverity2017.07BAD_CHECK_OF_WAIT_CONDImplementedCppcheck
Premium24.11.0premium-cert-con41-cHelix
QAC2025.2C2026C++5023Klocwork2025.2CERT.CONC.ATOMIC_COMP_FAIL_IN_LOOPParasoft
C/C++test2024.2CERT_C-CON41-aWrap functions that can fail spuriously in a
loopPolyspace Bug FinderR2025bCERT C: Rule CON41-CChecks for situations where
functions that can spuriously fail are not wrapped in loop (rule fully
covered)Related VulnerabilitiesSearch forvulnerabilitiesresulting from the
violation of this rule on theCERT website.Related GuidelinesKey here(explains
table format and definitions)TaxonomyTaxonomy itemRelationshipCERT Oracle Secure
Coding Standard for JavaTHI03-J. Always invoke wait() and await() methods inside
a loopPrior to 2018-01-12: CERT: Unspecified RelationshipBibliography[ISO/IEC
9899:2024]7.17.7.4, "Theatomic_compare_exchangeGeneric Functions"[Lea
2000]1.3.2, "Liveness"3.2.2, "Monitor Mechanics"

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con41_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_atomic_compare_exchange_strong.c` → `test_con41_c_pass_wiki_atomic_compare_exchange_strong`
- ⏭️ NOT RUN `wiki_atomic_compare_exchange_weak.c` → `test_con41_c_pass_wiki_atomic_compare_exchange_weak`

---

### 🔶 CON32-C - Not Implemented (has tests)

<a id="rule-con32c"></a>

**Title:** Prevent data races when accessing bit-fields from multiple threads

**Description:** When accessing a bit-field, a thread may inadvertently access a separate bit-
field in adjacent memory. This is because compilers are required to store
multiple adjacent bit-fields in one storage unit whenever they fit.
Consequently, data races may exist not just on a bit-field accessed by multiple
threads but also on other bit-fields sharing the same byte or word. A similar
problem is discussed inCON43-C. Do not allow data races in multithreaded code,
but the issue described by this rule can be harder to diagnose because it may
not be obvious that the same memory location is being modified by multiple
threads. One approach for preventing data races in concurrent programming is to
use a mutex. When properly observed by all threads, a mutex can provide safe and
secure access to a shared object. However, mutexes provide no guarantees with
regard to other objects that might be accessed when the mutex is not controlled
by the accessing thread. Unfortunately, there is no portable way to determine
which adjacent bit-fields may be stored along with the desired bit-field.
Another approach is to insert a non-bit-field member between any two bit-fields
to ensure that each bit-field is the only one accessed within its storage unit.
This technique effectively guarantees that no two bit-fields are accessed
simultaneously.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_bit_field.c` → `test_con32_c_fail_wiki_bit_field`
- ⏭️ NOT RUN `wiki_bit_field_2.c` → `test_con32_c_fail_wiki_bit_field_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_bit_field_c11_mutex.c` → `test_con32_c_pass_wiki_bit_field_c11_mutex`
- ⏭️ NOT RUN `wiki_c11.c` → `test_con32_c_pass_wiki_c11`

---

### 🔶 CON34-C - Not Implemented (has tests)

<a id="rule-con34c"></a>

**Title:** Declare objects shared between threads with appropriate storage durations

**Description:** Accessing the automatic or thread-local variables of one thread from another
thread isimplementation-defined behaviorand can cause invalid memory accesses
because the execution of threads can be interwoven within the constraints of the
synchronization model. As a result, the referenced stack frame or thread-local
variable may no longer be valid when another thread tries to access it. Shared
static variables can be protected by thread synchronization mechanisms. However,
automatic (local) variables cannot be shared in the same manner because the
referenced stack frame's thread would need to stop executing, or some other
mechanism must be employed to ensure that the referenced stack frame is still
valid. Do not access automatic or thread-local objects from a thread other than
the one with which the object is associated. SeeDCL30-C. Declare objects with
appropriate storage durationsfor information on how to declare objects with
appropriate storage durations when data is not being shared between threads.
Noncompliant Code Example (Automatic Storage Duration)

**Test Coverage:** 8 tests (3 fail, 5 pass)

**Test Results:** 0/8 passed (0.0%), 8 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_automatic_storage_duration.c` → `test_con34_c_fail_wiki_automatic_storage_duration`
- ⏭️ NOT RUN `wiki_openmpparallel.c` → `test_con34_c_fail_wiki_openmpparallel`
- ⏭️ NOT RUN `wiki_thread_specific_storage.c` → `test_con34_c_fail_wiki_thread_specific_storage`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_allocated_storage_duration.c` → `test_con34_c_pass_wiki_allocated_storage_duration`
- ⏭️ NOT RUN `wiki_openmpparallelprivate.c` → `test_con34_c_pass_wiki_openmpparallelprivate`
- ⏭️ NOT RUN `wiki_static_storage_duration.c` → `test_con34_c_pass_wiki_static_storage_duration`
- ⏭️ NOT RUN `wiki_thread_local_storage_windows_visual_studio.c` → `test_con34_c_pass_wiki_thread_local_storage_windows_visual_studio`
- ⏭️ NOT RUN `wiki_thread_specific_storage.c` → `test_con34_c_pass_wiki_thread_specific_storage`

---

### 🔶 CON50-C - Not Implemented (has tests)

<a id="rule-con50c"></a>

**Title:** PP. Do not destroy a mutex while it is locked

**Description:** Mutex objects are used to protect shared data from being concurrently accessed.
If a mutex object is destroyed while a thread is blocked waiting for the
lock,critical sectionsand shared data are no longer protected. The C++ Standard,
[thread.mutex.class], paragraph 5 [ISO/IEC 14882-2014], states the following:
Similar wording exists
forstd::recursive_mutex,std::timed_mutex,std::recursive_timed_mutex,
andstd::shared_timed_mutex. These statements imply that destroying a mutex
object while a thread is waiting on it isundefined behavior.

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con50_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_con50_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_con50_c_pass_wiki_compliant_2`

---

### 🔶 CON09-C - Not Implemented (has tests)

<a id="rule-con09c"></a>

**Title:** Avoid the ABA problem when using lock-free algorithms

**Description:** Lock-free programming is a technique that allows concurrent updates of shared
data structures without using explicit locks. This method ensures that no
threads block for arbitrarily long times, and it thereby boosts performance.
Lock-free programming has the following advantages: Lock-free programming
requires the use of special atomic processor instructions, such as CAS (compare
and swap), LL/SC (load linked/store conditional), or the C
Standardatomic_compare_exchangegeneric functions.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_gnu_glib.c` → `test_con09_c_fail_wiki_gnu_glib`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con09_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_gnu_glib_mutex.c` → `test_con09_c_pass_wiki_gnu_glib_mutex`
- ⏭️ NOT RUN `wiki_mutex.c` → `test_con09_c_pass_wiki_mutex`

---

### ⚫ CON06-C - Not Implemented (no tests)

<a id="rule-con06c"></a>

**Title:** Ensure that every mutex outlives the data it protects

**Description:** Programs must not use instance locks to protect static shared data because
instance locks are ineffective when two or more instances of the class are
created. Consequently, failure to use a static lock object leaves the shared
state unprotected against concurrent access. Lock objects for classes that can
interact with untrusted code must also be private and final, as shown in
ruleLCK00-J. Use private final lock objects to synchronize classes that may
interact with untrusted code. This noncompliant code example attempts to guard
access to the staticcounterfield using a non-static lock object. When
twoRunnabletasks are started, they create two instances of the lock object and
lock on each instance separately. publicfinalclassCountBoxesimplementsRunnable
{privatestaticvolatileintcounter;// ...privatefinalObject lock
=newObject();@Overridepublicvoidrun() {synchronized(lock) {counter++;//
...}}publicstaticvoidmain(String[] args) {for(inti =0; i <2; i++)
{newThread(newCountBoxes()).start();}}}

**Test Coverage:** 0 tests (0 fail, 0 pass)

---

### 🔶 CON05-C - Not Implemented (has tests)

<a id="rule-con05c"></a>

**Title:** Do not perform operations that can block while holding a lock

**Description:** If a lock is being held and an operation that can block is performed, any other
thread that needs to acquire that lock may also block. This condition can
degrade system performance or cause a deadlock to occur. Blocking calls include,
but are not limited to, network, file, and console I/O. Using a blocking
operation while holding a lock may be unavoidable for a portable solution. For
instance, file access could be protected via a lock to prevent multiple threads
from mutating the contents of the file. Or, a thread may be required to block
while holding one or more locks and waiting to acquire another lock. In these
cases, attempt to hold the lock for the least time required. Additionally, if
acquiring multiple locks, the order of locking must avoid deadlock, as specified
inCON35-C. Avoid deadlock by locking in a predefined order. This noncompliant
example callsfopen()while a mutex is locked. The calls tofopen()andfclose()are
blocking and may block for an extended period of time if the file resides on a
network drive. While the call is blocked, other threads that are waiting for the
lock are also blocked.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con05_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_block_while_not_locked.c` → `test_con05_c_pass_wiki_block_while_not_locked`

---

### 🔶 CON33-C - Not Implemented (has tests)

<a id="rule-con33c"></a>

**Title:** Avoid race conditions when using library functions

**Description:** Some C standard library functions are not guaranteed to bereentrantwith respect
to threads. Functions such asstrtok()andasctime()return a pointer to the result
stored in function-allocated memory on a per-process basis. Other functions such
asrand()store state information in function-allocated memory on a per-process
basis. Multiple threads invoking the same function can cause concurrency
problems, which often result inabnormal behaviorand can cause more
seriousvulnerabilities, such asabnormal termination,denial-of-service attack,
and data integrity violations. According to the C Standard, the library
functions listed in the following table may contain data races when invoked by
multiple threads. FunctionsRemediationrand(),srand()MSC30-C. Do not use the
rand() function for generating pseudorandom numbersgetenv()ENV34-C. Do not store
pointers returned by certain functionsstrtok()strtok_r()in
POSIXstrerror()strerror_r()in
POSIXasctime(),ctime(),localtime(),gmtime()strftime()setlocale()Protect
multithreaded access to locale-specific functions with a
mutexATOMIC_VAR_INIT,atomic_init()Do not attempt to initialize an atomic
variable from multiple threadstmpnam()tmpnam_r()in
POSIXmbrtoc16(),c16rtomb(),mbrtoc32(),c32rtomb()Do not call with a nullmbstate_t
*argument

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con33_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posixstrerror_r.c` → `test_con33_c_pass_wiki_posixstrerror_r`

---

### 🔶 CON43-C - Not Implemented (has tests)

<a id="rule-con43c"></a>

**Title:** Do not allow data races in multithreaded code

**Description:** When multiple threads can read or modify the same data, use synchronization
techniques to avoid software flaws that can lead to securityvulnerabilities.Data
racescan often result inabnormal terminationordenial of service, but it is
possible for them to result in more serious vulnerabilities. The C Standard,
section 5.1.2.5, paragraph 35 [ISO/IEC 9899:2024], says: Assume this simplified
code is part of a multithreaded bank system. Threads callcredit()anddebit()as
money is deposited into and withdrawn from the single account. Because the
addition and subtraction operations are not atomic, it is possible that two
operations can occur concurrently, but only the result of one would be
saved—despite declaring theaccount_balancevolatile. For example, an attacker can
credit the account with a sum of money and make a large number of small debits
concurrently. Some of the debits might not affect the account balance because of
the race condition, so the attacker is effectively creating money. static
volatile int account_balance; void debit(int amount) { account_balance -=
amount; } void credit(int amount) { account_balance += amount; }

**Test Coverage:** 7 tests (3 fail, 4 pass)

**Test Results:** 0/7 passed (0.0%), 7 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_double_fetch.c` → `test_con43_c_fail_wiki_double_fetch`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con43_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_volatile.c` → `test_con43_c_fail_wiki_volatile`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_atomic.c` → `test_con43_c_pass_wiki_atomic`
- ⏭️ NOT RUN `wiki_c11_atomic.c` → `test_con43_c_pass_wiki_c11_atomic`
- ⏭️ NOT RUN `wiki_c11_fences.c` → `test_con43_c_pass_wiki_c11_fences`
- ⏭️ NOT RUN `wiki_mutex.c` → `test_con43_c_pass_wiki_mutex`

---

### 🔶 CON02-C - Not Implemented (has tests)

<a id="rule-con02c"></a>

**Title:** Do not use volatile as a synchronization primitive

**Description:** The C Standard, subclause 5.1.2.3, paragraph 2 [ISO/IEC 9899:2011], says:
Thevolatilekeyword informs the compiler that the qualified variable may change
in ways that cannot be determined; consequently, compiler optimizations must be
restricted for memory areas marked asvolatile. For example, the compiler is
forbidden to load the value into a register and subsequently reuse the loaded
value rather than accessing memory directly. This concept relates to
multithreading because incorrect caching of a shared variable may interfere with
the propagation of modified values between threads, causing some threads to view
stale data. Thevolatilekeyword is sometimes misunderstood to provide atomicity
for variables that are shared between threads in a multithreaded program.
Because the compiler is forbidden to either cache variables declared
asvolatilein registers or to reorder the sequence of reads and writes to any
given volatile variable, many programmers mistakenly believe that volatile
variables can correctly serve as synchronization primitives. Although the
compiler is forbidden to reorder the sequence of reads and writes to a
particular volatile variable, it may legally reorder these reads and writes with
respect to reads and writes toothermemory locations. This reordering alone is
sufficient to make volatile variables unsuitable for use as synchronization
primitives.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con02_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_con02_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_con02_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_critical_section_windows.c` → `test_con02_c_pass_wiki_critical_section_windows`

---

### 🔶 CON40-C - Not Implemented (has tests)

<a id="rule-con40c"></a>

**Title:** Do not refer to an atomic variable twice in an expression

**Description:** A consistent locking policy guarantees that multiple threads cannot
simultaneously access or modify shared data. Atomic variables eliminate the need
for locks by guaranteeing thread safety when certain operations are performed on
them. The thread-safe operations on atomic variables are specified in the C
Standard, subclauses 7.17.7 and 7.17.8 [ISO/IEC 9899:2024]. While atomic
operations can be combined, combined operations do not provide the thread safety
provided by individual atomic operations. Every time an atomic variable appears
on the left side of an assignment operator, including a compound assignment
operator such as*=, an atomic write is performed on the variable. The use of the
increment (++)or decrement(--)operators on an atomic variable constitutes an
atomic read-and-write operation and is consequently thread-safe. Any reference
of an atomic variable anywhere else in an expression indicates a distinct atomic
read on the variable. If the same atomic variable appears twice in an
expression, then two atomic reads, or an atomic read and an atomic write, are
required. Such a pair of atomic operations is not thread-safe, as another thread
can modify the atomic variable between the two operations. Consequently, an
atomic variable must not be referenced twice in the same expression.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_atomic_bool.c` → `test_con40_c_fail_wiki_atomic_bool`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_con40_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_con40_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compound_assignment.c` → `test_con40_c_pass_wiki_compound_assignment`

---

### 🔶 CON30-C - Not Implemented (has tests)

<a id="rule-con30c"></a>

**Title:** Clean up thread-specific storage

**Description:** Thetss_create()function creates a thread-specific storage pointer identified by
a key. Threads can allocate thread-specific storage and associate the storage
with a key that uniquely identifies the storage by calling thetss_set()function.
If not properly freed, this memory may be leaked. Ensure that thread-specific
storage is freed. In this noncompliant code example, each thread dynamically
allocates storage in theget_data()function, which is then associated with the
global key by the call totss_set()in theadd_data()function. This memory is
subsequently leaked when the threads terminate. #include <threads.h> #include
<stdlib.h> /* Global key to the thread-specific storage */ tss_t key; enum {
MAX_THREADS = 3 }; int *get_data(void) { int *arr = (int *)malloc(2 *
sizeof(int)); if (arr == NULL) { return arr; /* Report error */ } arr[0] = 10;
arr[1] = 42; return arr; } int add_data(void) { int *data = get_data(); if (data
== NULL) { return -1; /* Report error */ } if (thrd_success != tss_set(key,
(void *)data)) { /* Handle error */ } return 0; } void print_data(void) { /* Get
this thread's global data from key */ int *data = tss_get(key); if (data !=
NULL) { /* Print data */ } } int function(void *dummy) { if (add_data() != 0) {
return -1; /* Report error */ } print_data(); return 0; } int main(void) {
thrd_t thread_id[MAX_THREADS]; /* Create the key before creating the threads */
if (thrd_success != tss_create(&key, NULL)) { /* Handle error */ } /* Create
threads that would store specific storage */ for (size_t i = 0; i < MAX_THREADS;
i++) { if (thrd_success != thrd_create(&thread_id[i], function, NULL)) { /*
Handle error */ } } for (size_t i = 0; i < MAX_THREADS; i++) { if (thrd_success
!= thrd_join(thread_id[i], NULL)) { /* Handle error */ } } tss_delete(key);
return 0; }

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_con30_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_con30_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_con30_c_pass_wiki_compliant_2`

---

### 🔶 CON07-C - Not Implemented (has tests)

<a id="rule-con07c"></a>

**Title:** Ensure that compound operations on shared variables are atomic

**Description:** Compound operations are operations that consist of more than one discrete
operation. Expressions that include postfix or prefix increment (++), postfix or
prefix decrement (--), or compound assignment operators always result in
compound operations. Compound assignment expressions use operators such
as*=,/=,%=,+=,-=,<<=,>>=,^=, and|=. Compound operations on shared variables must
be performed atomically to preventdata races.Noncompliant Code Example (Logical
Negation)This noncompliant code example declares a shared_Boolflagvariable and
provides atoggle_flag()method that negates the current value offlag:#include
<stdbool.h> static bool flag = false; void toggle_flag(void) { flag = !flag; }
bool get_flag(void) { return flag; }Execution of this code may result in adata
racebecause the value offlagis read, negated, and written back.Consider, for
example, two threads that calltoggle_flag(). The expected effect of
togglingflagtwice is that it is restored to its original value. However, the
following scenario leavesflagin the incorrect
state:Timeflag=ThreadAction1truet1Reads the current value offlag,true, into a
cache2truet2Reads the current value offlag, (still)true, into a different
cache3truet1Toggles the temporary variable in the cache tofalse4truet2Toggles
the temporary variable in the different cache tofalse5falset1Writes the cache
variable's value toflag6falset2Writes the different cache variable's value
toflagAs a result, the effect of the call byt2is not reflected inflag; the
program behaves as iftoggle_flag()was called only once, not twice.Compliant
Solution (Mutex)This compliant solution restricts access toflagunder a mutex
lock:#include <threads.h> #include <stdbool.h> static bool flag = false; mtx_t
flag_mutex; /* Initialize flag_mutex */ bool init_mutex(int type) { /* Check
mutex type */ if (thrd_success != mtx_init(&flag_mutex, type)) { return false;
/* Report error */ } return true; } void toggle_flag(void) { if (thrd_success !=
mtx_lock(&flag_mutex)) { /* Handle error */ } flag = !flag; if (thrd_success !=
mtx_unlock(&flag_mutex)) { /* Handle error */ } } bool get_flag(void) { bool
temp_flag; if (thrd_success != mtx_lock(&flag_mutex)) { /* Handle error */ }
temp_flag = flag; if (thrd_success != mtx_unlock(&flag_mutex)) { /* Handle error
*/ } return temp_flag; }This solution guards reads and writes to theflagfield
with a lock on theflag_mutex. This lock ensures that changes toflagare visible
to all threads. Now, only two execution orders are possible. In one execution
order, t1obtains the mutex and completes the operation beforet2can acquire the
mutex, as shown here:Timeflag=ThreadAction1truet1Reads the current value
offlag,true, into a cache variable2truet1Toggles the cache variable
tofalse3falset1Writes the cache variable's value toflag4falset2Reads the current
value offlag,false, into a different cache variable5falset2Toggles the different
cache variable totrue6truet2Writes the different cache variable's value
toflagThe other execution order is similar, except thatt2starts and finishes
beforet1.Compliant Solution (atomic_compare_exchange_weak())This compliant
solution uses atomic variables and a compare-and-exchange operation to guarantee
that the correct value is stored inflag. All updates are visible to other
threads.#include <stdatomic.h> #include <stdbool.h> static atomic_bool flag;
void init_flag(void) { atomic_init(&flag, false); } void toggle_flag(void) {
bool old_flag = atomic_load(&flag); bool new_flag; do { new_flag = !old_flag; }
while (!atomic_compare_exchange_weak(&flag, &old_flag, new_flag)); } bool
get_flag(void) { return atomic_load(&flag); }An alternative solution is to use
theatomic_flagdata type for managing Boolean values atomically.Noncompliant Code
Example (Addition of Primitives)In this noncompliant code example, multiple
threads can invoke theset_values()method to set theaandbfields. Because this
code fails to test for integer overflow, users of this code must also ensure
that the arguments to theset_values()method can be added without overflow
(seeINT32-C. Ensure that operations on signed integers do not result in
overflowfor more information).static int a; static int b; int get_sum(void) {
return a + b; } void set_values(int new_a, int new_b) { a = new_a; b = new_b;
}Theget_sum()method contains a race condition. For example, whenaandbcurrently
have the values0andINT_MAX, respectively, and one thread callsget_sum()while
another callsset_values(INT_MAX, 0), theget_sum()method might return
either0orINT_MAX, or it might overflow. Overflow will occur when the first
thread readsaandbafter the second thread has set the value ofatoINT_MAXbut
before it has set the value ofbto0.Noncompliant Code Example (Addition of Atomic
Integers)In this noncompliant code example,aandbare replaced with atomic
integers.#include <stdatomic.h> static atomic_int a; static atomic_int b; void
init_ab(void) { atomic_init(&a, 0); atomic_init(&b, 0); } int get_sum(void) {
return atomic_load(&a) + atomic_load(&b); } void set_values(int new_a, int
new_b) { atomic_store(&a, new_a); atomic_store(&b, new_b); }The simple
replacement of the twointfields with atomic integers fails to eliminate the race
condition in the sum because the compound operationa.get() + b.get()is still
non-atomic. While a sum of some value ofaand some value ofbwill be returned,
there is no guarantee that this value represents the sum of the values ofaandbat
any particular moment.Compliant Solution (_Atomic struct)This compliant solution
uses an atomic struct, which guarantees that both numbers are read and written
together.#include <stdatomic.h> static _Atomic struct ab_s { int a, b; } ab;
void init_ab(void) { struct ab_s new_ab = {0, 0}; atomic_init(&ab, new_ab); }
int get_sum(void) { struct ab_s new_ab = atomic_load(&ab); return new_ab.a +
new_ab.b; } void set_values(int new_a, int new_b) { struct ab_s new_ab = {new_a,
new_b}; atomic_store(&ab, new_ab); }On most modern platforms, this will compile
to be lock-free.Compliant Solution (Mutex)This compliant solution protects
theset_values()andget_sum()methods with a mutex to ensure atomicity:#include
<threads.h> #include <stdbool.h> static int a; static int b; mtx_t flag_mutex;
/* Initialize everything */ bool init_all(int type) { /* Check mutex type */ a =
0; b = 0; if (thrd_success != mtx_init(&flag_mutex, type)) { return false; /*
Report error */ } return true; } int get_sum(void) { if (thrd_success !=
mtx_lock(&flag_mutex)) { /* Handle error */ } int sum = a + b; if (thrd_success
!= mtx_unlock(&flag_mutex)) { /* Handle error */ } return sum; } void
set_values(int new_a, int new_b) { if (thrd_success != mtx_lock(&flag_mutex)) {
/* Handle error */ } a = new_a; b = new_b; if (thrd_success !=
mtx_unlock(&flag_mutex)) { /* Handle error */ } }Thanks to the mutex, it is now
possible to add overflow checking to theget_sum()function without introducing
the possibility of a race condition.Risk AssessmentWhen operations on shared
variables are not atomic, unexpected results can be produced. For example,
information can be disclosed inadvertently because one user can receive
information about other users.RuleSeverityLikelihoodDetectableRepairablePriority
LevelCON07-CMediumProbableYesNoP8L2Automated
DetectionToolVersionCheckerDescriptionCodeSonar9.1p0CONCURRENCY.DATARACEData
RaceHelix QAC2025.2C1765C1114C1115C1116Related GuidelinesCERT Oracle Secure
Coding Standard for JavaVNA02-J. Ensure that compound operations on shared
variables are atomicMITRE CWECWE-366, Race condition within a
threadCWE-413,Improper resource lockingCWE-567,Unsynchronized access to shared
data in a multithreaded contextCWE-667, Improper lockingBibliography[ISO/IEC
14882:2011]Subclause 7.17, "Atomics"

**Test Coverage:** 4 tests (1 fail, 3 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_addition_of_primitives.c` → `test_con07_c_fail_wiki_addition_of_primitives`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_atomic_compare_exchange_weak.c` → `test_con07_c_pass_wiki_atomic_compare_exchange_weak`
- ⏭️ NOT RUN `wiki_atomic_struct.c` → `test_con07_c_pass_wiki_atomic_struct`
- ⏭️ NOT RUN `wiki_mutex.c` → `test_con07_c_pass_wiki_mutex`

---

## Category: DCL

<a id="category-dcl"></a>

**Implementation Status:** 4 / 31 rules (12.9%)

### 🔶 DCL23-C - Not Implemented (has tests)

<a id="rule-dcl23c"></a>

**Title:** Guarantee that mutually visible identifiers are unique

**Description:** According to subclause 6.2.7 of the C Standard [ISO/IEC 9899:2011], (See
alsoundefined behavior 14of Annex J.) Further, according to subclause 6.4.2.1,

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_source_character_set.c` → `test_dcl23_c_fail_wiki_source_character_set`
- ⏭️ NOT RUN `wiki_universal_character_names.c` → `test_dcl23_c_fail_wiki_universal_character_names`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_source_character_set.c` → `test_dcl23_c_pass_wiki_source_character_set`
- ⏭️ NOT RUN `wiki_universal_character_names.c` → `test_dcl23_c_pass_wiki_universal_character_names`

---

### ✅ DCL03-C - Implemented

<a id="rule-dcl03c"></a>

**Title:** Use a static assertion to test the value of a constant expression

**Description:** Assertions are a valuable diagnostic tool for finding and eliminating software
defects that may result invulnerabilities(seeMSC11-C. Incorporate diagnostic
tests using assertions). The runtimeassert()macro has some limitations, however,
in that it incurs a runtime overhead and because it callsabort(). Consequently,
the runtimeassert()macro is useful only for identifying incorrect assumptions
and not for runtime error checking. As a result, runtime assertions are
generally unsuitable for server programs or embedded systems. Static assertion
is a new facility in the C Standard. It takes the form static_assert(constant-
expression, string-literal);

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl03_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl03_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_dcl03_c_pass_wiki_compliant_2`

---

### 🔶 DCL02-C - Not Implemented (has tests)

<a id="rule-dcl02c"></a>

**Title:** Use visually distinct identifiers

**Description:** Use visually distinct identifiers with meaningful names to eliminate errors
resulting from misreading the spelling of an identifier during the development
and review of code. An identifier can denote an object; a function; a tag or a
member of a structure, union, or enumeration; a typedef name; a label name; a
macro name; or a macro parameter. Depending on the fonts used, certain
characters appear visually similar or even identical: CharacterSimilar Character
s0(zero)O(capitalo),Q(capitalq),D(capitald)1(one)I(capitali),l(lowercaseL)2(two)
Z(capitalz)5(five)S(capitals)8(eight)B(capitalb)n(lowercaseN)h(lowercaseH)m(lowe
rcaseM)rn(lowercaseR, lowercaseN)

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_source_character_set.c` → `test_dcl02_c_fail_wiki_source_character_set`
- ⏭️ NOT RUN `wiki_source_character_set_2.c` → `test_dcl02_c_fail_wiki_source_character_set_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_source_character_set.c` → `test_dcl02_c_pass_wiki_source_character_set`
- ⏭️ NOT RUN `wiki_source_character_set_2.c` → `test_dcl02_c_pass_wiki_source_character_set_2`

---

### ✅ DCL13-C - Implemented

<a id="rule-dcl13c"></a>

**Title:** Declare function parameters that are pointers to values not changed by the function as const

**Description:** Declaring function parametersconstindicates that the function promises not to
change these values. In C, function arguments are passed by value rather than by
reference. Although a function may change the values passed in, these changed
values are discarded once the function returns. For this reason, many
programmers assume a function will not change its arguments and that declaring
the function's parameters asconstis unnecessary. void foo(int x) { x = 3; /*
Visible only in the function */ /* ... */ }

**Test Coverage:** 5 tests (3 fail, 2 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl13_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_dcl13_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_dcl13_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl13_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_dcl13_c_pass_wiki_compliant_2`

---

### 🔶 DCL08-C - Not Implemented (has tests)

<a id="rule-dcl08c"></a>

**Title:** Properly encode relationships in constant definitions

**Description:** If a relation exists between constants, you should encode the relationship in
the definitions. Do not give two independent definitions, because a maintainer
may fail to preserve that relationship when modifying the code. As a corollary,
do not encode an impermanent or false relationship between constants, because
future modifications may result in an incorrect definition for the dependent
constant. In this noncompliant code example, the definition forOUT_STR_LENmust
always be two greater than the definition ofIN_STR_LEN. The following
definitions fail to embody this relationship: enum { IN_STR_LEN=18,
OUT_STR_LEN=20 };

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl08_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_dcl08_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl08_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_dcl08_c_pass_wiki_compliant_2`

---

### 🔶 DCL30-C - Not Implemented (has tests)

<a id="rule-dcl30c"></a>

**Title:** Declare objects with appropriate storage durations

**Description:** Every object has a storage duration that determines its
lifetime:static,thread,automatic, orallocated. According to the C Standard,
6.2.4, paragraph 2 [ISO/IEC 9899:2024], Do not attempt to access an object
outside of its lifetime. Attempting to do so isundefined behaviorand can lead to
an exploitablevulnerability. (See alsoundefined behavior 9in the C Standard,
Annex J.)

**Test Coverage:** 7 tests (3 fail, 4 pass)

**Test Results:** 0/7 passed (0.0%), 7 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_differing_storage_durations.c` → `test_dcl30_c_fail_wiki_differing_storage_durations`
- ⏭️ NOT RUN `wiki_output_parameter.c` → `test_dcl30_c_fail_wiki_output_parameter`
- ⏭️ NOT RUN `wiki_return_values.c` → `test_dcl30_c_fail_wiki_return_values`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_differing_storage_durations.c` → `test_dcl30_c_pass_wiki_differing_storage_durations`
- ⏭️ NOT RUN `wiki_output_parameter.c` → `test_dcl30_c_pass_wiki_output_parameter`
- ⏭️ NOT RUN `wiki_return_values.c` → `test_dcl30_c_pass_wiki_return_values`
- ⏭️ NOT RUN `wiki_same_storage_durations.c` → `test_dcl30_c_pass_wiki_same_storage_durations`

---

### 🔶 DCL19-C - Not Implemented (has tests)

<a id="rule-dcl19c"></a>

**Title:** Minimize the scope of variables and functions

**Description:** Variables and functions should be declared in the minimum scope from which all
references to the identifier are still possible. When a larger scope than
necessary is used, code becomes less readable, harder to maintain, and more
likely to reference unintended variables (seeDCL01-C. Do not reuse variable
names in subscopes). In this noncompliant code example, the
functioncounter()increments the global variablecountand then returns immediately
if this variable exceeds a maximum value:

**Test Coverage:** 6 tests (3 fail, 3 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_function_declaration.c` → `test_dcl19_c_fail_wiki_function_declaration`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl19_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_dcl19_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl19_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_dcl19_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_dcl19_c_pass_wiki_compliant_3`

---

### 🔶 DCL05-C - Not Implemented (has tests)

<a id="rule-dcl05c"></a>

**Title:** Use typedefs of non-pointer types only

**Description:** Using type definitions (typedef) can often improve code readability. However,
type definitions to pointer types can make it more difficult to writeconst-
correct code because theconstqualifier will be applied to the pointer type, not
to the underlying declared type. The following type definition improves
readability at the expense of introducing aconst-correctness issue. In this
example, theconstqualifier applies to thetypedefinstead of to the underlying
object type. Consequently,funcdoes not take a pointer to aconst struct objbut
instead takes aconstpointer to astruct obj. struct obj { int i; float f; };
typedef struct obj *ObjectPtr; void func(const ObjectPtr o) { /* Can actually
modify o's contents, against expectations */ }

**Test Coverage:** 6 tests (3 fail, 3 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl05_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_4.c` → `test_dcl05_c_fail_wiki_noncompliant_4`
- ⏭️ NOT RUN `wiki_windows.c` → `test_dcl05_c_fail_wiki_windows`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl05_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_4.c` → `test_dcl05_c_pass_wiki_compliant_4`
- ⏭️ NOT RUN `wiki_windows.c` → `test_dcl05_c_pass_wiki_windows`

---

### 🔶 DCL15-C - Not Implemented (has tests)

<a id="rule-dcl15c"></a>

**Title:** Declare file-scope objects or functions that do not need external linkage as static

**Description:** If a file-scope object or a function does not need to be visible outside of the
file, it should be hidden by being declared asstatic. This practice creates more
modular code and limits pollution of the global name space. Subclause 6.2.2 of
the C Standard [ISO/IEC 9899:2011] states: and

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl15_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl15_c_pass_wiki_compliant_1`

---

### 🔶 DCL21-C - Not Implemented (has tests)

<a id="rule-dcl21c"></a>

**Title:** Understand the storage of compound literals

**Description:** Subclause 6.5.2.5 of the C Standard [ISO/IEC 9899:2011] defines a compound
literal as The storage for this object is either static (if the compound literal
occurs at file scope) or automatic (if the compound literal occurs at block
scope), and the storage duration is associated with its immediate enclosing
block. For example, in the function void func(void) { int *ip =
(int[4]){1,2,3,4}; /* ... */ }

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl21_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl21_c_pass_wiki_compliant_1`

---

### 🔶 DCL06-C - Not Implemented (has tests)

<a id="rule-dcl06c"></a>

**Title:** Use meaningful symbolic constants to represent literal values

**Description:** The C language provides several different kinds of constants:integerconstants,
such as10and0x1C;floatingconstants, such as1.0and6.022e+23;
andcharacterconstants, such as'a'and'\x10'. C also provides string literals,
such as"hello, world"and"\n". These constants can all be referred to asliterals.
When used in program logic, literals can reduce the readability of source code.
As a result, literals, in general, and integer constants, in particular, are
frequently calledmagic numbersbecause their purpose is often obscured. Magic
numbers can be constant values that represent either an arbitrary value (such as
a determined appropriate buffer size) or a malleable concept (such as the age at
which a person is considered an adult, which can change between geopolitical
boundaries). Rather than embed literals in program logic, use appropriately
named symbolic constants to clarify the intent of the code. In addition, if a
specific value needs to be changed, reassigning a symbolic constant once is more
efficient and less error prone than replacing every instance of the value [Saks
2002]. The C programming language has several mechanisms for creating named,
symbolic constants:const-qualified objects, enumeration constants, andobject-
like macrodefinitions. Each of these mechanisms has associated advantages and
disadvantages.

**Test Coverage:** 7 tests (3 fail, 4 pass)

**Test Results:** 0/7 passed (0.0%), 7 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl06_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_dcl06_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_dcl06_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl06_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_4.c` → `test_dcl06_c_pass_wiki_compliant_4`
- ⏭️ NOT RUN `wiki_enum.c` → `test_dcl06_c_pass_wiki_enum`
- ⏭️ NOT RUN `wiki_sizeof.c` → `test_dcl06_c_pass_wiki_sizeof`

---

### 🔶 DCL37-C - Not Implemented (has tests)

<a id="rule-dcl37c"></a>

**Title:** Do not declare or define a reserved identifier

**Description:** According to the C Standard, 6.4.2.1 paragraph 7 [ISO/IEC 9899:2024], C
Standard, 7.1.3 paragraph 1 [ISO/IEC 9899:2024], Additionally, subclause 7.33
defines many other reserved identifiers for future library directions.

**Test Coverage:** 10 tests (5 fail, 5 pass)

**Test Results:** 0/10 passed (0.0%), 10 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_errno.c` → `test_dcl37_c_fail_wiki_errno`
- ⏭️ NOT RUN `wiki_file_scope_objects.c` → `test_dcl37_c_fail_wiki_file_scope_objects`
- ⏭️ NOT RUN `wiki_identifiers_with_external_linkage.c` → `test_dcl37_c_fail_wiki_identifiers_with_external_linkage`
- ⏭️ NOT RUN `wiki_include_guard.c` → `test_dcl37_c_fail_wiki_include_guard`
- ⏭️ NOT RUN `wiki_reserved_macros.c` → `test_dcl37_c_fail_wiki_reserved_macros`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_errno.c` → `test_dcl37_c_pass_wiki_errno`
- ⏭️ NOT RUN `wiki_file_scope_objects.c` → `test_dcl37_c_pass_wiki_file_scope_objects`
- ⏭️ NOT RUN `wiki_identifiers_with_external_linkage.c` → `test_dcl37_c_pass_wiki_identifiers_with_external_linkage`
- ⏭️ NOT RUN `wiki_include_guard.c` → `test_dcl37_c_pass_wiki_include_guard`
- ⏭️ NOT RUN `wiki_reserved_macros.c` → `test_dcl37_c_pass_wiki_reserved_macros`

---

### 🔶 DCL16-C - Not Implemented (has tests)

<a id="rule-dcl16c"></a>

**Title:** Use "L," not "l," to indicate a long value

**Description:** Lowercase letterl(ell) can easily be confused with the digit1(one). This can be
particularly confusing when indicating that an integer literal constant is a
long value. This recommendation is similar toDCL02-C. Use visually distinct
identifiers. Likewise, you should use uppercaseLLrather than lowercasellwhen
indicating that an integer literal constant is along longvalue. To be precise
when using modifiers to indicate the type of an integer literal, the first
character may not bel. It may beL,u, orU. Subsequent characters have no strict
case requirements. This noncompliant example highlights the result of adding an
integer and a long value even though it appears that two integers1111are being
added:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl16_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl16_c_pass_wiki_compliant_1`

---

### 🔶 DCL10-C - Not Implemented (has tests)

<a id="rule-dcl10c"></a>

**Title:** Maintain the contract between the writer and caller of variadic functions

**Description:** Variadic functions accept a variable number of arguments but are problematic.
Variadic functions define an implicit contract between the function writer and
the function user that allows the function to determine the number of arguments
passed in any particular invocation. Failure to enforce this contract may result
inundefined behavior. Seeundefined behavior 141of Appendix J of the C Standard.
In the following code example, the variadic functionaverage()calculates the
average value of the positive integer arguments passed to the function [Seacord
2013]. The function processes arguments until it encounters an argument with the
value ofva_eol(-1). enum { va_eol = -1 }; unsigned int average(int first, ...) {
unsigned int count = 0; unsigned int sum = 0; int i = first; va_list args;
va_start(args, first); while (i != va_eol) { sum += i; count++; i = va_arg(args,
int); } va_end(args); return(count ? (sum / count) : 0); }

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl10_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_dcl10_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl10_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_dcl10_c_pass_wiki_compliant_2`

---

### 🔶 DCL20-C - Not Implemented (has tests)

<a id="rule-dcl20c"></a>

**Title:** Explicitly specify void when a function accepts no arguments

**Description:** According to the C Standard, subclause 6.7.6.3, paragraph 14 [ISO/IEC
9899:2011], Subclause 6.11.6 states that Consequently, functions that accept no
arguments should explicitly declare avoidparameter in their parameter list. This
holds true in both the declaration and definition sections (which should match).

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_ambiguous_interface.c` → `test_dcl20_c_fail_wiki_ambiguous_interface`
- ⏭️ NOT RUN `wiki_information_outflow.c` → `test_dcl20_c_fail_wiki_information_outflow`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_ambiguous_interface.c` → `test_dcl20_c_pass_wiki_ambiguous_interface`
- ⏭️ NOT RUN `wiki_information_outflow.c` → `test_dcl20_c_pass_wiki_information_outflow`

---

### ✅ DCL01-C - Implemented

<a id="rule-dcl01c"></a>

**Title:** Do not reuse variable names in subscopes

**Description:** Do not use the same variable name in two scopes where one scope is contained in
another. For example, Reusing variable names leads to programmer confusion about
which variable is being modified. Additionally, if variable names are reused,
generally one or both of the variable names are too generic. This noncompliant
code example declares themsgidentifier at file scope and reuses the same
identifier to declare a character array local to thereport_error()function. The
programmer may unintentionally copy the function argument to the locally
declaredmsgarray within thereport_error()function. Depending on the programmer's
intention, it either fails to initialize the global variablemsgor allows the
localmsgbuffer to overflow by using the global valuemsgsizeas a bounds for the
local buffer.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl01_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_code_example.c` → `test_dcl01_c_fail_wiki_noncompliant_code_example`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl01_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_dcl01_c_pass_wiki_compliant_2`

---

### 🔶 DCL11-C - Not Implemented (has tests)

<a id="rule-dcl11c"></a>

**Title:** Understand the type issues associated with variadic functions

**Description:** The variable parameters of a variadic function—that is, those that correspond
with the position of the ellipsis—are interpreted by theva_arg()macro.
Theva_arg()macro is used to extract the next argument from an initialized
argument list within the body of a variadic function implementation. The size of
each parameter is determined by the specified type. If the type is inconsistent
with the corresponding argument, the behavior isundefinedand may result in
misinterpreted data or an alignment error (seeEXP36-C. Do not cast pointers into
more strictly aligned pointer types). The variable arguments to a variadic
function are not checked for type by the compiler. As a result, the programmer
is responsible for ensuring that they are compatible with the corresponding
parameter after the default argument promotions: The Cprintf()function is
implemented as a variadic function. This noncompliant code example swaps its
null-terminated byte string and integer parameters with respect to how they are
specified in the format string. Consequently, the integer is interpreted as a
pointer to a null-terminated byte string and dereferenced, which will likely
cause the program toabnormally terminate. Note that theerror_messagepointer is
likewise interpreted as an integer.

**Test Coverage:** 6 tests (3 fail, 3 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_null.c` → `test_dcl11_c_fail_wiki_null`
- ⏭️ NOT RUN `wiki_type_alignment_error.c` → `test_dcl11_c_fail_wiki_type_alignment_error`
- ⏭️ NOT RUN `wiki_type_interpretation_error.c` → `test_dcl11_c_fail_wiki_type_interpretation_error`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_null.c` → `test_dcl11_c_pass_wiki_null`
- ⏭️ NOT RUN `wiki_type_alignment_error.c` → `test_dcl11_c_pass_wiki_type_alignment_error`
- ⏭️ NOT RUN `wiki_type_interpretation_error.c` → `test_dcl11_c_pass_wiki_type_interpretation_error`

---

### 🔶 DCL39-C - Not Implemented (has tests)

<a id="rule-dcl39c"></a>

**Title:** Avoid information leakage when passing a structure across a trust boundary

**Description:** The C Standard, 6.7.3.2, discusses the layout of structure fields. It specifies
that non-bit-field members are aligned in animplementation-definedmanner and
that there may be padding within or at the end of a structure. Furthermore,
initializing the members of the structure does not guarantee initialization of
the padding bytes. The C Standard, 6.2.6.1, paragraph 6 [ISO/IEC 9899:2024],
states Additionally, the storage units in which a bit-field resides may also
have padding bits. For an object with automatic storage duration, these padding
bits do not take on specific values and can contribute to leaking sensitive
information. When passing a pointer to a structure across a trust boundary to a
different trusted domain, the programmer must ensure that the padding bytes and
bit-field storage unit padding bits of such a structure do not contain sensitive
information.

**Test Coverage:** 8 tests (3 fail, 5 pass)

**Test Results:** 0/8 passed (0.0%), 8 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_memset.c` → `test_dcl39_c_fail_wiki_memset`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl39_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_dcl39_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl39_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_5.c` → `test_dcl39_c_pass_wiki_compliant_5`
- ⏭️ NOT RUN `wiki_padding_bytes.c` → `test_dcl39_c_pass_wiki_padding_bytes`
- ⏭️ NOT RUN `wiki_structure_packinggcc.c` → `test_dcl39_c_pass_wiki_structure_packinggcc`
- ⏭️ NOT RUN `wiki_structure_packingmicrosoft_visual_studio.c` → `test_dcl39_c_pass_wiki_structure_packingmicrosoft_visual_studio`

---

### 🔶 DCL31-C - Not Implemented (has tests)

<a id="rule-dcl31c"></a>

**Title:** Declare identifiers before using them

**Description:** The C23 Standard requires type specifiers and forbids implicit function
declarations. The C90 Standard allows implicit typing of variables and
functions. Consequently, some existing legacy code uses implicit typing. Some C
compilers still support legacy code by allowing implicit typing, but it should
not be used for new code. Such animplementationmay choose to assume an implicit
declaration and continue translation to support existing programs that used this
feature. C no longer allows the absence of type specifiers in a declaration.The
C Standard, 6.7.3 paragraph 2 [ISO/IEC 9899:2024], states This noncompliant code
example omits the type specifier:

**Test Coverage:** 6 tests (3 fail, 3 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_implicit_function_declaration.c` → `test_dcl31_c_fail_wiki_implicit_function_declaration`
- ⏭️ NOT RUN `wiki_implicit_return_type.c` → `test_dcl31_c_fail_wiki_implicit_return_type`
- ⏭️ NOT RUN `wiki_implicitint.c` → `test_dcl31_c_fail_wiki_implicitint`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_implicit_function_declaration.c` → `test_dcl31_c_pass_wiki_implicit_function_declaration`
- ⏭️ NOT RUN `wiki_implicit_return_type.c` → `test_dcl31_c_pass_wiki_implicit_return_type`
- ⏭️ NOT RUN `wiki_implicitint.c` → `test_dcl31_c_pass_wiki_implicitint`

---

### 🔶 DCL18-C - Not Implemented (has tests)

<a id="rule-dcl18c"></a>

**Title:** Do not begin integer constants with 0 when specifying a decimal value

**Description:** The C Standard defines octal constants as a 0 followed by octal digits (0 1 2 3
4 5 6 7). Programming errors can occur when decimal values are mistakenly
specified as octal constants. In this noncompliant code example, a decimal
constant is mistakenly prefaced with zeros so that all the constants are a fixed
length: i_array[0] = 2719; i_array[1] = 4435; i_array[2] = 0042;

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl18_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl18_c_pass_wiki_compliant_1`

---

### 🔶 DCL22-C - Not Implemented (has tests)

<a id="rule-dcl22c"></a>

**Title:** Use volatile for data that cannot be cached

**Description:** An object that has volatile-qualified type may be modified in ways unknown to
theimplementationor have other unknown side effects. Asynchronous signal
handling, for example, may cause objects to be modified in a manner unknown to
the compiler. Without this type qualifier, unintended optimizations may occur.
These optimizations may cause race conditions because a programmer may write
code that prevents a race condition, yet the compiler is not aware of the
programmer's data model and may modify the code during compilation to permit
race conditions. Thevolatilekeyword eliminates this confusion by imposing
restrictions on access and caching. According to the C99 Rationale [C99
Rationale 2003], Type qualifying objects as volatile does not guarantee
synchronization between multiple threads, protect against simultaneous memory
accesses, or, unless used to declare objects of typesig_atomic_t, guarantee
atomicity of accesses to the object. For restrictions specific to signal
handlers, seeSIG31-C. Do not access shared objects in signal handlers. However,
type qualifying objects as volatile does ensure that a conforming compiler will
not elide or reorder access to the object.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl22_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl22_c_pass_wiki_compliant_1`

---

### 🔶 DCL09-C - Not Implemented (has tests)

<a id="rule-dcl09c"></a>

**Title:** Declare functions that return errno with a return type of errno_t

**Description:** When developing new code, declare functions that returnerrnowith a return type
oferrno_t. Many existing functions that returnerrnoare declared as returning a
value of typeint. It is semantically unclear by inspecting the function
declaration or prototype if these functions return an error status or a value
or, worse, some combination of the two. (SeeERR02-C. Avoid in-band error
indicators.) C11 Annex K introduced the new typeerrno_tthat is defined to be
typeintinerrno.hand elsewhere. Many of the functions defined in C11 Annex K
return values of this type. Theerrno_ttype should be used as the type of an
object that may contain only values that might be found inerrno. For example, a
function that returns the value oferrnoshould be declared as having the return
typeerrno_t. This recommendation depends on C11 Annex K being implemented. The
following code can be added to remove this dependency:

**Test Coverage:** 1 tests (0 fail, 1 pass)

**Test Results:** 0/1 passed (0.0%), 1 not run

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_dcl09_c_pass_wiki_posix`

---

### 🔶 DCL12-C - Not Implemented (has tests)

<a id="rule-dcl12c"></a>

**Title:** Implement abstract data types using opaque types

**Description:** Abstract data types are not restricted to object-oriented languages such as C++
and Java. They should be created and used in C language programs as well.
Abstract data types are most effective when used with private (opaque) data
types and information hiding. This noncompliant code example is based on the
managed string library developed by CERT [Burch 2006]. In this example, the
managed string type and the functions that operate on this type are defined in
thestring_m.hheader file as follows: struct string_mx { size_t size; size_t
maxsize; unsigned char strtype; char *cstr; }; typedef struct string_mx
string_mx; /* Function declarations */ extern errno_t strcpy_m(string_mx *s1,
const string_mx *s2); extern errno_t strcat_m(string_mx *s1, const string_mx
*s2); /* ... */

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl12_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl12_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2_2.c` → `test_dcl12_c_pass_wiki_compliant_2_2`

---

### 🔶 DCL07-C - Not Implemented (has tests)

<a id="rule-dcl07c"></a>

**Title:** Include the appropriate type information in function declarators

**Description:** Function declarators must be declared with the appropriate type information,
including a return type and parameter list. If type information is not properly
specified in a function declarator, the compiler cannot properly check function
type information. When using standard library calls, the easiest (and preferred)
way to obtain function declarators with appropriate type information is to
include the appropriate header file. Attempting to compile a program with a
function declarator that does not include the appropriate type information
typically generates a warning but does not prevent program compilation. These
warnings should be resolved. (SeeMSC00-C. Compile cleanly at high warning
levels.) This noncompliant code example uses theidentifier-listform for
parameter declarations:

**Test Coverage:** 6 tests (4 fail, 2 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_function_pointers.c` → `test_dcl07_c_fail_wiki_function_pointers`
- ⏭️ NOT RUN `wiki_function_prototypes.c` → `test_dcl07_c_fail_wiki_function_prototypes`
- ⏭️ NOT RUN `wiki_function_prototypes_2.c` → `test_dcl07_c_fail_wiki_function_prototypes_2`
- ⏭️ NOT RUN `wiki_non_prototype_format_declarators.c` → `test_dcl07_c_fail_wiki_non_prototype_format_declarators`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_function_pointers.c` → `test_dcl07_c_pass_wiki_function_pointers`
- ⏭️ NOT RUN `wiki_function_prototypes.c` → `test_dcl07_c_pass_wiki_function_prototypes`

---

### 🔶 DCL40-C - Not Implemented (has tests)

<a id="rule-dcl40c"></a>

**Title:** Do not create incompatible declarations of the same function or object

**Description:** Two or more incompatible declarations of the same function or object must not
appear in the same program because they result inundefined behavior. The C
Standard, 6.2.7, mentions that two types may be distinct yet compatible and
addresses preciselywhen two distinct types are compatible. The C Standard
identifies four situations in whichundefined behavior (UB)may arise as a result
of incompatible declarations of the same function or object:
UBDescriptionCode14Two declarations of the same object or function specify types
that are not compatible (6.2.7).All noncompliant code in this guideline30Two
identifiers differ only in nonsignificant characters (6.4.2.1).Excessively Long
Identifiers36An object has its stored value accessed other than by an lvalue of
an allowable type (6.5).Incompatible Object DeclarationsIncompatible Array
Declarations37A function is defined with a type that is not compatible with the
type (of the expression) pointed to by the expression that denotes the called
function (6.5.2.2).Incompatible Function DeclarationsExcessively Long
Identifiers

**Test Coverage:** 10 tests (5 fail, 5 pass)

**Test Results:** 0/10 passed (0.0%), 10 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_excessively_long_identifiers.c` → `test_dcl40_c_fail_wiki_excessively_long_identifiers`
- ⏭️ NOT RUN `wiki_incompatible_array_declarations.c` → `test_dcl40_c_fail_wiki_incompatible_array_declarations`
- ⏭️ NOT RUN `wiki_incompatible_function_declarations.c` → `test_dcl40_c_fail_wiki_incompatible_function_declarations`
- ⏭️ NOT RUN `wiki_incompatible_object_declarations.c` → `test_dcl40_c_fail_wiki_incompatible_object_declarations`
- ⏭️ NOT RUN `wiki_incompatible_variadic_function_declarations.c` → `test_dcl40_c_fail_wiki_incompatible_variadic_function_declarations`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_excessively_long_identifiers.c` → `test_dcl40_c_pass_wiki_excessively_long_identifiers`
- ⏭️ NOT RUN `wiki_incompatible_array_declarations.c` → `test_dcl40_c_pass_wiki_incompatible_array_declarations`
- ⏭️ NOT RUN `wiki_incompatible_function_declarations.c` → `test_dcl40_c_pass_wiki_incompatible_function_declarations`
- ⏭️ NOT RUN `wiki_incompatible_object_declarations.c` → `test_dcl40_c_pass_wiki_incompatible_object_declarations`
- ⏭️ NOT RUN `wiki_incompatible_variadic_function_declarations.c` → `test_dcl40_c_pass_wiki_incompatible_variadic_function_declarations`

---

### ✅ DCL00-C - Implemented

<a id="rule-dcl00c"></a>

**Title:** Const-qualify immutable objects

**Description:** Immutable objects should beconst-qualified. Enforcing object immutability
usingconstqualification helps ensure the correctness and security of
applications. ISO/IEC TR 24772, for example, recommends labeling parameters as
constant to avoid the unintentional modification of function arguments [ISO/IEC
TR 24772].STR05-C. Use pointers to const when referring to string
literalsdescribes a specialized case of this recommendation.
Addingconstqualification may propagate through a program; as you addconst,
qualifiers become still more necessary. This phenomenon is sometimes
calledconstpoisoning, which can frequently lead to violations ofEXP05-C. Do not
cast away a const qualification. Althoughconstqualification is a good idea, the
costs may outweigh the value in the remediation of existing code. A macro or an
enumeration constant may also be used instead of aconst-qualified
object.DCL06-C. Use meaningful symbolic constants to represent literal
valuesdescribes the relative merits of usingconst-qualified objects, enumeration
constants, and object-like macros. However, adding aconstqualifier to an
existing variable is a better first step than replacing the variable with an
enumeration constant or macro because the compiler will issue warnings on any
code that changes yourconst-qualified variable. Once you have verified that
aconst-qualified variable is not changed by any code, you may consider changing
it to an enumeration constant or macro, as best fits your design.

**Test Coverage:** 42 tests (31 fail, 11 pass)

**Test Results:** 0/42 passed (0.0%), 42 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_api_endpoints.c` → `test_dcl00_c_fail_testcases_api_endpoints`
- ⏭️ NOT RUN `testcases_array_not_const.c` → `test_dcl00_c_fail_testcases_array_not_const`
- ⏭️ NOT RUN `testcases_bit_masks.c` → `test_dcl00_c_fail_testcases_bit_masks`
- ⏭️ NOT RUN `testcases_buffer_sizes.c` → `test_dcl00_c_fail_testcases_buffer_sizes`
- ⏭️ NOT RUN `testcases_color_definitions.c` → `test_dcl00_c_fail_testcases_color_definitions`
- ⏭️ NOT RUN `testcases_compiler_flags.c` → `test_dcl00_c_fail_testcases_compiler_flags`
- ⏭️ NOT RUN `testcases_configuration_values.c` → `test_dcl00_c_fail_testcases_configuration_values`
- ⏭️ NOT RUN `testcases_coordinate_systems.c` → `test_dcl00_c_fail_testcases_coordinate_systems`
- ⏭️ NOT RUN `testcases_database_schema.c` → `test_dcl00_c_fail_testcases_database_schema`
- ⏭️ NOT RUN `testcases_device_registers.c` → `test_dcl00_c_fail_testcases_device_registers`
- ⏭️ NOT RUN `testcases_encryption_constants.c` → `test_dcl00_c_fail_testcases_encryption_constants`
- ⏭️ NOT RUN `testcases_enum_like_constants.c` → `test_dcl00_c_fail_testcases_enum_like_constants`
- ⏭️ NOT RUN `testcases_error_messages.c` → `test_dcl00_c_fail_testcases_error_messages`
- ⏭️ NOT RUN `testcases_file_paths.c` → `test_dcl00_c_fail_testcases_file_paths`
- ⏭️ NOT RUN `testcases_format_strings.c` → `test_dcl00_c_fail_testcases_format_strings`
- ⏭️ NOT RUN `testcases_function_pointer_table.c` → `test_dcl00_c_fail_testcases_function_pointer_table`
- ⏭️ NOT RUN `testcases_game_constants.c` → `test_dcl00_c_fail_testcases_game_constants`
- ⏭️ NOT RUN `testcases_global_constants.c` → `test_dcl00_c_fail_testcases_global_constants`
- ⏭️ NOT RUN `testcases_license_constants.c` → `test_dcl00_c_fail_testcases_license_constants`
- ⏭️ NOT RUN `testcases_loop_limits.c` → `test_dcl00_c_fail_testcases_loop_limits`
- ⏭️ NOT RUN `testcases_mathematical_constants.c` → `test_dcl00_c_fail_testcases_mathematical_constants`
- ⏭️ NOT RUN `testcases_menu_options.c` → `test_dcl00_c_fail_testcases_menu_options`
- ⏭️ NOT RUN `testcases_protocol_constants.c` → `test_dcl00_c_fail_testcases_protocol_constants`
- ⏭️ NOT RUN `testcases_string_literal_no_const.c` → `test_dcl00_c_fail_testcases_string_literal_no_const`
- ⏭️ NOT RUN `testcases_struct_immutable_fields.c` → `test_dcl00_c_fail_testcases_struct_immutable_fields`
- ⏭️ NOT RUN `testcases_switch_case_values.c` → `test_dcl00_c_fail_testcases_switch_case_values`
- ⏭️ NOT RUN `testcases_test_data.c` → `test_dcl00_c_fail_testcases_test_data`
- ⏭️ NOT RUN `testcases_timeout_values.c` → `test_dcl00_c_fail_testcases_timeout_values`
- ⏭️ NOT RUN `testcases_unmodified_local_variable.c` → `test_dcl00_c_fail_testcases_unmodified_local_variable`
- ⏭️ NOT RUN `testcases_validation_rules.c` → `test_dcl00_c_fail_testcases_validation_rules`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl00_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_const_alternatives.c` → `test_dcl00_c_pass_testcases_const_alternatives`
- ⏭️ NOT RUN `testcases_const_arrays_structures.c` → `test_dcl00_c_pass_testcases_const_arrays_structures`
- ⏭️ NOT RUN `testcases_const_best_practices.c` → `test_dcl00_c_pass_testcases_const_best_practices`
- ⏭️ NOT RUN `testcases_const_data_structures.c` → `test_dcl00_c_pass_testcases_const_data_structures`
- ⏭️ NOT RUN `testcases_const_file_operations.c` → `test_dcl00_c_pass_testcases_const_file_operations`
- ⏭️ NOT RUN `testcases_const_function_parameters.c` → `test_dcl00_c_pass_testcases_const_function_parameters`
- ⏭️ NOT RUN `testcases_const_global_constants.c` → `test_dcl00_c_pass_testcases_const_global_constants`
- ⏭️ NOT RUN `testcases_const_pointers_arrays.c` → `test_dcl00_c_pass_testcases_const_pointers_arrays`
- ⏭️ NOT RUN `testcases_const_string_literals.c` → `test_dcl00_c_pass_testcases_const_string_literals`
- ⏭️ NOT RUN `testcases_static_const_variables.c` → `test_dcl00_c_pass_testcases_static_const_variables`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl00_c_pass_wiki_compliant_1`

---

### 🔶 DCL38-C - Not Implemented (has tests)

<a id="rule-dcl38c"></a>

**Title:** Use the correct syntax when declaring a flexible array member

**Description:** Flexible array members are a special type of array in which the last element of
a structure with more than one named member has an incomplete array type; that
is, the size of the array is not specified explicitly within the structure. This
"struct hack" was widely used in practice and supported by a variety of
compilers. Consequently, a variety of different syntaxes have been used for
declaring flexible array members. For conforming C implementations, use the
syntax guaranteed to be valid by the C Standard. Flexible array members are
defined in the C Standard, 6.7.3.2, paragraph 20 [ISO/IEC 9899:2024], as
follows: Structures with a flexible array member can be used to produce code
with defined behavior. However, some restrictions apply:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl38_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl38_c_pass_wiki_compliant_1`

---

### 🔶 DCL04-C - Not Implemented (has tests)

<a id="rule-dcl04c"></a>

**Title:** Do not declare more than one variable per declaration

**Description:** Every declaration should be for a single variable, on its own line, with an
explanatory comment about the role of the variable. Declaring multiple variables
in a single declaration can cause confusion regarding the types of the variables
and their initial values. If more than one variable is declared in a
declaration, care must be taken that the type and initialized value of the
variable are handled correctly. In this noncompliant code example, a programmer
or code reviewer might mistakenly believe that the two variablessrcandcare
declared aschar *. In fact,srchas a type ofchar *, whereaschas a type ofchar.
char *src = 0, c = 0;

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl04_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_dcl04_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl04_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_dcl04_c_pass_wiki_compliant_2`

---

### 🔶 DCL17-C - Not Implemented (has tests)

<a id="rule-dcl17c"></a>

**Title:** Beware of miscompiled volatile-qualified variables

**Description:** As described in depth in ruleDCL22-C. Use volatile for data that cannot be
cached, avolatile-qualified variable "shall be evaluated strictly according to
the rules of the abstract machine" [ISO/IEC 9899:2011]. In other words,
thevolatilequalifier is used to instruct the compiler to not make caching
optimizations about a variable. However, as demonstrated in "Volatiles Are
Miscompiled, and What to Do about It" [Eide and Regehr], all tested compilers
generated some percentage of incorrect compiled code with regard
tovolatileaccesses. Therefore, it is necessary to know how your compiler behaves
when the standardvolatilebehavior is required. The authors also provide a
workaround that eliminates some or all of these errors. As demonstrated in Eide
and Regehr's work, the following code example compiles incorrectly using GCC
4.3.0 for IA32 and the-Osoptimization flag:

**Test Coverage:** 3 tests (2 fail, 1 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl17_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_dcl17_c_fail_wiki_noncompliant_2_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl17_c_pass_wiki_compliant_1`

---

### 🔶 DCL41-C - Not Implemented (has tests)

<a id="rule-dcl41c"></a>

**Title:** Do not declare variables inside a switch statement before the first case label

**Description:** According to the C Standard, 6.8.5.3, paragraph 4 [ISO/IEC 9899:2024], If a
programmer declares variables, initializes them before the first case statement,
and then tries to use them inside any of the case statements, those variables
will have scope inside theswitchblock but will not be initialized and will
consequently contain indeterminate values. Reading such values also
violatesEXP33-C. Do not read uninitialized memory. This noncompliant code
example declares variables and contains executable statements before the first
case label within theswitchstatement:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl41_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl41_c_pass_wiki_compliant_1`

---

### 🔶 DCL36-C - Not Implemented (has tests)

<a id="rule-dcl36c"></a>

**Title:** Do not declare an identifier with conflicting linkage classifications

**Description:** Linkage can make an identifier declared in different scopes or declared multiple
times within the same scope refer to the same object or function. Identifiers
are classified asexternally linked,internally linked, ornot linked. These three
kinds of linkage have the following characteristics [Kirch-Prinz 2002]:
According to the C Standard, 6.2.2 paragraph 3 [ISO/IEC 9899:2024], linkage is
determined as follows: Use of an identifier (within one translation unit)
classified as both internally and externally linked isundefined behavior. (See
alsoundefined behavior 8.) A translation unit includes the source file together
with its headers and all source files included via the preprocessing
directive#include.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_dcl36_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_dcl36_c_pass_wiki_compliant_1`

---

## Category: ENV

<a id="category-env"></a>

**Implementation Status:** 0 / 8 rules (0.0%)

### 🔶 ENV02-C - Not Implemented (has tests)

<a id="rule-env02c"></a>

**Title:** Beware of multiple environment variables with the same effective name

**Description:** Thegetenv()function searches an environment list for a string that matches a
specified name and returns a pointer to a string associated with the matched
list member. Subclause 7.22.4.6 of the C Standard [ISO/IEC 9899:2011] states:
Depending on theimplementation, multiple environment variables with the same
name may be allowed and can cause unexpected results if a program cannot
consistently choose the same value. The GNU glibc library addresses this issue
ingetenv()andsetenv()by always using the first variable it encounters and
ignoring the rest. However, it is unwise to rely on this behavior.

**Test Coverage:** 4 tests (3 fail, 1 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_env02_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_env02_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3_3.c` → `test_env02_c_fail_wiki_noncompliant_3_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_env02_c_pass_wiki_compliant_1`

---

### 🔶 ENV34-C - Not Implemented (has tests)

<a id="rule-env34c"></a>

**Title:** Do not store pointers returned by certain functions

**Description:** The C Standard, 7.24.4.6, paragraph 4 [ISO/IEC 9899:2024], states This paragraph
gives an implementation the latitude, for example, to return a pointer to a
statically allocated buffer. Consequently, do not store this pointer because the
string data it points to may be overwritten by a subsequent call to
thegetenv()function or invalidated by modifications to the environment. This
string should be referenced immediately and discarded. If later use is
anticipated, the string should be copied so the copy can be safely referenced as
needed. Thegetenv()function is not thread-safe. Make sure to address any
possible race conditions resulting from the use of this function.

**Test Coverage:** 4 tests (1 fail, 3 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_env34_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_env34_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_posix_or_c2x.c` → `test_env34_c_pass_wiki_posix_or_c2x`
- ⏭️ NOT RUN `wiki_windows.c` → `test_env34_c_pass_wiki_windows`

---

### 🔶 ENV33-C - Not Implemented (has tests)

<a id="rule-env33c"></a>

**Title:** Do not call system()

**Description:** The C Standardsystem()function executes a specified command by invoking
animplementation-definedcommand processor, such as a UNIX shell orCMD.EXEin
Microsoft Windows. The POSIXpopen()and Windows_popen()functions also invoke a
command processor but create a pipe between the calling program and the executed
command, returning a pointer to a stream that can be used to either read from or
write to the pipe [IEEE Std 1003.1:2013]. Use of thesystem()function can result
in exploitablevulnerabilities, in the worst case allowing execution of arbitrary
system commands. Situations in which calls tosystem()have high risk include the
following: Do not invoke a command processor viasystem()or equivalent functions
to execute a command.

**Test Coverage:** 6 tests (4 fail, 2 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_env33_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_env33_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3_3.c` → `test_env33_c_fail_wiki_noncompliant_3_3`
- ⏭️ NOT RUN `wiki_posix.c` → `test_env33_c_fail_wiki_posix`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_env33_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_windows.c` → `test_env33_c_pass_wiki_windows`

---

### 🔶 ENV30-C - Not Implemented (has tests)

<a id="rule-env30c"></a>

**Title:** Do not modify the object referenced by the return value of certain functions

**Description:** Some functions return a pointer to an object that cannot be modified without
causingundefined behavior. These functions
includegetenv(),setlocale(),localeconv(),asctime(), andstrerror(). In such
cases, the function call results must be treated as beingconst-qualified. The C
Standard, 7.24.4.6, paragraph 4 [ISO/IEC 9899:2024], definesgetenv()as follows:
If the string returned bygetenv()must be altered, a local copy should be
created. Altering the string returned bygetenv()isundefined behavior.
(Seeundefined behavior 189.)

**Test Coverage:** 45 tests (32 fail, 13 pass)

**Test Results:** 0/45 passed (0.0%), 45 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_additional_violations_1.c` → `test_env30_c_fail_testcases_additional_violations_1`
- ⏭️ NOT RUN `testcases_additional_violations_2.c` → `test_env30_c_fail_testcases_additional_violations_2`
- ⏭️ NOT RUN `testcases_additional_violations_3.c` → `test_env30_c_fail_testcases_additional_violations_3`
- ⏭️ NOT RUN `testcases_additional_violations_4.c` → `test_env30_c_fail_testcases_additional_violations_4`
- ⏭️ NOT RUN `testcases_additional_violations_5.c` → `test_env30_c_fail_testcases_additional_violations_5`
- ⏭️ NOT RUN `testcases_additional_violations_6.c` → `test_env30_c_fail_testcases_additional_violations_6`
- ⏭️ NOT RUN `testcases_advanced_modification_patterns.c` → `test_env30_c_fail_testcases_advanced_modification_patterns`
- ⏭️ NOT RUN `testcases_asctime_ctime_modification.c` → `test_env30_c_fail_testcases_asctime_ctime_modification`
- ⏭️ NOT RUN `testcases_configuration_violations.c` → `test_env30_c_fail_testcases_configuration_violations`
- ⏭️ NOT RUN `testcases_database_violations.c` → `test_env30_c_fail_testcases_database_violations`
- ⏭️ NOT RUN `testcases_display_violations.c` → `test_env30_c_fail_testcases_display_violations`
- ⏭️ NOT RUN `testcases_encoding_violations.c` → `test_env30_c_fail_testcases_encoding_violations`
- ⏭️ NOT RUN `testcases_environment_manipulation.c` → `test_env30_c_fail_testcases_environment_manipulation`
- ⏭️ NOT RUN `testcases_file_operations_violations.c` → `test_env30_c_fail_testcases_file_operations_violations`
- ⏭️ NOT RUN `testcases_final_violations_1.c` → `test_env30_c_fail_testcases_final_violations_1`
- ⏭️ NOT RUN `testcases_final_violations_2.c` → `test_env30_c_fail_testcases_final_violations_2`
- ⏭️ NOT RUN `testcases_final_violations_3.c` → `test_env30_c_fail_testcases_final_violations_3`
- ⏭️ NOT RUN `testcases_final_violations_4.c` → `test_env30_c_fail_testcases_final_violations_4`
- ⏭️ NOT RUN `testcases_format_violations.c` → `test_env30_c_fail_testcases_format_violations`
- ⏭️ NOT RUN `testcases_getenv_direct_modification.c` → `test_env30_c_fail_testcases_getenv_direct_modification`
- ⏭️ NOT RUN `testcases_localeconv_modification.c` → `test_env30_c_fail_testcases_localeconv_modification`
- ⏭️ NOT RUN `testcases_logging_violations.c` → `test_env30_c_fail_testcases_logging_violations`
- ⏭️ NOT RUN `testcases_multiple_function_violations.c` → `test_env30_c_fail_testcases_multiple_function_violations`
- ⏭️ NOT RUN `testcases_network_violations.c` → `test_env30_c_fail_testcases_network_violations`
- ⏭️ NOT RUN `testcases_parsing_violations.c` → `test_env30_c_fail_testcases_parsing_violations`
- ⏭️ NOT RUN `testcases_security_violations.c` → `test_env30_c_fail_testcases_security_violations`
- ⏭️ NOT RUN `testcases_setlocale_modification.c` → `test_env30_c_fail_testcases_setlocale_modification`
- ⏭️ NOT RUN `testcases_strerror_modification.c` → `test_env30_c_fail_testcases_strerror_modification`
- ⏭️ NOT RUN `testcases_thread_safety_violations.c` → `test_env30_c_fail_testcases_thread_safety_violations`
- ⏭️ NOT RUN `testcases_validation_violations.c` → `test_env30_c_fail_testcases_validation_violations`
- ⏭️ NOT RUN `wiki_getenv.c` → `test_env30_c_fail_wiki_getenv`
- ⏭️ NOT RUN `wiki_localeconv.c` → `test_env30_c_fail_wiki_localeconv`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_comprehensive_safe_usage.c` → `test_env30_c_pass_testcases_comprehensive_safe_usage`
- ⏭️ NOT RUN `testcases_safe_error_recovery.c` → `test_env30_c_pass_testcases_safe_error_recovery`
- ⏭️ NOT RUN `testcases_safe_getenv_usage.c` → `test_env30_c_pass_testcases_safe_getenv_usage`
- ⏭️ NOT RUN `testcases_safe_localeconv_usage.c` → `test_env30_c_pass_testcases_safe_localeconv_usage`
- ⏭️ NOT RUN `testcases_safe_multiple_functions.c` → `test_env30_c_pass_testcases_safe_multiple_functions`
- ⏭️ NOT RUN `testcases_safe_platform_usage.c` → `test_env30_c_pass_testcases_safe_platform_usage`
- ⏭️ NOT RUN `testcases_safe_setlocale_usage.c` → `test_env30_c_pass_testcases_safe_setlocale_usage`
- ⏭️ NOT RUN `testcases_safe_strerror_usage.c` → `test_env30_c_pass_testcases_safe_strerror_usage`
- ⏭️ NOT RUN `testcases_safe_thread_usage.c` → `test_env30_c_pass_testcases_safe_thread_usage`
- ⏭️ NOT RUN `testcases_safe_time_functions.c` → `test_env30_c_pass_testcases_safe_time_functions`
- ⏭️ NOT RUN `wiki_getenv_environment_not_modified.c` → `test_env30_c_pass_wiki_getenv_environment_not_modified`
- ⏭️ NOT RUN `wiki_getenv_modifying_the_environment_in_posix.c` → `test_env30_c_pass_wiki_getenv_modifying_the_environment_in_posix`
- ⏭️ NOT RUN `wiki_localeconv_copy.c` → `test_env30_c_pass_wiki_localeconv_copy`

---

### 🔶 ENV03-C - Not Implemented (has tests)

<a id="rule-env03c"></a>

**Title:** Sanitize the environment when invoking external programs

**Description:** Many programs and libraries, including the shared library loader on both UNIX
and Windows systems, depend on environment variable settings. Because
environment variables are inherited from the parent process when a program is
executed, an attacker can easily sabotage variables, causing a program to behave
in an unexpected and insecure manner [Viega 2003]. All programs, particularly
those running with higher privileges than the caller (such as those
withsetuid/setgidflags), should treat their environment as untrusted user input.
Because the environment is inherited by processes spawned by calls to
thefork(),system(), orexec()functions, it is important to verify that the
environment does not contain any values that can lead to unexpected behavior.
The best practice for such programs is to

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_posixls.c` → `test_env03_c_fail_wiki_posixls`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posixls.c` → `test_env03_c_pass_wiki_posixls`
- ⏭️ NOT RUN `wiki_posixls_2.c` → `test_env03_c_pass_wiki_posixls_2`

---

### 🔶 ENV32-C - Not Implemented (has tests)

<a id="rule-env32c"></a>

**Title:** All exit handlers must return normally

**Description:** The C Standard provides three functions that cause an application to terminate
normally:_Exit(),exit(), andquick_exit(). These are collectively calledexit
functions. When theexit()function is called, or control transfers out of
themain()entry point function, functions registered withatexit()are called (but
notat_quick_exit()). When thequick_exit()function is called, functions
registered withat_quick_exit()(but notatexit()) are called. These functions are
collectively calledexit handlers. When the_Exit()function is called, no exit
handlers or signal handlers are called. Exit handlers must terminate by
returning. It is important and potentially safety-critical for all exit handlers
to be allowed to perform their cleanup actions. This is particularly true
because the application programmer does not always know about handlers that may
have been installed by support libraries. Two specific issues include nested
calls to an exit function and terminating a call to anexithandler by
invokinglongjmp. A nested call to an exit function isundefined behavior.
(Seeundefined behavior 187.) This behavior can occur only when an exit function
is invoked from an exit handler or when an exit function is called from within a
signal handler. (SeeSIG30-C. Call only asynchronous-safe functions within signal
handlers.)

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_env32_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_env32_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_env32_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_env32_c_pass_wiki_compliant_2`

---

### 🔶 ENV31-C - Not Implemented (has tests)

<a id="rule-env31c"></a>

**Title:** Do not rely on an environment pointer following an operation that may invalidate it

**Description:** Some implementations provide a nonportable environment pointer that is valid
whenmain()is called but may be invalidated by operations that modify the
environment. The C Standard, J.5.2 [ISO/IEC 9899:2024], states Consequently,
under ahosted environmentsupporting this common extension, it is possible to
access the environment through a modified form ofmain():

**Test Coverage:** 6 tests (2 fail, 4 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_env31_c_fail_wiki_posix`
- ⏭️ NOT RUN `wiki_windows.c` → `test_env31_c_fail_wiki_windows`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_env31_c_pass_wiki_compliant_3`
- ⏭️ NOT RUN `wiki_compliant_4_2.c` → `test_env31_c_pass_wiki_compliant_4_2`
- ⏭️ NOT RUN `wiki_posix.c` → `test_env31_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_windows.c` → `test_env31_c_pass_wiki_windows`

---

### 🔶 ENV01-C - Not Implemented (has tests)

<a id="rule-env01c"></a>

**Title:** Do not make assumptions about the size of an environment variable

**Description:** Do not make any assumptions about the size of environment variables because an
adversary might have full control over the environment. If the environment
variable needs to be stored, the length of the associated string should be
calculated and the storage dynamically allocated (seeSTR31-C. Guarantee that
storage for strings has sufficient space for character data and the null
terminator). This noncompliant code example copies the string returned
bygetenv()into a fixed-size buffer: void f() { char path[PATH_MAX]; /* Requires
PATH_MAX to be defined */ strcpy(path, getenv("PATH")); /* Use path */ }

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_env01_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_env01_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_posix_or_c2x.c` → `test_env01_c_pass_wiki_posix_or_c2x`

---

## Category: ERR

<a id="category-err"></a>

**Implementation Status:** 2 / 11 rules (18.2%)

### 🔶 ERR05-C - Not Implemented (has tests)

<a id="rule-err05c"></a>

**Title:** Application-independent code should provide error detection without dictating error handling

**Description:** Application-independent code includes code that is When application-specific
code detects an error, it can immediately respond with a specific action, as in
if (something_really_bad_happens) { take_me_some_place_safe(); }

**Test Coverage:** 5 tests (1 fail, 4 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_err05_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_address_argument.c` → `test_err05_c_pass_wiki_address_argument`
- ⏭️ NOT RUN `wiki_global_error_indicator.c` → `test_err05_c_pass_wiki_global_error_indicator`
- ⏭️ NOT RUN `wiki_return_value.c` → `test_err05_c_pass_wiki_return_value`
- ⏭️ NOT RUN `wiki_setjmpandlongjmp.c` → `test_err05_c_pass_wiki_setjmpandlongjmp`

---

### 🔶 ERR01-C - Not Implemented (has tests)

<a id="rule-err01c"></a>

**Title:** Use ferror() rather than errno to check for FILE stream errors

**Description:** Useferror()rather thanerrnoto check whether an error has occurred on a file
stream (for example, after a long chain ofstdiocalls). Theferror()function tests
the error indicator for a specified stream and returns nonzero if and only if
the error indicator is set for the stream. Manyimplementationsof thestdiopackage
adjust their behavior slightly ifstdoutis a terminal. To make the determination,
these implementations perform some operation that fails (withENOTTY) ifstdoutis
not a terminal. Although the output operation goes on to complete
successfully,errnostill containsENOTTY. This behavior can be mildly confusing,
but it is not strictly incorrect because it is meaningful for a program to
inspect the contents oferrnoonly after an error has been reported. More
precisely,errnois meaningful only after a library function that setserrnoon
error has returned an error code. errno = 0; printf("This\n"); printf("is\n");
printf("a\n"); printf("test.\n"); if (errno != 0) { fprintf(stderr, "printf
failed: %s\n", strerror(errno)); }

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_err01_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_err01_c_pass_wiki_compliant_1`

---

### ✅ ERR33-C - Implemented

<a id="rule-err33c"></a>

**Title:** Detect and handle standard library errors

**Description:** The majority of the standard library functions, including I/O functions and
memory allocation functions, return either a valid value or a value of the
correct return type that indicates an error (for example, −1 or a null pointer).
Assuming that all calls to such functions will succeed and failing to check the
return value for an indication of an error is a dangerous practice that may lead
tounexpectedorundefined behaviorwhen an error occurs. It is essential that
programs detect and appropriately handle all errors in accordance with an error-
handling policy. The successful completion or failure of each of the standard
library functions listed in the following table shall be determined either by
comparing the function’s return value with the value listed in the column
labeled “Error Return” or by calling one of the library functions mentioned in
the footnotes. Standard Library Functions

**Test Coverage:** 51 tests (35 fail, 16 pass)

**Test Results:** 0/51 passed (0.0%), 51 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_atexit_unchecked.c` → `test_err33_c_fail_testcases_atexit_unchecked`
- ⏭️ NOT RUN `testcases_calloc_unchecked.c` → `test_err33_c_fail_testcases_calloc_unchecked`
- ⏭️ NOT RUN `testcases_fclose_unchecked.c` → `test_err33_c_fail_testcases_fclose_unchecked`
- ⏭️ NOT RUN `testcases_fflush_unchecked.c` → `test_err33_c_fail_testcases_fflush_unchecked`
- ⏭️ NOT RUN `testcases_fgets_unchecked.c` → `test_err33_c_fail_testcases_fgets_unchecked`
- ⏭️ NOT RUN `testcases_file_open_unchecked.c` → `test_err33_c_fail_testcases_file_open_unchecked`
- ⏭️ NOT RUN `testcases_fprintf_unchecked.c` → `test_err33_c_fail_testcases_fprintf_unchecked`
- ⏭️ NOT RUN `testcases_fread_unchecked.c` → `test_err33_c_fail_testcases_fread_unchecked`
- ⏭️ NOT RUN `testcases_freopen_unchecked.c` → `test_err33_c_fail_testcases_freopen_unchecked`
- ⏭️ NOT RUN `testcases_fscanf_unchecked.c` → `test_err33_c_fail_testcases_fscanf_unchecked`
- ⏭️ NOT RUN `testcases_fseek_unchecked.c` → `test_err33_c_fail_testcases_fseek_unchecked`
- ⏭️ NOT RUN `testcases_ftell_unchecked.c` → `test_err33_c_fail_testcases_ftell_unchecked`
- ⏭️ NOT RUN `testcases_fwrite_unchecked.c` → `test_err33_c_fail_testcases_fwrite_unchecked`
- ⏭️ NOT RUN `testcases_getenv_unchecked.c` → `test_err33_c_fail_testcases_getenv_unchecked`
- ⏭️ NOT RUN `testcases_malloc_unchecked.c` → `test_err33_c_fail_testcases_malloc_unchecked`
- ⏭️ NOT RUN `testcases_mktime_unchecked.c` → `test_err33_c_fail_testcases_mktime_unchecked`
- ⏭️ NOT RUN `testcases_putenv_unchecked.c` → `test_err33_c_fail_testcases_putenv_unchecked`
- ⏭️ NOT RUN `testcases_realloc_unchecked.c` → `test_err33_c_fail_testcases_realloc_unchecked`
- ⏭️ NOT RUN `testcases_remove_unchecked.c` → `test_err33_c_fail_testcases_remove_unchecked`
- ⏭️ NOT RUN `testcases_rename_unchecked.c` → `test_err33_c_fail_testcases_rename_unchecked`
- ⏭️ NOT RUN `testcases_setlocale_unchecked.c` → `test_err33_c_fail_testcases_setlocale_unchecked`
- ⏭️ NOT RUN `testcases_setvbuf_unchecked.c` → `test_err33_c_fail_testcases_setvbuf_unchecked`
- ⏭️ NOT RUN `testcases_signal_unchecked.c` → `test_err33_c_fail_testcases_signal_unchecked`
- ⏭️ NOT RUN `testcases_strdup_unchecked.c` → `test_err33_c_fail_testcases_strdup_unchecked`
- ⏭️ NOT RUN `testcases_strftime_unchecked.c` → `test_err33_c_fail_testcases_strftime_unchecked`
- ⏭️ NOT RUN `testcases_system_unchecked.c` → `test_err33_c_fail_testcases_system_unchecked`
- ⏭️ NOT RUN `testcases_time_unchecked.c` → `test_err33_c_fail_testcases_time_unchecked`
- ⏭️ NOT RUN `testcases_tmpfile_unchecked.c` → `test_err33_c_fail_testcases_tmpfile_unchecked`
- ⏭️ NOT RUN `testcases_tmpnam_unchecked.c` → `test_err33_c_fail_testcases_tmpnam_unchecked`
- ⏭️ NOT RUN `testcases_ungetc_unchecked.c` → `test_err33_c_fail_testcases_ungetc_unchecked`
- ⏭️ NOT RUN `wiki_calloc.c` → `test_err33_c_fail_wiki_calloc`
- ⏭️ NOT RUN `wiki_fseek.c` → `test_err33_c_fail_wiki_fseek`
- ⏭️ NOT RUN `wiki_realloc.c` → `test_err33_c_fail_wiki_realloc`
- ⏭️ NOT RUN `wiki_setlocale.c` → `test_err33_c_fail_wiki_setlocale`
- ⏭️ NOT RUN `wiki_snprintf.c` → `test_err33_c_fail_wiki_snprintf`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_file_open_check.c` → `test_err33_c_pass_testcases_file_open_check`
- ⏭️ NOT RUN `testcases_getenv_check.c` → `test_err33_c_pass_testcases_getenv_check`
- ⏭️ NOT RUN `testcases_malloc_check.c` → `test_err33_c_pass_testcases_malloc_check`
- ⏭️ NOT RUN `testcases_realloc_check.c` → `test_err33_c_pass_testcases_realloc_check`
- ⏭️ NOT RUN `testcases_remove_check.c` → `test_err33_c_pass_testcases_remove_check`
- ⏭️ NOT RUN `testcases_setlocale_check.c` → `test_err33_c_pass_testcases_setlocale_check`
- ⏭️ NOT RUN `testcases_signal_check.c` → `test_err33_c_pass_testcases_signal_check`
- ⏭️ NOT RUN `testcases_system_check.c` → `test_err33_c_pass_testcases_system_check`
- ⏭️ NOT RUN `testcases_time_check.c` → `test_err33_c_pass_testcases_time_check`
- ⏭️ NOT RUN `testcases_tmpfile_check.c` → `test_err33_c_pass_testcases_tmpfile_check`
- ⏭️ NOT RUN `wiki_calloc.c` → `test_err33_c_pass_wiki_calloc`
- ⏭️ NOT RUN `wiki_fseek.c` → `test_err33_c_pass_wiki_fseek`
- ⏭️ NOT RUN `wiki_realloc.c` → `test_err33_c_pass_wiki_realloc`
- ⏭️ NOT RUN `wiki_setlocale.c` → `test_err33_c_pass_wiki_setlocale`
- ⏭️ NOT RUN `wiki_snprintf.c` → `test_err33_c_pass_wiki_snprintf`
- ⏭️ NOT RUN `wiki_snprintfnull.c` → `test_err33_c_pass_wiki_snprintfnull`

---

### 🔶 ERR34-C - Not Implemented (has tests)

<a id="rule-err34c"></a>

**Title:** Detect errors when converting a string to a number

**Description:** The process of parsing an integer or floating-point number from a string can
produce many errors. The string might not contain a number. It might contain a
number of the correct type that is out of range (such as an integer that is
larger thanINT_MAX). The string may also contain extra information after the
number, which may or may not be useful after the conversion. These error
conditions must be detected and addressed when a string-to-number conversion is
performed using a C Standard Library function.
Thestrtol(),strtoll(),strtoimax(),strtoul(), strtoull(),strtoumax(),
strtof(),strtod(), andstrtold()functions convert the initial portion of a null-
terminated byte string to along int,long long int,intmax_t,unsigned long
int,unsigned long long int, uintmax_t, float, double, andlong
doublerepresentation, respectively. Use one of the C Standard
Librarystrto*()functions to parse an integer or floating-point number from a
string. These functions provide more robust error handling than alternative
solutions. Also, use thestrtol()function to convert to a smaller signed integer
type such assigned int,signed short, andsigned char, testing the result against
the range limits for that type. Likewise, use thestrtoul()function to convert to
a smaller unsigned integer type such asunsigned int,unsigned short, andunsigned
char, and test the result against the range limits for that type. These range
tests do nothing if the smaller type happens to have the same size and
representation for a particular implementation.

**Test Coverage:** 4 tests (3 fail, 1 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_atoi.c` → `test_err34_c_fail_wiki_atoi`
- ⏭️ NOT RUN `wiki_atoi_2.c` → `test_err34_c_fail_wiki_atoi_2`
- ⏭️ NOT RUN `wiki_noncompliant_example_sscanf.c` → `test_err34_c_fail_wiki_noncompliant_example_sscanf`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_strtol.c` → `test_err34_c_pass_wiki_strtol`

---

### ✅ ERR07-C - Implemented

<a id="rule-err07c"></a>

**Title:** Prefer functions that support error checking over equivalent functions that don't

**Description:** When you have a choice of two functions to accomplish the same task, prefer the
one with better error checking and reporting. The following table shows a list
of C standard library functions that provide limited or no error checking and
reporting along with preferable alternatives:
FunctionPreferableAlternativeCommentsatofstrtodNo error indication,undefined
behavioron erroratoistrtolNo error indication, undefined behavior on
erroratolstrtolNo error indication, undefined behavior on erroratollstrtollNo
error indication, undefined behavior on errorrewindfseekNo error indication,
silent failure on errorsetbufsetvbufNo error indication, silent failure on
errorctimeasctime/localtimeUndefined behavior iflocaltimefails

**Test Coverage:** 6 tests (3 fail, 3 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_atoi.c` → `test_err07_c_fail_wiki_atoi`
- ⏭️ NOT RUN `wiki_rewind.c` → `test_err07_c_fail_wiki_rewind`
- ⏭️ NOT RUN `wiki_setbuf.c` → `test_err07_c_fail_wiki_setbuf`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_fseek.c` → `test_err07_c_pass_wiki_fseek`
- ⏭️ NOT RUN `wiki_setvbuf.c` → `test_err07_c_pass_wiki_setvbuf`
- ⏭️ NOT RUN `wiki_strtol.c` → `test_err07_c_pass_wiki_strtol`

---

### 🔶 ERR02-C - Not Implemented (has tests)

<a id="rule-err02c"></a>

**Title:** Avoid in-band error indicators

**Description:** Avoidin-band error indicatorswhile designing interfaces. This practice is
commonly used by C library functions but is not recommended. One example from
the C Standard of a troublesome in-band error indicator isEOF(seeFIO34-C.
Distinguish between characters read from a file and EOF or WEOF). Another
problematic use of in-band error indicators from the C Standard involving
thesize_tandtime_ttypes is described by This noncompliant code example is from
the Linux Kernel Mailing List archive site, although similar examples are
common: int i; ssize_t count = 0; for (i = 0; i < 9; ++i) { count += sprintf(
buf + count, "%02x ", ((u8 *)&slreg_num)[i] ); } count += sprintf(buf + count,
"\n");

**Test Coverage:** 5 tests (2 fail, 3 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_posixssize_t.c` → `test_err02_c_fail_wiki_posixssize_t`
- ⏭️ NOT RUN `wiki_sprintf.c` → `test_err02_c_fail_wiki_sprintf`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posixsize_t.c` → `test_err02_c_pass_wiki_posixsize_t`
- ⏭️ NOT RUN `wiki_sprintf_m.c` → `test_err02_c_pass_wiki_sprintf_m`
- ⏭️ NOT RUN `wiki_sprintf_m_2.c` → `test_err02_c_pass_wiki_sprintf_m_2`

---

### ⚫ ERR00-C - Not Implemented (no tests)

<a id="rule-err00c"></a>

**Title:** Adopt and implement a consistent and comprehensive error-handling policy

**Description:** A secure system is invariably subject to stresses, such as those caused by
attack, erroneous or malicious inputs, hardware or software faults,
unanticipated user behavior, and unexpected environmental changes that are
outside the bounds of "normal operation." Yet the system must continue to
deliver essential services in a timely manner, safely and securely. To
accomplish this, the system must exhibit qualities such
asrobustness,reliability,error tolerance,fault tolerance, performance, and
security. All of these system-quality attributes depend on consistent and
comprehensive error handling that supports the goals of the overall system.
ISO/IEC TR 24772, section 6.39.1 [ISO/IEC TR 24772], says: Effective error
handling (which includes error reporting, report aggregation, analysis,
response, and recovery) is a central aspect of the design, implementation,
maintenance, and operation of systems that exhibit survivability under stress.
Survivability is the capability of a system to fulfill its mission, in a timely
manner, despite an attack, accident, or other stress that is outside the bounds
of normal operation [Lipson 2000]. If full services cannot be maintained under a
given stress, survivable systems degrade gracefully, continue to deliver
essential services, and recover full services as conditions permit.

**Test Coverage:** 0 tests (0 fail, 0 pass)

---

### 🔶 ERR04-C - Not Implemented (has tests)

<a id="rule-err04c"></a>

**Title:** Choose an appropriate termination strategy

**Description:** Some errors, such as out-of-range values, might be the result of erroneous user
input. Interactive programs typically handle such errors by rejecting the input
and prompting the user for an acceptable value. Servers reject invalid user
input by indicating an error to the client while at the same time continuing to
service other clients' valid requests. Allrobustprograms must be prepared to
gracefully handle resource exhaustion, such as low memory or disk space
conditions, at a minimum by preventing the loss of user data kept in volatile
storage. Interactive programs may give the user the option to save data on an
alternative medium, whereas network servers may respond by reducing throughput
or otherwise degrading the quality of service. However, when certain kinds of
errors are detected, such as irrecoverable logic errors, rather than risk data
corruption by continuing to execute in an indeterminate state, the appropriate
strategy may be for the system to quickly shut down, allowing the operator to
start it afresh in a determinate state. ISO/IEC TR 24772:2013, Section 6.39,
"Termination Strategy [REU]," [ISO/IEC TR 24772:2013], says: And

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_err04_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_err04_c_pass_wiki_compliant_1`

---

### 🔶 ERR06-C - Not Implemented (has tests)

<a id="rule-err06c"></a>

**Title:** Understand the termination behavior of assert() and abort()

**Description:** The C Standard, subclause 7.2.1.1 [ISO/IEC 9899:2011], definesassert()to have
the following behavior: Becauseassert()callsabort(), cleanup functions
registered withatexit()are not called. If the intention of the programmer is to
properly clean up in the case of a failed assertion, then runtime assertions
should be replaced with static assertions where possible. (SeeDCL03-C. Use a
static assertion to test the value of a constant expression.) When the assertion
is based on runtime data, theassertshould be replaced with a runtime check that
implements the adopted error strategy (seeERR00-C. Adopt and implement a
consistent and comprehensive error-handling policy). SeeERR04-C. Choose an
appropriate termination strategyfor more information on program termination
strategies andMSC11-C. Incorporate diagnostic tests using assertionsfor more
information on using theassert()macro.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_err06_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_err06_c_pass_wiki_compliant_1`

---

### 🔶 ERR30-C - Not Implemented (has tests)

<a id="rule-err30c"></a>

**Title:** Take care when reading errno

**Description:** The value oferrnois initialized to zero at program startup, but it is never
subsequently set to zero by any C standard library function. The value
oferrnomay be set to nonzero by a C standard library function call whether or
not there is an error, provided the use oferrnois not documented in the
description of the function. It is meaningful for a program to inspect the
contents oferrnoonly after an error might have occurred. More precisely,errnois
meaningful only after a library function that setserrnoon error has returned an
error code. According to Question 20.4 of C-FAQ [Summit 2005], Note thatatoi()is
not required to set the value oferrno.

**Test Coverage:** 7 tests (3 fail, 4 pass)

**Test Results:** 0/7 passed (0.0%), 7 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_fopen.c` → `test_err30_c_fail_wiki_fopen`
- ⏭️ NOT RUN `wiki_ftell.c` → `test_err30_c_fail_wiki_ftell`
- ⏭️ NOT RUN `wiki_strtoul.c` → `test_err30_c_fail_wiki_strtoul`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_fopen_c.c` → `test_err30_c_pass_wiki_fopen_c`
- ⏭️ NOT RUN `wiki_fopen_posix.c` → `test_err30_c_pass_wiki_fopen_posix`
- ⏭️ NOT RUN `wiki_ftell.c` → `test_err30_c_pass_wiki_ftell`
- ⏭️ NOT RUN `wiki_strtoul.c` → `test_err30_c_pass_wiki_strtoul`

---

### 🔶 ERR32-C - Not Implemented (has tests)

<a id="rule-err32c"></a>

**Title:** Do not rely on indeterminate values of errno

**Description:** According to the C Standard Annex J.2 (133) [ISO/IEC 9899:2024], the behavior of
a program isundefinedwhen Seeundefined behavior 133. A signal handler is allowed
to callsignal();if that fails,signal()returnsSIG_ERRand setserrnoto a positive
value. However, if the event that caused a signal was external (not the result
of the program callingabort()orraise()), the only functions the signal handler
may call are_Exit()orabort(), or it may callsignal()on the signal currently
being handled; ifsignal()fails, the value oferrnoisindeterminate.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_err32_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_posix.c` → `test_err32_c_fail_wiki_posix`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_err32_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_posix.c` → `test_err32_c_pass_wiki_posix`

---

## Category: EXP

<a id="category-exp"></a>

**Implementation Status:** 6 / 31 rules (19.4%)

### 🔶 EXP36-C - Not Implemented (has tests)

<a id="rule-exp36c"></a>

**Title:** Do not cast pointers into more strictly aligned pointer types

**Description:** Do not convert a pointer value to a pointer type that is more strictly aligned
than the referenced type. Different alignments are possible for different types
of objects. If the type-checking system is overridden by an explicit cast or the
pointer is converted to a void pointer (void *) and then to a different type,
the alignment of an object may be changed. The C Standard, 6.3.2.3, paragraph 7
[ISO/IEC 9899:2024], states Seeundefined behavior 24.

**Test Coverage:** 6 tests (3 fail, 3 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp36_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_exp36_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_exp36_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_exp36_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_exp36_c_pass_wiki_compliant_3`
- ⏭️ NOT RUN `wiki_intermediate_object.c` → `test_exp36_c_pass_wiki_intermediate_object`

---

### ✅ EXP07-C - Implemented

<a id="rule-exp07c"></a>

**Title:** Do not diminish the benefits of constants by assuming their values in expressions

**Description:** If a constant value is given for an identifier, do not diminish the
maintainability of the code in which it is used by assuming its value in
expressions. Simply giving the constant a name is not enough to ensure
modifiability; you must be careful to always use the name, and remember that the
value can change. This recommendation is related toDCL06-C. Use meaningful
symbolic constants to represent literal values. The headerstdio.hdefines
theBUFSIZmacro, which expands to an integer constant expression that is the size
of the buffer used by thesetbuf()function. This noncompliant code example
defeats the purpose of definingBUFSIZas a constant by assuming its value in the
following expression: #include <stdio.h> /* ... */ nblocks = 1 + ((nbytes - 1)
>> 9); /* BUFSIZ = 512 = 2^9 */

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 2/2 passed (100.0%)

#### Fail Tests (Should Detect Violations)

- ✅ PASS `wiki_noncompliant_1.c` → `test_exp07_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ✅ PASS `wiki_compliant_1.c` → `test_exp07_c_pass_wiki_compliant_1`

---

### 🔶 EXP39-C - Not Implemented (has tests)

<a id="rule-exp39c"></a>

**Title:** Do not access a variable through a pointer of an incompatible type

**Description:** Modifying a variable through a pointer of an incompatible type (other
thanunsigned char) can lead to unpredictable results.Subclause 6.2.7 of the C
Standard states that two types may be distinct yet compatible and addresses
preciselywhen two distinct types are compatible. This problem is often caused by
a violation of aliasing rules. The C Standard, 6.5, paragraph 7 [ISO/IEC
9899:2024], specifies those circumstances in which an object may or may not be
aliased. Accessing an object by means of any otherlvalueexpression (other
thanunsigned char) isundefined behavior 36.

**Test Coverage:** 8 tests (4 fail, 4 pass)

**Test Results:** 0/8 passed (0.0%), 8 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp39_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_exp39_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_exp39_c_fail_wiki_noncompliant_3`
- ⏭️ NOT RUN `wiki_noncompliant_4.c` → `test_exp39_c_fail_wiki_noncompliant_4`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp39_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_exp39_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_exp39_c_pass_wiki_compliant_3`
- ⏭️ NOT RUN `wiki_compliant_4.c` → `test_exp39_c_pass_wiki_compliant_4`

---

### 🔶 EXP46-C - Not Implemented (has tests)

<a id="rule-exp46c"></a>

**Title:** Do not use a bitwise operator with a Boolean-like operand

**Description:** Mixing bitwise and relational operators in the same full expression can be a
sign of a logic error in the expression where a logical operator is usually the
intended operator. Do not use the bitwise AND (&), bitwise OR (|), or bitwise
XOR (^) operators with an operand of type_Bool, or the result of arelational-
expressionorequality-expression. If the bitwise operator is intended, it should
be indicated with use of a parenthesized expression. In this noncompliant code
example, a bitwise&operator is used with the results of twoequality-expressions:
if (getuid() == 0 & getgid() == 0) { /* ... */ }

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp46_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp46_c_pass_wiki_compliant_1`

---

### ✅ EXP15-C - Implemented

<a id="rule-exp15c"></a>

**Title:** Do not place a semicolon on the same line as an if, for, or while statement

**Description:** Do not use a semicolon on the same line as anif,for, orwhilestatement because it
typically indicates programmer error and can result in unexpected behavior. In
this noncompliant code example, a semicolon is used on the same line as
anifstatement: if (a == b); { /* ... */ }

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp15_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp15_c_pass_wiki_compliant_1`

---

### 🔶 EXP47-C - Not Implemented (has tests)

<a id="rule-exp47c"></a>

**Title:** Do not call va_arg with an argument of the incorrect type

**Description:** The variable arguments passed to a variadic function are accessed by calling
theva_arg()macro. This macro accepts theva_listrepresenting the variable
arguments of the function invocation and the type denoting the expected argument
type for the argument being retrieved. The macro is typically invoked within a
loop, being called once for each expected argument. However, there are no type
safety guarantees that the type passed tova_argmatches the type passed by the
caller, and there are generally no compile-time checks that prevent the macro
from being invoked with no argument available to the function call. The C
Standard, 7.16.1.1, states [ISO/IEC 9899:2024], in part: Ensure that an
invocation of theva_arg()macro does not attempt to access an argument that was
not passed to the variadic function. Further, the type passed to
theva_arg()macro must match the type passed to the variadic function after
default argument promotions have been applied. Either circumstance results
inundefined behavior 141.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp47_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_exp47_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp47_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_exp47_c_pass_wiki_compliant_2`

---

### 🔶 EXP45-C - Not Implemented (has tests)

<a id="rule-exp45c"></a>

**Title:** Do not perform assignments in selection statements

**Description:** Do not use the assignment operator in the contexts listed in the following table
because doing so typically indicates programmer error and can result
inunexpected behavior. OperatorContextifControlling expressionwhileControlling
expressiondo ... whileControlling expressionforSecond operand?:First
operand?:Second or third operands, where the ternary expression is used in any
of these contexts&&Either operand||either operand,Second operand, when the comma
expression is used in any of these contexts Performing assignment statements in
other contexts do not violate this rule. However, they may violate other rules,
such asEXP30-C. Do not depend on the order of evaluation for side effects.

**Test Coverage:** 8 tests (2 fail, 6 pass)

**Test Results:** 0/8 passed (0.0%), 8 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp45_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_exp45_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_6.c` → `test_exp45_c_pass_wiki_compliant_6`
- ⏭️ NOT RUN `wiki_forstatement.c` → `test_exp45_c_pass_wiki_forstatement`
- ⏭️ NOT RUN `wiki_intentional_assignment.c` → `test_exp45_c_pass_wiki_intentional_assignment`
- ⏭️ NOT RUN `wiki_rhs_variable.c` → `test_exp45_c_pass_wiki_rhs_variable`
- ⏭️ NOT RUN `wiki_rhs_variable_2.c` → `test_exp45_c_pass_wiki_rhs_variable_2`
- ⏭️ NOT RUN `wiki_unintentional_assignment.c` → `test_exp45_c_pass_wiki_unintentional_assignment`

---

### 🔶 EXP12-C - Not Implemented (has tests)

<a id="rule-exp12c"></a>

**Title:** Do not ignore values returned by functions

**Description:** Many functions return useful values whether or not the function has side
effects. In most cases, this value is used to signify whether the function
successfully completed its task or if some error occurred (seeERR02-C. Avoid in-
band error indicators). Other times, the value is the result of some computation
and is an integral part of the function's API. Subclause 6.8.3 of the C Standard
[ISO/IEC 9899:2011] states: All expression statements, such as function calls
with an ignored value, are implicitly cast tovoid. Because a return value often
contains important information about possible errors, it should always be
checked; otherwise, the cast should be made explicit to signify programmer
intent. If a function returns no meaningful value, it should be declared with
return typevoid.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp12_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp12_c_pass_wiki_compliant_1`

---

### 🔶 EXP32-C - Not Implemented (has tests)

<a id="rule-exp32c"></a>

**Title:** Do not access a volatile object through a nonvolatile reference

**Description:** An object that has volatile-qualified type may be modified in ways unknown to
theimplementationor have other unknownside effects. Referencing a volatile
object by using a non-volatile lvalue isundefined behavior. The C Standard,
6.7.4 paragraph 7 [ISO/IEC 9899:2024], states Seeundefined behavior 62. In this
noncompliant code example, a volatile object is accessed through a non-volatile-
qualified reference, resulting inundefined behavior 62:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp32_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp32_c_pass_wiki_compliant_1`

---

### ✅ EXP00-C - Implemented

<a id="rule-exp00c"></a>

**Title:** Use parentheses for precedence of operation

**Description:** C programmers commonly make errors regarding the precedence rules of C operators
because of the unintuitive low-precedence levels of&,|,^,<<, and>>. Mistakes
regarding precedence rules can be avoided by the suitable use of parentheses.
Using parentheses defensively reduces errors and, if not taken to excess, makes
the code more readable. Subclause 6.5 of the C Standard defines the precedence
of operation by the order of the subclauses. The intent of the expression in
this noncompliant code example is to test the least significant bit ofx:

**Test Coverage:** 4 tests (3 fail, 1 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp00_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_exp00_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3_3.c` → `test_exp00_c_fail_wiki_noncompliant_3_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp00_c_pass_wiki_compliant_1`

---

### 🔶 EXP30-C - Not Implemented (has tests)

<a id="rule-exp30c"></a>

**Title:** Do not depend on the order of evaluation for side effects

**Description:** Evaluation of an expression may produceside effects. At specific points during
execution, known assequence points, all side effects of previous evaluations are
complete, and no side effects of subsequent evaluations have yet taken place. Do
not depend on the order of evaluation for side effects unless there is an
intervening sequence point. The C Standard, 6.5, paragraph 2 [ISO/IEC
9899:2024], states This requirement must be met for each allowable ordering of
the subexpressions of a full expression; otherwise, the behavior isundefined.
(Seeundefined behavior 34.)

**Test Coverage:** 8 tests (3 fail, 5 pass)

**Test Results:** 0/8 passed (0.0%), 8 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp30_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_exp30_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_exp30_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp30_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2_2.c` → `test_exp30_c_pass_wiki_compliant_2_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_exp30_c_pass_wiki_compliant_3`
- ⏭️ NOT RUN `wiki_compliant_4_2.c` → `test_exp30_c_pass_wiki_compliant_4_2`
- ⏭️ NOT RUN `wiki_compliant_5.c` → `test_exp30_c_pass_wiki_compliant_5`

---

### ✅ EXP05-C - Implemented

<a id="rule-exp05c"></a>

**Title:** Do not cast away a const qualification

**Description:** Do not cast away aconstqualification on an object of pointer type. Casting away
theconstqualification allows a program to modify the object referred to by the
pointer, which may result inundefined behavior. Seeundefined behavior 61in
Appendix J of the C Standard. As an illustration, the C Standard [ISO/IEC
9899:2011] provides a footnote (subclause 6.7.3, paragraph 4):
Theremove_spaces()function in this noncompliant code example accepts a pointer
to a stringstrand a string lengthslenand removes the space character from the
string by shifting the remaining characters toward the front of the string. The
functionremove_spaces()is passed aconstcharpointer as an argument.
Theconstqualification is cast away, and then the contents of the string are
modified.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp05_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_exp05_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp05_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_exp05_c_pass_wiki_compliant_2`

---

### 🔶 EXP11-C - Not Implemented (has tests)

<a id="rule-exp11c"></a>

**Title:** Do not make assumptions regarding the layout of structures with bit-fields

**Description:** The internal representations of bit-field structures have several properties
(such as internal padding) that areimplementation-defined. Additionally, bit-
field structures have several implementation-defined constraints: Consequently,
it is impossible to write portable safe code that makes assumptions regarding
the layout of bit-field structure members. Bit-fields can be used to allow flags
or other integer values with small ranges to be packed together to save storage
space. Bit-fields can improve the storage efficiency of structures. Compilers
typically allocate consecutive bit-field structure members into the sameint-
sized storage, as long as they fit completely into that storage unit. However,
the order of allocation within a storage unit is implementation-defined.
Someimplementationsareright-to-left: the first member occupies the low-order
position of the storage unit. Others areleft-to-right: the first member occupies
the high-order position of the storage unit. Calculations that depend on the
order of bits within a storage unit may produce different results on different
implementations.

**Test Coverage:** 7 tests (5 fail, 2 pass)

**Test Results:** 0/7 passed (0.0%), 7 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_bit_field_alignment.c` → `test_exp11_c_fail_wiki_bit_field_alignment`
- ⏭️ NOT RUN `wiki_bit_field_alignment_2.c` → `test_exp11_c_fail_wiki_bit_field_alignment_2`
- ⏭️ NOT RUN `wiki_bit_field_alignment_3.c` → `test_exp11_c_fail_wiki_bit_field_alignment_3`
- ⏭️ NOT RUN `wiki_bit_field_alignment_4.c` → `test_exp11_c_fail_wiki_bit_field_alignment_4`
- ⏭️ NOT RUN `wiki_bit_field_overlap.c` → `test_exp11_c_fail_wiki_bit_field_overlap`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_bit_field_alignment.c` → `test_exp11_c_pass_wiki_bit_field_alignment`
- ⏭️ NOT RUN `wiki_bit_field_overlap.c` → `test_exp11_c_pass_wiki_bit_field_overlap`

---

### 🔶 EXP08-C - Not Implemented (has tests)

<a id="rule-exp08c"></a>

**Title:** Ensure pointer arithmetic is used correctly

**Description:** When performing pointer arithmetic, the size of the value to add to a pointer is
automatically scaled to the size of the type of the pointed-to object. For
instance, when adding a value to the byte address of a 4-byte integer, the value
is scaled by a factor of 4 and then added to the pointer. Failing to understand
how pointer arithmetic works can lead to miscalculations that result in serious
errors, such as buffer overflows. In this noncompliant code example, integer
values returned byparseint(getdata())are stored into an array
ofINTBUFSIZEelements of typeintcalledbuf[Dowd 2006]. If data is available for
insertion intobuf(which is indicated byhavedata()) andbuf_ptrhas not been
incremented pastbuf + sizeof(buf), an integer value is stored at the address
referenced bybuf_ptr. However, thesizeofoperator returns the total number of
bytes inbuf, which is typically a multiple of the number of elements inbuf. This
value is scaled to the size of an integer and added tobuf. As a result, the
check to make sure integers are not written past the end ofbufis incorrect, and
a buffer overflow is possible. int buf[INTBUFSIZE]; int *buf_ptr = buf; while
(havedata() && buf_ptr < (buf + sizeof(buf))) { *buf_ptr++ =
parseint(getdata()); }

**Test Coverage:** 5 tests (2 fail, 3 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp08_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_exp08_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp08_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2_2.c` → `test_exp08_c_pass_wiki_compliant_2_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_exp08_c_pass_wiki_compliant_3`

---

### 🔶 EXP43-C - Not Implemented (has tests)

<a id="rule-exp43c"></a>

**Title:** Avoid undefined behavior when using restrict-qualified pointers

**Description:** An object that is accessed through arestrict-qualified pointer has a special
association with that pointer. This association requires that all accesses to
that object use, directly or indirectly, the value of that particular pointer.
The intended use of therestrictqualifier is to promote optimization, and
deleting all instances of the qualifier from a program does not change its
meaning (that is, observable behavior). In the absence of this qualifier, other
pointers can alias this object. Caching the value in an object designated
through arestrict-qualified pointer is safe at the beginning of the block in
which the pointer is declared because no preexisting aliases may also be used to
reference that object. The cached value must be restored to the object by the
end of the block, where preexisting aliases again become available. New aliases
may be formed within the block, but these must all depend on the value of
therestrict-qualified pointer so that they can be identified and adjusted to
refer to the cached value. For arestrict-qualified pointer at file scope, the
block is the body of each function in the file [Walls 2006]. Developers should
be aware that C++ does not support therestrictqualifier, but some C++ compiler
implementations support an equivalent qualifier as an extension. The C Standard
[ISO/IEC 9899:2024] identifies the followingundefined behavior 66: This is an
oversimplification, however, and it is important to review the formal definition
ofrestrictin subclause 6.7.3.1 of the C Standard to properly understand
undefined behaviors associated with the use ofrestrict-qualified pointers.

**Test Coverage:** 12 tests (6 fail, 6 pass)

**Test Results:** 0/12 passed (0.0%), 12 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp43_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_exp43_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_exp43_c_fail_wiki_noncompliant_3`
- ⏭️ NOT RUN `wiki_noncompliant_4.c` → `test_exp43_c_fail_wiki_noncompliant_4`
- ⏭️ NOT RUN `wiki_noncompliant_5.c` → `test_exp43_c_fail_wiki_noncompliant_5`
- ⏭️ NOT RUN `wiki_noncompliant_6.c` → `test_exp43_c_fail_wiki_noncompliant_6`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp43_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_exp43_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_exp43_c_pass_wiki_compliant_3`
- ⏭️ NOT RUN `wiki_compliant_4.c` → `test_exp43_c_pass_wiki_compliant_4`
- ⏭️ NOT RUN `wiki_compliant_5.c` → `test_exp43_c_pass_wiki_compliant_5`
- ⏭️ NOT RUN `wiki_compliant_6.c` → `test_exp43_c_pass_wiki_compliant_6`

---

### 🔶 EXP42-C - Not Implemented (has tests)

<a id="rule-exp42c"></a>

**Title:** Do not compare padding data

**Description:** The C Standard, 6.7.3.2 paragraph 19 [ISO/IEC 9899:2024], states Subclause
6.7.11, paragraph 10, states that The only exception is that padding bits are
set to zero when a static or thread-local object is implicitly initialized
(paragraph 11):

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp42_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp42_c_pass_wiki_compliant_1`

---

### 🔶 EXP09-C - Not Implemented (has tests)

<a id="rule-exp09c"></a>

**Title:** Use sizeof to determine the size of a type or variable

**Description:** Do not hard code the size of a type into an application. Because of alignment,
padding, and differences in basic types (e.g., 32-bit versus 64-bit pointers),
the size of most types can vary between compilers and even versions of the same
compiler. Using thesizeofoperator to determine sizes improves the clarity of
what is meant and ensures that changes between compilers or versions will not
affect the code. Type alignment requirements can also affect the size of
structures. For example, the size of the following structure isimplementation-
defined: struct s { int i; double d; };

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp09_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp09_c_pass_wiki_compliant_1`

---

### 🔶 EXP44-C - Not Implemented (has tests)

<a id="rule-exp44c"></a>

**Title:** Do not rely on side effects in operands to sizeof, _Alignof, or _Generic

**Description:** Some operators do not evaluate their operands beyond the type information the
operands provide. When using one of these operators, do not pass an operand that
would otherwise yield a side effect since the side effect will not be generated.
Thesizeofoperator yields the size (in bytes) of its operand, which may be an
expression or the parenthesized name of a type. In most cases, the operand is
not evaluated. A possible exception is when the type of the operand is a
variable length array type (VLA); then the expression is evaluated. When part of
the operand of the sizeof operator is a VLA type and when changing the value of
the VLA's size expression would not affect the result of the operator, it
isunspecifiedwhether or not the size expression is evaluated. (Seeunspecified
behavior 22.) The operand passed to_Alignofis never evaluated, despite not being
an expression. For instance, if the operand is a VLA type and the VLA's size
expression contains a side effect, that side effect is never evaluated.

**Test Coverage:** 8 tests (4 fail, 4 pass)

**Test Results:** 0/8 passed (0.0%), 8 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_alignof.c` → `test_exp44_c_fail_wiki_alignof`
- ⏭️ NOT RUN `wiki_generic.c` → `test_exp44_c_fail_wiki_generic`
- ⏭️ NOT RUN `wiki_sizeof.c` → `test_exp44_c_fail_wiki_sizeof`
- ⏭️ NOT RUN `wiki_sizeof_vla.c` → `test_exp44_c_fail_wiki_sizeof_vla`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_alignof.c` → `test_exp44_c_pass_wiki_alignof`
- ⏭️ NOT RUN `wiki_generic.c` → `test_exp44_c_pass_wiki_generic`
- ⏭️ NOT RUN `wiki_sizeof.c` → `test_exp44_c_pass_wiki_sizeof`
- ⏭️ NOT RUN `wiki_sizeof_vla.c` → `test_exp44_c_pass_wiki_sizeof_vla`

---

### 🔶 EXP19-C - Not Implemented (has tests)

<a id="rule-exp19c"></a>

**Title:** Use braces for the body of an if, for, or while statement

**Description:** Opening and closing braces forif,for, andwhilestatements should always be used
even if the statement's body contains only a single statement. If anif,while,
orforstatement is used in a macro, the macro definition should not conclude with
a semicolon. (SeePRE11-C. Do not conclude macro definitions with a semicolon.)
Braces improve the uniformity and readability of code. More important, when
inserting an additional statement into a body containing only a single
statement, it is easy to forget to add braces because the indentation gives
strong (but misleading) guidance to the structure.

**Test Coverage:** 8 tests (5 fail, 3 pass)

**Test Results:** 0/8 passed (0.0%), 8 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_empty_block.c` → `test_exp19_c_fail_wiki_empty_block`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp19_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_exp19_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_exp19_c_fail_wiki_noncompliant_3`
- ⏭️ NOT RUN `wiki_noncompliant_4_2.c` → `test_exp19_c_fail_wiki_noncompliant_4_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp19_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_exp19_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_empty_block.c` → `test_exp19_c_pass_wiki_empty_block`

---

### 🔶 EXP37-C - Not Implemented (has tests)

<a id="rule-exp37c"></a>

**Title:** Call functions with the correct number and type of arguments

**Description:** Do not call a function with the wrong number or type of arguments. The C
Standard identifies two distinct situations in whichundefined behavior(UB) may
arise as a result of invoking a function using a declaration that is
incompatible with its definition or by supplying incorrect types or numbers of
arguments: UBDescription25A pointer is used to call a function whose type is not
compatible with the referenced type (6.3.2.3).37A function is defined with a
type that is not compatible with the type (of the expression) pointed to by the
expression that denotes the called function (6.5.2.2).

**Test Coverage:** 10 tests (5 fail, 5 pass)

**Test Results:** 0/10 passed (0.0%), 10 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp37_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_exp37_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_exp37_c_fail_wiki_noncompliant_3`
- ⏭️ NOT RUN `wiki_posix.c` → `test_exp37_c_fail_wiki_posix`
- ⏭️ NOT RUN `wiki_posix_2.c` → `test_exp37_c_fail_wiki_posix_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_complex_number.c` → `test_exp37_c_pass_wiki_complex_number`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_exp37_c_pass_wiki_compliant_3`
- ⏭️ NOT RUN `wiki_compliant_4.c` → `test_exp37_c_pass_wiki_compliant_4`
- ⏭️ NOT RUN `wiki_posix.c` → `test_exp37_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_real_number.c` → `test_exp37_c_pass_wiki_real_number`

---

### 🔶 EXP20-C - Not Implemented (has tests)

<a id="rule-exp20c"></a>

**Title:** Perform explicit tests to determine success, true and false, and equality

**Description:** Perform explicit tests to determine success, true/false, and equality to improve
the readability and maintainability of code and for compatibility with common
conventions. In particular, do not default the test for nonzero. For instance,
suppose afoo()function returns 0 to indicate failure or a nonzero value to
indicate success. Testing for inequality with 0, if (foo() != 0) ...

**Test Coverage:** 8 tests (5 fail, 3 pass)

**Test Results:** 0/8 passed (0.0%), 8 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp20_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_exp20_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3_2.c` → `test_exp20_c_fail_wiki_noncompliant_3_2`
- ⏭️ NOT RUN `wiki_noncompliant_4.c` → `test_exp20_c_fail_wiki_noncompliant_4`
- ⏭️ NOT RUN `wiki_noncompliant_5_2.c` → `test_exp20_c_fail_wiki_noncompliant_5_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp20_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_exp20_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_exp20_c_pass_wiki_compliant_3`

---

### 🔶 EXP40-C - Not Implemented (has tests)

<a id="rule-exp40c"></a>

**Title:** Do not modify constant objects

**Description:** The C Standard, 6.7.4, paragraph 7 [ISO/IEC 9899:2024], states See alsoundefined
behavior 61. There are existing compilerimplementationsthat allowconst-qualified
objects to be modified without generating a warning message.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp40_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp40_c_pass_wiki_compliant_1`

---

### 🔶 EXP03-C - Not Implemented (has tests)

<a id="rule-exp03c"></a>

**Title:** Do not assume the size of a structure is the sum of the sizes of its members

**Description:** The size of a structure is not always equal to the sum of the sizes of its
members. Subclause 6.7.2.1 of the C Standard states, "There may be unnamed
padding within a structure object, but not at its beginning" [ISO/IEC
9899:2011]. This unnamed padding is often calledstructure padding. Structure
members are arranged in memory as they are declared in the program text. Padding
may be added to the structure to ensure the structure is properly aligned in
memory. Structure padding allows for faster member access on many architectures.
Rearranging the fields in astructcan change the size of thestruct. It is
possible to minimize padding anomalies if the fields are arranged in such a way
that fields of the same size are grouped together.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp03_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp03_c_pass_wiki_compliant_1`

---

### ✅ EXP33-C - Implemented

<a id="rule-exp33c"></a>

**Title:** Do not read uninitialized memory

**Description:** Local, automatic variables assume unexpected values if they are read before they
are initialized. The C Standard, 6.7.11, paragraph 11, specifies [ISO/IEC
9899:2024] Seeundefined behavior 11. When local, automatic variables are stored
on the program stack, for example, their values default to whichever values are
currently stored in stack memory.

**Test Coverage:** 50 tests (35 fail, 15 pass)

**Test Results:** 0/50 passed (0.0%), 50 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_arithmetic_uninitialized.c` → `test_exp33_c_fail_testcases_arithmetic_uninitialized`
- ⏭️ NOT RUN `testcases_array_buffer_uninitialized.c` → `test_exp33_c_fail_testcases_array_buffer_uninitialized`
- ⏭️ NOT RUN `testcases_basic_uninitialized_local.c` → `test_exp33_c_fail_testcases_basic_uninitialized_local`
- ⏭️ NOT RUN `testcases_bitfield_uninitialized.c` → `test_exp33_c_fail_testcases_bitfield_uninitialized`
- ⏭️ NOT RUN `testcases_cast_uninitialized.c` → `test_exp33_c_fail_testcases_cast_uninitialized`
- ⏭️ NOT RUN `testcases_comparison_uninitialized.c` → `test_exp33_c_fail_testcases_comparison_uninitialized`
- ⏭️ NOT RUN `testcases_complex_control_flow.c` → `test_exp33_c_fail_testcases_complex_control_flow`
- ⏭️ NOT RUN `testcases_compound_literal_uninitialized.c` → `test_exp33_c_fail_testcases_compound_literal_uninitialized`
- ⏭️ NOT RUN `testcases_conditional_initialization.c` → `test_exp33_c_fail_testcases_conditional_initialization`
- ⏭️ NOT RUN `testcases_const_uninitialized.c` → `test_exp33_c_fail_testcases_const_uninitialized`
- ⏭️ NOT RUN `testcases_dynamic_memory_uninitialized.c` → `test_exp33_c_fail_testcases_dynamic_memory_uninitialized`
- ⏭️ NOT RUN `testcases_enum_uninitialized.c` → `test_exp33_c_fail_testcases_enum_uninitialized`
- ⏭️ NOT RUN `testcases_file_io_uninitialized.c` → `test_exp33_c_fail_testcases_file_io_uninitialized`
- ⏭️ NOT RUN `testcases_flexible_array_uninitialized.c` → `test_exp33_c_fail_testcases_flexible_array_uninitialized`
- ⏭️ NOT RUN `testcases_function_parameter_issues.c` → `test_exp33_c_fail_testcases_function_parameter_issues`
- ⏭️ NOT RUN `testcases_goto_uninitialized.c` → `test_exp33_c_fail_testcases_goto_uninitialized`
- ⏭️ NOT RUN `testcases_loop_uninitialized.c` → `test_exp33_c_fail_testcases_loop_uninitialized`
- ⏭️ NOT RUN `testcases_macro_uninitialized.c` → `test_exp33_c_fail_testcases_macro_uninitialized`
- ⏭️ NOT RUN `testcases_memory_copy_uninitialized.c` → `test_exp33_c_fail_testcases_memory_copy_uninitialized`
- ⏭️ NOT RUN `testcases_pointer_uninitialized.c` → `test_exp33_c_fail_testcases_pointer_uninitialized`
- ⏭️ NOT RUN `testcases_recursive_uninitialized.c` → `test_exp33_c_fail_testcases_recursive_uninitialized`
- ⏭️ NOT RUN `testcases_register_uninitialized.c` → `test_exp33_c_fail_testcases_register_uninitialized`
- ⏭️ NOT RUN `testcases_signal_handler_uninitialized.c` → `test_exp33_c_fail_testcases_signal_handler_uninitialized`
- ⏭️ NOT RUN `testcases_struct_member_uninitialized.c` → `test_exp33_c_fail_testcases_struct_member_uninitialized`
- ⏭️ NOT RUN `testcases_ternary_uninitialized.c` → `test_exp33_c_fail_testcases_ternary_uninitialized`
- ⏭️ NOT RUN `testcases_thread_local_uninitialized.c` → `test_exp33_c_fail_testcases_thread_local_uninitialized`
- ⏭️ NOT RUN `testcases_typedef_uninitialized.c` → `test_exp33_c_fail_testcases_typedef_uninitialized`
- ⏭️ NOT RUN `testcases_union_uninitialized.c` → `test_exp33_c_fail_testcases_union_uninitialized`
- ⏭️ NOT RUN `testcases_va_list_uninitialized.c` → `test_exp33_c_fail_testcases_va_list_uninitialized`
- ⏭️ NOT RUN `testcases_volatile_uninitialized.c` → `test_exp33_c_fail_testcases_volatile_uninitialized`
- ⏭️ NOT RUN `wiki_mbstate_t.c` → `test_exp33_c_fail_wiki_mbstate_t`
- ⏭️ NOT RUN `wiki_posix_entropy.c` → `test_exp33_c_fail_wiki_posix_entropy`
- ⏭️ NOT RUN `wiki_realloc.c` → `test_exp33_c_fail_wiki_realloc`
- ⏭️ NOT RUN `wiki_return_by_reference.c` → `test_exp33_c_fail_wiki_return_by_reference`
- ⏭️ NOT RUN `wiki_uninitialized_local.c` → `test_exp33_c_fail_wiki_uninitialized_local`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_comprehensive_initialization.c` → `test_exp33_c_pass_testcases_comprehensive_initialization`
- ⏭️ NOT RUN `testcases_safe_advanced_patterns.c` → `test_exp33_c_pass_testcases_safe_advanced_patterns`
- ⏭️ NOT RUN `testcases_safe_array_initialization.c` → `test_exp33_c_pass_testcases_safe_array_initialization`
- ⏭️ NOT RUN `testcases_safe_control_flow.c` → `test_exp33_c_pass_testcases_safe_control_flow`
- ⏭️ NOT RUN `testcases_safe_dynamic_memory.c` → `test_exp33_c_pass_testcases_safe_dynamic_memory`
- ⏭️ NOT RUN `testcases_safe_function_interfaces.c` → `test_exp33_c_pass_testcases_safe_function_interfaces`
- ⏭️ NOT RUN `testcases_safe_initialization_patterns.c` → `test_exp33_c_pass_testcases_safe_initialization_patterns`
- ⏭️ NOT RUN `testcases_safe_pointer_initialization.c` → `test_exp33_c_pass_testcases_safe_pointer_initialization`
- ⏭️ NOT RUN `testcases_safe_struct_initialization.c` → `test_exp33_c_pass_testcases_safe_struct_initialization`
- ⏭️ NOT RUN `testcases_safe_unsigned_char_exception.c` → `test_exp33_c_pass_testcases_safe_unsigned_char_exception`
- ⏭️ NOT RUN `wiki_mbstate_t.c` → `test_exp33_c_pass_wiki_mbstate_t`
- ⏭️ NOT RUN `wiki_posix_entropy.c` → `test_exp33_c_pass_wiki_posix_entropy`
- ⏭️ NOT RUN `wiki_realloc.c` → `test_exp33_c_pass_wiki_realloc`
- ⏭️ NOT RUN `wiki_return_by_reference.c` → `test_exp33_c_pass_wiki_return_by_reference`
- ⏭️ NOT RUN `wiki_uninitialized_local.c` → `test_exp33_c_pass_wiki_uninitialized_local`

---

### 🔶 EXP13-C - Not Implemented (has tests)

<a id="rule-exp13c"></a>

**Title:** Treat relational and equality operators as if they were nonassociative

**Description:** The relational and equality operators are left-associative in C. Consequently,
C, unlike many other languages, allows chaining of relational and equality
operators. Subclause 6.5.8, footnote 107, of the C Standard [ISO/IEC 9899:2011],
says: These operators areleft-associative, which means the leftmost comparison
is performed first, and the result is compared with the rightmost comparison.
This syntax allows a programmer to write an expression (particularly an
expression used as a condition) that can be easily misinterpreted. Although this
noncompliant code example compiles correctly, it is unlikely that it means what
the author of the code intended:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp13_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp13_c_pass_wiki_compliant_1`

---

### 🔶 EXP10-C - Not Implemented (has tests)

<a id="rule-exp10c"></a>

**Title:** Do not depend on the order of evaluation of subexpressions or the order in which side effects take place

**Description:** The order of evaluation of subexpressions and the order in whichside effectstake
place are frequently defined asunspecified behaviorby the C Standard.
Counterintuitively,unspecified behaviorin behavior for which the standard
provides two or more possibilities and imposes no further requirements on which
is chosen in any instance. Consequently, unspecified behavior can be a
portability issue because differentimplementationscan make different choices. If
dynamic scheduling is used, however, there may not be a fixed-code execution
sequence over the life of a process. Operations that can be executed in
different sequences may in fact be executed in a different order. According to
the C Standard, subclause 6.5 [ISO/IEC 9899:2011], Following are specific
examples of situations in which the order of evaluation of subexpressions or the
order in whichside effectstake place is unspecified:

**Test Coverage:** 3 tests (2 fail, 1 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp10_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_exp10_c_fail_wiki_noncompliant_2_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp10_c_pass_wiki_compliant_1`

---

### 🔶 EXP02-C - Not Implemented (has tests)

<a id="rule-exp02c"></a>

**Title:** Be aware of the short-circuit behavior of the logical AND and OR operators

**Description:** The logical AND and logical OR operators (&&and||, respectively) exhibit "short-
circuit" operation. That is, the second operand is not evaluated if the result
can be deduced solely by evaluating the first operand. Programmers should
exercise caution if the second operand containsside effectsbecause it may not be
apparent whether the side effects actually occur. In the following code, the
value ofiis incremented only wheni >= 0:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp02_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp02_c_pass_wiki_compliant_1`

---

### 🔶 EXP14-C - Not Implemented (has tests)

<a id="rule-exp14c"></a>

**Title:** Beware of integer promotion when performing bitwise operations on integer types smaller than int

**Description:** DeprecatedThis guideline has been deprecated byINT02-C. Understand integer
conversion rules Integer types smaller thanintare promoted when an operation is
performed on them. If all values of the original type can be represented as
anint, the value of the smaller type is converted to anint; otherwise, it is
converted to anunsigned int(seeINT02-C. Understand integer conversion rules). If
the conversion is to a wider type, the original value is zero-extended for
unsigned values or sign-extended for signed types. Consequently, bitwise
operations on integer types smaller thanintmay have unexpected results. This
noncompliant code example demonstrates how performing bitwise operations on
integer types smaller thanintmay have unexpected results.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp14_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp14_c_pass_wiki_compliant_1`

---

### 🔶 EXP35-C - Not Implemented (has tests)

<a id="rule-exp35c"></a>

**Title:** Do not modify objects with temporary lifetime

**Description:** The C11 Standard [ISO/IEC 9899:2011] introduced a new term:temporary lifetime.
This term still remains in the C23 Standard. Modifying an object with temporary
lifetime isundefined behavior. According to subclause 6.2.4, paragraph 8
[ISO/IEC 9899:2024] This definition differs from the C99 Standard (which defines
modifying the result of a function call or accessing it after the next sequence
point as undefined behavior) because a temporary object's lifetime ends when the
evaluation containing the full expression or full declarator ends, so the result
of a function call can be accessed. This extension to the lifetime of a
temporary also removes a quiet change to C90 and improves compatibility with
C++. C functions may not return arrays; however, functions can return a pointer
to an array or astructorunionthat contains arrays.Consequently, in any version
of C, if a function call returns by value astructorunioncontaining an array, do
not modify those arrays within the expression containing the function call.In
C99 and older, do not access an array returned by a function after the next
sequence point or after the evaluation of the containing full expression or full
declarator ends.

**Test Coverage:** 7 tests (3 fail, 4 pass)

**Test Results:** 0/7 passed (0.0%), 7 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp35_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_exp35_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_exp35_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_c11_and_newer.c` → `test_exp35_c_pass_wiki_c11_and_newer`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_exp35_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_exp35_c_pass_wiki_compliant_3`
- ⏭️ NOT RUN `wiki_compliant_4.c` → `test_exp35_c_pass_wiki_compliant_4`

---

### 🔶 EXP16-C - Not Implemented (has tests)

<a id="rule-exp16c"></a>

**Title:** Do not compare function pointers to constant values

**Description:** Comparing a function pointer to a value that is not a null function pointer of
the same type will be diagnosed because it typically indicates programmer error
and can result inunexpected behavior. Implicit comparisons will be diagnosed, as
well. In this noncompliant code example, the addresses of the POSIX
functionsgetuidandgeteuidare compared for equality to 0. Because no function
address shall be null, the first subexpression will always evaluate to false
(0), and the second subexpression always to true (nonzero). Consequently, the
entire expression will always evaluate to true, leading to a potential security
vulnerability. /* First the options that are allowed only for root */ if (getuid
== 0 || geteuid != 0) { /* ... */ }

**Test Coverage:** 6 tests (3 fail, 3 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp16_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_exp16_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_exp16_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp16_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_exp16_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_exp16_c_pass_wiki_compliant_3`

---

### ✅ EXP34-C - Implemented

<a id="rule-exp34c"></a>

**Title:** Do not dereference null pointers

**Description:** Dereferencing a null pointer isundefined behavior. On many platforms,
dereferencing a null pointer results inabnormal program termination, but this is
not required by the standard. See "Clever Attack Exploits Fully-Patched Linux
Kernel" [Goodin 2009] for an example of a code executionexploitthat resulted
from a null pointer dereference. This noncompliant code example is derived from
a real-world example taken from a vulnerable version of thelibpnglibrary as
deployed on a popular ARM-based cell phone [Jack 2007]. Thelibpnglibrary allows
applications to read, create, and manipulate PNG (Portable Network Graphics)
raster image files. Thelibpnglibrary implements its own wrapper tomalloc()that
returns a null pointer on error or on being passed a 0-byte-length argument.

**Test Coverage:** 46 tests (33 fail, 13 pass)

**Test Results:** 0/46 passed (0.0%), 46 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_after_free.c` → `test_exp34_c_fail_testcases_after_free`
- ⏭️ NOT RUN `testcases_arithmetic.c` → `test_exp34_c_fail_testcases_arithmetic`
- ⏭️ NOT RUN `testcases_array_null.c` → `test_exp34_c_fail_testcases_array_null`
- ⏭️ NOT RUN `testcases_assign_null.c` → `test_exp34_c_fail_testcases_assign_null`
- ⏭️ NOT RUN `testcases_callback_null.c` → `test_exp34_c_fail_testcases_callback_null`
- ⏭️ NOT RUN `testcases_calloc_fail.c` → `test_exp34_c_fail_testcases_calloc_fail`
- ⏭️ NOT RUN `testcases_conditional.c` → `test_exp34_c_fail_testcases_conditional`
- ⏭️ NOT RUN `testcases_direct_null.c` → `test_exp34_c_fail_testcases_direct_null`
- ⏭️ NOT RUN `testcases_double_free.c` → `test_exp34_c_fail_testcases_double_free`
- ⏭️ NOT RUN `testcases_fgets_null.c` → `test_exp34_c_fail_testcases_fgets_null`
- ⏭️ NOT RUN `testcases_file_null.c` → `test_exp34_c_fail_testcases_file_null`
- ⏭️ NOT RUN `testcases_func_param.c` → `test_exp34_c_fail_testcases_func_param`
- ⏭️ NOT RUN `testcases_getenv_null.c` → `test_exp34_c_fail_testcases_getenv_null`
- ⏭️ NOT RUN `testcases_list_null.c` → `test_exp34_c_fail_testcases_list_null`
- ⏭️ NOT RUN `testcases_loop_null.c` → `test_exp34_c_fail_testcases_loop_null`
- ⏭️ NOT RUN `testcases_malloc_fail.c` → `test_exp34_c_fail_testcases_malloc_fail`
- ⏭️ NOT RUN `testcases_memcpy_null.c` → `test_exp34_c_fail_testcases_memcpy_null`
- ⏭️ NOT RUN `testcases_multi_level.c` → `test_exp34_c_fail_testcases_multi_level`
- ⏭️ NOT RUN `testcases_nested_ptr.c` → `test_exp34_c_fail_testcases_nested_ptr`
- ⏭️ NOT RUN `testcases_printf_null.c` → `test_exp34_c_fail_testcases_printf_null`
- ⏭️ NOT RUN `testcases_realloc_fail.c` → `test_exp34_c_fail_testcases_realloc_fail`
- ⏭️ NOT RUN `testcases_return_null.c` → `test_exp34_c_fail_testcases_return_null`
- ⏭️ NOT RUN `testcases_strchr_null.c` → `test_exp34_c_fail_testcases_strchr_null`
- ⏭️ NOT RUN `testcases_strdup_fail.c` → `test_exp34_c_fail_testcases_strdup_fail`
- ⏭️ NOT RUN `testcases_string_null.c` → `test_exp34_c_fail_testcases_string_null`
- ⏭️ NOT RUN `testcases_strtok_null.c` → `test_exp34_c_fail_testcases_strtok_null`
- ⏭️ NOT RUN `testcases_struct_null.c` → `test_exp34_c_fail_testcases_struct_null`
- ⏭️ NOT RUN `testcases_switch_null.c` → `test_exp34_c_fail_testcases_switch_null`
- ⏭️ NOT RUN `testcases_uninitialized.c` → `test_exp34_c_fail_testcases_uninitialized`
- ⏭️ NOT RUN `testcases_void_cast.c` → `test_exp34_c_fail_testcases_void_cast`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_exp34_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_exp34_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_exp34_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_array_check.c` → `test_exp34_c_pass_testcases_array_check`
- ⏭️ NOT RUN `testcases_callback.c` → `test_exp34_c_pass_testcases_callback`
- ⏭️ NOT RUN `testcases_conditional.c` → `test_exp34_c_pass_testcases_conditional`
- ⏭️ NOT RUN `testcases_file_handle.c` → `test_exp34_c_pass_testcases_file_handle`
- ⏭️ NOT RUN `testcases_guard_func.c` → `test_exp34_c_pass_testcases_guard_func`
- ⏭️ NOT RUN `testcases_linked_list.c` → `test_exp34_c_pass_testcases_linked_list`
- ⏭️ NOT RUN `testcases_null_check.c` → `test_exp34_c_pass_testcases_null_check`
- ⏭️ NOT RUN `testcases_safe_alloc.c` → `test_exp34_c_pass_testcases_safe_alloc`
- ⏭️ NOT RUN `testcases_string_safe.c` → `test_exp34_c_pass_testcases_string_safe`
- ⏭️ NOT RUN `testcases_struct_ptr.c` → `test_exp34_c_pass_testcases_struct_ptr`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_exp34_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_exp34_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_exp34_c_pass_wiki_compliant_3`

---

## Category: FIO

<a id="category-fio"></a>

**Implementation Status:** 3 / 35 rules (8.6%)

### 🔶 FIO39-C - Not Implemented (has tests)

<a id="rule-fio39c"></a>

**Title:** Do not alternately input and output from a stream without an intervening flush or positioning call

**Description:** The C Standard, 7.23.5.3, paragraph 7 [ISO/IEC 9899:2024], places the following
restrictions on update streams: The following scenarios can result inundefined
behavior. (Seeundefined behavior 156.) Consequently, a call tofseek(),fflush(),
orfsetpos()is necessary between input and output to the same stream. SeeERR07-C.
Prefer functions that support error checking over equivalent functions that
don'tfor more information on whyfseek()is preferred overrewind().

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio39_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio39_c_pass_wiki_compliant_1`

---

### 🔶 FIO50-C - Not Implemented (has tests)

<a id="rule-fio50c"></a>

**Title:** PP. Do not alternately input and output from a file stream without an intervening positioning call

**Description:** The C++ Standard, [filebuf], paragraph 2 [ISO/IEC 14882-2014], states the
following: The C Standard, subclause 7.19.5.3, paragraph 6 [ISO/IEC 9899:1999],
places the following restrictions onFILEobjects opened for both reading and
writing: Consequently, the following scenarios can result inundefined behavior:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio50_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio50_c_pass_wiki_compliant_1`

---

### 🔶 FIO40-C - Not Implemented (has tests)

<a id="rule-fio40c"></a>

**Title:** Reset strings on fgets() or fgetws() failure

**Description:** If either of the C Standardfgets()orfgetws()functions fail, the contents of the
array being written isindeterminate. (Seeundefined behavior 175.) It is
necessary to reset the string to a known value to avoid errors on subsequent
string manipulation functions. In this noncompliant code example, an error flag
is set iffgets()fails. However,bufis not reset and has indeterminate contents:
#include <stdio.h> enum { BUFFER_SIZE = 1024 }; void func(FILE *file) { char
buf[BUFFER_SIZE]; if (fgets(buf, sizeof(buf), file) == NULL) { /* Set error flag
and continue */ } }

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio40_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio40_c_pass_wiki_compliant_1`

---

### 🔶 FIO03-C - Not Implemented (has tests)

<a id="rule-fio03c"></a>

**Title:** Do not make assumptions about fopen() and file creation

**Description:** The Cfopen()function is used to open an existing file or create a new one. The
C11 version of thefopen()function provides a mode flag,x, that provides the
mechanism needed to determine if the file that is to be opened exists. Not using
this mode flag can lead to a program overwriting or accessing an unintended
file. In this noncompliant code example, the file referenced byfile_nameis
opened for writing. This example is noncompliant if the programmer's intent was
to create a new file, but the referenced file already exists. char *file_name;
FILE *fp; /* Initialize file_name */ fp = fopen(file_name, "w"); if (!fp) { /*
Handle error */ }

**Test Coverage:** 5 tests (1 fail, 4 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_fopen.c` → `test_fio03_c_fail_wiki_fopen`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_fdopen_posix.c` → `test_fio03_c_pass_wiki_fdopen_posix`
- ⏭️ NOT RUN `wiki_fopenx_c11.c` → `test_fio03_c_pass_wiki_fopenx_c11`
- ⏭️ NOT RUN `wiki_open_posix.c` → `test_fio03_c_pass_wiki_open_posix`
- ⏭️ NOT RUN `wiki_windows.c` → `test_fio03_c_pass_wiki_windows`

---

### 🔶 FIO42-C - Not Implemented (has tests)

<a id="rule-fio42c"></a>

**Title:** Close files when they are no longer needed

**Description:** A call to thefopen()orfreopen()function must be matched with a call
tofclose()before the lifetime of the last pointer that stores the return value
of the call has ended or before normal program termination, whichever occurs
first. In general, this rule should also be applied to other functions with open
and close resources, such as the POSIXopen()andclose()functions, or the
Microsoft WindowsCreateFile()andCloseHandle()functions. This code example is
noncompliant because the file opened by the call tofopen()is not closed before
functionfunc()returns:

**Test Coverage:** 8 tests (4 fail, 4 pass)

**Test Results:** 0/8 passed (0.0%), 8 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_exit.c` → `test_fio42_c_fail_wiki_exit`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio42_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_posix.c` → `test_fio42_c_fail_wiki_posix`
- ⏭️ NOT RUN `wiki_windows.c` → `test_fio42_c_fail_wiki_windows`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio42_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_exit.c` → `test_fio42_c_pass_wiki_exit`
- ⏭️ NOT RUN `wiki_posix.c` → `test_fio42_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_windows.c` → `test_fio42_c_pass_wiki_windows`

---

### ⚫ FIO11-C - Not Implemented (no tests)

<a id="rule-fio11c"></a>

**Title:** Take care when specifying the mode parameter of fopen()

**Description:** The C Standard identifies specific strings to use for themodeon calls
tofopen()andfopen_s(). C11 provides a new mode flag,x, that provides the
mechanism needed to determine if the file that is to be opened exists. To be
strictlyconformingand portable, one of the strings from the following table
(adapted from the C Standard, subclause 7.21.5.2 [ISO/IEC 9899:2011]) must be
used: Strings to Use for the Mode on Calls tofopen()andfopen_s()
modeStringResultrOpen text file for readingwTruncate to zero length or create
text file for writingwxCreate text file for writingaAppend; open or create text
file for writing at end-of-filerbOpen binary file for readingwbTruncate to zero
length or create binary file for writingwbxCreate binary file for
writingabAppend; open or create binary file for writing at end-of-filer+Open
text file for update (reading and writing)w+Truncate to zero length or create
text file for updatew+xCreate text file for updatea+Append; open or create text
file for update, writing at end-of-filer+borrb+Open binary file for update
(reading and writing)w+borwb+Truncate to zero length or create binary file for
updatew+bxorwb+xCreate binary file for updatea+borab+Append; open or create
binary file for update, writing at end-of-file

**Test Coverage:** 0 tests (0 fail, 0 pass)

---

### 🔶 FIO23-C - Not Implemented (has tests)

<a id="rule-fio23c"></a>

**Title:** Do not exit with unflushed data in stdout or stderr

**Description:** DeprecatedThis guideline does not apply to code that need conform only to C23.
Code that must conform to older versions of the C standard should still comply
with this guideline. The C standard makes no guarantees as to when output
tostdout(standard output) orstderr(standard error) is actually flushed. On many
platforms, output tostdoutis buffered unlessstdoutoutputs to a terminal,
andstderroutput is typically not buffered. However, programs are free to modify
the buffering rules for eitherstdoutorstderr. Programs are also free to
explicitly closestdoutorstderr; if they do not do so, these streams will be
closed upon program termination. Closing any output stream requires flushing any
data that has not yet been written to the stream. The flushing operation
(manually handled by thefflush()function) can fail for several reasons. The
output stream may be directed to a file in a filesystem with no remaining free
space, or to a network socket that fails. Checking for the success of
afflush()operation is mandatory for a secure program, and hence checking the
result of afclose()operation is also required.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_atexit.c` → `test_fio23_c_fail_wiki_atexit`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio23_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_atexit.c` → `test_fio23_c_pass_wiki_atexit`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio23_c_pass_wiki_compliant_1`

---

### 🔶 FIO46-C - Not Implemented (has tests)

<a id="rule-fio46c"></a>

**Title:** Do not access a closed file

**Description:** Using the value of a pointer to aFILEobject after the associated file is closed
isundefined behavior. (Seeundefined behavior 153.) Programs that close the
standard streams (especiallystdoutbut alsostderrandstdin) must be careful not to
use these streams in subsequent function calls, particularly those that
implicitly operate on them (such asprintf(),perror(), andgetc()). This rule can
be generalized to other file representations. In this noncompliant code example,
thestdoutstream is used after it is closed:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio46_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio46_c_pass_wiki_compliant_1`

---

### 🔶 FIO41-C - Not Implemented (has tests)

<a id="rule-fio41c"></a>

**Title:** Do not call getc(), putc(), getwc(), or putwc() with a stream argument that has side effects

**Description:** Do not invokegetc()orputc()or their wide-character
analoguesgetwc()andputwc()with a stream argument that has side effects. The
stream argument passed to these macros may be evaluated more than once if these
functions are implemented as unsafe macros. (SeePRE31-C. Avoid side effects in
arguments to unsafe macrosfor more information.) This rule does not apply to the
character argument inputc()or the wide-character argument inputwc(), which is
guaranteed to be evaluated exactly once. This noncompliant code example calls
thegetc()function with an expression as the stream argument. Ifgetc()is
implemented as a macro, the file may be opened multiple times. (SeeFIO24-C. Do
not open a file that is already open.)

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_getc.c` → `test_fio41_c_fail_wiki_getc`
- ⏭️ NOT RUN `wiki_putc.c` → `test_fio41_c_fail_wiki_putc`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_getc.c` → `test_fio41_c_pass_wiki_getc`
- ⏭️ NOT RUN `wiki_putc.c` → `test_fio41_c_pass_wiki_putc`

---

### 🔶 FIO13-C - Not Implemented (has tests)

<a id="rule-fio13c"></a>

**Title:** Never push back anything other than one read character

**Description:** Subclause 7.21.7.10 of the C Standard [ISO/IEC 9899:2011] definesungetc()as
follows: Consequently, multiple calls toungetc()on the same stream must be
separated by a call to a read function or a file-positioning function (which
will discard any data pushed byungetc()). Likewise, forungetwc(), C guarantees
only one wide character of pushback (subclause 7.29.3.10). Consequently,
multiple calls toungetwc()on the same stream must be separated by a call to a
read function or a file-positioning function (which will discard any data pushed
byungetwc()).

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio13_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio13_c_pass_wiki_compliant_1`

---

### 🔶 FIO06-C - Not Implemented (has tests)

<a id="rule-fio06c"></a>

**Title:** Create files with appropriate access permissions

**Description:** Creating a file with insufficiently restrictive access permissions may allow an
unprivileged user to access that file. Although access permissions are heavily
dependent on the file system, many file-creation functions provide mechanisms to
set (or at least influence) access permissions. When these functions are used to
create files, appropriate access permissions should be specified to prevent
unintended access. When setting access permissions, it is important to make sure
that an attacker cannot alter them. (SeeFIO15-C. Ensure that file operations are
performed in a secure directory.) Thefopen()function does not allow the
programmer to explicitly specify file access permissions. In this noncompliant
code example, if the call tofopen()creates a new file, the access permissions
areimplementation-defined:

**Test Coverage:** 3 tests (2 fail, 1 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_fopen.c` → `test_fio06_c_fail_wiki_fopen`
- ⏭️ NOT RUN `wiki_open_posix.c` → `test_fio06_c_fail_wiki_open_posix`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_open_posix.c` → `test_fio06_c_pass_wiki_open_posix`

---

### 🔶 FIO01-C - Not Implemented (has tests)

<a id="rule-fio01c"></a>

**Title:** Be careful using functions that use file names for identification

**Description:** Many file-related security vulnerabilities result from a program accessing an
unintended file object because file names are only loosely bound to underlying
file objects. File names provide no information regarding the nature of the file
object itself. Furthermore, the binding of a file name to a file object is
reasserted every time the file name is used in an operation. File descriptors
andFILEpointers are bound to underlying file objects by the operating system.
(SeeFIO03-C. Do not make assumptions about fopen() and file creation.) Accessing
files via file descriptors orFILEpointers rather than file names provides a
greater degree of certainty as to which object is actually acted upon. It is
recommended that files be accessed through file descriptors orFILEpointers where
possible. The following C functions rely solely on file names for file
identification:

**Test Coverage:** 3 tests (2 fail, 1 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio01_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_posix.c` → `test_fio01_c_fail_wiki_posix`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_fio01_c_pass_wiki_posix`

---

### 🔶 FIO32-C - Not Implemented (has tests)

<a id="rule-fio32c"></a>

**Title:** Do not perform operations on devices that are only appropriate for files

**Description:** File names on many operating systems, including Windows and UNIX, may be used to
accessspecial files, which are actually devices. Reserved Microsoft Windows
device names includeAUX,CON,PRN,COM1, andLPT1or paths using the\\.\device
namespace. Device files on UNIX systems are used to apply access rights and to
direct operations on the files to the appropriate device drivers. Performing
operations on device files that are intended for ordinary character or binary
files can result in crashes anddenial-of-service attacks. For example, when
Windows attempts to interpret the device name as a file resource, it performs an
invalid resource access that usually results in a crash [Howard 2002]. Device
files in UNIX can be a security risk when an attacker can access them in an
unauthorized way. For example, if attackers can read or write to
the/dev/kmemdevice, they may be able to alter the priority, UID, or other
attributes of their process or simply crash the system. Similarly, access to
disk devices, tape devices, network devices, and terminals being used by other
processes can lead to problems [Garfinkel 1996].

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio32_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_windows.c` → `test_fio32_c_fail_wiki_windows`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_fio32_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_windows.c` → `test_fio32_c_pass_wiki_windows`

---

### 🔶 FIO08-C - Not Implemented (has tests)

<a id="rule-fio08c"></a>

**Title:** Take care when calling remove() on an open file

**Description:** Invokingremove()on an open file isimplementation-defined. Removing an open file
is sometimes recommended to hide the names of temporary files that may be prone
to attack. (SeeFIO21-C. Do not create temporary files in shared directories.) In
cases requiring the removal of an open file, a more strongly defined function,
such as the POSIXunlink()function, should be considered. To be strictly
conforming and portable,remove()shouldnotbe called on an open file. This
noncompliant code example shows a case where a file is removed while it is still
open:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio08_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_fio08_c_pass_wiki_posix`

---

### 🔶 FIO38-C - Not Implemented (has tests)

<a id="rule-fio38c"></a>

**Title:** Do not copy a FILE object

**Description:** According to the C Standard, 7.23.3, paragraph 6 [ISO/IEC 9899:2024],
Consequently, do not copy aFILEobject. This noncompliant code example can fail
because a by-value copy ofstdoutis being used in the call tofputs():

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio38_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio38_c_pass_wiki_compliant_1`

---

### 🔶 FIO44-C - Not Implemented (has tests)

<a id="rule-fio44c"></a>

**Title:** Only use values for fsetpos() that are returned from fgetpos()

**Description:** The C Standard, 7.23.9.3 paragraph 2 [ISO/IEC 9899:2024], defines the following
behavior forfsetpos(): Invoking thefsetpos()function with any other values
forposisundefined behavior 181. This noncompliant code example attempts to read
three values from a file and then set the file position pointer back to the
beginning of the file:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio44_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio44_c_pass_wiki_compliant_1`

---

### 🔶 FIO21-C - Not Implemented (has tests)

<a id="rule-fio21c"></a>

**Title:** Do not create temporary files in shared directories

**Description:** Programmers frequently create temporary files in directories that are writable
by everyone (examples are/tmpand/var/tmpon UNIX and%TEMP%on Windows) and may be
purged regularly (for example, every night or during reboot). Temporary files
are commonly used for auxiliary storage for data that does not need to, or
otherwise cannot, reside in memory and also as a means of communicating with
other processes by transferring data through the file system. For example, one
process will create a temporary file in a shared directory with a well-known
name or a temporary name that is communicated to collaborating processes. The
file then can be used to share information among these collaborating processes.
This practice is dangerous because a well-known file in a shared directory can
be easily hijacked or manipulated by an attacker.Mitigationstrategies include
the following:

**Test Coverage:** 6 tests (5 fail, 1 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_fopenopenwithtmpnam.c` → `test_fio21_c_fail_wiki_fopenopenwithtmpnam`
- ⏭️ NOT RUN `wiki_fopenopenwithtmpnam_2.c` → `test_fio21_c_fail_wiki_fopenopenwithtmpnam_2`
- ⏭️ NOT RUN `wiki_fopenopenwithtmpnam_3.c` → `test_fio21_c_fail_wiki_fopenopenwithtmpnam_3`
- ⏭️ NOT RUN `wiki_mktempopen_posix.c` → `test_fio21_c_fail_wiki_mktempopen_posix`
- ⏭️ NOT RUN `wiki_tmpfile.c` → `test_fio21_c_fail_wiki_tmpfile`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_mkstemp_posix.c` → `test_fio21_c_pass_wiki_mkstemp_posix`

---

### ✅ FIO37-C - Implemented

<a id="rule-fio37c"></a>

**Title:** Do not assume that fgets() or fgetws() returns a nonempty string when successful

**Description:** Errors can occur when incorrect assumptions are made about the type of data
being read. These assumptions may be violated, for example, when binary data has
been read from a file instead of text from a user's terminal or the output of a
process is piped tostdin.(SeeFIO14-C. Understand the difference between text
mode and binary mode with file streams.) On some systems, it may also be
possible to input a null byte (as well as other binary codes) from the keyboard.
Subclause 7.23.7.2 of the C Standard paragraph 3 [ISO/IEC 9899:2024] says, The
wide-character functionfgetws()has the same behavior. Therefore,
iffgets()orfgetws()returns a non-null pointer, it is safe to assume that the
array contains data. However, it is erroneous to assume that the array contains
a nonempty string because the data may contain null characters.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio37_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio37_c_pass_wiki_compliant_1`

---

### 🔶 FIO20-C - Not Implemented (has tests)

<a id="rule-fio20c"></a>

**Title:** Avoid unintentional truncation when using fgets() or fgetws()

**Description:** Thefgets()andfgetws()functions are typically used to read a newline-terminated
line of input from a stream. Both functions read at most one less than the
number of narrow or wide characters specified by an argumentnfrom a stream to a
string. Truncation errors can occur ifn - 1is less than the number of characters
appearing in the input string prior to the new-line narrow or wide character
(which is retained) or after end-of-file. This can result in the accidental
truncation of user input. This noncompliant code example copies the input string
into a buffer, and assumes it captured all of the user's input. #include
<stdbool.h> #include <stdio.h> bool get_data(char *buffer, int size) { if
(fgets(buffer, size, stdin)) { return true; } return false; } void func(void) {
char buf[8]; if (get_data(buf, sizeof(buf))) { printf("The user input %s\n",
buf); } else { printf("Error getting data from the user\n"); } }

**Test Coverage:** 4 tests (1 fail, 3 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio20_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_expanding_buffer.c` → `test_fio20_c_pass_wiki_expanding_buffer`
- ⏭️ NOT RUN `wiki_fail_on_truncation.c` → `test_fio20_c_pass_wiki_fail_on_truncation`
- ⏭️ NOT RUN `wiki_posixgetline.c` → `test_fio20_c_pass_wiki_posixgetline`

---

### 🔶 FIO02-C - Not Implemented (has tests)

<a id="rule-fio02c"></a>

**Title:** Canonicalize path names originating from tainted sources

**Description:** Path names, directory names, and file names may contain characters that
makevalidationdifficult and inaccurate. Furthermore, any path name component can
be a symbolic link, which further obscures the actual location or identity of a
file. To simplify file name validation, it is recommended that names be
translated into theircanonicalform. Canonicalizing file names makes it much
easier to verify a path, directory, or file name by making it easier to compare
names. Because the canonical form can vary between operating systems and file
systems, it is best to use operating-system-specific mechanisms for
canonicalization. As an illustration, here is a function that ensures that a
path name refers to a file in the user's home directory on POSIX systems:

**Test Coverage:** 7 tests (3 fail, 4 pass)

**Test Results:** 0/7 passed (0.0%), 7 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio02_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_posix.c` → `test_fio02_c_fail_wiki_posix`
- ⏭️ NOT RUN `wiki_windows.c` → `test_fio02_c_fail_wiki_windows`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_glibc.c` → `test_fio02_c_pass_wiki_glibc`
- ⏭️ NOT RUN `wiki_posix.c` → `test_fio02_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_posix_2.c` → `test_fio02_c_pass_wiki_posix_2`
- ⏭️ NOT RUN `wiki_posix_3.c` → `test_fio02_c_pass_wiki_posix_3`

---

### 🔶 FIO10-C - Not Implemented (has tests)

<a id="rule-fio10c"></a>

**Title:** Take care when using the rename() function

**Description:** Therename()function has the following prototype: int rename(const char
*src_file, const char *dest_file); If the file referenced bydest_fileexists
prior to callingrename(), the behavior isimplementation-defined. On POSIX
systems, the destination file is removed. On Windows systems, therename()fails.
Consequently, issues arise when trying to write portable code or when trying to
implement alternative behavior.

**Test Coverage:** 7 tests (2 fail, 5 pass)

**Test Results:** 0/7 passed (0.0%), 7 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_fio10_c_fail_wiki_posix`
- ⏭️ NOT RUN `wiki_windows.c` → `test_fio10_c_fail_wiki_windows`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_fio10_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_preserve_existing_destination_file.c` → `test_fio10_c_pass_wiki_preserve_existing_destination_file`
- ⏭️ NOT RUN `wiki_remove_existing_destination_file.c` → `test_fio10_c_pass_wiki_remove_existing_destination_file`
- ⏭️ NOT RUN `wiki_windows.c` → `test_fio10_c_pass_wiki_windows`
- ⏭️ NOT RUN `wiki_windows_2.c` → `test_fio10_c_pass_wiki_windows_2`

---

### 🔶 FIO18-C - Not Implemented (has tests)

<a id="rule-fio18c"></a>

**Title:** Never expect fwrite() to terminate the writing process at a null character

**Description:** The C Standard, subclause 7.21.8.2 [ISO/IEC 9899:2011], defines
thefwrite()function as follows: The definition does not state that
thefwrite()function will stop copying characters into the file if a null
character is encountered. Therefore, when writing a null-terminated byte string
to a file using thefwrite()function, always use the length of the string plus 1
(to account for the null character) as thenmembparameter. In this noncompliant
code example, the size of the buffer is stored insize1, butsize2number of
characters are written to the file. Ifsize2is greater thansize1,write()will not
stop copying characters at the null character.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio18_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio18_c_pass_wiki_compliant_1`

---

### 🔶 FIO45-C - Not Implemented (has tests)

<a id="rule-fio45c"></a>

**Title:** Avoid TOCTOU race conditions while accessing files

**Description:** ATOCTOU(time-of-check, time-of-use)race condition is possible when two or more
concurrent processes are operating on a shared file system [Seacord 2013b].
Typically, the first access is a check to verify some attribute of the file,
followed by a call to use the file. An attacker can alter the file between the
two accesses, or replace the file with a symbolic or hard link to a different
file. These TOCTOU conditions can be exploited when a program performs two or
more file operations on the same file name or path name. A program that performs
two or more file operations on a single file name or path name creates a race
window between the two file operations. This race window comes from the
assumption that the file name or path name refers to the same resource both
times. If an attacker can modify the file, remove it, or replace it with a
different file, then this assumption will not hold. If an existing file is
opened for writing with thewmode argument, the file's previous contents (if any)
are destroyed. This noncompliant code example tries to prevent an existing file
from being overwritten by first opening it for reading before opening it for
writing. An attacker can exploit the race window between the two calls
tofopen()to overwrite an existing file.

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio45_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio45_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_posix.c` → `test_fio45_c_pass_wiki_posix`

---

### ⚫ FIO14-C - Not Implemented (no tests)

<a id="rule-fio14c"></a>

**Title:** Understand the difference between text mode and binary mode with file streams

**Description:** Input and output are mapped into logical data streams whose properties are more
uniform than their various inputs and outputs. Two forms of mapping are
supported, one for text streams and one for binary streams. They differ in the
actual representation of data as well as in the functionality of some C
functions. Characters may have to be altered to conform to differing conventions
for representing text in thehost environment. As a consequence, data read to or
written from a text stream will not necessarily compare equal to the stream's
byte content. The following code opens the filemyfileas a text stream:

**Test Coverage:** 0 tests (0 fail, 0 pass)

---

### ✅ FIO30-C - Implemented

<a id="rule-fio30c"></a>

**Title:** Exclude user input from format strings

**Description:** Never call a formatted I/O function with a format string containing atainted
value. An attacker who can fully or partially control the contents of a format
string can crash a vulnerable process, view the contents of the stack, view
memory content, or write to an arbitrary memory location. Consequently, the
attacker can execute arbitrary code with the permissions of the vulnerable
process [Seacord 2013b]. Formatted output functions are particularly dangerous
because many programmers are unaware of their capabilities. For example,
formatted output functions can be used to write an integer value to a specified
address using the%nconversion specifier. Theincorrect_password()function in this
noncompliant code example is called during identification and authentication to
display an error message if the specified user is not found or the password is
incorrect. The function accepts the name of the user as a string referenced
byuser. This is an exemplar ofuntrusted datathat originates from an
unauthenticated user. The function constructs an error message that is then
output tostderrusing the C Standardfprintf()function. #include <stdio.h>
#include <stdlib.h> #include <string.h> void incorrect_password(const char
*user) { int ret; /* User names are restricted to 256 or fewer characters */
static const char msg_format[] = "%s cannot be authenticated.\n"; size_t len =
strlen(user) + sizeof(msg_format); char *msg = (char *)malloc(len); if (msg ==
NULL) { /* Handle error */ } ret = snprintf(msg, len, msg_format, user); if (ret
< 0) { /* Handle error */ } else if (ret >= len) { /* Handle truncated output */
} fprintf(stderr, msg); free(msg); }

**Test Coverage:** 45 tests (32 fail, 13 pass)

**Test Results:** 0/45 passed (0.0%), 45 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_vuln_array.c` → `test_fio30_c_fail_testcases_vuln_array`
- ⏭️ NOT RUN `testcases_vuln_buffer.c` → `test_fio30_c_fail_testcases_vuln_buffer`
- ⏭️ NOT RUN `testcases_vuln_concat.c` → `test_fio30_c_fail_testcases_vuln_concat`
- ⏭️ NOT RUN `testcases_vuln_config.c` → `test_fio30_c_fail_testcases_vuln_config`
- ⏭️ NOT RUN `testcases_vuln_dprintf.c` → `test_fio30_c_fail_testcases_vuln_dprintf`
- ⏭️ NOT RUN `testcases_vuln_dynamic.c` → `test_fio30_c_fail_testcases_vuln_dynamic`
- ⏭️ NOT RUN `testcases_vuln_env.c` → `test_fio30_c_fail_testcases_vuln_env`
- ⏭️ NOT RUN `testcases_vuln_error.c` → `test_fio30_c_fail_testcases_vuln_error`
- ⏭️ NOT RUN `testcases_vuln_file.c` → `test_fio30_c_fail_testcases_vuln_file`
- ⏭️ NOT RUN `testcases_vuln_fprintf.c` → `test_fio30_c_fail_testcases_vuln_fprintf`
- ⏭️ NOT RUN `testcases_vuln_fscanf.c` → `test_fio30_c_fail_testcases_vuln_fscanf`
- ⏭️ NOT RUN `testcases_vuln_function.c` → `test_fio30_c_fail_testcases_vuln_function`
- ⏭️ NOT RUN `testcases_vuln_global.c` → `test_fio30_c_fail_testcases_vuln_global`
- ⏭️ NOT RUN `testcases_vuln_indirect.c` → `test_fio30_c_fail_testcases_vuln_indirect`
- ⏭️ NOT RUN `testcases_vuln_malloc.c` → `test_fio30_c_fail_testcases_vuln_malloc`
- ⏭️ NOT RUN `testcases_vuln_network.c` → `test_fio30_c_fail_testcases_vuln_network`
- ⏭️ NOT RUN `testcases_vuln_pointer.c` → `test_fio30_c_fail_testcases_vuln_pointer`
- ⏭️ NOT RUN `testcases_vuln_printf1.c` → `test_fio30_c_fail_testcases_vuln_printf1`
- ⏭️ NOT RUN `testcases_vuln_printf2.c` → `test_fio30_c_fail_testcases_vuln_printf2`
- ⏭️ NOT RUN `testcases_vuln_scanf.c` → `test_fio30_c_fail_testcases_vuln_scanf`
- ⏭️ NOT RUN `testcases_vuln_snprintf.c` → `test_fio30_c_fail_testcases_vuln_snprintf`
- ⏭️ NOT RUN `testcases_vuln_sprintf.c` → `test_fio30_c_fail_testcases_vuln_sprintf`
- ⏭️ NOT RUN `testcases_vuln_sscanf.c` → `test_fio30_c_fail_testcases_vuln_sscanf`
- ⏭️ NOT RUN `testcases_vuln_struct.c` → `test_fio30_c_fail_testcases_vuln_struct`
- ⏭️ NOT RUN `testcases_vuln_syslog.c` → `test_fio30_c_fail_testcases_vuln_syslog`
- ⏭️ NOT RUN `testcases_vuln_template.c` → `test_fio30_c_fail_testcases_vuln_template`
- ⏭️ NOT RUN `testcases_vuln_vfprintf.c` → `test_fio30_c_fail_testcases_vuln_vfprintf`
- ⏭️ NOT RUN `testcases_vuln_vprintf.c` → `test_fio30_c_fail_testcases_vuln_vprintf`
- ⏭️ NOT RUN `testcases_vuln_vsnprintf.c` → `test_fio30_c_fail_testcases_vuln_vsnprintf`
- ⏭️ NOT RUN `testcases_vuln_vsprintf.c` → `test_fio30_c_fail_testcases_vuln_vsprintf`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio30_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_posix.c` → `test_fio30_c_fail_wiki_posix`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_safe_const.c` → `test_fio30_c_pass_testcases_safe_const`
- ⏭️ NOT RUN `testcases_safe_fprintf.c` → `test_fio30_c_pass_testcases_safe_fprintf`
- ⏭️ NOT RUN `testcases_safe_fputs.c` → `test_fio30_c_pass_testcases_safe_fputs`
- ⏭️ NOT RUN `testcases_safe_logging.c` → `test_fio30_c_pass_testcases_safe_logging`
- ⏭️ NOT RUN `testcases_safe_printf.c` → `test_fio30_c_pass_testcases_safe_printf`
- ⏭️ NOT RUN `testcases_safe_puts.c` → `test_fio30_c_pass_testcases_safe_puts`
- ⏭️ NOT RUN `testcases_safe_scanf.c` → `test_fio30_c_pass_testcases_safe_scanf`
- ⏭️ NOT RUN `testcases_safe_snprintf.c` → `test_fio30_c_pass_testcases_safe_snprintf`
- ⏭️ NOT RUN `testcases_safe_sprintf.c` → `test_fio30_c_pass_testcases_safe_sprintf`
- ⏭️ NOT RUN `testcases_safe_vprintf.c` → `test_fio30_c_pass_testcases_safe_vprintf`
- ⏭️ NOT RUN `wiki_fprintf.c` → `test_fio30_c_pass_wiki_fprintf`
- ⏭️ NOT RUN `wiki_fputs.c` → `test_fio30_c_pass_wiki_fputs`
- ⏭️ NOT RUN `wiki_posix.c` → `test_fio30_c_pass_wiki_posix`

---

### 🔶 FIO09-C - Not Implemented (has tests)

<a id="rule-fio09c"></a>

**Title:** Be careful with binary data when transferring data across systems

**Description:** Portability is a concern when using thefread()andfwrite()functions across
multiple, heterogeneous systems. In particular, it is never guaranteed that
reading or writing of scalar data types such as integers, let alone aggregate
types such as arrays or structures, will preserve the representation or value of
the data. Implementations may differ in structure padding, floating-point model,
number of bits per byte, endianness, and other attributes that cause binary data
formats to be incompatible. This noncompliant code example reads data from a
file stream into a data structure: struct myData { char c; long l; }; /* ... */
FILE *file; struct myData data; /* Initialize file */ if (fread(&data,
sizeof(struct myData), 1, file) < sizeof(struct myData)) { /* Handle error */ }

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio09_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio09_c_pass_wiki_compliant_1`

---

### ✅ FIO34-C - Implemented

<a id="rule-fio34c"></a>

**Title:** Distinguish between characters read from a file and EOF or WEOF

**Description:** TheEOFmacro represents a negative value that is used to indicate that the file
is exhausted and no data remains when reading data from a file.EOFis an example
of anin-band error indicator. In-band error indicators are problematic to work
with, and the creation of new in-band-error indicators is discouraged byERR02-C.
Avoid in-band error indicators. The byte I/O functionsfgetc(),getc(),
andgetchar()all read a character from a stream and return it as
anint.(SeeSTR00-C. Represent characters using an appropriate type.) If the
stream is at the end of the file, the end-of-file indicator for the stream is
set and the function returnsEOF. If a read error occurs, the error indicator for
the stream is set and the function returnsEOF. If these functions succeed, they
cast the character returned into anunsigned char. BecauseEOFis negative, it
should not match any unsigned character value. However, this is only true
forimplementationswhere theinttype is wider thanchar. On an implementation
whereintandcharhave the same width, a character-reading function can read and
return a valid character that has the same bit-pattern asEOF. This could occur,
for example, if an attacker inserted a value that looked likeEOFinto the file or
data stream to alter the behavior of the program.

**Test Coverage:** 48 tests (37 fail, 11 pass)

**Test Results:** 0/48 passed (0.0%), 48 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_base64_decode_char.c` → `test_fio34_c_fail_testcases_base64_decode_char`
- ⏭️ NOT RUN `testcases_binary_search_char.c` → `test_fio34_c_fail_testcases_binary_search_char`
- ⏭️ NOT RUN `testcases_buffer_fill_char.c` → `test_fio34_c_fail_testcases_buffer_fill_char`
- ⏭️ NOT RUN `testcases_byte_compare_char.c` → `test_fio34_c_fail_testcases_byte_compare_char`
- ⏭️ NOT RUN `testcases_cast_to_char.c` → `test_fio34_c_fail_testcases_cast_to_char`
- ⏭️ NOT RUN `testcases_char_copy_loop.c` → `test_fio34_c_fail_testcases_char_copy_loop`
- ⏭️ NOT RUN `testcases_char_counter_wrong.c` → `test_fio34_c_fail_testcases_char_counter_wrong`
- ⏭️ NOT RUN `testcases_checksum_char.c` → `test_fio34_c_fail_testcases_checksum_char`
- ⏭️ NOT RUN `testcases_compress_char.c` → `test_fio34_c_fail_testcases_compress_char`
- ⏭️ NOT RUN `testcases_config_parser_char.c` → `test_fio34_c_fail_testcases_config_parser_char`
- ⏭️ NOT RUN `testcases_csv_parser_char.c` → `test_fio34_c_fail_testcases_csv_parser_char`
- ⏭️ NOT RUN `testcases_escape_sequence_char.c` → `test_fio34_c_fail_testcases_escape_sequence_char`
- ⏭️ NOT RUN `testcases_fgetc_char_type.c` → `test_fio34_c_fail_testcases_fgetc_char_type`
- ⏭️ NOT RUN `testcases_filter_wrong_type.c` → `test_fio34_c_fail_testcases_filter_wrong_type`
- ⏭️ NOT RUN `testcases_getc_char_type.c` → `test_fio34_c_fail_testcases_getc_char_type`
- ⏭️ NOT RUN `testcases_getchar_char_type.c` → `test_fio34_c_fail_testcases_getchar_char_type`
- ⏭️ NOT RUN `testcases_hash_calc_char.c` → `test_fio34_c_fail_testcases_hash_calc_char`
- ⏭️ NOT RUN `testcases_hex_dump_char.c` → `test_fio34_c_fail_testcases_hex_dump_char`
- ⏭️ NOT RUN `testcases_image_header_char.c` → `test_fio34_c_fail_testcases_image_header_char`
- ⏭️ NOT RUN `testcases_json_parser_char.c` → `test_fio34_c_fail_testcases_json_parser_char`
- ⏭️ NOT RUN `testcases_line_reader_char.c` → `test_fio34_c_fail_testcases_line_reader_char`
- ⏭️ NOT RUN `testcases_log_parser_char.c` → `test_fio34_c_fail_testcases_log_parser_char`
- ⏭️ NOT RUN `testcases_no_eof_check.c` → `test_fio34_c_fail_testcases_no_eof_check`
- ⏭️ NOT RUN `testcases_no_error_check.c` → `test_fio34_c_fail_testcases_no_error_check`
- ⏭️ NOT RUN `testcases_parser_char_type.c` → `test_fio34_c_fail_testcases_parser_char_type`
- ⏭️ NOT RUN `testcases_printf_format_char.c` → `test_fio34_c_fail_testcases_printf_format_char`
- ⏭️ NOT RUN `testcases_protocol_parser_char.c` → `test_fio34_c_fail_testcases_protocol_parser_char`
- ⏭️ NOT RUN `testcases_search_char_wrong.c` → `test_fio34_c_fail_testcases_search_char_wrong`
- ⏭️ NOT RUN `testcases_stream_cipher_char.c` → `test_fio34_c_fail_testcases_stream_cipher_char`
- ⏭️ NOT RUN `testcases_text_processor_char.c` → `test_fio34_c_fail_testcases_text_processor_char`
- ⏭️ NOT RUN `testcases_unsigned_char_type.c` → `test_fio34_c_fail_testcases_unsigned_char_type`
- ⏭️ NOT RUN `testcases_url_decode_char.c` → `test_fio34_c_fail_testcases_url_decode_char`
- ⏭️ NOT RUN `testcases_word_count_char.c` → `test_fio34_c_fail_testcases_word_count_char`
- ⏭️ NOT RUN `testcases_xml_parser_char.c` → `test_fio34_c_fail_testcases_xml_parser_char`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio34_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_nonportable.c` → `test_fio34_c_fail_wiki_nonportable`
- ⏭️ NOT RUN `wiki_wide_characters.c` → `test_fio34_c_fail_wiki_wide_characters`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_char_count_proper.c` → `test_fio34_c_pass_testcases_char_count_proper`
- ⏭️ NOT RUN `testcases_copy_file_proper.c` → `test_fio34_c_pass_testcases_copy_file_proper`
- ⏭️ NOT RUN `testcases_fgetc_proper.c` → `test_fio34_c_pass_testcases_fgetc_proper`
- ⏭️ NOT RUN `testcases_filter_chars_proper.c` → `test_fio34_c_pass_testcases_filter_chars_proper`
- ⏭️ NOT RUN `testcases_getc_proper.c` → `test_fio34_c_pass_testcases_getc_proper`
- ⏭️ NOT RUN `testcases_getchar_proper.c` → `test_fio34_c_pass_testcases_getchar_proper`
- ⏭️ NOT RUN `testcases_hex_dump_proper.c` → `test_fio34_c_pass_testcases_hex_dump_proper`
- ⏭️ NOT RUN `testcases_line_reader_proper.c` → `test_fio34_c_pass_testcases_line_reader_proper`
- ⏭️ NOT RUN `testcases_stream_parser_proper.c` → `test_fio34_c_pass_testcases_stream_parser_proper`
- ⏭️ NOT RUN `testcases_ungetc_proper.c` → `test_fio34_c_pass_testcases_ungetc_proper`
- ⏭️ NOT RUN `wiki_portable.c` → `test_fio34_c_pass_wiki_portable`

---

### 🔶 FIO24-C - Not Implemented (has tests)

<a id="rule-fio24c"></a>

**Title:** Do not open a file that is already open

**Description:** Opening a file that is already open hasimplementation-defined behavior,
according to the C Standard, 7.21.3, paragraph 8 [ISO/IEC 9899:2011]: Some
implementations do not allow multiple copies of the same file to be open at the
same time. Consequently, portable code cannot depend on what will happen if this
rule is violated. Even on implementations that do not outright fail to open an
already-opened file, aTOCTOU(time-of-check, time-of-use) race condition exists
in which the second open could operate on a different file from the first due to
the file being moved or deleted (seeFIO45-C. Avoid TOCTOU race conditions while
accessing filesfor more details on TOCTOU race conditions). This noncompliant
code example logs the program's state at runtime:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio24_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio24_c_pass_wiki_compliant_1`

---

### 🔶 FIO19-C - Not Implemented (has tests)

<a id="rule-fio19c"></a>

**Title:** Do not use fseek() and ftell() to compute the size of a regular file

**Description:** Understanding the difference between text mode and binary mode is important when
using functions that operate on file streams. (SeeFIO14-C. Understand the
difference between text mode and binary mode with file streamsfor more
information.) Subclause 7.21.9.2 of the C Standard [ISO/IEC 9899:2011] specifies
the following behavior forfseek()when opening a binary file in binary mode: In
addition, footnote 268 of subclause 7.21.3 says:

**Test Coverage:** 5 tests (2 fail, 3 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_binary_file.c` → `test_fio19_c_fail_wiki_binary_file`
- ⏭️ NOT RUN `wiki_text_file.c` → `test_fio19_c_fail_wiki_text_file`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posixfstat.c` → `test_fio19_c_pass_wiki_posixfstat`
- ⏭️ NOT RUN `wiki_posixftello.c` → `test_fio19_c_pass_wiki_posixftello`
- ⏭️ NOT RUN `wiki_windows.c` → `test_fio19_c_pass_wiki_windows`

---

### 🔶 FIO47-C - Not Implemented (has tests)

<a id="rule-fio47c"></a>

**Title:** Use valid format strings

**Description:** The formatted output functions (fprintf()and related functions) convert, format,
and print their arguments under control of aformatstring. The C Standard,
7.23.6.1, paragraph 3 [ISO/IEC 9899:2024], specifies Eachconversion
specificationis introduced by the%character followed (in order) by Common
mistakes in creating format strings include

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio47_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio47_c_pass_wiki_compliant_1`

---

### 🔶 FIO05-C - Not Implemented (has tests)

<a id="rule-fio05c"></a>

**Title:** Identify files using multiple file attributes

**Description:** Files can often be identified by attributes other than the file name, such as by
comparing file ownership or creation time. Information about a file that has
been created and closed can be stored and then used to validate the identity of
the file when it is reopened. Comparing multiple attributes of the file
increases the likelihood that the reopened file is the same file that had been
previously operated on. File identification is less of an issue if applications
maintain their files in secure directories, where they can be accessed only by
the owner of the file and (possibly) by a system administrator. (SeeFIO15-C.
Ensure that file operations are performed in a secure directory.)

**Test Coverage:** 5 tests (2 fail, 3 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_owner.c` → `test_fio05_c_fail_wiki_owner`
- ⏭️ NOT RUN `wiki_reopen.c` → `test_fio05_c_fail_wiki_reopen`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posix_devicei_node.c` → `test_fio05_c_pass_wiki_posix_devicei_node`
- ⏭️ NOT RUN `wiki_posix_open_only_once.c` → `test_fio05_c_pass_wiki_posix_open_only_once`
- ⏭️ NOT RUN `wiki_posix_owner.c` → `test_fio05_c_pass_wiki_posix_owner`

---

### 🔶 FIO15-C - Not Implemented (has tests)

<a id="rule-fio15c"></a>

**Title:** Ensure that file operations are performed in a secure directory

**Description:** File operations should be performed in asecure directory. In most cases, a
secure directory is a directory in which no one other than the user, or possibly
the administrator, has the ability to create, rename, delete, or otherwise
manipulate files. (Other users may read or search the directory but generally
may not modify the directory's contents in any way.) Also, other users must not
be able to delete or rename files in the parent of the secure directory and all
higher directories, although creating new files or deleting or renaming files
they own is permissible. Performing file operations in a secure directory
eliminates the possibility that an attacker might tamper with the files or file
system toexploita file systemvulnerabilityin a program. These vulnerabilities
often exist because there is a loose binding between the file name and the
actual file. (SeeFIO01-C. Be careful using functions that use file names for
identification.) In some cases, file operations can be performed securely
anywhere. In other cases, the only way to ensure secure file operations is to
perform the operation within a secure directory. Ensuring that file systems are
configured in a safe manner is typically a system administration function.
However, programs can often check that a file system is securely configured
before performing file operations that may lead to security vulnerabilities if
the system is misconfigured. There is a slight possibility that file systems
will be reconfigured in an insecure manner while a process is running and after
the check has been made. As a result, it is always advisable to implement your
code in a secure manner (that is, consistent with the other rules and
recommendations in this section) even when running in a secure directory.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio15_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_fio15_c_fail_wiki_noncompliant_2_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_fio15_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_posix_2.c` → `test_fio15_c_pass_wiki_posix_2`

---

### 🔶 FIO17-C - Not Implemented (has tests)

<a id="rule-fio17c"></a>

**Title:** Do not rely on an ending null character when using fread()

**Description:** Thefread()function, as defined in the C Standard, subclause 7.21.8.1 [ISO/IEC
9899:2011], does not explicitly null-terminate the read character sequence.
Although the content of a file has a properly null-terminated character
sequence, ifnmembis less than the total length of the characters,
thefread()function will not read afternmembcharacters.fread()will not append a
null character to the end of the string being read to. Suppose we have a null-
terminated character sequence in a file, and we need to extract a null-
terminated byte string:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio17_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio17_c_pass_wiki_compliant_1`

---

### 🔶 FIO51-C - Not Implemented (has tests)

<a id="rule-fio51c"></a>

**Title:** PP. Close files when they are no longer needed

**Description:** A call to thestd::basic_filebuf<T>::open()function must be matched with a call
tostd::basic_filebuf<T>::close()before the lifetime of the last pointer that
stores the return value of the call has ended or before normal program
termination, whichever occurs first. Note
thatstd::basic_ifstream<T>,std::basic_ofstream<T>, andstd::basic_fstream<T>all
maintain an internal reference to astd::basic_filebuf<T>object on
whichopen()andclose()are called as needed. Properly managing an object of one of
these types (by not leaking the object) is sufficient to ensure compliance with
this rule. Often, the best solution is to use the stream object by value
semantics instead of via dynamic memory allocation, ensuring compliance
withMEM51-CPP. Properly deallocate dynamically allocated resources. However,
that is still insufficient for situations in which destructors are not
automatically called. In this noncompliant code example,
astd::fstreamobjectfileis constructed. The constructor
forstd::fstreamcallsstd::basic_filebuf<T>::open(), and the
defaultstd::terminate_handlercalled bystd::terminate()isstd::abort(), which does
not call destructors. Consequently, the underlyingstd::basic_filebuf<T>object
maintained by the object is not properly closed.

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio51_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio51_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_fio51_c_pass_wiki_compliant_2`

---

### 🔶 FIO22-C - Not Implemented (has tests)

<a id="rule-fio22c"></a>

**Title:** Close files before spawning processes

**Description:** StandardFILEobjects and their underlying representation (file descriptors on
POSIX platforms or handles elsewhere) are a finite resource that must be
carefully managed. The number of files that animplementationguarantees may be
open simultaneously is bounded by theFOPEN_MAXmacro defined in<stdio.h>. The
value of the macro is guaranteed to be at least 8. Consequently, portable
programs must either avoid keeping more thanFOPEN_MAXfiles at the same time or
be prepared for functions such asfopen()to fail due to resource exhaustion.
Failing to close files when they are no longer needed may allow attackers to
exhaust, and possibly manipulate, system resources. This phenomenon is sometimes
calledfile descriptor leakage, although file pointers may also be used as an
attack vector. In addition, keeping files open longer than necessary increases
the risk that data written into in-memory file buffers will not be flushed in
the event ofabnormal program termination. To prevent file descriptor leaks and
to guarantee that any buffered data will be flushed into permanent storage,
files must be closed when they are no longer needed. The behavior of a program
isundefinedwhen it uses the value of a pointer to aFILEobject after the
associated file is closed (seeundefined behavior 153.) Programs that close the
standard streams (especiallystdoutbut alsostderrandstdin) must be careful not to
use the stream objects in subsequent function calls, particularly those that
implicitly operate on such objects (such asprintf(),perror(), andgetc()).

**Test Coverage:** 4 tests (1 fail, 3 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_fio22_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_fio22_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_linux.c` → `test_fio22_c_pass_wiki_linux`
- ⏭️ NOT RUN `wiki_posix.c` → `test_fio22_c_pass_wiki_posix`

---

## Category: FLP

<a id="category-flp"></a>

**Implementation Status:** 0 / 13 rules (0.0%)

### 🔶 FLP03-C - Not Implemented (has tests)

<a id="rule-flp03c"></a>

**Title:** Detect and handle floating-point errors

**Description:** Errors during floating-point operations are often neglected by programmers who
instead focus on validating operands before an operation. Errors that occur
during floating-point operations are admittedly difficult to determine and
diagnose, but the benefits of doing so often outweigh the costs. This
recommendation suggests ways to capture errors during floating-point operations.
The following code exhibits undefined behavior:
int j = 0; int iResult = 1 / j;

**Test Coverage:** 4 tests (1 fail, 3 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_flp03_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_c.c` → `test_flp03_c_pass_wiki_c`
- ⏭️ NOT RUN `wiki_windows.c` → `test_flp03_c_pass_wiki_windows`
- ⏭️ NOT RUN `wiki_windows_seh.c` → `test_flp03_c_pass_wiki_windows_seh`

---

### 🔶 FLP07-C - Not Implemented (has tests)

<a id="rule-flp07c"></a>

**Title:** Cast the return value of a function that returns a floating-point type

**Description:** Cast the return value of a function that returns a floating point type to ensure
predictable program execution.
Subclause 6.8.6.4, paragraph 3, of the C Standard [ISO/IEC 9899:2011] states:
This paragraph is annotated (footnote 160) as follows:

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_flp07_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_outside_the_function.c` → `test_flp07_c_pass_wiki_outside_the_function`
- ⏭️ NOT RUN `wiki_within_the_function.c` → `test_flp07_c_pass_wiki_within_the_function`

---

### 🔶 FLP34-C - Not Implemented (has tests)

<a id="rule-flp34c"></a>

**Title:** Ensure that floating-point conversions are within range of the new type

**Description:** If a floating-point value is to be converted to a floating-point value of a
smaller range and precision or to an integer type, or if an integer type is to
be converted to a floating-point type, the value must be representable in the
destination type.
The C Standard, 6.3.1.4, paragraph 2 [ISO/IEC 9899:2024], says,
Paragraph 2 of the same subclause says,

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_floattoint.c` → `test_flp34_c_fail_wiki_floattoint`
- ⏭️ NOT RUN `wiki_narrowing_conversion.c` → `test_flp34_c_fail_wiki_narrowing_conversion`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_floattoint.c` → `test_flp34_c_pass_wiki_floattoint`
- ⏭️ NOT RUN `wiki_narrowing_conversion.c` → `test_flp34_c_pass_wiki_narrowing_conversion`

---

### 🔶 FLP32-C - Not Implemented (has tests)

<a id="rule-flp32c"></a>

**Title:** Prevent or detect domain and range errors in math functions

**Description:** The C Standard, 7.12.1 [ISO/IEC 9899:2024], defines three types of errors that
relate specifically to math functions in<math.h>. Paragraph 2 states
Paragraph 3 states
Paragraph 4 states

**Test Coverage:** 8 tests (4 fail, 4 pass)

**Test Results:** 0/8 passed (0.0%), 8 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_asin_subnormal_number.c` → `test_flp32_c_fail_wiki_asin_subnormal_number`
- ⏭️ NOT RUN `wiki_pow.c` → `test_flp32_c_fail_wiki_pow`
- ⏭️ NOT RUN `wiki_sinh_range_errors.c` → `test_flp32_c_fail_wiki_sinh_range_errors`
- ⏭️ NOT RUN `wiki_sqrt.c` → `test_flp32_c_fail_wiki_sqrt`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_asin_subnormal_number.c` → `test_flp32_c_pass_wiki_asin_subnormal_number`
- ⏭️ NOT RUN `wiki_pow.c` → `test_flp32_c_pass_wiki_pow`
- ⏭️ NOT RUN `wiki_sinh_range_errors.c` → `test_flp32_c_pass_wiki_sinh_range_errors`
- ⏭️ NOT RUN `wiki_sqrt.c` → `test_flp32_c_pass_wiki_sqrt`

---

### 🔶 FLP30-C - Not Implemented (has tests)

<a id="rule-flp30c"></a>

**Title:** Do not use floating-point variables as loop counters

**Description:** Because floating-point numbers represent real numbers, it is often mistakenly
assumed that they can represent any simple fraction exactly. Floating-point
numbers are subject to representational limitations just as integers are, and
binary floating-point numbers cannot represent all real numbers exactly, even if
they can be represented in a small number of decimal digits.
In addition, because floating-point numbers can represent large values, it is
often mistakenly assumed that they can represent all significant digits of those
values. To gain a large dynamic range, floating-point numbers maintain a fixed
number of precision bits (also called the significand) and an exponent, which
limit the number of significant digits they can represent.
Different implementations have different precision limitations, and to keep code
portable, floating-point variables must not be used as the loop induction
variable. See Goldberg's work for an introduction to this topic [Goldberg 1991].

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_flp30_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_flp30_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_flp30_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_flp30_c_pass_wiki_compliant_2`

---

### ⚫ FLP00-C - Not Implemented (no tests)

<a id="rule-flp00c"></a>

**Title:** Understand the limitations of floating-point numbers

**Description:** The C programming language provides the ability to use floating-point numbers
for calculations. The C Standard specifies requirements on
aconformingimplementationfor floating-point numbers but makes few guarantees
about the specific underlying floating-point representation because of the
existence of competing floating-point systems.
By definition, a floating-point number is of finite precision and, regardless of
the underlying implementation, is prone to errors associated with rounding.
(SeeFLP01-C. Take care in rearranging floating-point expressionsandFLP02-C.
Avoid using floating-point numbers when precise computation is needed.)
The most common floating-point system is specified by the IEEE 754 standard. An
older floating-point system is the IBM floating-point representation (sometimes
called IBM/370). Each of these systems has different precisions and ranges of
representable values. As a result, they do not represent all of the same values,
are not binary compatible, and have different associated error rates.

**Test Coverage:** 0 tests (0 fail, 0 pass)

---

### ⚫ FLP01-C - Not Implemented (no tests)

<a id="rule-flp01c"></a>

**Title:** Take care in rearranging floating-point expressions

**Description:** Be careful when rearranging floating-point expressions to ensure the greatest
accuracy of the result.
Subclause 5.1.2.3, paragraph 14, of the C Standard [ISO/IEC 9899:2011], states:
Failure to understand the limitations in precision of floating-point-represented
numbers and their implications on the arrangement of expressions can cause
unexpected arithmetic results.

**Test Coverage:** 0 tests (0 fail, 0 pass)

---

### 🔶 FLP02-C - Not Implemented (has tests)

<a id="rule-flp02c"></a>

**Title:** Avoid using floating-point numbers when precise computation is needed

**Description:** Computers can represent only a finite number of digits. It is therefore
impossible to precisely represent repeating binary-representation values such as
1/3 or 1/5 with the most common floating-point representation: binary floating
point.
When precise computation is necessary, use alternative representations that can
accurately represent the values. For example, if you are performing arithmetic
on decimal values and need an exact decimal rounding, represent the values in
binary-coded decimal instead of using floating-point values. Another option is
decimal floating-point arithmetic, as specified by ANSI/IEEE 754-2007. ISO/IEC
WG14 has drafted a proposal to add support for decimal floating-point arithmetic
to the C language [ISO/IEC DTR 24732].
When precise computation is necessary, carefully and methodically estimate the
maximum cumulative error of the computations, regardless of whether decimal or
binary is used, to ensure that the resulting error is within tolerances.
Consider using numerical analysis to properly understand the problem. An
introduction can be found in David Goldberg's "What Every Computer Scientist
Should Know about Floating-Point Arithmetic" [Goldberg 1991].

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_flp02_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_flp02_c_fail_wiki_noncompliant_2_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_flp02_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2_2.c` → `test_flp02_c_pass_wiki_compliant_2_2`

---

### 🔶 FLP04-C - Not Implemented (has tests)

<a id="rule-flp04c"></a>

**Title:** Check floating-point inputs for exceptional values

**Description:** Floating-point numbers can take on two classes of exceptional values; infinity
and NaN (not-a-number). These values are returned as the result of exceptional
or otherwise unresolvable floating-point operations. (See alsoFLP32-C. Prevent
or detect domain and range errors in math functions.) Additionally, they can be
directly input by a user byscanfor similar functions. Failure to detect and
handle such values can result inundefined behavior.
NaN values are particularly problematic because the expression NaN == NaN (for
every possible value of NaN) returns false. Any comparisons made with NaN as one
of the arguments returns false, and all arithmetic functions on NaNs simply
propagate them through the code. Hence, a NaN entered in one location in the
code and not properly handled could potentially cause problems in other, more
distant sections.
Formatted-input functions such asscanfwill accept the valuesINF,INFINITY,
orNAN(case insensitive) as valid inputs for the%fformat specification, allowing
malicious users to feed them directly to a program. Programs should therefore
check to ensure that all input floating-point values (especially those
controlled by the user) have neither of these values if doing so would be
inappropriate. The<math.h>library provides two macros for this
purpose:isinfandisnan.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_flp04_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_flp04_c_pass_wiki_compliant_1`

---

### 🔶 FLP36-C - Not Implemented (has tests)

<a id="rule-flp36c"></a>

**Title:** Preserve precision when converting integral values to floating-point type

**Description:** Narrower arithmetic types can be cast to wider types without any effect on the
magnitude of numeric values. However, whereas integer types represent exact
values, floating-point types have limited precision. The C Standard, 6.3.1.4
paragraph 3 [ISO/IEC 9899:2024], states
Conversion from integral types to floating-point types without sufficient
precision can lead to loss of precision (loss of least significant bits). No
runtime exception occurs despite the loss.
In this noncompliant example, a large value of typelong intis converted to a
value of typefloatwithout ensuring it is representable in the type:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_flp36_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_flp36_c_pass_wiki_compliant_1`

---

### 🔶 FLP37-C - Not Implemented (has tests)

<a id="rule-flp37c"></a>

**Title:** Do not use object representations to compare floating-point values

**Description:** The object representation for floating-point values is implementation defined.
However, an implementation that defines the__STDC_IEC_559__macro shall conform
to theIEC 60559 floating-point standard and uses what is frequently referred to
asIEEE 754 floating-point arithmetic[ISO/IEC 9899:2024]. The floating-point
object representation used by IEC 60559 is one of the most common floating-point
object representations in use today.
All floating-point object representations use specific bit patterns to encode
the value of the floating-point number being represented. However, equivalence
of floating-point values is not encoded solely by the bit pattern used to
represent the value. For instance, if the floating-point format supports
negative zero values (as IEC 60559 does), the values-0.0and0.0are equivalent and
will compare as equal, but the bit patterns used in the object representation
are not identical. Similarly, if two floating-point values are both (the same)
NaN, they will not compare as equal, despite the bit patterns being identical,
because they are not equivalent.
Do not compare floating-point object representations directly, such as by
callingmemcmp()or its moral equivalents. Instead, the equality operators
(==and!=) should be used to determine if two floating-point values are
equivalent.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_flp37_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_flp37_c_pass_wiki_compliant_1`

---

### 🔶 FLP06-C - Not Implemented (has tests)

<a id="rule-flp06c"></a>

**Title:** Convert integers to floating point for floating-point operations

**Description:** Using integer arithmetic to calculate a value for assignment to a floating-point
variable may lead to loss of information. This problem can be avoided by
converting one of the integers in the expression to a floating type.
When converting integers to floating-point values, and vice versa, it is
important to carry out proper range checks to avoid undefined behavior
(seeFLP34-C. Ensure that floating-point conversions are within range of the new
type).
In this noncompliant code example, the division and multiplication operations
take place on integers and are then converted to floating point. Consequently,
floating-point variablesd,e, andfare not initialized correctly because the
operations take place before the values are converted to floating-point values.
The results are truncated to the nearest integer or may overflow.

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_flp06_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_conversion.c` → `test_flp06_c_pass_wiki_conversion`
- ⏭️ NOT RUN `wiki_floating_point_literal.c` → `test_flp06_c_pass_wiki_floating_point_literal`

---

### 🔶 FLP05-C - Not Implemented (has tests)

<a id="rule-flp05c"></a>

**Title:** Do not use denormalized numbers

**Description:** Most implementations of C use the IEEE 754 standard for floating-point
representation. In this representation, floats are encoded using 1 sign bit, 8
exponent bits, and 23 mantissa bits. Doubles are encoded and used exactly the
same way, except they use 1 sign bit, 11 exponent bits, and 52 mantissa bits.
These bits encode the values ofs, the sign;M, the significand; andE, the
exponent. Floating-point numbers are then calculated as (−1)s*M* 2E.
Ordinarily, all of the mantissa bits are used to express significant figures, in
addition to a leading 1, which is implied and therefore left out. Consequently,
floats ordinarily have 24 significant bits of precision, and doubles ordinarily
have 53 significant bits of precision. Such numbers are callednormalized
numbers. All floating-point numbers are limited in the sense that they have
fixed precision. SeeFLP00-C. Understand the limitations of floating-point
numbers.
Mantissa bits are used to express extremely small numbers that are too small to
encode normally because of the lack of available exponent bits. Using mantissa
bits extends the possible range of exponents. Because these bits no longer
function as significant bits of precision, the total precision of extremely
small numbers is less than usual. Such numbers are calleddenormalized,and they
are more limited than normalized numbers. However, even using normalized numbers
where precision is required can pose a risk. SeeFLP02-C. Avoid using floating-
point numbers when precise computation is neededfor more information.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_flp05_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_flp05_c_fail_wiki_noncompliant_2_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_flp05_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2_2.c` → `test_flp05_c_pass_wiki_compliant_2_2`

---

## Category: INT

<a id="category-int"></a>

**Implementation Status:** 3 / 23 rules (13.0%)

### 🔶 INT36-C - Not Implemented (has tests)

<a id="rule-int36c"></a>

**Title:** Converting a pointer to integer or integer to pointer

**Description:** Although programmers often use integers and pointers interchangeably in C,
pointer-to-integer and integer-to-pointer conversions areimplementation-defined.
Conversions between integers and pointers can have undesired consequences
depending on theimplementation. According to the C Standard, subclause 6.3.2.3
[ISO/IEC 9899:2024],

**Test Coverage:** 5 tests (3 fail, 2 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int36_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_int36_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_int36_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_int36_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_int36_c_pass_wiki_compliant_2`

---

### 🔶 INT09-C - Not Implemented (has tests)

<a id="rule-int09c"></a>

**Title:** Ensure enumeration constants map to unique values

**Description:** A C enumeration defines a type with a finite set of values represented by
identifiers known asenumeration constants, or enumerators. An enumerator is a
constant integer expression whose value is representable as anint. Although the
language allows multiple enumerators of the same type to have the same value, it
is a common expectation that all enumerators of the same type have distinct
values. However, defining two or more enumerators of the same type to have the
same value can lead to some nonobvious errors. In this noncompliant code
example, two enumerators of typeColorare assigned explicit values. It may not be
obvious to the programmer thatyellowandindigohave been declared to be identical
values (6), as aregreenandviolet(7). Probably the least dangerous error that can
result from such a definition is attempting to use the enumerators as labels of
aswitchstatement. Because all labels in aswitchstatement are required to be
unique, the following code violates this semantic constraint and is required to
be diagnosed by aconformingcompiler: enum Color { red=4, orange, yellow, green,
blue, indigo=6, violet }; const char* color_name(enum Color col) { switch (col)
{ case red: return "red"; case orange: return "orange"; case yellow: return
"yellow"; case green: return "green"; case blue: return "blue"; case indigo:
return "indigo"; /* Error: duplicate label (yellow) */ case violet: return
"violet"; /* Error: duplicate label (green) */ } }

**Test Coverage:** 4 tests (1 fail, 3 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int09_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_int09_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2_2.c` → `test_int09_c_pass_wiki_compliant_2_2`
- ⏭️ NOT RUN `wiki_compliant_3_3.c` → `test_int09_c_pass_wiki_compliant_3_3`

---

### ✅ INT30-C - Implemented

<a id="rule-int30c"></a>

**Title:** Ensure that unsigned integer operations do not wrap

**Description:** The C Standard, 6.2.5, paragraph 11 [ISO/IEC 9899:2024], states This behavior is
more informally calledunsigned integer wrapping. Unsigned integer operations can
wrap if the resulting value cannot be represented by the underlying
representation of the integer. The following table indicates which operators can
result in wrapping:
OperatorWrapOperatorWrapOperatorWrapOperatorWrap+Yes-=Yes<<Yes<No-
Yes*=Yes>>No>No*Yes/=No&No>=No/No%=No|No<=No%No<<=Yes^No==No++Yes>>=No~No!=No--
Yes&=No!No&&No=No|=Noun +No||No+=Yes^=Noun -Yes?:No

**Test Coverage:** 47 tests (33 fail, 14 pass)

**Test Results:** 0/47 passed (0.0%), 47 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_addition_nowrap.c` → `test_int30_c_fail_testcases_addition_nowrap`
- ⏭️ NOT RUN `testcases_aligned_alloc_wrap.c` → `test_int30_c_fail_testcases_aligned_alloc_wrap`
- ⏭️ NOT RUN `testcases_array_index_add.c` → `test_int30_c_fail_testcases_array_index_add`
- ⏭️ NOT RUN `testcases_buffer_copy_size.c` → `test_int30_c_fail_testcases_buffer_copy_size`
- ⏭️ NOT RUN `testcases_calloc_wrap.c` → `test_int30_c_fail_testcases_calloc_wrap`
- ⏭️ NOT RUN `testcases_compound_add_assign.c` → `test_int30_c_fail_testcases_compound_add_assign`
- ⏭️ NOT RUN `testcases_compound_multiply.c` → `test_int30_c_fail_testcases_compound_multiply`
- ⏭️ NOT RUN `testcases_compound_subtract.c` → `test_int30_c_fail_testcases_compound_subtract`
- ⏭️ NOT RUN `testcases_decrement_loop.c` → `test_int30_c_fail_testcases_decrement_loop`
- ⏭️ NOT RUN `testcases_hash_table_size.c` → `test_int30_c_fail_testcases_hash_table_size`
- ⏭️ NOT RUN `testcases_image_size_calc.c` → `test_int30_c_fail_testcases_image_size_calc`
- ⏭️ NOT RUN `testcases_increment_loop.c` → `test_int30_c_fail_testcases_increment_loop`
- ⏭️ NOT RUN `testcases_left_shift_compound.c` → `test_int30_c_fail_testcases_left_shift_compound`
- ⏭️ NOT RUN `testcases_left_shift_wrap.c` → `test_int30_c_fail_testcases_left_shift_wrap`
- ⏭️ NOT RUN `testcases_loop_bound_add.c` → `test_int30_c_fail_testcases_loop_bound_add`
- ⏭️ NOT RUN `testcases_multiplication_buffer_size.c` → `test_int30_c_fail_testcases_multiplication_buffer_size`
- ⏭️ NOT RUN `testcases_multiplication_malloc.c` → `test_int30_c_fail_testcases_multiplication_malloc`
- ⏭️ NOT RUN `testcases_nested_multiply.c` → `test_int30_c_fail_testcases_nested_multiply`
- ⏭️ NOT RUN `testcases_network_packet_size.c` → `test_int30_c_fail_testcases_network_packet_size`
- ⏭️ NOT RUN `testcases_offset_calculation.c` → `test_int30_c_fail_testcases_offset_calculation`
- ⏭️ NOT RUN `testcases_pointer_arithmetic_add.c` → `test_int30_c_fail_testcases_pointer_arithmetic_add`
- ⏭️ NOT RUN `testcases_post_decrement.c` → `test_int30_c_fail_testcases_post_decrement`
- ⏭️ NOT RUN `testcases_pre_increment.c` → `test_int30_c_fail_testcases_pre_increment`
- ⏭️ NOT RUN `testcases_realloc_size.c` → `test_int30_c_fail_testcases_realloc_size`
- ⏭️ NOT RUN `testcases_size_t_addition.c` → `test_int30_c_fail_testcases_size_t_addition`
- ⏭️ NOT RUN `testcases_string_buffer_calc.c` → `test_int30_c_fail_testcases_string_buffer_calc`
- ⏭️ NOT RUN `testcases_struct_array_alloc.c` → `test_int30_c_fail_testcases_struct_array_alloc`
- ⏭️ NOT RUN `testcases_subtraction_underflow.c` → `test_int30_c_fail_testcases_subtraction_underflow`
- ⏭️ NOT RUN `testcases_user_input_multiply.c` → `test_int30_c_fail_testcases_user_input_multiply`
- ⏭️ NOT RUN `testcases_vla_size.c` → `test_int30_c_fail_testcases_vla_size`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int30_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_int30_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_int30_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_addition_postcondition.c` → `test_int30_c_pass_testcases_addition_postcondition`
- ⏭️ NOT RUN `testcases_addition_precondition.c` → `test_int30_c_pass_testcases_addition_precondition`
- ⏭️ NOT RUN `testcases_increment_bounded.c` → `test_int30_c_pass_testcases_increment_bounded`
- ⏭️ NOT RUN `testcases_loop_bounds_checked.c` → `test_int30_c_pass_testcases_loop_bounds_checked`
- ⏭️ NOT RUN `testcases_multiplication_check_malloc.c` → `test_int30_c_pass_testcases_multiplication_check_malloc`
- ⏭️ NOT RUN `testcases_multiplication_two_checks.c` → `test_int30_c_pass_testcases_multiplication_two_checks`
- ⏭️ NOT RUN `testcases_saturation_arithmetic.c` → `test_int30_c_pass_testcases_saturation_arithmetic`
- ⏭️ NOT RUN `testcases_size_t_add_check.c` → `test_int30_c_pass_testcases_size_t_add_check`
- ⏭️ NOT RUN `testcases_subtraction_check.c` → `test_int30_c_pass_testcases_subtraction_check`
- ⏭️ NOT RUN `testcases_wider_type.c` → `test_int30_c_pass_testcases_wider_type`
- ⏭️ NOT RUN `wiki_c23_checked_integers.c` → `test_int30_c_pass_wiki_c23_checked_integers`
- ⏭️ NOT RUN `wiki_compliant_7.c` → `test_int30_c_pass_wiki_compliant_7`
- ⏭️ NOT RUN `wiki_postcondition_test.c` → `test_int30_c_pass_wiki_postcondition_test`
- ⏭️ NOT RUN `wiki_precondition_test.c` → `test_int30_c_pass_wiki_precondition_test`

---

### 🔶 INT07-C - Not Implemented (has tests)

<a id="rule-int07c"></a>

**Title:** Use only explicitly signed or unsigned char type for numeric values

**Description:** The three typeschar,signed char, andunsigned charare collectively called
thecharacter types. Compilers have the latitude to definecharto have the same
range, representation, and behavior aseithersigned charorunsigned char.
Irrespective of the choice made,charis a separate type from the other two and
isnotcompatible with either. Use onlysigned charandunsigned chartypes for the
storage and use of numeric values because it is the only portable way to
guarantee the signedness of the character types (seeSTR00-C. Represent
characters using an appropriate typefor more information on representing
characters). In this noncompliant code example, thechar-type variablecmay be
signed or unsigned. Assuming 8-bit, two's complement character types, this code
may print out eitheri/c = 5(unsigned) ori/c = -17(signed). It is much more
difficult to reason about the correctness of a program without knowing if these
integers are signed or unsigned.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int07_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_int07_c_pass_wiki_compliant_1`

---

### 🔶 INT17-C - Not Implemented (has tests)

<a id="rule-int17c"></a>

**Title:** Define integer constants in an implementation-independent manner

**Description:** Integer constants are often used as masks or specific bit values. Frequently,
these constants are expressed in hexadecimal form to indicate to the programmer
how the data might be represented in the machine. However, hexadecimal integer
constants are frequently used in a nonportable manner. In this pedagogical
noncompliant code example, theflipbits()function complements the value stored
inxby performing a bitwise exclusive OR against a mask with all bits set to 1.
Forimplementationswhereunsigned longis represented by a 32-bit value, each bit
ofxis correctly complemented. /* (Incorrect) Set all bits in mask to 1 */ const
unsigned long mask = 0xFFFFFFFF; unsigned long flipbits(unsigned long x) {
return x ^ mask; }

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int17_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_int17_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_1.c` → `test_int17_c_pass_wiki_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_int17_c_pass_wiki_compliant_2`

---

### 🔶 INT05-C - Not Implemented (has tests)

<a id="rule-int05c"></a>

**Title:** Do not use input functions to convert character data if they cannot handle all possible inputs

**Description:** Do not use functions that input characters and convert them to integers if the
functions cannot handle all possible inputs. For example, formatted input
functions such asscanf(),fscanf(),vscanf(), andvfscanf()can be used to read
string data fromstdinor (in the cases offscanf()andvfscanf()) other input
streams. These functions work fine for valid integer values but lack robust
error handling for invalid values. Alternatively, input character data as a
null-terminated byte string and convert to an integer value usingstrtol()or a
related function. (SeeERR34-C. Detect errors when converting a string to a
number.) This noncompliant code example uses thescanf()function to read a string
fromstdinand convert it to along. Thescanf()andfscanf()functions haveundefined
behaviorif the value of the result of this operation cannot be represented as an
integer.

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int05_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_int05_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_linux.c` → `test_int05_c_pass_wiki_linux`

---

### 🔶 INT01-C - Not Implemented (has tests)

<a id="rule-int01c"></a>

**Title:** Use size_t or rsize_t for all integer values representing the size of an object

**Description:** Thesize_ttype is the unsigned integer type of the result of thesizeofoperator.
Variables of typesize_tare guaranteed to be of sufficient precision to represent
the size of an object. The limit ofsize_tis specified by theSIZE_MAXmacro. The
typesize_tgenerally covers the entire address space. The C Standard, Annex K
(normative), "Bounds-checking interfaces," introduces a new type,rsize_t,
defined to besize_tbut explicitly used to hold the size of a single object
[Meyers 2004]. In code that documents this purpose by using the typersize_t, the
size of an object can be checked to verify that it is no larger thanRSIZE_MAX,
the maximum size of a normal single object, which provides additional input
validation for library functions. SeeVOID STR07-C. Use the bounds-checking
interfaces for string manipulationfor additional discussion of C11 Annex K. Any
variable that is used to represent the size of an object, including integer
values used as sizes, indices, loop counters, and lengths, should be
declaredrsize_t, if available. Otherwise, it should be declaredsize_t.

**Test Coverage:** 3 tests (2 fail, 1 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int01_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_int01_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_c11_annex_k.c` → `test_int01_c_pass_wiki_c11_annex_k`

---

### 🔶 INT35-C - Not Implemented (has tests)

<a id="rule-int35c"></a>

**Title:** Use correct integer precisions

**Description:** Integer types in C have both asizeand aprecision. The size indicates the number
of bytes used by an object and can be retrieved for any object or type using
thesizeofoperator. The precision of an integer type is the number of bits it
uses to represent values, excluding any sign and padding bits.Padding bits
contribute to the integer's size, but not to its precision. Consequently,
inferring the precision of an integer type from its size may result in too large
a value, which can then lead to incorrect assumptions about the numeric range of
these types. Programmers should use correct integer precisions in their code,
and in particular, should not use thesizeofoperator to compute the precision of
an integer type on architectures that use padding bits or in strictly conforming
(that is, portable) programs.Noncompliant Code ExampleThis noncompliant code
example illustrates a function that produces 2 raised to the power of the
function argument. To prevent undefined behavior (Seeundefined behavior 48.) in
compliance withINT34-C. Do not shift an expression by a negative number of bits
or by greater than or equal to the number of bits that exist in the operand, the
function ensures that the argument is less than the number of bits used to store
a value of typeunsigned int.#include <limits.h> unsigned int pow2(unsigned int
exp) { if (exp >= sizeof(unsigned int) * CHAR_BIT) { /* Handle error */ } return
1 << exp; }However, if this code runs on a platform whereunsigned inthas one or
more padding bits, it can still result in values forexpthat are too large. For
example, on a platform that storesunsigned intin 64 bits, but uses only 48 bits
to represent the value, a left shift of 56 bits would result in undefined
behavior (Seeundefined behavior 48.).Compliant Solution (popcount())This
compliant solution uses apopcount()function, which counts the number of bits set
on any unsigned integer, allowing this code to determine the precision of any
integer type, signed or unsigned.#include <stddef.h> #include <stdint.h> /*
Returns the number of set bits */ size_t popcount(uintmax_t num) { size_t
precision = 0; while (num != 0) { if (num % 2 == 1) { precision++; } num >>= 1;
} return precision; } #define PRECISION(umax_value)
popcount(umax_value)Implementations can replace thePRECISION()macro with a type-
generic macro that returns an integer constant expression that is the precision
of the specified type for that implementation. This return value can then be
used anywhere an integer constant expression can be used, such as in a static
assertion. (SeeDCL03-C. Use a static assertion to test the value of a constant
expression.) The following type generic macro, for example, might be used for a
specific implementation targeting the IA-32 architecture:#define
PRECISION(value) _Generic(value, \ unsigned char : 8, \ unsigned short: 16, \
unsigned int : 32, \ unsigned long : 32, \ unsigned long long : 64, \ signed
char : 7, \ signed short : 15, \ signed int : 31, \ signed long : 31, \ signed
long long : 63)The revised version of thepow2()function uses thePRECISION()macro
to determine the precision of the unsigned type:#include <stddef.h> #include
<stdint.h> #include <limits.h> extern size_t popcount(uintmax_t); #define
PRECISION(umax_value) popcount(umax_value) unsigned int pow2(unsigned int exp) {
if (exp >= PRECISION(UINT_MAX)) { /* Handle error */ } return 1 << exp;
}Implementation DetailsSome platforms, such as the Cray Linux Environment (CLE;
supported on Cray XT CNL compute nodes), providea _popcntinstruction that can
substitute for thepopcount()function.#define PRECISION(umax_value)
_popcnt(umax_value)Compliant Solution (C23)The C23 standard provides
various*_WIDTHmacros that define the number of width bits for each integer type.
This is effectively the size of the type (multiplied by 8) less the number of
padding bits. The following compliant solution uses theUINT_WIDTHtype to obtain
the width of an un#include <limits.h> unsigned int pow2(unsigned int exp) { if
(exp >= UINT_WIDTH) { /* Handle error */ } return 1 << exp; }Risk
AssessmentMistaking an integer's size for its precision can permit invalid
precision arguments to operations such as bitwise shifts, resulting in undefined
behavior.

**Test Coverage:** 5 tests (1 fail, 4 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int35_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_c23.c` → `test_int35_c_pass_wiki_c23`
- ⏭️ NOT RUN `wiki_popcount.c` → `test_int35_c_pass_wiki_popcount`
- ⏭️ NOT RUN `wiki_popcount_2.c` → `test_int35_c_pass_wiki_popcount_2`
- ⏭️ NOT RUN `wiki_popcount_3.c` → `test_int35_c_pass_wiki_popcount_3`

---

### 🔶 INT33-C - Not Implemented (has tests)

<a id="rule-int33c"></a>

**Title:** Ensure that division and remainder operations do not result in divide-by-zero errors

**Description:** The C Standard identifies the following condition under which division and
remainder operations result inundefined behavior (UB): UBDescription41The value
of the second operand of the/or%operator is zero (6.5.5). Ensure that division
and remainder operations do not result in divide-by-zero errors.

**Test Coverage:** 44 tests (32 fail, 12 pass)

**Test Results:** 0/44 passed (0.0%), 44 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_array_empty.c` → `test_int33_c_fail_testcases_array_empty`
- ⏭️ NOT RUN `testcases_array_index.c` → `test_int33_c_fail_testcases_array_index`
- ⏭️ NOT RUN `testcases_bitwise_zero.c` → `test_int33_c_fail_testcases_bitwise_zero`
- ⏭️ NOT RUN `testcases_calc_expr.c` → `test_int33_c_fail_testcases_calc_expr`
- ⏭️ NOT RUN `testcases_cmd_arg.c` → `test_int33_c_fail_testcases_cmd_arg`
- ⏭️ NOT RUN `testcases_compound_op.c` → `test_int33_c_fail_testcases_compound_op`
- ⏭️ NOT RUN `testcases_conditional.c` → `test_int33_c_fail_testcases_conditional`
- ⏭️ NOT RUN `testcases_direct_zero.c` → `test_int33_c_fail_testcases_direct_zero`
- ⏭️ NOT RUN `testcases_enum_zero.c` → `test_int33_c_fail_testcases_enum_zero`
- ⏭️ NOT RUN `testcases_file_input.c` → `test_int33_c_fail_testcases_file_input`
- ⏭️ NOT RUN `testcases_func_param.c` → `test_int33_c_fail_testcases_func_param`
- ⏭️ NOT RUN `testcases_func_return.c` → `test_int33_c_fail_testcases_func_return`
- ⏭️ NOT RUN `testcases_global_var.c` → `test_int33_c_fail_testcases_global_var`
- ⏭️ NOT RUN `testcases_input_unchecked.c` → `test_int33_c_fail_testcases_input_unchecked`
- ⏭️ NOT RUN `testcases_logic_result.c` → `test_int33_c_fail_testcases_logic_result`
- ⏭️ NOT RUN `testcases_loop_decrement.c` → `test_int33_c_fail_testcases_loop_decrement`
- ⏭️ NOT RUN `testcases_macro_unsafe.c` → `test_int33_c_fail_testcases_macro_unsafe`
- ⏭️ NOT RUN `testcases_modulo_compound.c` → `test_int33_c_fail_testcases_modulo_compound`
- ⏭️ NOT RUN `testcases_modulo_zero.c` → `test_int33_c_fail_testcases_modulo_zero`
- ⏭️ NOT RUN `testcases_multi_dim.c` → `test_int33_c_fail_testcases_multi_dim`
- ⏭️ NOT RUN `testcases_nested_call.c` → `test_int33_c_fail_testcases_nested_call`
- ⏭️ NOT RUN `testcases_pointer_deref.c` → `test_int33_c_fail_testcases_pointer_deref`
- ⏭️ NOT RUN `testcases_recursive.c` → `test_int33_c_fail_testcases_recursive`
- ⏭️ NOT RUN `testcases_shift_zero.c` → `test_int33_c_fail_testcases_shift_zero`
- ⏭️ NOT RUN `testcases_static_zero.c` → `test_int33_c_fail_testcases_static_zero`
- ⏭️ NOT RUN `testcases_struct_field.c` → `test_int33_c_fail_testcases_struct_field`
- ⏭️ NOT RUN `testcases_switch_case.c` → `test_int33_c_fail_testcases_switch_case`
- ⏭️ NOT RUN `testcases_time_zero.c` → `test_int33_c_fail_testcases_time_zero`
- ⏭️ NOT RUN `testcases_union_field.c` → `test_int33_c_fail_testcases_union_field`
- ⏭️ NOT RUN `testcases_var_zero.c` → `test_int33_c_fail_testcases_var_zero`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int33_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_int33_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_array_avg.c` → `test_int33_c_pass_testcases_array_avg`
- ⏭️ NOT RUN `testcases_basic_check.c` → `test_int33_c_pass_testcases_basic_check`
- ⏭️ NOT RUN `testcases_errno_check.c` → `test_int33_c_pass_testcases_errno_check`
- ⏭️ NOT RUN `testcases_fraction.c` → `test_int33_c_pass_testcases_fraction`
- ⏭️ NOT RUN `testcases_gcd_algo.c` → `test_int33_c_pass_testcases_gcd_algo`
- ⏭️ NOT RUN `testcases_input_valid.c` → `test_int33_c_pass_testcases_input_valid`
- ⏭️ NOT RUN `testcases_loop_step.c` → `test_int33_c_pass_testcases_loop_step`
- ⏭️ NOT RUN `testcases_macro_safe.c` → `test_int33_c_pass_testcases_macro_safe`
- ⏭️ NOT RUN `testcases_modulo_check.c` → `test_int33_c_pass_testcases_modulo_check`
- ⏭️ NOT RUN `testcases_time_calc.c` → `test_int33_c_pass_testcases_time_calc`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_int33_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_int33_c_pass_wiki_compliant_2`

---

### 🔶 INT13-C - Not Implemented (has tests)

<a id="rule-int13c"></a>

**Title:** Use bitwise operators only on unsigned operands

**Description:** Bitwise operators include the complement operator~, bitwise shift
operators>>and<<, bitwise AND operator&, bitwise exclusive OR operator^, bitwise
inclusive OR operator|and compound assignment operators >>=, <<=, &=, ^= and |=.
Bitwise operators should be used only with unsigned integer operands, as the
results of bitwise operations on signed integers areimplementation-defined. The
C11 standard, section 6.5, paragraph 4[ISO/IEC 9899:2011], states: Furthermore,
the bitwise shift operators << and>>are undefined under many circumstances, and
are implementation-defined for signed integers for more circumstances; see
ruleINT34-C. Do not shift an expression by a negative number of bits or by
greater than or equal to the number of bits that exist in the operandfor more
information.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_right_shift.c` → `test_int13_c_fail_wiki_right_shift`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_right_shift.c` → `test_int13_c_pass_wiki_right_shift`

---

### 🔶 INT04-C - Not Implemented (has tests)

<a id="rule-int04c"></a>

**Title:** Enforce limits on integer values originating from tainted sources

**Description:** All integer values originating fromtainted sourcesshould be evaluated to
determine if they have identifiable upper and lower bounds. If so, these limits
should be enforced by the interface. Restricting the input of excessively large
or small integers helps prevent overflow, truncation, and other type range
errors. Furthermore, it is easier to find and correct input problems than it is
to trace internal errors back to faulty inputs. In this noncompliant code
example,lengthis the value of a user-defined (and thus potentially untrusted)
environment variable whose value is used to determine the size of a dynamically
allocated array,table. In compliance withINT30-C. Ensure that unsigned integer
operations do not wrap, the code preventsunsigned integer wrappingbut does not
impose any upper bound on the size of the array, making it possible for the user
to cause the program to use an excessive amount of memory. char**
create_table(void) { const char* const lenstr = getenv("TABLE_SIZE"); const
size_t length = lenstr ? strtoul(lenstr, NULL, 10) : 0; if (length > SIZE_MAX /
sizeof(char *)) return NULL; /* Indicate error to caller */ const size_t
table_size = length * sizeof(char *); char** const table = (char
**)malloc(table_size); if (table == NULL) return NULL; /* Indicate error to
caller */ /* Initialize table... */ return table; }

**Test Coverage:** 6 tests (3 fail, 3 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_heartbleed.c` → `test_int04_c_fail_wiki_heartbleed`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int04_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_int04_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_int04_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_int04_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_heartbleed.c` → `test_int04_c_pass_wiki_heartbleed`

---

### 🔶 INT00-C - Not Implemented (has tests)

<a id="rule-int00c"></a>

**Title:** Understand the data model used by your implementation(s)

**Description:** Adata modeldefines the sizes assigned to standard data types. It is important to
understand the data models used by yourimplementation. However, if your code
depends on any assumptions not guaranteed by the standard, you should provide
static assertions to ensure that your assumptions are valid. (SeeDCL03-C. Use a
static assertion to test the value of a constant expression.) Assumptions
concerning integer sizes may become invalid, for example, when porting from a
32-bit architecture to a 64-bit architecture. Data
TypeiAPX86IA-32IA-64SPARC-64ARM-32Alpha64-bit Linux, FreeBSD,NetBSD, and
OpenBSDchar8888888short16161616161616int16323232323232long32323264326464long
longN/A646464646464Pointer16/32326464326464 Code frequently embeds assumptions
about data models. For example, some code bases require pointer andlongto have
the same size, whereas other large code bases requireintandlongto be the same
size [van de Voort 2007]. These types of assumptions, while common, make the
code difficult to port and make the ports error prone. One solution is to avoid
anyimplementation-defined behavior. However, this practice can result in
inefficient code. Another solution is to include either static or runtime
assertions near any platform-specific assumptions, so they can be easily
detected and corrected during porting.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int00_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_int00_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_int00_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_int00_c_pass_wiki_compliant_2`

---

### 🔶 INT08-C - Not Implemented (has tests)

<a id="rule-int08c"></a>

**Title:** Verify that all integer values are in range

**Description:** Integer operations must result in an integer value within the range of the
integer type (that is, the resulting value is the same as the result produced by
unlimited-range integers). Frequently, the range is more restrictive depending
on the use of the integer value, for example, as an index. Integer values can be
verified by code review or bystatic analysis. Integer overflow isundefined
behavior, so a compiled program can do anything, including go off to play the
Game of Life. Furthermore, a compiler may perform optimizations that assume an
overflow will never occur, which can easily yield unexpected results. Compilers
can optimize awayifstatements that check whether an overflow occurred.
SeeMSC15-C. Do not depend on undefined behaviorfor an example. Verifiably in-
range operations are often preferable to treating out-of-range values as an
error condition because the handling of these errors has been repeatedly shown
to causedenial-of-serviceproblems in actual applications. The quintessential
example is the failure of the Ariane 5 launcher, which occurred because of an
improperly handled conversion error that resulted in the processor being shut
down [Lions 1996].

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int08_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_int08_c_pass_wiki_compliant_1`

---

### 🔶 INT12-C - Not Implemented (has tests)

<a id="rule-int12c"></a>

**Title:** Do not make assumptions about the type of a plain int bit-field when used in an expression

**Description:** Bit-fields can be used to allow flags or other integer values with small ranges
to be packed together to save storage space. It isimplementation-definedwhether
the specifierintdesignates the same type assigned intor the same type asunsigned
intfor bit-fields. According to the C Standard [ISO/IEC 9899:2011], C integer
promotions also require that "if anintcan represent all values of the original
type (as restricted by the width, for a bit-field), the value is converted to
anint; otherwise, it is converted to anunsigned int." This issue is similar to
the signedness of plainchar, discussed inINT07-C. Use only explicitly signed or
unsigned char type for numeric values. A plainintbit-field that is treated as
unsigned will promote tointas long as its field width is less than that
ofintbecauseintcan hold all values of the original type. This behavior is the
same as that of a plainchartreated as unsigned. However, a plainintbit-field
treated as unsigned will promote tounsigned intif its field width is the same as
that ofint. This difference makes a plainintbit-field even trickier than a
plainchar.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int12_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_int12_c_pass_wiki_compliant_1`

---

### 🔶 INT02-C - Not Implemented (has tests)

<a id="rule-int02c"></a>

**Title:** Understand integer conversion rules

**Description:** Conversions can occur explicitly as the result of a cast or implicitly as
required by an operation. Although conversions are generally required for the
correct execution of a program, they can also lead to lost or misinterpreted
data. Conversion of an operand value to a compatible type causes no change to
the value or the representation. The C integer conversion rules define how C
compilers handle conversions. These rules includeinteger promotions,integer
conversion rank, and theusual arithmetic conversions. The intent of the rules is
to ensure that the conversions result in the same numerical values and that
these values minimize surprises in the rest of the computation. Prestandard C
usually preferred to preserve signedness of the type. Integer types smaller
thanintare promoted when an operation is performed on them. If all values of the
original type can be represented as anint, the value of the smaller type is
converted to anint; otherwise, it is converted to anunsigned int. Integer
promotions are applied as part of the usual arithmetic conversions to certain
argument expressions; operands of the unary+,-, and~operators; and operands of
the shift operators. The following code fragment shows the application of
integer promotions:

**Test Coverage:** 9 tests (4 fail, 5 pass)

**Test Results:** 0/9 passed (0.0%), 9 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_comparison.c` → `test_int02_c_fail_wiki_comparison`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_int02_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_int02_c_fail_wiki_noncompliant_3`
- ⏭️ NOT RUN `wiki_noncompliant_4.c` → `test_int02_c_fail_wiki_noncompliant_4`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_int02_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2_2.c` → `test_int02_c_pass_wiki_compliant_2_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_int02_c_pass_wiki_compliant_3`
- ⏭️ NOT RUN `wiki_compliant_4.c` → `test_int02_c_pass_wiki_compliant_4`
- ⏭️ NOT RUN `wiki_compliant_5.c` → `test_int02_c_pass_wiki_compliant_5`

---

### 🔶 INT34-C - Not Implemented (has tests)

<a id="rule-int34c"></a>

**Title:** Do not shift an expression by a negative number of bits or by greater than or equal to the number of bits that exist in the operand

**Description:** Bitwise shifts include left-shift operations of the formshift-
expression<<additive-expressionand right-shift operations of the formshift-
expression>>additive-expression. The standard integer promotions are first
performed on the operands, each of which has an integer type. The type of the
result is that of the promoted left operand. If the value of the right operand
is negative or is greater than or equal to the width of the promoted left
operand, the behavior isundefined. (Seeundefined behavior 48.) Do not shift an
expression by a negative number of bits or by a number greater than or equal to
theprecisionof the promoted left operand. The precision of an integer type is
the number of bits it uses to represent values, excluding any sign and padding
bits. For unsigned integer types, the width and the precision are the same;
whereas for signed integer types, the width is one greater than the precision.
This rule uses precision instead of width because, in almost every case, an
attempt to shift by a number of bits greater than or equal to the precision of
the operand indicates a bug (logic error). A logic error is different from
overflow, in which there is simply a representational deficiency. In general,
shifts should be performed only on unsigned operands. (SeeINT13-C. Use bitwise
operators only on unsigned operands.) The result ofE1 << E2isE1left-shiftedE2bit
positions; vacated bits are filled with zeros. The following diagram illustrates
the left-shift operation.

**Test Coverage:** 6 tests (2 fail, 4 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_left_shift_signed_type.c` → `test_int34_c_fail_wiki_left_shift_signed_type`
- ⏭️ NOT RUN `wiki_left_shift_unsigned_type.c` → `test_int34_c_fail_wiki_left_shift_unsigned_type`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_left_shift_signed_type.c` → `test_int34_c_pass_wiki_left_shift_signed_type`
- ⏭️ NOT RUN `wiki_left_shift_signed_type_2.c` → `test_int34_c_pass_wiki_left_shift_signed_type_2`
- ⏭️ NOT RUN `wiki_left_shift_unsigned_type.c` → `test_int34_c_pass_wiki_left_shift_unsigned_type`
- ⏭️ NOT RUN `wiki_right_shift.c` → `test_int34_c_pass_wiki_right_shift`

---

### 🔶 INT15-C - Not Implemented (has tests)

<a id="rule-int15c"></a>

**Title:** Use intmax_t or uintmax_t for formatted IO on programmer-defined integer types

**Description:** Few programmers consider the issues around formatted I/O and type definitions. A
programmer-defined integer type might be any type supported by
theimplementation, even a type larger thanunsigned long long. For example, given
an implementation that supports 128-bit unsigned integers and provides
auint_fast128_ttype, a programmer may define the following type: typedef
uint_fast128_t mytypedef_t; Furthermore, the definition of programmer-defined
types may change, which creates a problem when these types are used with
formatted output functions, such asprintf(), and formatted input functions, such
asscanf(). (SeeFIO47-C. Use valid format strings.)

**Test Coverage:** 5 tests (2 fail, 3 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_printf.c` → `test_int15_c_fail_wiki_printf`
- ⏭️ NOT RUN `wiki_scanf.c` → `test_int15_c_fail_wiki_scanf`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_microsoftprintf.c` → `test_int15_c_pass_wiki_microsoftprintf`
- ⏭️ NOT RUN `wiki_printf.c` → `test_int15_c_pass_wiki_printf`
- ⏭️ NOT RUN `wiki_strtoumax.c` → `test_int15_c_pass_wiki_strtoumax`

---

### 🔶 INT10-C - Not Implemented (has tests)

<a id="rule-int10c"></a>

**Title:** Do not assume a positive remainder when using the % operator

**Description:** In C89 (and historical K&Rimplementations), the meaning of the remainder
operator for negative operands wasimplementation-defined. This behavior was
changed in C99, and the change remains in C11. Because not all C compilers are
strictly C-conforming, programmers cannot rely on the behavior of the%operator
if they need to run on a wide range of platforms with many different compilers.
The C Standard, subclause 6.5.5 [ISO/IEC 9899:2011], states:

**Test Coverage:** 3 tests (2 fail, 1 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int10_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_int10_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_unsigned_types.c` → `test_int10_c_pass_wiki_unsigned_types`

---

### 🔶 INT16-C - Not Implemented (has tests)

<a id="rule-int16c"></a>

**Title:** Do not make assumptions about representation of signed integers

**Description:** Although many common implementations use a two's complement representation of
signed integers, the C Standard declares such use asimplementation-definedand
allows all of the following representations: This is a specific example
ofMSC14-C. Do not introduce unnecessary platform dependencies. One way to check
whether a number is even or odd is to examine the least significant bit, but the
results will be inconsistent. Specifically, this example gives unexpected
behavior on all one's complement implementations:

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int16_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_int16_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_int16_c_pass_wiki_compliant_2`

---

### ✅ INT18-C - Implemented

<a id="rule-int18c"></a>

**Title:** Evaluate integer expressions in a larger size before comparing or assigning to that size

**Description:** If an integer expression involving an operation is compared to or assigned to a
larger integer size, that integer expression should be evaluated in that larger
size by explicitly casting one of the operands. This code example is
noncompliant on systems wheresize_tis an unsigned 32-bit value andlong longis a
64-bit value. In this example, the programmer tests for wrapping by
comparingSIZE_MAXtolength + BLOCK_HEADER_SIZE. Becauselengthis declared
assize_t, the addition is performed as a 32-bit operation and can result in
wrapping. The comparison withSIZE_MAXwill always test false. If any wrapping
occurs,malloc()will allocate insufficient space formBlock, which can lead to a
subsequent buffer overflow. #include <stdlib.h> #include <stdint.h> /* For
SIZE_MAX */ enum { BLOCK_HEADER_SIZE = 16 }; void *AllocateBlock(size_t length)
{ struct memBlock *mBlock; if (length + BLOCK_HEADER_SIZE > (unsigned long
long)SIZE_MAX) return NULL; mBlock = (struct memBlock *)malloc( length +
BLOCK_HEADER_SIZE ); if (!mBlock) { return NULL; } /* Fill in block header and
return data portion */ return mBlock; }

**Test Coverage:** 7 tests (3 fail, 4 pass)

**Test Results:** 0/7 passed (0.0%), 7 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int18_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_int18_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_size_t.c` → `test_int18_c_fail_wiki_size_t`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_int18_c_pass_wiki_compliant_3`
- ⏭️ NOT RUN `wiki_rearrange_expression.c` → `test_int18_c_pass_wiki_rearrange_expression`
- ⏭️ NOT RUN `wiki_size_t.c` → `test_int18_c_pass_wiki_size_t`
- ⏭️ NOT RUN `wiki_upcast.c` → `test_int18_c_pass_wiki_upcast`

---

### 🔶 INT31-C - Not Implemented (has tests)

<a id="rule-int31c"></a>

**Title:** Ensure that integer conversions do not result in lost or misinterpreted data

**Description:** Integer conversions, both implicit and explicit (using a cast), must be
guaranteed not to result in lost or misinterpreted data. This rule is
particularly true for integer values that originate from untrusted sources and
are used in any of the following ways: This rule also applies to arguments
passed to the following library functions that are converted tounsigned char:
and to arguments to the following library functions that are converted tochar:

**Test Coverage:** 12 tests (6 fail, 6 pass)

**Test Results:** 0/12 passed (0.0%), 12 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_memset.c` → `test_int31_c_fail_wiki_memset`
- ⏭️ NOT RUN `wiki_signed_loss_of_precision.c` → `test_int31_c_fail_wiki_signed_loss_of_precision`
- ⏭️ NOT RUN `wiki_signed_to_unsigned.c` → `test_int31_c_fail_wiki_signed_to_unsigned`
- ⏭️ NOT RUN `wiki_time_treturn_value.c` → `test_int31_c_fail_wiki_time_treturn_value`
- ⏭️ NOT RUN `wiki_unsigned_loss_of_precision.c` → `test_int31_c_fail_wiki_unsigned_loss_of_precision`
- ⏭️ NOT RUN `wiki_unsigned_to_signed.c` → `test_int31_c_fail_wiki_unsigned_to_signed`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_memset.c` → `test_int31_c_pass_wiki_memset`
- ⏭️ NOT RUN `wiki_signed_loss_of_precision.c` → `test_int31_c_pass_wiki_signed_loss_of_precision`
- ⏭️ NOT RUN `wiki_signed_to_unsigned.c` → `test_int31_c_pass_wiki_signed_to_unsigned`
- ⏭️ NOT RUN `wiki_time_treturn_value.c` → `test_int31_c_pass_wiki_time_treturn_value`
- ⏭️ NOT RUN `wiki_unsigned_loss_of_precision.c` → `test_int31_c_pass_wiki_unsigned_loss_of_precision`
- ⏭️ NOT RUN `wiki_unsigned_to_signed.c` → `test_int31_c_pass_wiki_unsigned_to_signed`

---

### 🔶 INT14-C - Not Implemented (has tests)

<a id="rule-int14c"></a>

**Title:** Avoid performing bitwise and arithmetic operations on the same data

**Description:** Avoid performing bitwise and arithmetic operations on the same data. In
particular, bitwise operations are frequently performed on arithmetic values as
a form of premature optimization. Bitwise operators include the unary
operator~and the binary operators<<,>>,&,^, and|. Although such operations are
valid and will compile, they can reduce code readability. Declaring a variable
as containing a numeric value or a bitmap makes the programmer's intentions
clearer and the code more maintainable. Bitmapped types may be defined to
further separate bit collections from numeric types. Doing so may make it easier
to verify that bitwise operations are performed only on variables that represent
bitmaps. typedef uint32_t bitmap32_t; bitmap32_t x = 0x000007f3; x = (x << 2) |
3; /* Shifts in two 1-bits from the right */

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_left_shift.c` → `test_int14_c_fail_wiki_left_shift`
- ⏭️ NOT RUN `wiki_right_shift.c` → `test_int14_c_fail_wiki_right_shift`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_left_shift.c` → `test_int14_c_pass_wiki_left_shift`
- ⏭️ NOT RUN `wiki_right_shift.c` → `test_int14_c_pass_wiki_right_shift`

---

### ✅ INT32-C - Implemented

<a id="rule-int32c"></a>

**Title:** Ensure that operations on signed integers do not result in overflow

**Description:** Signed integer overflow isundefined behavior 36.
Consequently,implementationshave considerable latitude in how they deal with
signed integer overflow. (SeeMSC15-C. Do not depend on undefined behavior.) An
implementation that defines signed integer types as being modulo, for example,
need not detect integer overflow. Implementations may also trap on signed
arithmetic overflows, or simply assume that overflows will never happen and
generate object code accordingly. It is also possible for the same conforming
implementation to emit code that exhibits different behavior in different
contexts. For example, an implementation may determine that a signed integer
loop control variable declared in a local scope cannot overflow and may emit
efficient code on the basis of that determination, while the same implementation
may determine that a global variable used in a similar context will wrap. For
these reasons, it is important to ensure that operations on signed integers do
not result in overflow. Of particular importance are operations on signed
integer values that originate from atainted sourceand are used as Integer
operations will overflow if the resulting value cannot be represented by the
underlying representation of the integer. The following table indicates which
operations can result in overflow.

**Test Coverage:** 56 tests (37 fail, 19 pass)

**Test Results:** 0/56 passed (0.0%), 56 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_abs_min.c` → `test_int32_c_fail_testcases_abs_min`
- ⏭️ NOT RUN `testcases_accum_var.c` → `test_int32_c_fail_testcases_accum_var`
- ⏭️ NOT RUN `testcases_add_basic.c` → `test_int32_c_fail_testcases_add_basic`
- ⏭️ NOT RUN `testcases_add_neg.c` → `test_int32_c_fail_testcases_add_neg`
- ⏭️ NOT RUN `testcases_array_calc.c` → `test_int32_c_fail_testcases_array_calc`
- ⏭️ NOT RUN `testcases_avg_calc.c` → `test_int32_c_fail_testcases_avg_calc`
- ⏭️ NOT RUN `testcases_bitshift.c` → `test_int32_c_fail_testcases_bitshift`
- ⏭️ NOT RUN `testcases_buf_size.c` → `test_int32_c_fail_testcases_buf_size`
- ⏭️ NOT RUN `testcases_compound.c` → `test_int32_c_fail_testcases_compound`
- ⏭️ NOT RUN `testcases_coord_calc.c` → `test_int32_c_fail_testcases_coord_calc`
- ⏭️ NOT RUN `testcases_decr_min.c` → `test_int32_c_fail_testcases_decr_min`
- ⏭️ NOT RUN `testcases_div_min.c` → `test_int32_c_fail_testcases_div_min`
- ⏭️ NOT RUN `testcases_fact_calc.c` → `test_int32_c_fail_testcases_fact_calc`
- ⏭️ NOT RUN `testcases_func_add.c` → `test_int32_c_fail_testcases_func_add`
- ⏭️ NOT RUN `testcases_hash_calc.c` → `test_int32_c_fail_testcases_hash_calc`
- ⏭️ NOT RUN `testcases_incr_max.c` → `test_int32_c_fail_testcases_incr_max`
- ⏭️ NOT RUN `testcases_loop_over.c` → `test_int32_c_fail_testcases_loop_over`
- ⏭️ NOT RUN `testcases_mul_basic.c` → `test_int32_c_fail_testcases_mul_basic`
- ⏭️ NOT RUN `testcases_mul_neg.c` → `test_int32_c_fail_testcases_mul_neg`
- ⏭️ NOT RUN `testcases_neg_min.c` → `test_int32_c_fail_testcases_neg_min`
- ⏭️ NOT RUN `testcases_pow_calc.c` → `test_int32_c_fail_testcases_pow_calc`
- ⏭️ NOT RUN `testcases_ptr_arith.c` → `test_int32_c_fail_testcases_ptr_arith`
- ⏭️ NOT RUN `testcases_range_calc.c` → `test_int32_c_fail_testcases_range_calc`
- ⏭️ NOT RUN `testcases_shift_neg.c` → `test_int32_c_fail_testcases_shift_neg`
- ⏭️ NOT RUN `testcases_shift_over.c` → `test_int32_c_fail_testcases_shift_over`
- ⏭️ NOT RUN `testcases_size_mult.c` → `test_int32_c_fail_testcases_size_mult`
- ⏭️ NOT RUN `testcases_sub_basic.c` → `test_int32_c_fail_testcases_sub_basic`
- ⏭️ NOT RUN `testcases_sub_neg.c` → `test_int32_c_fail_testcases_sub_neg`
- ⏭️ NOT RUN `testcases_sum_array.c` → `test_int32_c_fail_testcases_sum_array`
- ⏭️ NOT RUN `testcases_time_calc.c` → `test_int32_c_fail_testcases_time_calc`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_int32_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_int32_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_int32_c_fail_wiki_noncompliant_3`
- ⏭️ NOT RUN `wiki_noncompliant_4.c` → `test_int32_c_fail_wiki_noncompliant_4`
- ⏭️ NOT RUN `wiki_noncompliant_5.c` → `test_int32_c_fail_wiki_noncompliant_5`
- ⏭️ NOT RUN `wiki_noncompliant_6.c` → `test_int32_c_fail_wiki_noncompliant_6`
- ⏭️ NOT RUN `wiki_noncompliant_7.c` → `test_int32_c_fail_wiki_noncompliant_7`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_abs_check.c` → `test_int32_c_pass_testcases_abs_check`
- ⏭️ NOT RUN `testcases_add_check.c` → `test_int32_c_pass_testcases_add_check`
- ⏭️ NOT RUN `testcases_array_idx.c` → `test_int32_c_pass_testcases_array_idx`
- ⏭️ NOT RUN `testcases_div_check.c` → `test_int32_c_pass_testcases_div_check`
- ⏭️ NOT RUN `testcases_incr_check.c` → `test_int32_c_pass_testcases_incr_check`
- ⏭️ NOT RUN `testcases_mul_check.c` → `test_int32_c_pass_testcases_mul_check`
- ⏭️ NOT RUN `testcases_neg_check.c` → `test_int32_c_pass_testcases_neg_check`
- ⏭️ NOT RUN `testcases_shift_check.c` → `test_int32_c_pass_testcases_shift_check`
- ⏭️ NOT RUN `testcases_size_calc.c` → `test_int32_c_pass_testcases_size_calc`
- ⏭️ NOT RUN `testcases_sub_check.c` → `test_int32_c_pass_testcases_sub_check`
- ⏭️ NOT RUN `wiki_c23_checked_integers.c` → `test_int32_c_pass_wiki_c23_checked_integers`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_int32_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_10.c` → `test_int32_c_pass_wiki_compliant_10`
- ⏭️ NOT RUN `wiki_compliant_11.c` → `test_int32_c_pass_wiki_compliant_11`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_int32_c_pass_wiki_compliant_3`
- ⏭️ NOT RUN `wiki_compliant_5.c` → `test_int32_c_pass_wiki_compliant_5`
- ⏭️ NOT RUN `wiki_compliant_6.c` → `test_int32_c_pass_wiki_compliant_6`
- ⏭️ NOT RUN `wiki_compliant_8.c` → `test_int32_c_pass_wiki_compliant_8`
- ⏭️ NOT RUN `wiki_compliant_9.c` → `test_int32_c_pass_wiki_compliant_9`

---

## Category: MEM

<a id="category-mem"></a>

**Implementation Status:** 3 / 17 rules (17.6%)

### ✅ MEM33-C - Implemented

<a id="rule-mem33c"></a>

**Title:** Allocate and copy structures containing a flexible array member dynamically

**Description:** The C Standard, 6.7.3.2, paragraph 20 [ISO/IEC 9899:2024], says The following is
an example of a structure that contains a flexible array member: struct
flex_array_struct { int num; int data[]; };

**Test Coverage:** 46 tests (33 fail, 13 pass)

**Test Results:** 0/46 passed (0.0%), 46 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_array_initialization.c` → `test_mem33_c_fail_testcases_array_initialization`
- ⏭️ NOT RUN `testcases_array_of_flex_structs.c` → `test_mem33_c_fail_testcases_array_of_flex_structs`
- ⏭️ NOT RUN `testcases_automatic_storage.c` → `test_mem33_c_fail_testcases_automatic_storage`
- ⏭️ NOT RUN `testcases_calloc_wrong_calculation.c` → `test_mem33_c_fail_testcases_calloc_wrong_calculation`
- ⏭️ NOT RUN `testcases_const_flex_struct.c` → `test_mem33_c_fail_testcases_const_flex_struct`
- ⏭️ NOT RUN `testcases_direct_assignment.c` → `test_mem33_c_fail_testcases_direct_assignment`
- ⏭️ NOT RUN `testcases_file_io_structure.c` → `test_mem33_c_fail_testcases_file_io_structure`
- ⏭️ NOT RUN `testcases_flexible_array_casting.c` → `test_mem33_c_fail_testcases_flexible_array_casting`
- ⏭️ NOT RUN `testcases_function_parameter_by_value.c` → `test_mem33_c_fail_testcases_function_parameter_by_value`
- ⏭️ NOT RUN `testcases_global_storage.c` → `test_mem33_c_fail_testcases_global_storage`
- ⏭️ NOT RUN `testcases_incomplete_memcpy.c` → `test_mem33_c_fail_testcases_incomplete_memcpy`
- ⏭️ NOT RUN `testcases_incorrect_size_calculation.c` → `test_mem33_c_fail_testcases_incorrect_size_calculation`
- ⏭️ NOT RUN `testcases_memset_insufficient_size.c` → `test_mem33_c_fail_testcases_memset_insufficient_size`
- ⏭️ NOT RUN `testcases_multiple_flex_members.c` → `test_mem33_c_fail_testcases_multiple_flex_members`
- ⏭️ NOT RUN `testcases_nested_struct_violation.c` → `test_mem33_c_fail_testcases_nested_struct_violation`
- ⏭️ NOT RUN `testcases_offsetof_violation.c` → `test_mem33_c_fail_testcases_offsetof_violation`
- ⏭️ NOT RUN `testcases_pass_by_value.c` → `test_mem33_c_fail_testcases_pass_by_value`
- ⏭️ NOT RUN `testcases_pointer_arithmetic_error.c` → `test_mem33_c_fail_testcases_pointer_arithmetic_error`
- ⏭️ NOT RUN `testcases_realloc_incorrect_size.c` → `test_mem33_c_fail_testcases_realloc_incorrect_size`
- ⏭️ NOT RUN `testcases_recursive_struct_flex.c` → `test_mem33_c_fail_testcases_recursive_struct_flex`
- ⏭️ NOT RUN `testcases_return_by_value.c` → `test_mem33_c_fail_testcases_return_by_value`
- ⏭️ NOT RUN `testcases_sizeof_miscalculation.c` → `test_mem33_c_fail_testcases_sizeof_miscalculation`
- ⏭️ NOT RUN `testcases_stack_array_access.c` → `test_mem33_c_fail_testcases_stack_array_access`
- ⏭️ NOT RUN `testcases_static_storage.c` → `test_mem33_c_fail_testcases_static_storage`
- ⏭️ NOT RUN `testcases_struct_initialization_list.c` → `test_mem33_c_fail_testcases_struct_initialization_list`
- ⏭️ NOT RUN `testcases_threading_shared_flex.c` → `test_mem33_c_fail_testcases_threading_shared_flex`
- ⏭️ NOT RUN `testcases_typedef_flex_array.c` → `test_mem33_c_fail_testcases_typedef_flex_array`
- ⏭️ NOT RUN `testcases_union_with_flex_array.c` → `test_mem33_c_fail_testcases_union_with_flex_array`
- ⏭️ NOT RUN `testcases_volatile_flex_struct.c` → `test_mem33_c_fail_testcases_volatile_flex_struct`
- ⏭️ NOT RUN `testcases_zero_length_array_confusion.c` → `test_mem33_c_fail_testcases_zero_length_array_confusion`
- ⏭️ NOT RUN `wiki_copying.c` → `test_mem33_c_fail_wiki_copying`
- ⏭️ NOT RUN `wiki_function_arguments.c` → `test_mem33_c_fail_wiki_function_arguments`
- ⏭️ NOT RUN `wiki_storage_duration.c` → `test_mem33_c_fail_wiki_storage_duration`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_calloc_correct_usage.c` → `test_mem33_c_pass_testcases_calloc_correct_usage`
- ⏭️ NOT RUN `testcases_correct_memcpy.c` → `test_mem33_c_pass_testcases_correct_memcpy`
- ⏭️ NOT RUN `testcases_dynamic_array_management.c` → `test_mem33_c_pass_testcases_dynamic_array_management`
- ⏭️ NOT RUN `testcases_error_handling_patterns.c` → `test_mem33_c_pass_testcases_error_handling_patterns`
- ⏭️ NOT RUN `testcases_file_io_compliant.c` → `test_mem33_c_pass_testcases_file_io_compliant`
- ⏭️ NOT RUN `testcases_memset_full_size.c` → `test_mem33_c_pass_testcases_memset_full_size`
- ⏭️ NOT RUN `testcases_pass_by_pointer.c` → `test_mem33_c_pass_testcases_pass_by_pointer`
- ⏭️ NOT RUN `testcases_proper_allocation.c` → `test_mem33_c_pass_testcases_proper_allocation`
- ⏭️ NOT RUN `testcases_realloc_proper_size.c` → `test_mem33_c_pass_testcases_realloc_proper_size`
- ⏭️ NOT RUN `testcases_struct_copying_function.c` → `test_mem33_c_pass_testcases_struct_copying_function`
- ⏭️ NOT RUN `wiki_copying.c` → `test_mem33_c_pass_wiki_copying`
- ⏭️ NOT RUN `wiki_function_arguments.c` → `test_mem33_c_pass_wiki_function_arguments`
- ⏭️ NOT RUN `wiki_storage_duration.c` → `test_mem33_c_pass_wiki_storage_duration`

---

### 🔶 MEM10-C - Not Implemented (has tests)

<a id="rule-mem10c"></a>

**Title:** Define and use a pointer validation function

**Description:** Many functions accept pointers as arguments. If the function dereferences
aninvalid pointer(as inEXP34-C. Do not dereference null pointers) or reads or
writes to a pointer that does not refer to an object, the results areundefined.
Typically, the program willterminate abnormallywhen an invalid pointer is
dereferenced, but it is possible for an invalid pointer to be dereferenced and
its memory changed without abnormal termination [Jack 2007]. Such programs can
be difficult to debug because of the difficulty in determining if a pointer
isvalid. One way to eliminate invalid pointers is to define a function that
accepts a pointer argument and indicates whether or not the pointer isvalidfor
some definition of valid. For example, the following function declares any
pointer to be valid exceptNULL: int valid(void *ptr) { return (ptr != NULL); }

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_mem10_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_mem10_c_pass_wiki_compliant_1`

---

### ✅ MEM31-C - Implemented

<a id="rule-mem31c"></a>

**Title:** Free dynamically allocated memory when no longer needed

**Description:** Before the lifetime of the last pointer that stores the return value of a call
to a standard memory allocation function has ended, it must be matched by a call
tofree()with that pointer value. In this noncompliant example, the object
allocated by the call tomalloc()is not freed before the end of the lifetime of
the last pointertext_bufferreferring to the object: #include <stdlib.h> enum {
BUFFER_SIZE = 32 }; int f(void) { char *text_buffer = (char
*)malloc(BUFFER_SIZE); if (text_buffer == NULL) { return -1; } return 0; }

**Test Coverage:** 100 tests (69 fail, 31 pass)

**Test Results:** 0/100 passed (0.0%), 100 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_abort_leak.c` → `test_mem31_c_fail_testcases_abort_leak`
- ⏭️ NOT RUN `testcases_array_elem_leak.c` → `test_mem31_c_fail_testcases_array_elem_leak`
- ⏭️ NOT RUN `testcases_array_leak.c` → `test_mem31_c_fail_testcases_array_leak`
- ⏭️ NOT RUN `testcases_atexit_leak.c` → `test_mem31_c_fail_testcases_atexit_leak`
- ⏭️ NOT RUN `testcases_callback_leak.c` → `test_mem31_c_fail_testcases_callback_leak`
- ⏭️ NOT RUN `testcases_calloc_leak.c` → `test_mem31_c_fail_testcases_calloc_leak`
- ⏭️ NOT RUN `testcases_complex_control_flow_double_free.c` → `test_mem31_c_fail_testcases_complex_control_flow_double_free`
- ⏭️ NOT RUN `testcases_condition_leak.c` → `test_mem31_c_fail_testcases_condition_leak`
- ⏭️ NOT RUN `testcases_conditional_double_free.c` → `test_mem31_c_fail_testcases_conditional_double_free`
- ⏭️ NOT RUN `testcases_double_alloc.c` → `test_mem31_c_fail_testcases_double_alloc`
- ⏭️ NOT RUN `testcases_double_free_basic.c` → `test_mem31_c_fail_testcases_double_free_basic`
- ⏭️ NOT RUN `testcases_double_free_in_loop.c` → `test_mem31_c_fail_testcases_double_free_in_loop`
- ⏭️ NOT RUN `testcases_early_return.c` → `test_mem31_c_fail_testcases_early_return`
- ⏭️ NOT RUN `testcases_error_handling_double_free.c` → `test_mem31_c_fail_testcases_error_handling_double_free`
- ⏭️ NOT RUN `testcases_exception_double_free.c` → `test_mem31_c_fail_testcases_exception_double_free`
- ⏭️ NOT RUN `testcases_exception_path.c` → `test_mem31_c_fail_testcases_exception_path`
- ⏭️ NOT RUN `testcases_fail_case_1.c` → `test_mem31_c_fail_testcases_fail_case_1`
- ⏭️ NOT RUN `testcases_fail_case_10.c` → `test_mem31_c_fail_testcases_fail_case_10`
- ⏭️ NOT RUN `testcases_fail_case_11.c` → `test_mem31_c_fail_testcases_fail_case_11`
- ⏭️ NOT RUN `testcases_fail_case_12.c` → `test_mem31_c_fail_testcases_fail_case_12`
- ⏭️ NOT RUN `testcases_fail_case_13.c` → `test_mem31_c_fail_testcases_fail_case_13`
- ⏭️ NOT RUN `testcases_fail_case_14.c` → `test_mem31_c_fail_testcases_fail_case_14`
- ⏭️ NOT RUN `testcases_fail_case_15.c` → `test_mem31_c_fail_testcases_fail_case_15`
- ⏭️ NOT RUN `testcases_fail_case_16.c` → `test_mem31_c_fail_testcases_fail_case_16`
- ⏭️ NOT RUN `testcases_fail_case_17.c` → `test_mem31_c_fail_testcases_fail_case_17`
- ⏭️ NOT RUN `testcases_fail_case_18.c` → `test_mem31_c_fail_testcases_fail_case_18`
- ⏭️ NOT RUN `testcases_fail_case_19.c` → `test_mem31_c_fail_testcases_fail_case_19`
- ⏭️ NOT RUN `testcases_fail_case_2.c` → `test_mem31_c_fail_testcases_fail_case_2`
- ⏭️ NOT RUN `testcases_fail_case_20.c` → `test_mem31_c_fail_testcases_fail_case_20`
- ⏭️ NOT RUN `testcases_fail_case_21.c` → `test_mem31_c_fail_testcases_fail_case_21`
- ⏭️ NOT RUN `testcases_fail_case_22.c` → `test_mem31_c_fail_testcases_fail_case_22`
- ⏭️ NOT RUN `testcases_fail_case_23.c` → `test_mem31_c_fail_testcases_fail_case_23`
- ⏭️ NOT RUN `testcases_fail_case_24.c` → `test_mem31_c_fail_testcases_fail_case_24`
- ⏭️ NOT RUN `testcases_fail_case_25.c` → `test_mem31_c_fail_testcases_fail_case_25`
- ⏭️ NOT RUN `testcases_fail_case_26.c` → `test_mem31_c_fail_testcases_fail_case_26`
- ⏭️ NOT RUN `testcases_fail_case_27.c` → `test_mem31_c_fail_testcases_fail_case_27`
- ⏭️ NOT RUN `testcases_fail_case_28.c` → `test_mem31_c_fail_testcases_fail_case_28`
- ⏭️ NOT RUN `testcases_fail_case_29.c` → `test_mem31_c_fail_testcases_fail_case_29`
- ⏭️ NOT RUN `testcases_fail_case_3.c` → `test_mem31_c_fail_testcases_fail_case_3`
- ⏭️ NOT RUN `testcases_fail_case_30.c` → `test_mem31_c_fail_testcases_fail_case_30`
- ⏭️ NOT RUN `testcases_fail_case_4.c` → `test_mem31_c_fail_testcases_fail_case_4`
- ⏭️ NOT RUN `testcases_fail_case_5.c` → `test_mem31_c_fail_testcases_fail_case_5`
- ⏭️ NOT RUN `testcases_fail_case_6.c` → `test_mem31_c_fail_testcases_fail_case_6`
- ⏭️ NOT RUN `testcases_fail_case_7.c` → `test_mem31_c_fail_testcases_fail_case_7`
- ⏭️ NOT RUN `testcases_fail_case_8.c` → `test_mem31_c_fail_testcases_fail_case_8`
- ⏭️ NOT RUN `testcases_fail_case_9.c` → `test_mem31_c_fail_testcases_fail_case_9`
- ⏭️ NOT RUN `testcases_func_ptr_leak.c` → `test_mem31_c_fail_testcases_func_ptr_leak`
- ⏭️ NOT RUN `testcases_global_leak.c` → `test_mem31_c_fail_testcases_global_leak`
- ⏭️ NOT RUN `testcases_goto_leak.c` → `test_mem31_c_fail_testcases_goto_leak`
- ⏭️ NOT RUN `testcases_loop_leak.c` → `test_mem31_c_fail_testcases_loop_leak`
- ⏭️ NOT RUN `testcases_lost_pointer.c` → `test_mem31_c_fail_testcases_lost_pointer`
- ⏭️ NOT RUN `testcases_macro_leak.c` → `test_mem31_c_fail_testcases_macro_leak`
- ⏭️ NOT RUN `testcases_multiple_paths_double_free.c` → `test_mem31_c_fail_testcases_multiple_paths_double_free`
- ⏭️ NOT RUN `testcases_nested_leak.c` → `test_mem31_c_fail_testcases_nested_leak`
- ⏭️ NOT RUN `testcases_no_free.c` → `test_mem31_c_fail_testcases_no_free`
- ⏭️ NOT RUN `testcases_partial_free.c` → `test_mem31_c_fail_testcases_partial_free`
- ⏭️ NOT RUN `testcases_realloc_leak.c` → `test_mem31_c_fail_testcases_realloc_leak`
- ⏭️ NOT RUN `testcases_realloc_misuse_double_free.c` → `test_mem31_c_fail_testcases_realloc_misuse_double_free`
- ⏭️ NOT RUN `testcases_recursive_leak.c` → `test_mem31_c_fail_testcases_recursive_leak`
- ⏭️ NOT RUN `testcases_setjmp_leak.c` → `test_mem31_c_fail_testcases_setjmp_leak`
- ⏭️ NOT RUN `testcases_signal_leak.c` → `test_mem31_c_fail_testcases_signal_leak`
- ⏭️ NOT RUN `testcases_stack_var_leak.c` → `test_mem31_c_fail_testcases_stack_var_leak`
- ⏭️ NOT RUN `testcases_string_leak.c` → `test_mem31_c_fail_testcases_string_leak`
- ⏭️ NOT RUN `testcases_struct_leak.c` → `test_mem31_c_fail_testcases_struct_leak`
- ⏭️ NOT RUN `testcases_switch_leak.c` → `test_mem31_c_fail_testcases_switch_leak`
- ⏭️ NOT RUN `testcases_thread_leak.c` → `test_mem31_c_fail_testcases_thread_leak`
- ⏭️ NOT RUN `testcases_union_leak.c` → `test_mem31_c_fail_testcases_union_leak`
- ⏭️ NOT RUN `testcases_wrong_order.c` → `test_mem31_c_fail_testcases_wrong_order`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_mem31_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_array_free.c` → `test_mem31_c_pass_testcases_array_free`
- ⏭️ NOT RUN `testcases_basic_free.c` → `test_mem31_c_pass_testcases_basic_free`
- ⏭️ NOT RUN `testcases_calloc_free.c` → `test_mem31_c_pass_testcases_calloc_free`
- ⏭️ NOT RUN `testcases_conditional_freeing_guards.c` → `test_mem31_c_pass_testcases_conditional_freeing_guards`
- ⏭️ NOT RUN `testcases_data_structure_cleanup.c` → `test_mem31_c_pass_testcases_data_structure_cleanup`
- ⏭️ NOT RUN `testcases_defensive_programming.c` → `test_mem31_c_pass_testcases_defensive_programming`
- ⏭️ NOT RUN `testcases_error_free.c` → `test_mem31_c_pass_testcases_error_free`
- ⏭️ NOT RUN `testcases_function_based_memory_management.c` → `test_mem31_c_pass_testcases_function_based_memory_management`
- ⏭️ NOT RUN `testcases_goto_cleanup.c` → `test_mem31_c_pass_testcases_goto_cleanup`
- ⏭️ NOT RUN `testcases_loop_free.c` → `test_mem31_c_pass_testcases_loop_free`
- ⏭️ NOT RUN `testcases_multiple_allocations_tracking.c` → `test_mem31_c_pass_testcases_multiple_allocations_tracking`
- ⏭️ NOT RUN `testcases_nested_free.c` → `test_mem31_c_pass_testcases_nested_free`
- ⏭️ NOT RUN `testcases_pass_case_1.c` → `test_mem31_c_pass_testcases_pass_case_1`
- ⏭️ NOT RUN `testcases_pass_case_10.c` → `test_mem31_c_pass_testcases_pass_case_10`
- ⏭️ NOT RUN `testcases_pass_case_2.c` → `test_mem31_c_pass_testcases_pass_case_2`
- ⏭️ NOT RUN `testcases_pass_case_3.c` → `test_mem31_c_pass_testcases_pass_case_3`
- ⏭️ NOT RUN `testcases_pass_case_4.c` → `test_mem31_c_pass_testcases_pass_case_4`
- ⏭️ NOT RUN `testcases_pass_case_5.c` → `test_mem31_c_pass_testcases_pass_case_5`
- ⏭️ NOT RUN `testcases_pass_case_6.c` → `test_mem31_c_pass_testcases_pass_case_6`
- ⏭️ NOT RUN `testcases_pass_case_7.c` → `test_mem31_c_pass_testcases_pass_case_7`
- ⏭️ NOT RUN `testcases_pass_case_8.c` → `test_mem31_c_pass_testcases_pass_case_8`
- ⏭️ NOT RUN `testcases_pass_case_9.c` → `test_mem31_c_pass_testcases_pass_case_9`
- ⏭️ NOT RUN `testcases_proper_error_handling.c` → `test_mem31_c_pass_testcases_proper_error_handling`
- ⏭️ NOT RUN `testcases_realloc_free.c` → `test_mem31_c_pass_testcases_realloc_free`
- ⏭️ NOT RUN `testcases_resource_cleanup_error_paths.c` → `test_mem31_c_pass_testcases_resource_cleanup_error_paths`
- ⏭️ NOT RUN `testcases_safe_realloc_usage.c` → `test_mem31_c_pass_testcases_safe_realloc_usage`
- ⏭️ NOT RUN `testcases_safe_wrapper_functions.c` → `test_mem31_c_pass_testcases_safe_wrapper_functions`
- ⏭️ NOT RUN `testcases_single_allocation_deallocation.c` → `test_mem31_c_pass_testcases_single_allocation_deallocation`
- ⏭️ NOT RUN `testcases_string_free.c` → `test_mem31_c_pass_testcases_string_free`
- ⏭️ NOT RUN `testcases_struct_free.c` → `test_mem31_c_pass_testcases_struct_free`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_mem31_c_pass_wiki_compliant_1`

---

### 🔶 MEM11-C - Not Implemented (has tests)

<a id="rule-mem11c"></a>

**Title:** Do not assume infinite heap space

**Description:** Memory is a limited resource and can be exhausted. Available memory is typically
bounded by the sum of the amount of physical memory and the swap space allocated
to the operating system by the administrator. For example, a system with 1GB of
physical memory configured with 2GB of swap space may be able to allocate, at
most, 3GB of heap space total to all running processes (minus the size of the
operating system itself and the text and data segments of all running
processes). Once all virtual memory has been allocated, requests for more memory
will fail. As discussed inERR33-C. Detect and handle standard library errors,
programs that fail to check for and properly handle memory allocation failures
will haveundefined behaviorand are likely to crash when heap space is exhausted.
Heap exhaustion can result from Ifmalloc()is unable to return the requested
memory, it returnsNULLinstead. However, simply checking for and handling memory
allocation failures may not be sufficient. Programs such as long-servers that
manipulate large data sets need to be designed in a way that permits them to
deliver their services when system resources, including the heap, are in short
supply. Making use of additional storage devices, such as disk space or
databases, is essential in such systems.

**Test Coverage:** 1 tests (1 fail, 0 pass)

**Test Results:** 0/1 passed (0.0%), 1 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_mem11_c_fail_wiki_noncompliant_1`

---

### 🔶 MEM01-C - Not Implemented (has tests)

<a id="rule-mem01c"></a>

**Title:** Store a new value in pointers immediately after free()

**Description:** Dangling pointers can lead to exploitable double-free and access-freed-
memoryvulnerabilities. A simple yet effective way to eliminate dangling pointers
and avoid many memory-related vulnerabilities is to set pointers toNULLafter
they are freed or to set them to another valid object. In this noncompliant code
example, the type of a message is used to determine how to process the message
itself. It is assumed thatmessage_typeis an integer andmessageis a pointer to an
array of characters that were allocated dynamically.
Ifmessage_typeequalsvalue_1, the message is processed accordingly. A similar
operation occurs whenmessage_typeequalsvalue_2. However, ifmessage_type ==
value_1evaluates to true andmessage_type == value_2also evaluates to true,
thenmessageis freed twice, resulting in a double-free vulnerability. char
*message; int message_type; /* Initialize message and message_type */ if
(message_type == value_1) { /* Process message type 1 */ free(message); } /*
...*/ if (message_type == value_2) { /* Process message type 2 */ free(message);
}

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_mem01_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_mem01_c_pass_wiki_compliant_1`

---

### 🔶 MEM34-C - Not Implemented (has tests)

<a id="rule-mem34c"></a>

**Title:** Only free memory allocated dynamically

**Description:** The C Standard, Annex J (184) [ISO/IEC 9899:2024], states that the behavior of a
program isundefinedwhen See alsoundefined behavior 184. Freeing memory that is
not allocated dynamically can result in heap corruption and other serious
errors. Do not callfree()on a pointer other than one returned by a standard
memory allocation function, such asmalloc(),calloc(),realloc(),
oraligned_alloc().

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_mem34_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_realloc.c` → `test_mem34_c_fail_wiki_realloc`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_mem34_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_realloc.c` → `test_mem34_c_pass_wiki_realloc`

---

### 🔶 MEM06-C - Not Implemented (has tests)

<a id="rule-mem06c"></a>

**Title:** Ensure that sensitive data is not written out to disk

**Description:** Developers should take steps to prevent sensitive information such as passwords,
cryptographic keys, and other secrets from being inadvertently leaked.
Preventive measures include attempting to keep such data from being written to
disk. Two common mechanisms by which data is inadvertently written to disk
areswappingandcore dumps. Many general-purpose operating systems implement a
virtual-memory-management technique calledpaging(also calledswapping) to
transfer pages between main memory and an auxiliary store, such as a disk drive.
This feature is typically implemented as a task running in the kernel of the
operating system, and its operation is invisible to the running program.

**Test Coverage:** 4 tests (1 fail, 3 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_mem06_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_mem06_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_privileged_process_posix.c` → `test_mem06_c_pass_wiki_privileged_process_posix`
- ⏭️ NOT RUN `wiki_windows.c` → `test_mem06_c_pass_wiki_windows`

---

### 🔶 MEM36-C - Not Implemented (has tests)

<a id="rule-mem36c"></a>

**Title:** Do not modify the alignment of objects by calling realloc()

**Description:** Do not invokerealloc()to modify the size of allocated objects that have stricter
alignment requirements than those guaranteed bymalloc(). Storage allocated by a
call to the standardaligned_alloc()function, for example, can have stricter than
normal alignment requirements. The C standard requires only that a pointer
returned byrealloc()be suitably aligned so that it may be assigned to a pointer
to any type of object with a fundamental alignment requirement. This
noncompliant code example returns a pointer to allocated memory that has been
aligned to a 4096-byte boundary. If theresizeargument to therealloc()function is
larger than the object referenced byptr, thenrealloc()will allocate new memory
that is suitably aligned so that it may be assigned to a pointer to any type of
object with a fundamental alignment requirement but may not preserve the
stricter alignment of the original object. #include <stdlib.h> void func(void) {
size_t resize = 1024; size_t alignment = 1 << 12; int *ptr; int *ptr1; if (NULL
== (ptr = (int *)aligned_alloc(alignment, sizeof(int)))) { /* Handle error */ }
if (NULL == (ptr1 = (int *)realloc(ptr, resize))) { /* Handle error */ } }

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_mem36_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_mem36_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_windows.c` → `test_mem36_c_pass_wiki_windows`

---

### 🔶 MEM04-C - Not Implemented (has tests)

<a id="rule-mem04c"></a>

**Title:** Beware of zero-length allocations

**Description:** When the requested size is 0, the behavior of the memory allocation
functionsmalloc(),calloc(), andrealloc()isimplementation-defined. Subclause
7.22.3 of the C Standard [ISO/IEC 9899:2011] states: In addition, the amount of
storage allocated by a successful call to the allocation function when 0 bytes
was requested isunspecified. Seeunspecified behavior 41in subclause J.1 of the C
Standard. In cases where the memory allocation functions return a non-null
pointer, reading from or writing to the allocated memory area results
inundefined behavior. Typically, the pointer refers to a zero-length block of
memory consisting entirely of control structures. Overwriting these control
structures damages the data structures used by the memory manager.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_malloc.c` → `test_mem04_c_fail_wiki_malloc`
- ⏭️ NOT RUN `wiki_realloc.c` → `test_mem04_c_fail_wiki_realloc`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_malloc.c` → `test_mem04_c_pass_wiki_malloc`
- ⏭️ NOT RUN `wiki_realloc.c` → `test_mem04_c_pass_wiki_realloc`

---

### 🔶 MEM07-C - Not Implemented (has tests)

<a id="rule-mem07c"></a>

**Title:** Ensure that the arguments to calloc(), when multiplied, do not wrap

**Description:** DeprecatedThis guideline does not apply to code that need conform only to C23.
Code that must conform to older versions of the C standard should still comply
with this guideline. Thecalloc()function takes two arguments: the number of
elements to allocate and the storage size of those elements.
Typically,calloc()implementationsmultiply these arguments to determine how much
memory to allocate. Historically, some implementations failed to check whether
out-of-bounds results silently wrapped [RUS-CERT Advisory 2002-08:02]. If the
result of multiplying the number of elements to allocate and the storage size
wraps, less memory is allocated than was requested. As a result, it is necessary
to ensure that these arguments, when multiplied, do not wrap. Modern
implementations of the C standard library should check for wrap. If
thecalloc()function implemented by the libraries used for a particular
implementation properly handlesunsigned integer wrapping(in conformance
withINT30-C. Ensure that unsigned integer operations do not wrap) when
multiplying the number of elements to allocate and the storage size, that is
sufficient to comply with this recommendation and no further action is required.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_mem07_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_mem07_c_pass_wiki_compliant_1`

---

### 🔶 MEM02-C - Not Implemented (has tests)

<a id="rule-mem02c"></a>

**Title:** Immediately cast the result of a memory allocation function call into a pointer to the allocated type

**Description:** An object of typevoid *is a generic data pointer. It can point to any data
object. For any incomplete or object typeT, C permits implicit conversion fromT
*tovoid *or fromvoid *toT *. C Standard memory allocation
functionsaligned_alloc(),malloc(),calloc(), andrealloc()usevoid *to declare
parameters and return types of functions designed to work for objects of
different types. For example, the C library declaresmalloc()as void
*malloc(size_t);

**Test Coverage:** 10 tests (3 fail, 7 pass)

**Test Results:** 0/10 passed (0.0%), 10 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_mem02_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_mem02_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3_3.c` → `test_mem02_c_fail_wiki_noncompliant_3_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_hand_coded.c` → `test_mem02_c_pass_wiki_hand_coded`
- ⏭️ NOT RUN `wiki_macros.c` → `test_mem02_c_pass_wiki_macros`
- ⏭️ NOT RUN `wiki_macros_2.c` → `test_mem02_c_pass_wiki_macros_2`
- ⏭️ NOT RUN `wiki_macros_3.c` → `test_mem02_c_pass_wiki_macros_3`
- ⏭️ NOT RUN `wiki_macros_4.c` → `test_mem02_c_pass_wiki_macros_4`
- ⏭️ NOT RUN `wiki_macros_5.c` → `test_mem02_c_pass_wiki_macros_5`
- ⏭️ NOT RUN `wiki_macros_6.c` → `test_mem02_c_pass_wiki_macros_6`

---

### 🔶 MEM12-C - Not Implemented (has tests)

<a id="rule-mem12c"></a>

**Title:** Consider using a goto chain when leaving a function on error when using and releasing resources

**Description:** Many functions require the allocation of multiple resources. Failing and
returning somewhere in the middle of this function without freeing all of the
allocated resources could produce a memory leak. It is a common error to forget
to free one (or all) of the resources in this manner, so agotochain is the
simplest and cleanest way to organize exits while preserving the order of freed
resources. In this noncompliant example, exit code is written for every instance
in which the function can terminate prematurely. Notice how failing to
closefin2produces a resource leak, leaving an open file descriptor. Please note
that these examples assumeerrno_tandNOERRto be defined, as recommended
inDCL09-C. Declare functions that return errno with a return type of errno_t. An
equivalent compatible example would defineerrno_tas anintandNOERRas zero.

**Test Coverage:** 4 tests (1 fail, 3 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_mem12_c_fail_wiki_posix`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_copy_processfrom_linux_kernel.c` → `test_mem12_c_pass_wiki_copy_processfrom_linux_kernel`
- ⏭️ NOT RUN `wiki_posix_goto_chain.c` → `test_mem12_c_pass_wiki_posix_goto_chain`
- ⏭️ NOT RUN `wiki_posix_nested_ifs.c` → `test_mem12_c_pass_wiki_posix_nested_ifs`

---

### 🔶 MEM00-C - Not Implemented (has tests)

<a id="rule-mem00c"></a>

**Title:** Allocate and free memory in the same module, at the same level of abstraction

**Description:** Dynamic memory management is a common source of programming flaws that can lead
to securityvulnerabilities. Poor memory management can lead to security issues,
such as heap-buffer overflows, dangling pointers, and double-free issues
[Seacord 2013]. From the programmer's perspective, memory management involves
allocating memory, reading and writing to memory, and deallocating memory.
Allocating and freeing memory in different modules and levels of abstraction may
make it difficult to determine when and if a block of memory has been freed,
leading to programming defects, such as memory leaks, double-
freevulnerabilities, accessing freed memory, or writing to freed or unallocated
memory. To avoid these situations, memory should be allocated and freed at the
same level of abstraction and, ideally, in the same code module. This includes
the use of the following memory allocation and deallocation functions described
in subclause 7.23.3 of the C Standard [ISO/IEC 9899:2011]:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_mem00_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_mem00_c_pass_wiki_compliant_1`

---

### 🔶 MEM03-C - Not Implemented (has tests)

<a id="rule-mem03c"></a>

**Title:** Clear sensitive information stored in reusable resources

**Description:** Sensitive data stored in reusable resources may be inadvertently leaked to a
less privileged user or attacker if not properly cleared. Examples of reusable
resources include The manner in which sensitive information can be properly
cleared varies depending on the resource type and platform. Dynamic memory
managers are not required to clear freed memory and generally do not because of
the additional runtime overhead. Furthermore, dynamic memory managers are free
to reallocate this same memory. As a result, it is possible to accidentally leak
sensitive information if it is not cleared before calling a function that frees
dynamic memory. Programmers also cannot rely on memory being cleared during
allocation.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_free.c` → `test_mem03_c_fail_wiki_free`
- ⏭️ NOT RUN `wiki_realloc.c` → `test_mem03_c_fail_wiki_realloc`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_mem03_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_mem03_c_pass_wiki_compliant_2`

---

### ✅ MEM30-C - Implemented

<a id="rule-mem30c"></a>

**Title:** Do not access freed memory

**Description:** Evaluating a pointer—including dereferencing the pointer, using it as an operand
of an arithmetic operation, type casting it, and using it as the right-hand side
of an assignment—into memory that has been deallocated by a memory management
function isundefined behavior 183. Pointers to memory that has been deallocated
are calleddangling pointers. Accessing a dangling pointer can result in
exploitablevulnerabilities. According to the C Standard, using the value of a
pointer that refers to space deallocated by a call to
thefree()orrealloc()function is undefined behavior. (Seeundefined behavior 183.)
Reading a pointer to deallocated memory isundefined behavior 183because the
pointer value isindeterminateand might be atrap representation. Fetching a trap
representation might perform a hardware trap (but is not required to).

**Test Coverage:** 48 tests (34 fail, 14 pass)

**Test Results:** 0/48 passed (0.0%), 48 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_alias_uaf.c` → `test_mem30_c_fail_testcases_alias_uaf`
- ⏭️ NOT RUN `testcases_array_uaf.c` → `test_mem30_c_fail_testcases_array_uaf`
- ⏭️ NOT RUN `testcases_basic_uaf.c` → `test_mem30_c_fail_testcases_basic_uaf`
- ⏭️ NOT RUN `testcases_callback_uaf.c` → `test_mem30_c_fail_testcases_callback_uaf`
- ⏭️ NOT RUN `testcases_cond_uaf.c` → `test_mem30_c_fail_testcases_cond_uaf`
- ⏭️ NOT RUN `testcases_double_free.c` → `test_mem30_c_fail_testcases_double_free`
- ⏭️ NOT RUN `testcases_error_uaf.c` → `test_mem30_c_fail_testcases_error_uaf`
- ⏭️ NOT RUN `testcases_func_uaf.c` → `test_mem30_c_fail_testcases_func_uaf`
- ⏭️ NOT RUN `testcases_global_uaf.c` → `test_mem30_c_fail_testcases_global_uaf`
- ⏭️ NOT RUN `testcases_goto_uaf.c` → `test_mem30_c_fail_testcases_goto_uaf`
- ⏭️ NOT RUN `testcases_list_uaf.c` → `test_mem30_c_fail_testcases_list_uaf`
- ⏭️ NOT RUN `testcases_loop_uaf.c` → `test_mem30_c_fail_testcases_loop_uaf`
- ⏭️ NOT RUN `testcases_macro_uaf.c` → `test_mem30_c_fail_testcases_macro_uaf`
- ⏭️ NOT RUN `testcases_memcpy_uaf.c` → `test_mem30_c_fail_testcases_memcpy_uaf`
- ⏭️ NOT RUN `testcases_nested_uaf.c` → `test_mem30_c_fail_testcases_nested_uaf`
- ⏭️ NOT RUN `testcases_partial_uaf.c` → `test_mem30_c_fail_testcases_partial_uaf`
- ⏭️ NOT RUN `testcases_ptr_arith.c` → `test_mem30_c_fail_testcases_ptr_arith`
- ⏭️ NOT RUN `testcases_realloc_uaf.c` → `test_mem30_c_fail_testcases_realloc_uaf`
- ⏭️ NOT RUN `testcases_recursive.c` → `test_mem30_c_fail_testcases_recursive`
- ⏭️ NOT RUN `testcases_return_uaf.c` → `test_mem30_c_fail_testcases_return_uaf`
- ⏭️ NOT RUN `testcases_setjmp_uaf.c` → `test_mem30_c_fail_testcases_setjmp_uaf`
- ⏭️ NOT RUN `testcases_signal_uaf.c` → `test_mem30_c_fail_testcases_signal_uaf`
- ⏭️ NOT RUN `testcases_static_uaf.c` → `test_mem30_c_fail_testcases_static_uaf`
- ⏭️ NOT RUN `testcases_string_uaf.c` → `test_mem30_c_fail_testcases_string_uaf`
- ⏭️ NOT RUN `testcases_struct_uaf.c` → `test_mem30_c_fail_testcases_struct_uaf`
- ⏭️ NOT RUN `testcases_switch_uaf.c` → `test_mem30_c_fail_testcases_switch_uaf`
- ⏭️ NOT RUN `testcases_thread_uaf.c` → `test_mem30_c_fail_testcases_thread_uaf`
- ⏭️ NOT RUN `testcases_union_uaf.c` → `test_mem30_c_fail_testcases_union_uaf`
- ⏭️ NOT RUN `testcases_vla_uaf.c` → `test_mem30_c_fail_testcases_vla_uaf`
- ⏭️ NOT RUN `testcases_write_uaf.c` → `test_mem30_c_fail_testcases_write_uaf`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_mem30_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_mem30_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_mem30_c_fail_wiki_noncompliant_3`
- ⏭️ NOT RUN `wiki_noncompliant_4.c` → `test_mem30_c_fail_wiki_noncompliant_4`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_safe_array.c` → `test_mem30_c_pass_testcases_safe_array`
- ⏭️ NOT RUN `testcases_safe_basic.c` → `test_mem30_c_pass_testcases_safe_basic`
- ⏭️ NOT RUN `testcases_safe_check.c` → `test_mem30_c_pass_testcases_safe_check`
- ⏭️ NOT RUN `testcases_safe_cond.c` → `test_mem30_c_pass_testcases_safe_cond`
- ⏭️ NOT RUN `testcases_safe_double.c` → `test_mem30_c_pass_testcases_safe_double`
- ⏭️ NOT RUN `testcases_safe_func.c` → `test_mem30_c_pass_testcases_safe_func`
- ⏭️ NOT RUN `testcases_safe_realloc.c` → `test_mem30_c_pass_testcases_safe_realloc`
- ⏭️ NOT RUN `testcases_safe_scope.c` → `test_mem30_c_pass_testcases_safe_scope`
- ⏭️ NOT RUN `testcases_safe_stack.c` → `test_mem30_c_pass_testcases_safe_stack`
- ⏭️ NOT RUN `testcases_safe_struct.c` → `test_mem30_c_pass_testcases_safe_struct`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_mem30_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_mem30_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_mem30_c_pass_wiki_compliant_3`
- ⏭️ NOT RUN `wiki_compliant_4.c` → `test_mem30_c_pass_wiki_compliant_4`

---

### 🔶 MEM35-C - Not Implemented (has tests)

<a id="rule-mem35c"></a>

**Title:** Allocate sufficient memory for an object

**Description:** The types of integer expressions used as size arguments
tomalloc(),calloc(),realloc(), oraligned_alloc()must have sufficient range to
represent the size of the objects to be stored. If size arguments are incorrect
or can be manipulated by an attacker, then a buffer overflow may occur.
Incorrect size arguments, inadequate range checking, integer overflow, or
truncation can result in the allocation of an inadequately sized buffer.
Typically, the amount of memory to allocate will be the size of the type of
object to allocate. When allocating space for an array, the size of the object
will be multiplied by the bounds of the array. When allocating space for a
structure containing a flexible array member, the size of the array member must
be added to the size of the structure. (SeeMEM33-C. Allocate and copy structures
containing a flexible array member dynamically.) Use the correct type of the
object when computing the size of memory to allocate. STR31-C. Guarantee that
storage for strings has sufficient space for character data and the null
terminatoris a specific instance of this rule.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_integer.c` → `test_mem35_c_fail_wiki_integer`
- ⏭️ NOT RUN `wiki_pointer.c` → `test_mem35_c_fail_wiki_pointer`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_integer.c` → `test_mem35_c_pass_wiki_integer`
- ⏭️ NOT RUN `wiki_pointer.c` → `test_mem35_c_pass_wiki_pointer`

---

### 🔶 MEM05-C - Not Implemented (has tests)

<a id="rule-mem05c"></a>

**Title:** Avoid large stack allocations

**Description:** Avoid excessive stack allocations, particularly in situations where the growth
of the stack can be controlled or influenced by an attacker. SeeINT04-C. Enforce
limits on integer values originating from tainted sourcesfor more information on
preventing attacker-controlled integers from exhausting memory. The C Standard
includes support for variable length arrays (VLAs). If the array length is
derived from anuntrusted datasource, an attacker can cause the process to
perform an excessive allocation on the stack. This noncompliant code example
temporarily stores data read from a source file into a buffer. The buffer is
allocated on the stack as a VLA of sizebufsize. Ifbufsizecan be controlled by a
malicious user, this code can beexploitedto cause adenial-of-service attack:

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_mem05_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_mem05_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_mem05_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_mem05_c_pass_wiki_compliant_2`

---

## Category: MSC

<a id="category-msc"></a>

**Implementation Status:** 1 / 8 rules (12.5%)

### 🔶 MSC30-C - Not Implemented (has tests)

<a id="rule-msc30c"></a>

**Title:** Do not use the rand() function for generating pseudorandom numbers

**Description:** Pseudorandom number generators use mathematical algorithms to produce a sequence
of numbers with good statistical properties, but the numbers produced are not
genuinely random. The C Standardrand()function makes no guarantees as to the
quality of the random sequence produced. The numbers generated by some
implementations ofrand()have a comparatively short cycle and the numbers can be
predictable. Applications that have strong pseudorandom number requirements must
use a generator that is known to be sufficient for their needs. The following
noncompliant code generates an ID with a numeric part produced by calling
therand()function. The IDs produced are predictable and have limited randomness.

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_msc30_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_msc30_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_windows.c` → `test_msc30_c_pass_wiki_windows`

---

### 🔶 MSC38-C - Not Implemented (has tests)

<a id="rule-msc38c"></a>

**Title:** Do not treat a predefined identifier as an object if it might only be implemented as a macro

**Description:** The C Standard, 7.1.4 paragraph 1, [ISO/IEC 9899:2024] states However, the C
Standard enumerates specific exceptions in which the behavior of accessing an
object or function expanded to be a standard library macro definition
isundefined. The macros
areassert,errno,math_errhandling,setjmp,va_arg,va_copy,va_end, andva_start.
These cases are described byundefined behaviors138,139,140,141, and143.
Programmers must not suppress these macros to access the underlying object or
function. In this noncompliant code example, the standardassert()macro is
suppressed in an attempt to pass it as a function pointer to
theexecute_handler()function. Attempting to suppress theassert()macro
isundefined behavior.

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_assert.c` → `test_msc38_c_fail_wiki_assert`
- ⏭️ NOT RUN `wiki_redefiningerrno.c` → `test_msc38_c_fail_wiki_redefiningerrno`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_assert.c` → `test_msc38_c_pass_wiki_assert`
- ⏭️ NOT RUN `wiki_declaringerrno.c` → `test_msc38_c_pass_wiki_declaringerrno`

---

### 🔶 MSC41-C - Not Implemented (has tests)

<a id="rule-msc41c"></a>

**Title:** Never hard code sensitive information

**Description:** Hard coding sensitive information, such as passwords or encryption keys can
expose the information to attackers. Anyone who has access to the executable or
dynamic library files can examine them for strings or other critical data,
revealing the sensitive information. Leaking data protected byInternational
Traffic in Arms Regulations(ITAR) or theHealth Insurance Portability and
Accountability Act (HIPAA) can also have legal consequences. Consequently,
programs must not hard code sensitive information. Hard coding sensitive
information also increases the need to manage and accommodate changes to the
code. For example, changing a hard-coded password in a deployed program may
require distribution of a patch[Chess 2007]. This noncompliant code example must
authenticate to a remote service with a code, using theauthenticate()function
declared below. It passes the authentication code to this function as a string
literal.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_hard_coded_database_password.c` → `test_msc41_c_fail_wiki_hard_coded_database_password`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_c23memset_explicit.c` → `test_msc41_c_pass_wiki_c23memset_explicit`

---

### 🔶 MSC33-C - Not Implemented (has tests)

<a id="rule-msc33c"></a>

**Title:** Do not pass invalid data to the asctime() function

**Description:** The C Standard, 7.29.3.1 [ISO/IEC 9899:2024], provides the following sample
implementation of theasctime()function: char *asctime(const struct tm *timeptr)
{ static const char wday_name[7][3] = { "Sun", "Mon", "Tue", "Wed", "Thu",
"Fri", "Sat" }; static const char mon_name[12][3] = { "Jan", "Feb", "Mar",
"Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" }; static char
result[26]; sprintf( result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
wday_name[timeptr->tm_wday], mon_name[timeptr->tm_mon], timeptr->tm_mday,
timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec, 1900 + timeptr->tm_year );
return result; } This function is supposed to output a character string of 26
characters at most, including the terminating null character. If we count the
length indicated by the format directives, we arrive at 25. Taking into account
the terminating null character, the array size of the string appears sufficient.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_msc33_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_strftime.c` → `test_msc33_c_pass_wiki_strftime`

---

### ✅ MSC32-C - Implemented

<a id="rule-msc32c"></a>

**Title:** Properly seed pseudorandom number generators

**Description:** A pseudorandom number generator (PRNG) is a deterministic algorithm capable of
generating sequences of numbers that approximate the properties of random
numbers. Each sequence is completely determined by the initial state of the PRNG
and the algorithm for changing the state. Most PRNGs make it possible to set the
initial state, also called theseed state. Setting the initial state is
calledseedingthe PRNG. Calling a PRNG in the same initial state, either without
seeding it explicitly or by seeding it with the same value, results in
generating the same sequence of random numbers in different runs of the program.
Consider a PRNG function that is seeded with some initial seed value and is
consecutively called to produce a sequence of random numbers,S. If the PRNG is
subsequently seeded with the same initial seed value, then it will generate the
same sequenceS. As a result, after the first run of an improperly seeded PRNG,
an attacker can predict the sequence of random numbers that will be generated in
the future runs. Improperly seeding or failing to seed the PRNG can lead
tovulnerabilities, especially in security protocols.

**Test Coverage:** 6 tests (2 fail, 4 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_msc32_c_fail_wiki_posix`
- ⏭️ NOT RUN `wiki_posix_2.c` → `test_msc32_c_fail_wiki_posix_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_msc32_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_posix_2.c` → `test_msc32_c_pass_wiki_posix_2`
- ⏭️ NOT RUN `wiki_windows.c` → `test_msc32_c_pass_wiki_windows`
- ⏭️ NOT RUN `wiki_windows_2.c` → `test_msc32_c_pass_wiki_windows_2`

---

### 🔶 MSC37-C - Not Implemented (has tests)

<a id="rule-msc37c"></a>

**Title:** Ensure that control never reaches the end of a non-void function

**Description:** If control reaches the closing curly brace (}) of a non-voidfunction without
evaluating areturnstatement, using the return value of the function call
isundefined behavior.(Seeundefined behavior 86.) In this noncompliant code
example, control reaches the end of thecheckpass()function when the two strings
passed tostrcmp()are not equal, resulting in undefined behavior. Many compilers
will generate code for thecheckpass()function, returning various values along
the execution path where noreturnstatement is defined. #include <string.h>
#include <stdio.h> int checkpass(const char *password) { if (strcmp(password,
"pass") == 0) { return 1; } } void func(const char *userinput) { if
(checkpass(userinput)) { printf("Success\n"); } }

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_msc37_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_msc37_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_msc37_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_msc37_c_pass_wiki_compliant_2`

---

### 🔶 MSC39-C - Not Implemented (has tests)

<a id="rule-msc39c"></a>

**Title:** Do not call va_arg() on a va_list that has an indeterminate value

**Description:** Variadic functions access their variable arguments by usingva_start()to
initialize an object of typeva_list, iteratively invoking theva_arg()macro, and
finally callingva_end(). Theva_listmay be passed as an argument to another
function, but callingva_arg()within that function causes theva_listto have
anindeterminate valuein the calling function. As a result, attempting to read
variable arguments without reinitializing theva_listcan haveunexpected behavior.
According to the C Standard, 7.16, paragraph 3 [ISO/IEC 9899:2024], This
noncompliant code example attempts to check that none of its variable arguments
are zero by passing ava_listto helper functioncontains_zero(). After the call
tocontains_zero(), the value ofapisindeterminate. #include <stdarg.h> #include
<stdio.h> int contains_zero(size_t count, va_list ap) { for (size_t i = 1; i <
count; ++i) { if (va_arg(ap, double) == 0.0) { return 1; } } return 0; } int
print_reciprocals(size_t count, ...) { va_list ap; va_start(ap, count); if
(contains_zero(count, ap)) { va_end(ap); return 1; } for (size_t i = 0; i <
count; ++i) { printf("%f ", 1.0 / va_arg(ap, double)); } va_end(ap); return 0; }

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_msc39_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_msc39_c_pass_wiki_compliant_1`

---

### 🔶 MSC40-C - Not Implemented (has tests)

<a id="rule-msc40c"></a>

**Title:** Do not violate constraints

**Description:** According to the C Standard, 3.8 [ISO/IEC 9899:2011], a constraint is a
"restriction, either syntactic or semantic, by which the exposition of language
elements is to be interpreted." Despite the similarity of the terms, a runtime
constraint is not a kind of constraint. Violating anyshallstatement within a
constraint clause in the C Standard requires animplementationto issue a
diagnostic message, the C Standard, 5.1.1.3 [ISO/IEC 9899:2011] states The C
Standard further explains in a footnote

**Test Coverage:** 7 tests (4 fail, 3 pass)

**Test Results:** 0/7 passed (0.0%), 7 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_inline_internal_linkage.c` → `test_msc40_c_fail_wiki_inline_internal_linkage`
- ⏭️ NOT RUN `wiki_inline_modifiable_static.c` → `test_msc40_c_fail_wiki_inline_modifiable_static`
- ⏭️ NOT RUN `wiki_inline_modifiablestatic.c` → `test_msc40_c_fail_wiki_inline_modifiablestatic`
- ⏭️ NOT RUN `wiki_inline_modifiablestatic_2.c` → `test_msc40_c_fail_wiki_inline_modifiablestatic_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_inline_internal_linkage.c` → `test_msc40_c_pass_wiki_inline_internal_linkage`
- ⏭️ NOT RUN `wiki_inline_modifiable_static.c` → `test_msc40_c_pass_wiki_inline_modifiable_static`
- ⏭️ NOT RUN `wiki_inline_modifiablestatic.c` → `test_msc40_c_pass_wiki_inline_modifiablestatic`

---

## Category: POS

<a id="category-pos"></a>

**Implementation Status:** 4 / 20 rules (20.0%)

### 🔶 POS05-C - Not Implemented (has tests)

<a id="rule-pos05c"></a>

**Title:** Limit access to files by creating a jail

**Description:** Creating a jail isolates a program from the rest of the file system. The idea is
to create a sandbox, so entities the program does not need to access under
normal operation are made inaccessible. This makes it much harder to abuse any
vulnerability that can otherwise lead to unconstrained system compromise and
consequently functions as a defense-in-depth strategy. A jail may consist of
world-viewable programs that require fewer resources to execute than those that
exist on that system. Jails are useful only when there is no way to elevate
privileges in the event of program failure. Additionally, care must be taken to
ensure that all the required resources (libraries, files, and so on) are
replicated within the jail directory and that no reference is made to other
parts of the file system from within this directory. It is also advisable to
administer restrictive read/write permissions on the jail directories and
resources on the basis of the program's privilege requirements. Although
creating jails is an effective security measure when used correctly, it is not a
surrogate for compliance with the other rules and recommendations in this
standard. A security flaw exists in this noncompliant code example resulting
from the absence of proper canonicalization measures on the file path. This
allows an attacker to traverse the file system and possibly write to a file of
the attacker's choice with the privileges of the vulnerable program. For
example, it may be possible to overwrite the password file (such as
the/etc/passwd, common to many POSIX-based systems) or a device file, such as
the mouse, which in turn can aid further exploitation or cause a denial of
service to occur.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos05_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_unix.c` → `test_pos05_c_pass_wiki_unix`

---

### 🔶 POS47-C - Not Implemented (has tests)

<a id="rule-pos47c"></a>

**Title:** Do not use threads that can be canceled asynchronously

**Description:** In threading, pthreads can optionally be set to cancel immediately or defer
until a specific cancellation point. Canceling asynchronously (immediately) is
dangerous, however, because most threads are in fact not safe to cancel
immediately. TheIEEE standards pagestates that Canceling asynchronously would
follow the same route as passing a signal into the thread to kill it, posing
problems similar to those inCON37-C. Do not call signal() in a multithreaded
program, which is strongly related toSIG02-C. Avoid using signals to implement
normal functionality. POS44-C and SIG02-C expand on the dangers of canceling a
thread suddenly, which can create adata race condition.

**Test Coverage:** 3 tests (2 fail, 1 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos47_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_pos47_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pos47_c_pass_wiki_compliant_1`

---

### 🔶 POS52-C - Not Implemented (has tests)

<a id="rule-pos52c"></a>

**Title:** Do not perform operations that can block while holding a POSIX lock

**Description:** If a lock is being held and an operation that can block is performed, any other
thread that needs to acquire that lock may also block. This condition can
degrade the performance of a system or cause a deadlock to occur. Blocking calls
include, but are not limited to: network, file, and console I/O. This rule is a
specific instance ofCON05-C. Do not perform operations that can block while
holding a lockusing POSIX threads. This noncompliant code example demonstrates
an occurrence of a blocking call that waits to receive data on a socket while a
mutex is locked. Therecv()call blocks until data arrives on the socket. While it
is blocked, other threads that are waiting for the lock are also blocked.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos52_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_block_while_not_locked.c` → `test_pos52_c_pass_wiki_block_while_not_locked`

---

### 🔶 POS34-C - Not Implemented (has tests)

<a id="rule-pos34c"></a>

**Title:** Do not call putenv() with a pointer to an automatic variable as the argument

**Description:** The POSIX functionputenv()is used to set environment variable values.
Theputenv()function does not create a copy of the string supplied to it as an
argument; rather, it inserts a pointer to the string into the environment array.
If a pointer to a buffer of automatic storage duration is supplied as an
argument toputenv(), the memory allocated for that buffer may be overwritten
when the containing function returns and stack memory is recycled. This behavior
is noted in the Open Group Base Specifications, Issue 6 [Open Group 2004]: The
actual problem occurs when passing apointerto an automatic variable toputenv().
An automatic pointer to a static buffer would work as intended. In this
noncompliant code example, a pointer to a buffer of automatic storage duration
is used as an argument toputenv()[Dowd 2006]. TheTESTenvironment variable may
take on an unintended value if it is accessed afterfunc()has returned and the
stack frame containingenvhas been recycled.

**Test Coverage:** 4 tests (1 fail, 3 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos34_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_heap_memory.c` → `test_pos34_c_pass_wiki_heap_memory`
- ⏭️ NOT RUN `wiki_setenv.c` → `test_pos34_c_pass_wiki_setenv`
- ⏭️ NOT RUN `wiki_static.c` → `test_pos34_c_pass_wiki_static`

---

### 🔶 POS01-C - Not Implemented (has tests)

<a id="rule-pos01c"></a>

**Title:** Check for the existence of links when dealing with files

**Description:** Many common operating systems such as Windows and UNIX support file links,
including hard links, symbolic (soft) links, and virtual drives. Hard links can
be created in UNIX with thelncommand or in Windows operating systems by calling
theCreateHardLink()function. Symbolic links can be created in UNIX using theln
-scommand or in Windows by using directory junctions in NTFS or the Linkd.exe
(Win 2K resource kit) or "junction" freeware. Virtual drives can also be created
in Windows using thesubstcommand. File links can create security issues for
programs that fail to consider the possibility that the file being opened may
actually be a link to a different file. This is especially dangerous when the
vulnerable program is running with elevated privileges. Frequently, there is no
need to check for the existence of symbolic links because this problem can be
solved using other techniques. When opening an existing file, for example, the
simplest solution is often to drop privileges to the privileges of the user.
This solution permits the use of links while preventing access to files for
which the user of the application is not privileged.

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos01_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_linux_21126_freebsd_solaris_10_posix1_2008o_nofollow.c` → `test_pos01_c_pass_wiki_linux_21126_freebsd_solaris_10_posix1_2008o_nofollow`
- ⏭️ NOT RUN `wiki_lstat_fopen_fstat.c` → `test_pos01_c_pass_wiki_lstat_fopen_fstat`

---

### 🔶 POS04-C - Not Implemented (has tests)

<a id="rule-pos04c"></a>

**Title:** Avoid using PTHREAD_MUTEX_NORMAL type mutex locks

**Description:** Pthread mutual exclusion (mutex) locks are used to avoid simultaneous usage of
common resources. Several types of mutex locks are defined by
pthreads:NORMAL,ERRORCHECK,RECURSIVE, andDEFAULT. POSIX
describesPTHREAD_MUTEX_NORMALlocks as having the following undefined behavior
[Open Group 2004]: TheDEFAULTmutex pthread is also generally mapped
toPTHREAD_MUTEX_NORMALbut is known to vary from platform to platform [SOL 2010].
Consequently,NORMALlocks should not be used, andERRORCHECKorRECURSIVElocks
should be defined explicitly when mutex locks are used.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos04_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pos04_c_pass_wiki_compliant_1`

---

### 🔶 POS38-C - Not Implemented (has tests)

<a id="rule-pos38c"></a>

**Title:** Beware of race conditions when using fork and file descriptors

**Description:** When forking a child process, file descriptors are copied to the child process,
which can result in concurrent operations on the file. Concurrent operations on
the same file can cause data to be read or written in a nondeterministic order,
creating race conditions and unpredictable behavior. In this example, the
programmer wishes to open a file, read a character, fork, and then have both
parent and child process read the second character of the file independently.
However, because both processes share a file descriptor, one process might get
the second character, and one might get the third. Furthermore, there is no
guarantee the reads are atomic—the processes might get unpredictable results.
Regardless of what the programmer is trying to accomplish with this code, this
code is incorrect because it contains a race condition. char c; pid_t pid; int
fd = open(filename, O_RDWR); if (fd == -1) { /* Handle error */ } read(fd, &c,
1); printf("root process:%c\n",c); pid = fork(); if (pid == -1) { /* Handle
error */ } if (pid == 0) { /*child*/ read(fd, &c, 1); printf("child:%c\n",c); }
else { /*parent*/ read(fd, &c, 1); printf("parent:%c\n",c); }

**Test Coverage:** 5 tests (3 fail, 2 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos38_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_pos38_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3_3.c` → `test_pos38_c_fail_wiki_noncompliant_3_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pos38_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2_2.c` → `test_pos38_c_pass_wiki_compliant_2_2`

---

### ✅ POS37-C - Implemented

<a id="rule-pos37c"></a>

**Title:** Ensure that privilege relinquishment is successful

**Description:** The POSIXsetuid()function has complex semantics and platform-specific behavior
[Open Group 2004]. The meaning of "appropriate privileges" varies from platform
to platform. For example, on Solaris, appropriate privileges forsetuid()means
that thePRIV_PROC_SETIDprivilege is in the effective privilege set of the
process. On BSD, it means that the effective user ID (EUID) is zero (that is,
the process is running as root) or thatuid=geteuid(). On Linux, it means that
the process hasCAP_SETUIDcapability and thatsetuid(geteuid())will fail if the
EUID is not equal to 0, the real user ID (RUID), or the saved set-user ID
(SSUID). Because of this complex behavior, desired privilege drops sometimes may
fail. For example, the range of Linux Kernel versions (2.2.0–2.2.15) is
vulnerable to an insufficient privilege attack whereinsetuid(getuid()did not
drop privileges as expected when the capability bits were set to zero. As a
precautionary measure, subtle behavior and error conditions for the targeted
implementation must be carefully noted.

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos37_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pos37_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_pos37_c_pass_wiki_compliant_2`

---

### 🔶 POS44-C - Not Implemented (has tests)

<a id="rule-pos44c"></a>

**Title:** Do not use signals to terminate threads

**Description:** Do not send an uncaught signal to kill a thread because the signal kills the
entire process, not just the individual thread. This rule is a specific instance
ofSIG02-C. Avoid using signals to implement normal functionality.In POSIX
systems, using thesignal()function in a multithreaded program falls under
exceptionCON37C-C-EX0of ruleCON37-C. Do not call signal() in a multithreaded
program.Noncompliant Code ExampleThis code uses thepthread_kill()function to
send aSIGTERMsignal to the created thread. The thread receives the signal, and
the entire process is terminated.void func(void *foo) { /* Execution of thread
*/ } int main(void) { int result; pthread_t thread; if ((result =
pthread_create(&thread, NULL, func, 0)) != 0) { /* Handle Error */ } if ((result
= pthread_kill(thread, SIGTERM)) != 0) { /* Handle Error */ } /* This point is
not reached because the process terminates in pthread_kill() */ return 0;
}Compliant SolutionThis compliant code uses instead thepthread_cancel()function
to terminate the thread. The thread continues to run until it reaches a
cancellation point. SeeThe Open Group Base Specifications Issue 6, IEEE Std
1003.1, 2004 Edition[Open Group 2004] for lists of functions that are required
and allowed to be cancellation points. If the cancellation type is set to
asynchronous, the thread is terminated immediately. However, POSIX requires only
thepthread_cancel(),pthread_setcancelstate(),
andpthread_setcanceltype()functions to be async-cancel safe. An application that
calls other POSIX functions with asynchronous cancellation enabled is
nonconforming. Consequently, we recommend disallowing asynchronous cancellation,
as explained byPOS47-C. Do not use threads that can be canceled
asynchronously.void func(void *foo) { /* Execution of thread */ } int main(void)
{ int result; pthread_t thread; if ((result = pthread_create(&thread, NULL,
func, 0)) != 0) { /* Handle Error */ } if ((result = pthread_cancel(thread)) !=
0) { /* Handle Error */ } /* Continue executing */ return 0; }Risk
AssessmentSending the signal to a process causes it to beabnormally terminated.R
uleSeverityLikelihoodDetectableRepairablePriorityLevelPOS44-
CLowProbableNoNoP2L3Automated DetectionToolVersionCheckerDescriptionCodeSonar9.1
p0CONCURRENCY.BADFUNC.PTHREAD_KILLUse of pthread_killHelix
QAC2025.2C5034Klocwork2025.2MISRA.INCL.SIGNAL.2012Parasoft
C/C++test2024.2CERT_C-POS44-aThe 'pthread_kill', 'pthread_sigqueue' and 'tgkill'
functions should not be used to send signals to threadsPC-lint Plus1.4586Fully
supportedPolyspace Bug FinderR2025bCERT C: Rule POS44-CChecks for use of signal
to kill thread (rule fully covered)Related VulnerabilitiesSearch for
vulnerabilities resulting from the violation of this rule on theCERT
website.Bibliography[OpenBSD]signal()Man Page[MKS]pthread_cancel()Man Page[Open
Group 2004]Threads Overview

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos44_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pos44_c_pass_wiki_compliant_1`

---

### 🔶 POS51-C - Not Implemented (has tests)

<a id="rule-pos51c"></a>

**Title:** Avoid deadlock with POSIX threads by locking in predefined order

**Description:** Mutexes are often used to prevent multiple threads from accessing critical
resources at the same time. Sometimes, when locking mutexes, multiple threads
hold each other's lock, and the program consequently deadlocks. There are four
requirements for deadlock: Deadlock requires all four conditions, so to prevent
deadlock, prevent any one of the four conditions. This guideline recommends
locking the mutexes in a predefined order to prevent circular wait. This rule is
a specific instance ofCON35-C. Avoid deadlock by locking in predefined
orderusing POSIX threads. This noncompliant code example has behavior that
depends on the runtime environment and the platform's scheduler. However, with
proper timing, themain()function will deadlock when runningthr1andthr2,
wherethr1tries to lockba2's mutex, whilethr2tries to lock onba1's mutex in
thedeposit()function, and the program will not progress.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos51_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pos51_c_pass_wiki_compliant_1`

---

### 🔶 POS02-C - Not Implemented (has tests)

<a id="rule-pos02c"></a>

**Title:** Follow the principle of least privilege

**Description:** The principle of least privilege states that every program and every user of the
system should operate using the least set of privileges necessary to complete
the job [Saltzer 1974,Saltzer 1975]. The Build Security In website [DHS 2006]
provides additional definitions of this principle. Executing with minimal
privileges mitigates against exploitation in case a vulnerability is discovered
in the code. Privileged operations are often required in a program, though the
program might not need to retain the special privileges. For instance, a network
program may require superuser privileges to capture raw network packets but may
not require the same set of privileges for carrying out other tasks such as
packet analysis. Dropping or elevating privileges alternately according to
program requirements is a good design strategy. Moreover, assigning only the
required privileges limits the window of exposure for any privilege escalation
exploit to succeed. Consider a custom service that must bind to a well-known
port (below 1024). To prevent malicious entities from hijacking client
connections, the kernel imposes a condition so that only the superuser can use
thebind()system call to bind to these ports.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos02_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pos02_c_pass_wiki_compliant_1`

---

### 🔶 POS48-C - Not Implemented (has tests)

<a id="rule-pos48c"></a>

**Title:** Do not unlock or destroy another POSIX thread's mutex

**Description:** Mutexes are used to protect shared data structures being accessed concurrently.
The thread that locks the mutex owns it, and the owning thread should be the
only thread to unlock the mutex. If the mutex is destroyed while still in use,
critical sections and shared data are no longer protected. This rule is a
specific instance ofCON31-C. Do not unlock or destroy another thread's
mutexusing POSIX threads. In this noncompliant code example, a race condition
exists between a cleanup and a worker thread. The cleanup thread destroys the
lock, which it believes is no longer in use. If there is a heavy load on the
system, the worker thread that held the lock can take longer than expected. If
the lock is destroyed before the worker thread has completed modifying the
shared data, the program may exhibit unexpected behavior. pthread_mutex_t
theLock; int data; int cleanupAndFinish(void) { int result; if ((result =
pthread_mutex_destroy(&theLock)) != 0) { /* Handle error */ } data++; return
data; } void worker(int value) { if ((result = pthread_mutex_lock(&theLock)) !=
0) { /* Handle error */ } data += value; if ((result =
pthread_mutex_unlock(&theLock)) != 0) { /* Handle error */ } }

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos48_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pos48_c_pass_wiki_compliant_1`

---

### ✅ POS36-C - Implemented

<a id="rule-pos36c"></a>

**Title:** Observe correct revocation order while relinquishing privileges

**Description:** In case of set-user-ID and set-group-ID programs, when the effective user ID and
group ID are different from those of the real user, it is important to drop not
only the user-level privileges but also the group privileges. While doing so,
the order of revocation must be correct. POSIX definessetgid()to have the
following behavior [Open Group 2004]: This noncompliant code example drops
privileges to those of the real user and similarly drops the group privileges.
However, the order is incorrect because thesetgid()function must be run with
superuser privileges, but the call tosetuid()leaves the effective user ID as
nonzero. As a result, if a vulnerability is discovered in the program that
allows for the execution of arbitrary code, an attacker can regain the original
group privileges.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos36_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pos36_c_pass_wiki_compliant_1`

---

### 🔶 POS35-C - Not Implemented (has tests)

<a id="rule-pos35c"></a>

**Title:** Avoid race conditions while checking for the existence of a symbolic link

**Description:** Many common operating systems, such as Windows and UNIX, support symbolic (soft)
links. Symbolic links can be created in UNIX using theln -scommand or in Windows
by using directory junctions in NTFS or the Linkd.exe (Win 2K resource kit) or
"junction" freeware. If not properly performed, checking for the existence of
symbolic links can lead to race conditions. This rule is a specific instance of
ruleFIO45-C. Avoid TOCTOU race conditions while accessing files.

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos35_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posix1_2001_or_older.c` → `test_pos35_c_pass_wiki_posix1_2001_or_older`
- ⏭️ NOT RUN `wiki_posix1_2008_or_newer.c` → `test_pos35_c_pass_wiki_posix1_2008_or_newer`

---

### ✅ POS54-C - Implemented

<a id="rule-pos54c"></a>

**Title:** Detect and handle POSIX library errors

**Description:** All standard library functions, including I/O functions and memory allocation
functions, return either a valid value or a value of the correct return type
that indicates an error (for example, −1 or a null pointer). Assuming that all
calls to such functions will succeed and failing to check the return value for
an indication of an error is a dangerous practice that may lead
tounexpectedorundefined behaviorwhen an error occurs. It is essential that
programs detect and appropriately handle all errors in accordance with an error-
handling policy, as discussed inERR00-C. Adopt and implement a consistent and
comprehensive error-handling policy. In addition to the C standard library
functions mentioned inERR33-C. Detect and handle standard library errors, the
following functions defined in POSIX require error checking (list is not all-
inclusive). The successful completion or failure of each of the standard library
functions listed in the following table shall be determined either by comparing
the function’s return value with the value listed in the column labeled “Error
Return” or by calling one of the library functions mentioned in the footnotes to
the same column. FunctionSuccessful ReturnError Returnerrnofmemopen()Pointer to
aFILEobjectNULLENOMEMopen_memstream()Pointer to
aFILEobjectNULLENOMEMposix_memalign()0NonzeroUnchanged

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_pos54_c_fail_wiki_posix`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_posix.c` → `test_pos54_c_pass_wiki_posix`

---

### 🔶 POS50-C - Not Implemented (has tests)

<a id="rule-pos50c"></a>

**Title:** Declare objects shared between POSIX threads with appropriate storage durations

**Description:** Accessing the stack or thread-local variables of a thread from another thread
can cause invalid memory accesses because the execution of threads can be
interwoven within the constraints of the synchronization model. As a result, the
referenced stack frame or thread-local variable may not be valid when the other
thread tries to access it. Regular shared variables should be protected by
thread synchronization mechanisms. However, local variables should not be shared
in the same fashion because the referenced stack frame's thread would have to
stop executing, or some other way must be found to ensure that the referenced
stack frame is still valid. SeeCON32-C. Prevent data races when accessing bit-
fields from multiple threadsfor information on how to securely share
nonautomatic and non-thread-local variables. SeeDCL30-C. Declare objects with
appropriate storage durationsfor information on how to declare objects with
appropriate storage durations when data is not being shared between threads.
Note that this is a specific instance ofCON34-C. Declare objects shared between
threads with appropriate storage durationsfor POSIX threads. It is important to
note that local data can be used securely with threads when using other non-
POSIX thread interfaces, so the programmer should not always copy data into
nonlocal memory when sharing data with threads. For example, thesharedkeyword
inOpenMPcan be used in combination with OpenMP's threading interface to share
local memory without having to worry about whether local automatic variables
remain valid. Furthermore, copying the shared data into dynamic memory may
completely negate the performance benefits of multithreading.
ThecreateThread()function allocates an integer on the stack and passes a void
pointer, spawning off a new thread,childThread(). The order of thread execution
is interleaved, sovalcan reference an object outside of its lifetime, causing
the child thread to access an invalid memory location.

**Test Coverage:** 6 tests (2 fail, 4 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_automatic_storage.c` → `test_pos50_c_fail_wiki_automatic_storage`
- ⏭️ NOT RUN `wiki_thread_local_storage.c` → `test_pos50_c_fail_wiki_thread_local_storage`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_allocated_storage.c` → `test_pos50_c_pass_wiki_allocated_storage`
- ⏭️ NOT RUN `wiki_automatic_storage.c` → `test_pos50_c_pass_wiki_automatic_storage`
- ⏭️ NOT RUN `wiki_static_storage.c` → `test_pos50_c_pass_wiki_static_storage`
- ⏭️ NOT RUN `wiki_thread_local_storage.c` → `test_pos50_c_pass_wiki_thread_local_storage`

---

### 🔶 POS39-C - Not Implemented (has tests)

<a id="rule-pos39c"></a>

**Title:** Use the correct byte ordering when transferring data between systems

**Description:** Different system architectures use different byte ordering, either little endian
(least significant byte first) or big endian (most significant byte first).
IA-32 is an example of an architecture that implements little endian byte
ordering. In contrast, PowerPC and most Network Protocols (including TCP and IP)
use big endian. When transferring data between systems of different endianness,
the programmer must take care to reverse the byte ordering before interpreting
the data. The functionshtonl(),htons(),ntohl(), andntohs()can be used to
transfer between network byte ordering (big endian) and the host's byte
ordering. On big endian systems, these functions do nothing. They may also be
implemented as macros rather than functions.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos39_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pos39_c_pass_wiki_compliant_1`

---

### 🔶 POS53-C - Not Implemented (has tests)

<a id="rule-pos53c"></a>

**Title:** Do not use more than one mutex for concurrent waiting operations on a condition variable

**Description:** pthread_cond_wait()andpthread_cond_timedwait()take a condition variable and
locked mutex as arguments. These functions unlock the mutex until the condition
variable is signaled and then relock the mutex before returning. While a thread
is waiting on a particular condition variable and mutex, other threads may only
wait on the same condition variable if they also pass the same mutex as an
argument. This requirement is noted in theOpen Group Base Specifications, Issue
6: It also specifies thatpthread_cond_wait()may€ fail if: In this noncompliant
code example,mutex1protectscount1andmutex2protectscount2. Arace conditionexists
between thewaiter1andwaiter2threads because they use the same condition variable
with different mutexes. If both threads attempt to callpthread_cond_wait()at the
same time, one thread will succeed and the other thread will invokeundefined
behavior.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos53_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pos53_c_pass_wiki_compliant_1`

---

### ✅ POS30-C - Implemented

<a id="rule-pos30c"></a>

**Title:** Use the readlink() function properly

**Description:** Thereadlink()function reads where a link points to. It makesnoeffort to null-
terminate its second argument,buffer. Instead, it just returns the number of
characters it has written. Iflenis equal tosizeof(buf), the null terminator is
written 1 byte past the end ofbuf: char buf[1024]; ssize_t len =
readlink("/usr/bin/perl", buf, sizeof(buf)); buf[len] = '\0';

**Test Coverage:** 3 tests (2 fail, 1 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pos30_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_pos30_c_fail_wiki_noncompliant_2_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pos30_c_pass_wiki_compliant_1`

---

### 🔶 POS49-C - Not Implemented (has tests)

<a id="rule-pos49c"></a>

**Title:** When data must be accessed by multiple threads, provide a mutex and guarantee no adjacent data is also accessed

**Description:** When multiple threads must access or make modifications to a common variable,
they may also inadvertently access other variables adjacent in memory. This is
an artifact of variables being stored compactly, with one byte possibly holding
multiple variables, and is a common optimization on word-addressed machines.
Bit-fields are especially prone to this behavior because compliers are allowed
to store multiple bit-fields in one addressable byte or word. This implies that
race conditions may exist not just on a variable accessed by multiple threads
but also on other variables sharing the same byte or word address. This
recommendation is a specific instance ofCON32-C. Prevent data races when
accessing bit-fields from multiple threadsusing POSIX threads. A common tool for
preventing race conditions in concurrent programming is the mutex. When properly
observed by all threads, a mutex can provide safe and secure access to a common
variable; however, it guarantees nothing with regard to other variables that
might be accessed when a common variable is accessed. Unfortunately, there is no
portable way to determine which adjacent variables may be stored along with a
certain variable.

**Test Coverage:** 3 tests (2 fail, 1 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_bit_field.c` → `test_pos49_c_fail_wiki_bit_field`
- ⏭️ NOT RUN `wiki_bit_field_2.c` → `test_pos49_c_fail_wiki_bit_field_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_bit_field.c` → `test_pos49_c_pass_wiki_bit_field`

---

## Category: PRE

<a id="category-pre"></a>

**Implementation Status:** 4 / 16 rules (25.0%)

### ✅ PRE30-C - Implemented

<a id="rule-pre30c"></a>

**Title:** Do not create a universal character name through concatenation

**Description:** The C Standard supports universal character names that may be used in
identifiers, character constants, and string literals to designate characters
that are not in the basic character set. The universal character
name\Unnnnnnnndesignates the character whose 8-digit short identifier (as
specified by ISO/IEC 10646) isnnnnnnnn. Similarly, the universal character
name\unnnndesignates the character whose 4-digit short identifier isnnnn(and
whose 8-digit short identifier is0000nnnn). The C Standard, 5.1.1.2, paragraph 4
[ISO/IEC 9899:2024], says See alsoundefined behavior 3.

**Test Coverage:** 42 tests (31 fail, 11 pass)

**Test Results:** 0/42 passed (0.0%), 42 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_address_of.c` → `test_pre30_c_fail_testcases_address_of`
- ⏭️ NOT RUN `testcases_array_access.c` → `test_pre30_c_fail_testcases_array_access`
- ⏭️ NOT RUN `testcases_array_init.c` → `test_pre30_c_fail_testcases_array_init`
- ⏭️ NOT RUN `testcases_basic_concat.c` → `test_pre30_c_fail_testcases_basic_concat`
- ⏭️ NOT RUN `testcases_bitwise_op.c` → `test_pre30_c_fail_testcases_bitwise_op`
- ⏭️ NOT RUN `testcases_cast_operation.c` → `test_pre30_c_fail_testcases_cast_operation`
- ⏭️ NOT RUN `testcases_comma_expr.c` → `test_pre30_c_fail_testcases_comma_expr`
- ⏭️ NOT RUN `testcases_comparison.c` → `test_pre30_c_fail_testcases_comparison`
- ⏭️ NOT RUN `testcases_compound_assign.c` → `test_pre30_c_fail_testcases_compound_assign`
- ⏭️ NOT RUN `testcases_conditional_expr.c` → `test_pre30_c_fail_testcases_conditional_expr`
- ⏭️ NOT RUN `testcases_do_while.c` → `test_pre30_c_fail_testcases_do_while`
- ⏭️ NOT RUN `testcases_for_loop.c` → `test_pre30_c_fail_testcases_for_loop`
- ⏭️ NOT RUN `testcases_function_call.c` → `test_pre30_c_fail_testcases_function_call`
- ⏭️ NOT RUN `testcases_increment_op.c` → `test_pre30_c_fail_testcases_increment_op`
- ⏭️ NOT RUN `testcases_label_name.c` → `test_pre30_c_fail_testcases_label_name`
- ⏭️ NOT RUN `testcases_logical_and.c` → `test_pre30_c_fail_testcases_logical_and`
- ⏭️ NOT RUN `testcases_long_form_ucn.c` → `test_pre30_c_fail_testcases_long_form_ucn`
- ⏭️ NOT RUN `testcases_multi_declaration.c` → `test_pre30_c_fail_testcases_multi_declaration`
- ⏭️ NOT RUN `testcases_nested_macro.c` → `test_pre30_c_fail_testcases_nested_macro`
- ⏭️ NOT RUN `testcases_pointer_deref.c` → `test_pre30_c_fail_testcases_pointer_deref`
- ⏭️ NOT RUN `testcases_return_value.c` → `test_pre30_c_fail_testcases_return_value`
- ⏭️ NOT RUN `testcases_short_form_ucn.c` → `test_pre30_c_fail_testcases_short_form_ucn`
- ⏭️ NOT RUN `testcases_sizeof_op.c` → `test_pre30_c_fail_testcases_sizeof_op`
- ⏭️ NOT RUN `testcases_struct_member.c` → `test_pre30_c_fail_testcases_struct_member`
- ⏭️ NOT RUN `testcases_switch_case.c` → `test_pre30_c_fail_testcases_switch_case`
- ⏭️ NOT RUN `testcases_three_part_concat.c` → `test_pre30_c_fail_testcases_three_part_concat`
- ⏭️ NOT RUN `testcases_typedef_name.c` → `test_pre30_c_fail_testcases_typedef_name`
- ⏭️ NOT RUN `testcases_ucn_in_expression.c` → `test_pre30_c_fail_testcases_ucn_in_expression`
- ⏭️ NOT RUN `testcases_variable_assignment.c` → `test_pre30_c_fail_testcases_variable_assignment`
- ⏭️ NOT RUN `testcases_while_condition.c` → `test_pre30_c_fail_testcases_while_condition`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pre30_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_complete_ucn_arg.c` → `test_pre30_c_pass_testcases_complete_ucn_arg`
- ⏭️ NOT RUN `testcases_direct_ucn_usage.c` → `test_pre30_c_pass_testcases_direct_ucn_usage`
- ⏭️ NOT RUN `testcases_multiple_ucn_args.c` → `test_pre30_c_pass_testcases_multiple_ucn_args`
- ⏭️ NOT RUN `testcases_normal_concat.c` → `test_pre30_c_pass_testcases_normal_concat`
- ⏭️ NOT RUN `testcases_ucn_array_name.c` → `test_pre30_c_pass_testcases_ucn_array_name`
- ⏭️ NOT RUN `testcases_ucn_function_param.c` → `test_pre30_c_pass_testcases_ucn_function_param`
- ⏭️ NOT RUN `testcases_ucn_in_macro_body.c` → `test_pre30_c_pass_testcases_ucn_in_macro_body`
- ⏭️ NOT RUN `testcases_ucn_long_form.c` → `test_pre30_c_pass_testcases_ucn_long_form`
- ⏭️ NOT RUN `testcases_ucn_struct_member.c` → `test_pre30_c_pass_testcases_ucn_struct_member`
- ⏭️ NOT RUN `testcases_ucn_typedef.c` → `test_pre30_c_pass_testcases_ucn_typedef`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre30_c_pass_wiki_compliant_1`

---

### 🔶 PRE12-C - Not Implemented (has tests)

<a id="rule-pre12c"></a>

**Title:** Do not define unsafe macros

**Description:** Anunsafe function-like macrois one that, when expanded, evaluates its argument
more than once or does not evaluate it at all. Contrasted with function calls,
which always evaluate each of their arguments exactly once, unsafe function-like
macros often have unexpected and surprising effects and lead to subtle, hard-to-
find defects (seePRE31-C. Avoid side effects in arguments to unsafe macros).
Consequently, everyfunction-like macroshould evaluate each of its arguments
exactly once. Alternatively and preferably, defining function-like macros should
be avoided in favor of inline functions (seePRE00-C. Prefer inline or static
functions to function-like macros). The most severe problem withunsafe function-
like macrosis side effects of macro arguments, as shown in this noncompliant
code example: #define ABS(x) (((x) < 0) ? -(x) : (x)) void f(int n) { int m; m =
ABS(++n); /* ... */ }

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_multiple_argument_evaluation.c` → `test_pre12_c_fail_wiki_multiple_argument_evaluation`
- ⏭️ NOT RUN `wiki_multiple_argument_evaluation_2.c` → `test_pre12_c_fail_wiki_multiple_argument_evaluation_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_inline_function.c` → `test_pre12_c_pass_wiki_inline_function`
- ⏭️ NOT RUN `wiki_language_extension.c` → `test_pre12_c_pass_wiki_language_extension`

---

### 🔶 PRE11-C - Not Implemented (has tests)

<a id="rule-pre11c"></a>

**Title:** Do not conclude macro definitions with a semicolon

**Description:** Macros are frequently used to make source code more readable. Macro definitions,
regardless of whether they expand to a single or multiple statements, should not
conclude with a semicolon. (SeePRE10-C. Wrap multistatement macros in a do-while
loop.) If required, the semicolon should be included following the macro
expansion. Inadvertently inserting a semicolon at the end of the macro
definition can unexpectedly change the control flow of the program. Another way
to avoid this problem is to prefer inline or static functions over function-like
macros. (See alsoPRE00-C. Prefer inline or static functions to function-like
macros.) In general, the programmer should ensure that there is no semicolon at
the end of a macro definition. The responsibility for having a semicolon where
needed during the use of such a macro should be delegated to the person invoking
the macro.

**Test Coverage:** 6 tests (3 fail, 3 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pre11_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_pre11_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_pre11_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre11_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_pre11_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_pre11_c_pass_wiki_compliant_3`

---

### 🔶 PRE06-C - Not Implemented (has tests)

<a id="rule-pre06c"></a>

**Title:** Enclose header files in an include guard

**Description:** Until the early 1980s, large software development projects had a continual
problem with the inclusion of headers. One group might have produced
agraphics.h, for example, which started by includingio.h. Another group might
have producedkeyboard.h, which also includedio.h. Ifio.hcould not safely be
included several times, arguments would break out about which header should
include it. Sometimes an agreement was reached that each header should include
no other headers, and as a result, some application programs started with dozens
of#includelines, and sometimes they got the ordering wrong or forgot a required
header. All these complications disappeared with the discovery of a simple
technique: each header should#definea symbol that means "I have already been
included." The entire header is then enclosed in an include guard: #ifndef
HEADER_H #define HEADER_H /* ... Contents of <header.h> ... */ #endif /*
HEADER_H */

**Test Coverage:** 1 tests (0 fail, 1 pass)

**Test Results:** 0/1 passed (0.0%), 1 not run

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre06_c_pass_wiki_compliant_1`

---

### 🔶 PRE10-C - Not Implemented (has tests)

<a id="rule-pre10c"></a>

**Title:** Wrap multistatement macros in a do-while loop

**Description:** Macros are often used to execute a sequence of multiple statements as a group.
Inline functions are, in general, more suitable for this task (seePRE00-C.
Prefer inline or static functions to function-like macros). Occasionally,
however, they are not feasible (when macros are expected to operate on variables
of different types, for example). When multiple statements are used in a macro,
they should be bound together in ado-whileloop syntactically, so the macro can
appear safely insideifclauses or other places that expect a single statement or
a statement block. Note that this is only effective if none of the multiple
statements arebreakorcontinue, as they would be captured by thedo-whileloop.
(Alternatively, when anif,for, orwhilestatement uses braces even for a single
body statement, then multiple statements in a macro will expand correctly even
without ado-whileloop (seeEXP19-C. Use braces for the body of an if, for, or
while statement).

**Test Coverage:** 7 tests (6 fail, 1 pass)

**Test Results:** 0/7 passed (0.0%), 7 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pre10_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_pre10_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3_3.c` → `test_pre10_c_fail_wiki_noncompliant_3_3`
- ⏭️ NOT RUN `wiki_noncompliant_4.c` → `test_pre10_c_fail_wiki_noncompliant_4`
- ⏭️ NOT RUN `wiki_noncompliant_5_2.c` → `test_pre10_c_fail_wiki_noncompliant_5_2`
- ⏭️ NOT RUN `wiki_noncompliant_6_3.c` → `test_pre10_c_fail_wiki_noncompliant_6_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre10_c_pass_wiki_compliant_1`

---

### 🔶 PRE02-C - Not Implemented (has tests)

<a id="rule-pre02c"></a>

**Title:** Macro replacement lists should be parenthesized

**Description:** Macro replacement lists should be parenthesized to protect any lower-precedence
operators from the surrounding expression. See alsoPRE00-C. Prefer inline or
static functions to function-like macrosandPRE01-C. Use parentheses within
macros around parameter names. ThisCUBE()macro definition is noncompliant
because it fails to parenthesize the replacement list: #define CUBE(X) (X) * (X)
* (X) int i = 3; int a = 81 / CUBE(i);

**Test Coverage:** 8 tests (6 fail, 2 pass)

**Test Results:** 0/8 passed (0.0%), 8 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pre02_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_pre02_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3_3.c` → `test_pre02_c_fail_wiki_noncompliant_3_3`
- ⏭️ NOT RUN `wiki_noncompliant_4_4.c` → `test_pre02_c_fail_wiki_noncompliant_4_4`
- ⏭️ NOT RUN `wiki_noncompliant_5.c` → `test_pre02_c_fail_wiki_noncompliant_5`
- ⏭️ NOT RUN `wiki_noncompliant_6_2.c` → `test_pre02_c_fail_wiki_noncompliant_6_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre02_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_pre02_c_pass_wiki_compliant_2`

---

### 🔶 PRE07-C - Not Implemented (has tests)

<a id="rule-pre07c"></a>

**Title:** Avoid using repeated question marks

**Description:** Two consecutive question marks signify the start of a trigraph sequence.
According to the C Standard, subclause 5.2.1.1 [ISO/IEC 9899:2011], In this
noncompliant code example,a++is not executed because the trigraph sequence??/is
replaced by\, logically puttinga++on the same line as the comment: // What is
the value of a now??/ a++;

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pre07_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_pre07_c_fail_wiki_noncompliant_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre07_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_pre07_c_pass_wiki_compliant_2`

---

### 🔶 PRE01-C - Not Implemented (has tests)

<a id="rule-pre01c"></a>

**Title:** Use parentheses within macros around parameter names

**Description:** Parenthesize all parameter names in macro definitions. See alsoPRE00-C. Prefer
inline or static functions to function-like macrosandPRE02-C. Macro replacement
lists should be parenthesized. ThisCUBE()macro definition is noncompliant
because it fails to parenthesize the parameter names: #define CUBE(I) (I * I *
I)

**Test Coverage:** 4 tests (3 fail, 1 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pre01_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_pre01_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3_3.c` → `test_pre01_c_fail_wiki_noncompliant_3_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre01_c_pass_wiki_compliant_1`

---

### ✅ PRE31-C - Implemented

<a id="rule-pre31c"></a>

**Title:** Avoid side effects in arguments to unsafe macros

**Description:** Anunsafe function-like macrois one whose expansion results in evaluating one of
its parameters more than once or not at all. Never invoke an unsafe macro with
arguments containing an assignment, increment, decrement, volatile access,
input/output, or other expressions with side effects (including function calls,
which may cause side effects). The documentation for unsafe macros should warn
against invoking them with arguments with side effects, but the responsibility
is on the programmer using the macro. Because of the risks associated with their
use, it is recommended that the creation of unsafe function-like macros be
avoided. (SeePRE00-C. Prefer inline or static functions to function-like
macros.) This rule is similar toEXP44-C. Do not rely on side effects in operands
to sizeof, _Alignof, or _Generic.

**Test Coverage:** 48 tests (33 fail, 15 pass)

**Test Results:** 0/48 passed (0.0%), 48 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_abs_decrement.c` → `test_pre31_c_fail_testcases_abs_decrement`
- ⏭️ NOT RUN `testcases_abs_increment.c` → `test_pre31_c_fail_testcases_abs_increment`
- ⏭️ NOT RUN `testcases_array_access_increment.c` → `test_pre31_c_fail_testcases_array_access_increment`
- ⏭️ NOT RUN `testcases_assert_function_call.c` → `test_pre31_c_fail_testcases_assert_function_call`
- ⏭️ NOT RUN `testcases_assert_increment.c` → `test_pre31_c_fail_testcases_assert_increment`
- ⏭️ NOT RUN `testcases_assignment_in_macro.c` → `test_pre31_c_fail_testcases_assignment_in_macro`
- ⏭️ NOT RUN `testcases_bitwise_with_increment.c` → `test_pre31_c_fail_testcases_bitwise_with_increment`
- ⏭️ NOT RUN `testcases_cast_with_increment.c` → `test_pre31_c_fail_testcases_cast_with_increment`
- ⏭️ NOT RUN `testcases_clamp_increment.c` → `test_pre31_c_fail_testcases_clamp_increment`
- ⏭️ NOT RUN `testcases_comma_expr_increment.c` → `test_pre31_c_fail_testcases_comma_expr_increment`
- ⏭️ NOT RUN `testcases_compound_assignment.c` → `test_pre31_c_fail_testcases_compound_assignment`
- ⏭️ NOT RUN `testcases_fopen_in_macro.c` → `test_pre31_c_fail_testcases_fopen_in_macro`
- ⏭️ NOT RUN `testcases_getchar_in_macro.c` → `test_pre31_c_fail_testcases_getchar_in_macro`
- ⏭️ NOT RUN `testcases_logical_and_increment.c` → `test_pre31_c_fail_testcases_logical_and_increment`
- ⏭️ NOT RUN `testcases_malloc_in_macro.c` → `test_pre31_c_fail_testcases_malloc_in_macro`
- ⏭️ NOT RUN `testcases_max_both_sides.c` → `test_pre31_c_fail_testcases_max_both_sides`
- ⏭️ NOT RUN `testcases_max_pre_increment.c` → `test_pre31_c_fail_testcases_max_pre_increment`
- ⏭️ NOT RUN `testcases_min_post_decrement.c` → `test_pre31_c_fail_testcases_min_post_decrement`
- ⏭️ NOT RUN `testcases_nested_increment.c` → `test_pre31_c_fail_testcases_nested_increment`
- ⏭️ NOT RUN `testcases_pointer_deref_increment.c` → `test_pre31_c_fail_testcases_pointer_deref_increment`
- ⏭️ NOT RUN `testcases_rand_in_macro.c` → `test_pre31_c_fail_testcases_rand_in_macro`
- ⏭️ NOT RUN `testcases_scanf_in_macro.c` → `test_pre31_c_fail_testcases_scanf_in_macro`
- ⏭️ NOT RUN `testcases_square_increment.c` → `test_pre31_c_fail_testcases_square_increment`
- ⏭️ NOT RUN `testcases_strlen_in_macro.c` → `test_pre31_c_fail_testcases_strlen_in_macro`
- ⏭️ NOT RUN `testcases_strtok_in_macro.c` → `test_pre31_c_fail_testcases_strtok_in_macro`
- ⏭️ NOT RUN `testcases_struct_member_increment.c` → `test_pre31_c_fail_testcases_struct_member_increment`
- ⏭️ NOT RUN `testcases_swap_increment.c` → `test_pre31_c_fail_testcases_swap_increment`
- ⏭️ NOT RUN `testcases_ternary_increment.c` → `test_pre31_c_fail_testcases_ternary_increment`
- ⏭️ NOT RUN `testcases_time_in_macro.c` → `test_pre31_c_fail_testcases_time_in_macro`
- ⏭️ NOT RUN `testcases_volatile_access.c` → `test_pre31_c_fail_testcases_volatile_access`
- ⏭️ NOT RUN `wiki_assert.c` → `test_pre31_c_fail_wiki_assert`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pre31_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_pre31_c_fail_wiki_noncompliant_2_2`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_abs_separate_increment.c` → `test_pre31_c_pass_testcases_abs_separate_increment`
- ⏭️ NOT RUN `testcases_assert_no_side_effect.c` → `test_pre31_c_pass_testcases_assert_no_side_effect`
- ⏭️ NOT RUN `testcases_c11_generic.c` → `test_pre31_c_pass_testcases_c11_generic`
- ⏭️ NOT RUN `testcases_constant_args.c` → `test_pre31_c_pass_testcases_constant_args`
- ⏭️ NOT RUN `testcases_getchar_before_macro.c` → `test_pre31_c_pass_testcases_getchar_before_macro`
- ⏭️ NOT RUN `testcases_inline_function.c` → `test_pre31_c_pass_testcases_inline_function`
- ⏭️ NOT RUN `testcases_max_no_side_effect.c` → `test_pre31_c_pass_testcases_max_no_side_effect`
- ⏭️ NOT RUN `testcases_pure_function_arg.c` → `test_pre31_c_pass_testcases_pure_function_arg`
- ⏭️ NOT RUN `testcases_safe_macro_with_increment.c` → `test_pre31_c_pass_testcases_safe_macro_with_increment`
- ⏭️ NOT RUN `testcases_volatile_read_stored.c` → `test_pre31_c_pass_testcases_volatile_read_stored`
- ⏭️ NOT RUN `wiki_assert.c` → `test_pre31_c_pass_wiki_assert`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre31_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_pre31_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_pre31_c_pass_wiki_compliant_3`
- ⏭️ NOT RUN `wiki_gcc.c` → `test_pre31_c_pass_wiki_gcc`

---

### 🔶 PRE05-C - Not Implemented (has tests)

<a id="rule-pre05c"></a>

**Title:** Understand macro replacement when concatenating tokens or performing stringification

**Description:** It is necessary to understand how macro replacement works in C, particularly in
the context of concatenating tokens using the##operator and converting macro
parameters to strings using the#operator. The##preprocessing operator is used to
merge two tokens into one while expanding macros, which is calledtoken
pastingortoken concatenation. When a macro is expanded, the two tokens on either
side of each##operator are combined into a single token that replaces the##and
the two original tokens in the macro expansion [FSF 2005]. Token pasting is most
useful when one or both of the tokens come from a macro argument. If either of
the tokens next to a##is a parameter name, it is replaced by its actual argument
before##executes. The actual argument is not macro expanded first.

**Test Coverage:** 5 tests (3 fail, 2 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pre05_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_pre05_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_pre05_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre05_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_pre05_c_pass_wiki_compliant_2`

---

### 🔶 PRE08-C - Not Implemented (has tests)

<a id="rule-pre08c"></a>

**Title:** Guarantee that header file names are unique

**Description:** Make sure that included header file names are unique. According to the C
Standard, subclause 6.10.2, paragraph 5 [ISO/IEC 9899:2011], This means that To
guarantee that header file names are unique, all included files should differ
(in a case-insensitive manner) in their first eight characters or in their (one-
character) file extension.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pre08_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre08_c_pass_wiki_compliant_1`

---

### 🔶 PRE00-C - Not Implemented (has tests)

<a id="rule-pre00c"></a>

**Title:** Prefer inline or static functions to function-like macros

**Description:** Macros are dangerous because their use resembles that of real functions, but
they have different semantics. The inline function-specifier was introduced to
the C programming language in the C99 standard. Inline functions should be
preferred over macros when they can be used interchangeably. Making a function
an inline function suggests that calls to the function be as fast as possible by
using, for example, an alternative to the usual function call mechanism, such
asinline substitution. (See alsoPRE31-C. Avoid side effects in arguments to
unsafe macros,PRE01-C. Use parentheses within macros around parameter names,
andPRE02-C. Macro replacement lists should be parenthesized.) Inline
substitution is not textual substitution, nor does it create a new function. For
example, the expansion of a macro used within the body of the function uses the
definition it had at the point the function body appeared, not where the
function is called; and identifiers refer to the declarations in scope where the
body occurs. Arguably, a decision to inline a function is a low-level
optimization detail that the compiler should make without programmer input. The
use of inline functions should be evaluated on the basis of (a) how well they
are supported by targeted compilers, (b) what (if any) impact they have on the
performance characteristics of your system, and (c) portability concerns. Static
functions are often as good as inline functions and are supported in C.

**Test Coverage:** 8 tests (5 fail, 3 pass)

**Test Results:** 0/8 passed (0.0%), 8 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pre00_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_pre00_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_pre00_c_fail_wiki_noncompliant_3`
- ⏭️ NOT RUN `wiki_noncompliant_4_2.c` → `test_pre00_c_fail_wiki_noncompliant_4_2`
- ⏭️ NOT RUN `wiki_noncompliant_5.c` → `test_pre00_c_fail_wiki_noncompliant_5`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre00_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_pre00_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_pre00_c_pass_wiki_compliant_3`

---

### ✅ PRE32-C - Implemented

<a id="rule-pre32c"></a>

**Title:** Do not use preprocessor directives in invocations of function-like macros

**Description:** The arguments to a macro must not include preprocessor directives, such
as#define,#ifdef, and#include. Doing so results inundefined behavior, according
to the C Standard, 6.10.5, paragraph 11 [ISO/IEC 9899:2024]: See alsoundefined
behavior 92. This rule also applies to the use of preprocessor directives in
arguments to any function where it is unknown whether or not the function is
implemented using a macro. This includes all standard library functions, such
asmemcpy(),printf(), andassert(), because any standard library function may be
implemented as a macro. (C24, 7.1.4, paragraph 1).

**Test Coverage:** 42 tests (31 fail, 11 pass)

**Test Results:** 0/42 passed (0.0%), 42 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_assert_ifdef.c` → `test_pre32_c_fail_testcases_assert_ifdef`
- ⏭️ NOT RUN `testcases_calloc_ifdef.c` → `test_pre32_c_fail_testcases_calloc_ifdef`
- ⏭️ NOT RUN `testcases_custom_macro_ifdef.c` → `test_pre32_c_fail_testcases_custom_macro_ifdef`
- ⏭️ NOT RUN `testcases_define_in_macro.c` → `test_pre32_c_fail_testcases_define_in_macro`
- ⏭️ NOT RUN `testcases_elif_in_macro.c` → `test_pre32_c_fail_testcases_elif_in_macro`
- ⏭️ NOT RUN `testcases_endif_in_macro.c` → `test_pre32_c_fail_testcases_endif_in_macro`
- ⏭️ NOT RUN `testcases_error_in_macro.c` → `test_pre32_c_fail_testcases_error_in_macro`
- ⏭️ NOT RUN `testcases_fprintf_ifdef.c` → `test_pre32_c_fail_testcases_fprintf_ifdef`
- ⏭️ NOT RUN `testcases_fread_ifdef.c` → `test_pre32_c_fail_testcases_fread_ifdef`
- ⏭️ NOT RUN `testcases_fwrite_ifdef.c` → `test_pre32_c_fail_testcases_fwrite_ifdef`
- ⏭️ NOT RUN `testcases_ifndef_in_macro.c` → `test_pre32_c_fail_testcases_ifndef_in_macro`
- ⏭️ NOT RUN `testcases_include_in_macro.c` → `test_pre32_c_fail_testcases_include_in_macro`
- ⏭️ NOT RUN `testcases_line_in_macro.c` → `test_pre32_c_fail_testcases_line_in_macro`
- ⏭️ NOT RUN `testcases_malloc_ifdef.c` → `test_pre32_c_fail_testcases_malloc_ifdef`
- ⏭️ NOT RUN `testcases_memcpy_ifdef.c` → `test_pre32_c_fail_testcases_memcpy_ifdef`
- ⏭️ NOT RUN `testcases_memset_ifdef.c` → `test_pre32_c_fail_testcases_memset_ifdef`
- ⏭️ NOT RUN `testcases_multiple_args_ifdef.c` → `test_pre32_c_fail_testcases_multiple_args_ifdef`
- ⏭️ NOT RUN `testcases_nested_macro_ifdef.c` → `test_pre32_c_fail_testcases_nested_macro_ifdef`
- ⏭️ NOT RUN `testcases_pragma_in_macro.c` → `test_pre32_c_fail_testcases_pragma_in_macro`
- ⏭️ NOT RUN `testcases_printf_ifdef.c` → `test_pre32_c_fail_testcases_printf_ifdef`
- ⏭️ NOT RUN `testcases_realloc_ifdef.c` → `test_pre32_c_fail_testcases_realloc_ifdef`
- ⏭️ NOT RUN `testcases_snprintf_ifdef.c` → `test_pre32_c_fail_testcases_snprintf_ifdef`
- ⏭️ NOT RUN `testcases_sprintf_ifdef.c` → `test_pre32_c_fail_testcases_sprintf_ifdef`
- ⏭️ NOT RUN `testcases_strcmp_ifdef.c` → `test_pre32_c_fail_testcases_strcmp_ifdef`
- ⏭️ NOT RUN `testcases_strncat_ifdef.c` → `test_pre32_c_fail_testcases_strncat_ifdef`
- ⏭️ NOT RUN `testcases_strncmp_ifdef.c` → `test_pre32_c_fail_testcases_strncmp_ifdef`
- ⏭️ NOT RUN `testcases_strncpy_ifdef.c` → `test_pre32_c_fail_testcases_strncpy_ifdef`
- ⏭️ NOT RUN `testcases_strstr_ifdef.c` → `test_pre32_c_fail_testcases_strstr_ifdef`
- ⏭️ NOT RUN `testcases_undef_in_macro.c` → `test_pre32_c_fail_testcases_undef_in_macro`
- ⏭️ NOT RUN `testcases_warning_in_macro.c` → `test_pre32_c_fail_testcases_warning_in_macro`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pre32_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_assert_external_ifdef.c` → `test_pre32_c_pass_testcases_assert_external_ifdef`
- ⏭️ NOT RUN `testcases_inline_function.c` → `test_pre32_c_pass_testcases_inline_function`
- ⏭️ NOT RUN `testcases_macro_variable_precompute.c` → `test_pre32_c_pass_testcases_macro_variable_precompute`
- ⏭️ NOT RUN `testcases_malloc_size_const.c` → `test_pre32_c_pass_testcases_malloc_size_const`
- ⏭️ NOT RUN `testcases_memcpy_external_ifdef.c` → `test_pre32_c_pass_testcases_memcpy_external_ifdef`
- ⏭️ NOT RUN `testcases_memset_size_variable.c` → `test_pre32_c_pass_testcases_memset_size_variable`
- ⏭️ NOT RUN `testcases_printf_const_arg.c` → `test_pre32_c_pass_testcases_printf_const_arg`
- ⏭️ NOT RUN `testcases_sprintf_external_ifdef.c` → `test_pre32_c_pass_testcases_sprintf_external_ifdef`
- ⏭️ NOT RUN `testcases_square_const_macro.c` → `test_pre32_c_pass_testcases_square_const_macro`
- ⏭️ NOT RUN `testcases_strcmp_string_const.c` → `test_pre32_c_pass_testcases_strcmp_string_const`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre32_c_pass_wiki_compliant_1`

---

### 🔶 PRE04-C - Not Implemented (has tests)

<a id="rule-pre04c"></a>

**Title:** Do not reuse a standard header file name

**Description:** If a file with the same name as a standard header is placed in the search path
for included source files, the behavior isundefined. The following table from
the C Standard, subclause 7.1.2 [ISO/IEC 9899:2011], lists these standard
headers: <assert.h><float.h><math.h><stdatomic.h><stdlib.h><time.h><complex.h><i
nttypes.h><setjmp.h><stdbool.h><stdnoreturn.h><uchar.h><ctype.h><iso646.h><signa
l.h><stddef.h><string.h><wchar.h><errno.h><limits.h><stdalign.h><stdint.h><tgmat
h.h><wctype.h><fenv.h><locale.h><stdarg.h><stdio.h><threads.h>

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pre04_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre04_c_pass_wiki_compliant_1`

---

### 🔶 PRE13-C - Not Implemented (has tests)

<a id="rule-pre13c"></a>

**Title:** Use the Standard predefined macros to test for versions and features.

**Description:** The C Standard defines a set of predefined macros (see subclause 6.10.8) to help
the user determine if theimplementationbeing used is aconformingimplementation,
and if so, to which version of the C Standard it conforms. These macros can also
help the user to determine which of the standard features are implemented. The
following tables list these macros and indicate in which version of the C
Standard they were introduced. The following macros are required: Macro NameC90C
99C11__STDC__✓✓✓__STDC_HOSTED__✓✓__STDC_VERSION__1✓✓__DATE__✓✓✓__FILE__✓✓✓__LINE
__✓✓✓__TIME__✓✓✓

**Test Coverage:** 4 tests (1 fail, 3 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_checking_value_of_predefined_macro.c` → `test_pre13_c_fail_wiki_checking_value_of_predefined_macro`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_optional_language_features.c` → `test_pre13_c_pass_wiki_optional_language_features`
- ⏭️ NOT RUN `wiki_test_for_optional_feature.c` → `test_pre13_c_pass_wiki_test_for_optional_feature`
- ⏭️ NOT RUN `wiki_testing_for_definition_of_macro.c` → `test_pre13_c_pass_wiki_testing_for_definition_of_macro`

---

### ✅ PRE09-C - Implemented

<a id="rule-pre09c"></a>

**Title:** Do not replace secure functions with deprecated or obsolescent functions

**Description:** Macros are frequently used in the remediation of existing code to globally
replace one identifier with another, for example, when an existing API changes.
Although some risk is always involved, this practice becomes particularly
dangerous if a function name is replaced with the function name of a deprecated
or obsolescent function. Deprecated functions are defined by the C Standard and
Technical Corrigenda. Obsolescent functions are defined byMSC24-C. Do not use
deprecated or obsolescent functions. Although compliance with ruleMSC24-C. Do
not use deprecated or obsolescent functionsguarantees compliance with this
recommendation, the emphasis of this recommendation is the extremely risky and
deceptive practice of replacing functions with less secure alternatives. The
Internet Systems Consortium's (ISC) Dynamic Host Configuration Protocol (DHCP)
contained a vulnerability that introduced several potential buffer overflow
conditions [VU#654390]. ISC DHCP makes use of thevsnprintf()function for writing
various log file strings;vsnprintf()is defined in the Portable Operating System
Interface (POSIX®), Base Specifications, Issue 7 [IEEE Std 1003.1:2013] as well
as in the C Standard. For systems that do not supportvsnprintf(), a C include
file was created that defines thevsnprintf()function tovsprintf(), as shown in
this noncompliant code example:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_pre09_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_pre09_c_pass_wiki_compliant_1`

---

## Category: SIG

<a id="category-sig"></a>

**Implementation Status:** 2 / 7 rules (28.6%)

### 🔶 SIG00-C - Not Implemented (has tests)

<a id="rule-sig00c"></a>

**Title:** Mask signals handled by noninterruptible signal handlers

**Description:** A signal is a mechanism for transferring control that is typically used to
notify a process that an event has occurred. That process can then respond to
the event accordingly. The C Standard provides functions for sending and
handling signals within a C program. Processes handle signals by registering a
signal handler using thesignal()function, which is specified as void
(*signal(int sig, void (*func)(int)))(int);

**Test Coverage:** 44 tests (32 fail, 12 pass)

**Test Results:** 0/44 passed (0.0%), 44 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_async_unsafe_operations.c` → `test_sig00_c_fail_testcases_async_unsafe_operations`
- ⏭️ NOT RUN `testcases_errno_corruption.c` → `test_sig00_c_fail_testcases_errno_corruption`
- ⏭️ NOT RUN `testcases_file_operations_handler.c` → `test_sig00_c_fail_testcases_file_operations_handler`
- ⏭️ NOT RUN `testcases_fork_in_handler.c` → `test_sig00_c_fail_testcases_fork_in_handler`
- ⏭️ NOT RUN `testcases_global_state_corruption.c` → `test_sig00_c_fail_testcases_global_state_corruption`
- ⏭️ NOT RUN `testcases_library_function_call.c` → `test_sig00_c_fail_testcases_library_function_call`
- ⏭️ NOT RUN `testcases_malloc_in_handler.c` → `test_sig00_c_fail_testcases_malloc_in_handler`
- ⏭️ NOT RUN `testcases_multiple_signals_unmasked.c` → `test_sig00_c_fail_testcases_multiple_signals_unmasked`
- ⏭️ NOT RUN `testcases_mutex_deadlock.c` → `test_sig00_c_fail_testcases_mutex_deadlock`
- ⏭️ NOT RUN `testcases_nested_interruption.c` → `test_sig00_c_fail_testcases_nested_interruption`
- ⏭️ NOT RUN `testcases_recursive_signal.c` → `test_sig00_c_fail_testcases_recursive_signal`
- ⏭️ NOT RUN `testcases_reentrant_handler.c` → `test_sig00_c_fail_testcases_reentrant_handler`
- ⏭️ NOT RUN `testcases_shared_buffer_race.c` → `test_sig00_c_fail_testcases_shared_buffer_race`
- ⏭️ NOT RUN `testcases_signal_array_bounds.c` → `test_sig00_c_fail_testcases_signal_array_bounds`
- ⏭️ NOT RUN `testcases_signal_database_ops.c` → `test_sig00_c_fail_testcases_signal_database_ops`
- ⏭️ NOT RUN `testcases_signal_directory_ops.c` → `test_sig00_c_fail_testcases_signal_directory_ops`
- ⏭️ NOT RUN `testcases_signal_environment_vars.c` → `test_sig00_c_fail_testcases_signal_environment_vars`
- ⏭️ NOT RUN `testcases_signal_handler_exit.c` → `test_sig00_c_fail_testcases_signal_handler_exit`
- ⏭️ NOT RUN `testcases_signal_handler_printf.c` → `test_sig00_c_fail_testcases_signal_handler_printf`
- ⏭️ NOT RUN `testcases_signal_linked_list.c` → `test_sig00_c_fail_testcases_signal_linked_list`
- ⏭️ NOT RUN `testcases_signal_longjmp.c` → `test_sig00_c_fail_testcases_signal_longjmp`
- ⏭️ NOT RUN `testcases_signal_nested_calls.c` → `test_sig00_c_fail_testcases_signal_nested_calls`
- ⏭️ NOT RUN `testcases_signal_pipe_operations.c` → `test_sig00_c_fail_testcases_signal_pipe_operations`
- ⏭️ NOT RUN `testcases_signal_shared_memory.c` → `test_sig00_c_fail_testcases_signal_shared_memory`
- ⏭️ NOT RUN `testcases_signal_socket_operations.c` → `test_sig00_c_fail_testcases_signal_socket_operations`
- ⏭️ NOT RUN `testcases_signal_storm.c` → `test_sig00_c_fail_testcases_signal_storm`
- ⏭️ NOT RUN `testcases_signal_thread_unsafe.c` → `test_sig00_c_fail_testcases_signal_thread_unsafe`
- ⏭️ NOT RUN `testcases_signal_unsafe_arithmetic.c` → `test_sig00_c_fail_testcases_signal_unsafe_arithmetic`
- ⏭️ NOT RUN `testcases_static_variable_race.c` → `test_sig00_c_fail_testcases_static_variable_race`
- ⏭️ NOT RUN `testcases_unmask_signal.c` → `test_sig00_c_fail_testcases_unmask_signal`
- ⏭️ NOT RUN `testcases_unmasked_timer_signal.c` → `test_sig00_c_fail_testcases_unmasked_timer_signal`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_sig00_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_atomic_operations_only.c` → `test_sig00_c_pass_testcases_atomic_operations_only`
- ⏭️ NOT RUN `testcases_masked_multiple_signals.c` → `test_sig00_c_pass_testcases_masked_multiple_signals`
- ⏭️ NOT RUN `testcases_masked_signal.c` → `test_sig00_c_pass_testcases_masked_signal`
- ⏭️ NOT RUN `testcases_masked_timer_handler.c` → `test_sig00_c_pass_testcases_masked_timer_handler`
- ⏭️ NOT RUN `testcases_minimal_signal_handler.c` → `test_sig00_c_pass_testcases_minimal_signal_handler`
- ⏭️ NOT RUN `testcases_safe_errno_handling.c` → `test_sig00_c_pass_testcases_safe_errno_handling`
- ⏭️ NOT RUN `testcases_self_pipe_trick.c` → `test_sig00_c_pass_testcases_self_pipe_trick`
- ⏭️ NOT RUN `testcases_signal_safe_logging.c` → `test_sig00_c_pass_testcases_signal_safe_logging`
- ⏭️ NOT RUN `testcases_signal_synchronization.c` → `test_sig00_c_pass_testcases_signal_synchronization`
- ⏭️ NOT RUN `testcases_signalfd_approach.c` → `test_sig00_c_pass_testcases_signalfd_approach`
- ⏭️ NOT RUN `testcases_sigwait_synchronous.c` → `test_sig00_c_pass_testcases_sigwait_synchronous`
- ⏭️ NOT RUN `wiki_posix.c` → `test_sig00_c_pass_wiki_posix`

---

### 🔶 SIG01-C - Not Implemented (has tests)

<a id="rule-sig01c"></a>

**Title:** Understand implementation-specific details regarding signal handler persistence

**Description:** Thesignal()function hasimplementation-definedbehavior and behaves differently on
Windows, for example, than it does on many UNIX systems. The following code
example shows this behavior: #include <stdio.h> #include <signal.h> volatile
sig_atomic_t e_flag = 0; void handler(int signum) { e_flag = 1; } int main(void)
{ if (signal(SIGINT, handler) == SIG_ERR) { /* Handle error */ } while (!e_flag)
{} puts("Escaped from first while ()"); e_flag = 0; while (!e_flag) {}
puts("Escaped from second while ()"); return 0; }

**Test Coverage:** 47 tests (35 fail, 12 pass)

**Test Results:** 0/47 passed (0.0%), 47 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_assume_persistence.c` → `test_sig01_c_fail_testcases_assume_persistence`
- ⏭️ NOT RUN `testcases_async_signal_assumption.c` → `test_sig01_c_fail_testcases_async_signal_assumption`
- ⏭️ NOT RUN `testcases_concurrent_signals.c` → `test_sig01_c_fail_testcases_concurrent_signals`
- ⏭️ NOT RUN `testcases_handler_reset_race.c` → `test_sig01_c_fail_testcases_handler_reset_race`
- ⏭️ NOT RUN `testcases_implicit_reset_ignore.c` → `test_sig01_c_fail_testcases_implicit_reset_ignore`
- ⏭️ NOT RUN `testcases_legacy_signal_pattern.c` → `test_sig01_c_fail_testcases_legacy_signal_pattern`
- ⏭️ NOT RUN `testcases_multiple_signals_assumption.c` → `test_sig01_c_fail_testcases_multiple_signals_assumption`
- ⏭️ NOT RUN `testcases_nested_signal_handling.c` → `test_sig01_c_fail_testcases_nested_signal_handling`
- ⏭️ NOT RUN `testcases_no_verification.c` → `test_sig01_c_fail_testcases_no_verification`
- ⏭️ NOT RUN `testcases_platform_inconsistent.c` → `test_sig01_c_fail_testcases_platform_inconsistent`
- ⏭️ NOT RUN `testcases_realtime_signal_assumption.c` → `test_sig01_c_fail_testcases_realtime_signal_assumption`
- ⏭️ NOT RUN `testcases_signal_blocking_assumption.c` → `test_sig01_c_fail_testcases_signal_blocking_assumption`
- ⏭️ NOT RUN `testcases_signal_chain_assumption.c` → `test_sig01_c_fail_testcases_signal_chain_assumption`
- ⏭️ NOT RUN `testcases_signal_cleanup_assumption.c` → `test_sig01_c_fail_testcases_signal_cleanup_assumption`
- ⏭️ NOT RUN `testcases_signal_default_assumption.c` → `test_sig01_c_fail_testcases_signal_default_assumption`
- ⏭️ NOT RUN `testcases_signal_delivery_assumption.c` → `test_sig01_c_fail_testcases_signal_delivery_assumption`
- ⏭️ NOT RUN `testcases_signal_disposition_assumption.c` → `test_sig01_c_fail_testcases_signal_disposition_assumption`
- ⏭️ NOT RUN `testcases_signal_errno_assumption.c` → `test_sig01_c_fail_testcases_signal_errno_assumption`
- ⏭️ NOT RUN `testcases_signal_inheritance.c` → `test_sig01_c_fail_testcases_signal_inheritance`
- ⏭️ NOT RUN `testcases_signal_library_assumption.c` → `test_sig01_c_fail_testcases_signal_library_assumption`
- ⏭️ NOT RUN `testcases_signal_loop_assumption.c` → `test_sig01_c_fail_testcases_signal_loop_assumption`
- ⏭️ NOT RUN `testcases_signal_mask_assumption.c` → `test_sig01_c_fail_testcases_signal_mask_assumption`
- ⏭️ NOT RUN `testcases_signal_persistence.c` → `test_sig01_c_fail_testcases_signal_persistence`
- ⏭️ NOT RUN `testcases_signal_portability_assumption.c` → `test_sig01_c_fail_testcases_signal_portability_assumption`
- ⏭️ NOT RUN `testcases_signal_queue_assumption.c` → `test_sig01_c_fail_testcases_signal_queue_assumption`
- ⏭️ NOT RUN `testcases_signal_reentrant_assumption.c` → `test_sig01_c_fail_testcases_signal_reentrant_assumption`
- ⏭️ NOT RUN `testcases_signal_restart_assumption.c` → `test_sig01_c_fail_testcases_signal_restart_assumption`
- ⏭️ NOT RUN `testcases_signal_state_assumption.c` → `test_sig01_c_fail_testcases_signal_state_assumption`
- ⏭️ NOT RUN `testcases_signal_thread_assumption.c` → `test_sig01_c_fail_testcases_signal_thread_assumption`
- ⏭️ NOT RUN `testcases_signal_timing_assumption.c` → `test_sig01_c_fail_testcases_signal_timing_assumption`
- ⏭️ NOT RUN `testcases_system_specific_behavior.c` → `test_sig01_c_fail_testcases_system_specific_behavior`
- ⏭️ NOT RUN `testcases_windows_unix_difference.c` → `test_sig01_c_fail_testcases_windows_unix_difference`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_sig01_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_sig01_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_unix.c` → `test_sig01_c_fail_wiki_unix`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_cross_platform_signal_safety.c` → `test_sig01_c_pass_testcases_cross_platform_signal_safety`
- ⏭️ NOT RUN `testcases_defensive_signal_programming.c` → `test_sig01_c_pass_testcases_defensive_signal_programming`
- ⏭️ NOT RUN `testcases_platform_aware_handling.c` → `test_sig01_c_pass_testcases_platform_aware_handling`
- ⏭️ NOT RUN `testcases_portable_signal_patterns.c` → `test_sig01_c_pass_testcases_portable_signal_patterns`
- ⏭️ NOT RUN `testcases_proper_handler_registration.c` → `test_sig01_c_pass_testcases_proper_handler_registration`
- ⏭️ NOT RUN `testcases_reliable_signal_handling.c` → `test_sig01_c_pass_testcases_reliable_signal_handling`
- ⏭️ NOT RUN `testcases_sigaction_persistence.c` → `test_sig01_c_pass_testcases_sigaction_persistence`
- ⏭️ NOT RUN `testcases_signal_behavior_verification.c` → `test_sig01_c_pass_testcases_signal_behavior_verification`
- ⏭️ NOT RUN `testcases_signal_safe_patterns.c` → `test_sig01_c_pass_testcases_signal_safe_patterns`
- ⏭️ NOT RUN `testcases_signal_state_management.c` → `test_sig01_c_pass_testcases_signal_state_management`
- ⏭️ NOT RUN `wiki_posix.c` → `test_sig01_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_unix_and_windows.c` → `test_sig01_c_pass_wiki_unix_and_windows`

---

### 🔶 SIG34-C - Not Implemented (has tests)

<a id="rule-sig34c"></a>

**Title:** Do not call signal() from within interruptible signal handlers

**Description:** A signal handler should not reassert its desire to handle its own signal. This
is often done onnonpersistentplatforms—that is, platforms that, upon receiving a
signal, reset the handler for the signal to SIG_DFL before calling the bound
signal handler. Callingsignal()under these conditions presents a race condition.
(SeeSIG01-C. Understand implementation-specific details regarding signal handler
persistence.) A signal handler may callsignal()only if it does not need to
beasynchronous-safe(that is, if all relevant signals are masked so that the
handler cannot be interrupted). On nonpersistent platforms, this noncompliant
code example contains a race window, starting when the host environment resets
the signal and ending when the handler callssignal(). During that time, a second
signal sent to the program will trigger the default signal behavior,
consequently defeating the persistent behavior implied by the call
tosignal()from within the handler to reassert the binding.

**Test Coverage:** 44 tests (33 fail, 11 pass)

**Test Results:** 0/44 passed (0.0%), 44 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_atomicity_attempt.c` → `test_sig34_c_fail_testcases_atomicity_attempt`
- ⏭️ NOT RUN `testcases_chained_handlers.c` → `test_sig34_c_fail_testcases_chained_handlers`
- ⏭️ NOT RUN `testcases_cleanup_signal.c` → `test_sig34_c_fail_testcases_cleanup_signal`
- ⏭️ NOT RUN `testcases_competing_signals.c` → `test_sig34_c_fail_testcases_competing_signals`
- ⏭️ NOT RUN `testcases_complex_state_management.c` → `test_sig34_c_fail_testcases_complex_state_management`
- ⏭️ NOT RUN `testcases_conditional_signal.c` → `test_sig34_c_fail_testcases_conditional_signal`
- ⏭️ NOT RUN `testcases_debug_logging_signal.c` → `test_sig34_c_fail_testcases_debug_logging_signal`
- ⏭️ NOT RUN `testcases_different_handlers.c` → `test_sig34_c_fail_testcases_different_handlers`
- ⏭️ NOT RUN `testcases_dynamic_registration.c` → `test_sig34_c_fail_testcases_dynamic_registration`
- ⏭️ NOT RUN `testcases_error_condition_signal.c` → `test_sig34_c_fail_testcases_error_condition_signal`
- ⏭️ NOT RUN `testcases_handler_factory.c` → `test_sig34_c_fail_testcases_handler_factory`
- ⏭️ NOT RUN `testcases_handler_rotation.c` → `test_sig34_c_fail_testcases_handler_rotation`
- ⏭️ NOT RUN `testcases_handler_swapping.c` → `test_sig34_c_fail_testcases_handler_swapping`
- ⏭️ NOT RUN `testcases_interrupt_signal_modification.c` → `test_sig34_c_fail_testcases_interrupt_signal_modification`
- ⏭️ NOT RUN `testcases_legacy_patterns.c` → `test_sig34_c_fail_testcases_legacy_patterns`
- ⏭️ NOT RUN `testcases_masking_attempt.c` → `test_sig34_c_fail_testcases_masking_attempt`
- ⏭️ NOT RUN `testcases_multiple_signals_handler.c` → `test_sig34_c_fail_testcases_multiple_signals_handler`
- ⏭️ NOT RUN `testcases_nested_signal_calls.c` → `test_sig34_c_fail_testcases_nested_signal_calls`
- ⏭️ NOT RUN `testcases_new_signal_registration.c` → `test_sig34_c_fail_testcases_new_signal_registration`
- ⏭️ NOT RUN `testcases_persistence_attempt.c` → `test_sig34_c_fail_testcases_persistence_attempt`
- ⏭️ NOT RUN `testcases_platform_assumptions.c` → `test_sig34_c_fail_testcases_platform_assumptions`
- ⏭️ NOT RUN `testcases_race_condition.c` → `test_sig34_c_fail_testcases_race_condition`
- ⏭️ NOT RUN `testcases_recursive_nested.c` → `test_sig34_c_fail_testcases_recursive_nested`
- ⏭️ NOT RUN `testcases_reset_disposition.c` → `test_sig34_c_fail_testcases_reset_disposition`
- ⏭️ NOT RUN `testcases_self_reregister.c` → `test_sig34_c_fail_testcases_self_reregister`
- ⏭️ NOT RUN `testcases_signal_cascading.c` → `test_sig34_c_fail_testcases_signal_cascading`
- ⏭️ NOT RUN `testcases_signal_in_handler.c` → `test_sig34_c_fail_testcases_signal_in_handler`
- ⏭️ NOT RUN `testcases_signal_multiplexing.c` → `test_sig34_c_fail_testcases_signal_multiplexing`
- ⏭️ NOT RUN `testcases_signal_priority_management.c` → `test_sig34_c_fail_testcases_signal_priority_management`
- ⏭️ NOT RUN `testcases_signal_storm_handler.c` → `test_sig34_c_fail_testcases_signal_storm_handler`
- ⏭️ NOT RUN `testcases_signal_type_behavior.c` → `test_sig34_c_fail_testcases_signal_type_behavior`
- ⏭️ NOT RUN `testcases_time_dependent_race.c` → `test_sig34_c_fail_testcases_time_dependent_race`
- ⏭️ NOT RUN `wiki_posix.c` → `test_sig34_c_fail_wiki_posix`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_atomic_registration.c` → `test_sig34_c_pass_testcases_atomic_registration`
- ⏭️ NOT RUN `testcases_flag_only_handlers.c` → `test_sig34_c_pass_testcases_flag_only_handlers`
- ⏭️ NOT RUN `testcases_no_disposition_changes.c` → `test_sig34_c_pass_testcases_no_disposition_changes`
- ⏭️ NOT RUN `testcases_no_handler_modification.c` → `test_sig34_c_pass_testcases_no_handler_modification`
- ⏭️ NOT RUN `testcases_proper_signal_masking.c` → `test_sig34_c_pass_testcases_proper_signal_masking`
- ⏭️ NOT RUN `testcases_safe_signal_patterns.c` → `test_sig34_c_pass_testcases_safe_signal_patterns`
- ⏭️ NOT RUN `testcases_self_pipe_trick.c` → `test_sig34_c_pass_testcases_self_pipe_trick`
- ⏭️ NOT RUN `testcases_sigaction_exclusive.c` → `test_sig34_c_pass_testcases_sigaction_exclusive`
- ⏭️ NOT RUN `testcases_sigaction_persistent.c` → `test_sig34_c_pass_testcases_sigaction_persistent`
- ⏭️ NOT RUN `testcases_signalfd_safe.c` → `test_sig34_c_pass_testcases_signalfd_safe`
- ⏭️ NOT RUN `wiki_posix.c` → `test_sig34_c_pass_wiki_posix`

---

### ✅ SIG31-C - Implemented

<a id="rule-sig31c"></a>

**Title:** Do not access shared objects in signal handlers

**Description:** Accessing or modifying shared objects in signal handlers can result in race
conditions that can leave data in an inconsistent state. The two exceptions (C
Standard, 5.1.2.3, paragraph 5) to this rule are the ability to read from and
write to lock-free atomic objects and variables of typevolatile sig_atomic_t.
Accessing any other type of object from a signal handler isundefined behavior.
(Seeundefined behavior 131.) The need for thevolatilekeyword is described
inDCL22-C. Use volatile for data that cannot be cached. The typesig_atomic_tis
the integer type of an object that can be accessed as an atomic entity even in
the presence of asynchronous interrupts. The type
ofsig_atomic_tisimplementation-defined, though it provides some guarantees.
Integer values ranging fromSIG_ATOMIC_MINthroughSIG_ATOMIC_MAX, inclusive, may
be safely stored to a variable of the type. In addition, whensig_atomic_tis a
signed integer type,SIG_ATOMIC_MINmust be no greater than−127andSIG_ATOMIC_MAXno
less than127. Otherwise,SIG_ATOMIC_MINmust be0andSIG_ATOMIC_MAXmust be no less
than255. The macrosSIG_ATOMIC_MINandSIG_ATOMIC_MAXare defined in the
header<stdint.h>.

**Test Coverage:** 43 tests (31 fail, 12 pass)

**Test Results:** 0/43 passed (0.0%), 43 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_authentication_session.c` → `test_sig31_c_fail_testcases_authentication_session`
- ⏭️ NOT RUN `testcases_circular_buffer_access.c` → `test_sig31_c_fail_testcases_circular_buffer_access`
- ⏭️ NOT RUN `testcases_complex_struct_access.c` → `test_sig31_c_fail_testcases_complex_struct_access`
- ⏭️ NOT RUN `testcases_database_connection.c` → `test_sig31_c_fail_testcases_database_connection`
- ⏭️ NOT RUN `testcases_dynamic_memory_access.c` → `test_sig31_c_fail_testcases_dynamic_memory_access`
- ⏭️ NOT RUN `testcases_error_state_access.c` → `test_sig31_c_fail_testcases_error_state_access`
- ⏭️ NOT RUN `testcases_file_descriptor_access.c` → `test_sig31_c_fail_testcases_file_descriptor_access`
- ⏭️ NOT RUN `testcases_file_scope_variable.c` → `test_sig31_c_fail_testcases_file_scope_variable`
- ⏭️ NOT RUN `testcases_filesystem_state.c` → `test_sig31_c_fail_testcases_filesystem_state`
- ⏭️ NOT RUN `testcases_global_array_access.c` → `test_sig31_c_fail_testcases_global_array_access`
- ⏭️ NOT RUN `testcases_global_cache_access.c` → `test_sig31_c_fail_testcases_global_cache_access`
- ⏭️ NOT RUN `testcases_global_config_access.c` → `test_sig31_c_fail_testcases_global_config_access`
- ⏭️ NOT RUN `testcases_global_flags_access.c` → `test_sig31_c_fail_testcases_global_flags_access`
- ⏭️ NOT RUN `testcases_global_registry_access.c` → `test_sig31_c_fail_testcases_global_registry_access`
- ⏭️ NOT RUN `testcases_linked_list_access.c` → `test_sig31_c_fail_testcases_linked_list_access`
- ⏭️ NOT RUN `testcases_logging_state_access.c` → `test_sig31_c_fail_testcases_logging_state_access`
- ⏭️ NOT RUN `testcases_network_connection_state.c` → `test_sig31_c_fail_testcases_network_connection_state`
- ⏭️ NOT RUN `testcases_performance_metrics.c` → `test_sig31_c_fail_testcases_performance_metrics`
- ⏭️ NOT RUN `testcases_process_control_structures.c` → `test_sig31_c_fail_testcases_process_control_structures`
- ⏭️ NOT RUN `testcases_resource_pool_access.c` → `test_sig31_c_fail_testcases_resource_pool_access`
- ⏭️ NOT RUN `testcases_shared_counters.c` → `test_sig31_c_fail_testcases_shared_counters`
- ⏭️ NOT RUN `testcases_shared_data_access.c` → `test_sig31_c_fail_testcases_shared_data_access`
- ⏭️ NOT RUN `testcases_shared_hash_table.c` → `test_sig31_c_fail_testcases_shared_hash_table`
- ⏭️ NOT RUN `testcases_shared_queue_system.c` → `test_sig31_c_fail_testcases_shared_queue_system`
- ⏭️ NOT RUN `testcases_signal_handler_state.c` → `test_sig31_c_fail_testcases_signal_handler_state`
- ⏭️ NOT RUN `testcases_state_machine_access.c` → `test_sig31_c_fail_testcases_state_machine_access`
- ⏭️ NOT RUN `testcases_static_variable_access.c` → `test_sig31_c_fail_testcases_static_variable_access`
- ⏭️ NOT RUN `testcases_string_buffer_access.c` → `test_sig31_c_fail_testcases_string_buffer_access`
- ⏭️ NOT RUN `testcases_thread_shared_data.c` → `test_sig31_c_fail_testcases_thread_shared_data`
- ⏭️ NOT RUN `testcases_timing_information.c` → `test_sig31_c_fail_testcases_timing_information`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_sig31_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_atomic_operations_only.c` → `test_sig31_c_pass_testcases_atomic_operations_only`
- ⏭️ NOT RUN `testcases_flag_only_handler.c` → `test_sig31_c_pass_testcases_flag_only_handler`
- ⏭️ NOT RUN `testcases_isolated_handler_state.c` → `test_sig31_c_pass_testcases_isolated_handler_state`
- ⏭️ NOT RUN `testcases_minimal_signal_handling.c` → `test_sig31_c_pass_testcases_minimal_signal_handling`
- ⏭️ NOT RUN `testcases_no_shared_access.c` → `test_sig31_c_pass_testcases_no_shared_access`
- ⏭️ NOT RUN `testcases_self_pipe_trick.c` → `test_sig31_c_pass_testcases_self_pipe_trick`
- ⏭️ NOT RUN `testcases_sig_atomic_only.c` → `test_sig31_c_pass_testcases_sig_atomic_only`
- ⏭️ NOT RUN `testcases_signal_masking_protection.c` → `test_sig31_c_pass_testcases_signal_masking_protection`
- ⏭️ NOT RUN `testcases_signal_synchronization.c` → `test_sig31_c_pass_testcases_signal_synchronization`
- ⏭️ NOT RUN `testcases_signalfd_safe_access.c` → `test_sig31_c_pass_testcases_signalfd_safe_access`
- ⏭️ NOT RUN `wiki_lock_free_atomic_access.c` → `test_sig31_c_pass_wiki_lock_free_atomic_access`
- ⏭️ NOT RUN `wiki_writingvolatile_sig_atomic_t.c` → `test_sig31_c_pass_wiki_writingvolatile_sig_atomic_t`

---

### 🔶 SIG02-C - Not Implemented (has tests)

<a id="rule-sig02c"></a>

**Title:** Avoid using signals to implement normal functionality

**Description:** Avoid using signals to implement normal functionality. Signal handlers are
severely limited in the actions they can perform in a portably secure manner.
Their use should be reserved for abnormal events that can be serviced by little
more than logging. This noncompliant code example uses signals as a means to
pass state changes around in a multithreaded environment: /* THREAD 1 */ int
do_work(void) { /* ... */ kill(THR2_PID, SIGUSR1); } /* THREAD 2 */ volatile
sig_atomic_t flag; void sigusr1_handler(int signum) { flag = 1; } int
wait_and_work(void) { flag = 0; while (!flag) {} /* ... */ }

**Test Coverage:** 46 tests (34 fail, 12 pass)

**Test Results:** 0/46 passed (0.0%), 46 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_signal_app_startup.c` → `test_sig02_c_fail_testcases_signal_app_startup`
- ⏭️ NOT RUN `testcases_signal_backup_maintenance.c` → `test_sig02_c_fail_testcases_signal_backup_maintenance`
- ⏭️ NOT RUN `testcases_signal_cache_management.c` → `test_sig02_c_fail_testcases_signal_cache_management`
- ⏭️ NOT RUN `testcases_signal_communication.c` → `test_sig02_c_fail_testcases_signal_communication`
- ⏭️ NOT RUN `testcases_signal_config_updates.c` → `test_sig02_c_fail_testcases_signal_config_updates`
- ⏭️ NOT RUN `testcases_signal_data_transfer.c` → `test_sig02_c_fail_testcases_signal_data_transfer`
- ⏭️ NOT RUN `testcases_signal_database_operations.c` → `test_sig02_c_fail_testcases_signal_database_operations`
- ⏭️ NOT RUN `testcases_signal_debugging_profiling.c` → `test_sig02_c_fail_testcases_signal_debugging_profiling`
- ⏭️ NOT RUN `testcases_signal_driven_loop.c` → `test_sig02_c_fail_testcases_signal_driven_loop`
- ⏭️ NOT RUN `testcases_signal_event_system.c` → `test_sig02_c_fail_testcases_signal_event_system`
- ⏭️ NOT RUN `testcases_signal_file_monitoring.c` → `test_sig02_c_fail_testcases_signal_file_monitoring`
- ⏭️ NOT RUN `testcases_signal_health_monitoring.c` → `test_sig02_c_fail_testcases_signal_health_monitoring`
- ⏭️ NOT RUN `testcases_signal_job_scheduling.c` → `test_sig02_c_fail_testcases_signal_job_scheduling`
- ⏭️ NOT RUN `testcases_signal_load_balancing.c` → `test_sig02_c_fail_testcases_signal_load_balancing`
- ⏭️ NOT RUN `testcases_signal_logging_auditing.c` → `test_sig02_c_fail_testcases_signal_logging_auditing`
- ⏭️ NOT RUN `testcases_signal_messaging_system.c` → `test_sig02_c_fail_testcases_signal_messaging_system`
- ⏭️ NOT RUN `testcases_signal_metrics_collection.c` → `test_sig02_c_fail_testcases_signal_metrics_collection`
- ⏭️ NOT RUN `testcases_signal_network_protocol.c` → `test_sig02_c_fail_testcases_signal_network_protocol`
- ⏭️ NOT RUN `testcases_signal_notification_system.c` → `test_sig02_c_fail_testcases_signal_notification_system`
- ⏭️ NOT RUN `testcases_signal_rate_limiting.c` → `test_sig02_c_fail_testcases_signal_rate_limiting`
- ⏭️ NOT RUN `testcases_signal_resource_management.c` → `test_sig02_c_fail_testcases_signal_resource_management`
- ⏭️ NOT RUN `testcases_signal_service_discovery.c` → `test_sig02_c_fail_testcases_signal_service_discovery`
- ⏭️ NOT RUN `testcases_signal_session_management.c` → `test_sig02_c_fail_testcases_signal_session_management`
- ⏭️ NOT RUN `testcases_signal_state_machine.c` → `test_sig02_c_fail_testcases_signal_state_machine`
- ⏭️ NOT RUN `testcases_signal_synchronization.c` → `test_sig02_c_fail_testcases_signal_synchronization`
- ⏭️ NOT RUN `testcases_signal_task_queue.c` → `test_sig02_c_fail_testcases_signal_task_queue`
- ⏭️ NOT RUN `testcases_signal_thread_coordination.c` → `test_sig02_c_fail_testcases_signal_thread_coordination`
- ⏭️ NOT RUN `testcases_signal_timing_scheduler.c` → `test_sig02_c_fail_testcases_signal_timing_scheduler`
- ⏭️ NOT RUN `testcases_signal_ui_events.c` → `test_sig02_c_fail_testcases_signal_ui_events`
- ⏭️ NOT RUN `testcases_signal_workflow_orchestration.c` → `test_sig02_c_fail_testcases_signal_workflow_orchestration`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_sig02_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_sig02_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3_2.c` → `test_sig02_c_fail_wiki_noncompliant_3_2`
- ⏭️ NOT RUN `wiki_noncompliant_4_3.c` → `test_sig02_c_fail_wiki_noncompliant_4_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_condition_variable_sync.c` → `test_sig02_c_pass_testcases_condition_variable_sync`
- ⏭️ NOT RUN `testcases_event_driven_architecture.c` → `test_sig02_c_pass_testcases_event_driven_architecture`
- ⏭️ NOT RUN `testcases_message_queue_ipc.c` → `test_sig02_c_pass_testcases_message_queue_ipc`
- ⏭️ NOT RUN `testcases_pipe_communication.c` → `test_sig02_c_pass_testcases_pipe_communication`
- ⏭️ NOT RUN `testcases_proper_threading.c` → `test_sig02_c_pass_testcases_proper_threading`
- ⏭️ NOT RUN `testcases_select_poll_io.c` → `test_sig02_c_pass_testcases_select_poll_io`
- ⏭️ NOT RUN `testcases_shared_memory_communication.c` → `test_sig02_c_pass_testcases_shared_memory_communication`
- ⏭️ NOT RUN `testcases_signal_error_only.c` → `test_sig02_c_pass_testcases_signal_error_only`
- ⏭️ NOT RUN `testcases_socket_communication.c` → `test_sig02_c_pass_testcases_socket_communication`
- ⏭️ NOT RUN `testcases_timer_polling_operations.c` → `test_sig02_c_pass_testcases_timer_polling_operations`
- ⏭️ NOT RUN `wiki_posix.c` → `test_sig02_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_windows.c` → `test_sig02_c_pass_wiki_windows`

---

### 🔶 SIG35-C - Not Implemented (has tests)

<a id="rule-sig35c"></a>

**Title:** Do not return from a computational exception signal handler

**Description:** According to the C Standard, 7.14.1.1 paragraph 3 [ISO/IEC 9899:2024], if a
signal handler returns when it has been entered as a result of a computational
exception (that is, with the value of its argument ofSIGFPE,SIGILL,SIGSEGV, or
any other implementation-defined value corresponding to such an exception)
returns, then the behavior isundefined. (Seeundefined behavior 130.) The
Portable Operating System Interface (POSIX®), Base Specifications, Issue 7 [IEEE
Std 1003.1:2013], addsSIGBUSto the list of computational exception signal
handlers: Do not return fromSIGFPE,SIGILL,SIGSEGV, or any other implementation-
defined value corresponding to a computational exception, such asSIGBUSon POSIX
systems, regardless of how the signal was generated.

**Test Coverage:** 43 tests (31 fail, 12 pass)

**Test Results:** 0/43 passed (0.0%), 43 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_cleanup_and_return.c` → `test_sig35_c_fail_testcases_cleanup_and_return`
- ⏭️ NOT RUN `testcases_complex_logic_return.c` → `test_sig35_c_fail_testcases_complex_logic_return`
- ⏭️ NOT RUN `testcases_conditional_return.c` → `test_sig35_c_fail_testcases_conditional_return`
- ⏭️ NOT RUN `testcases_continue_execution_return.c` → `test_sig35_c_fail_testcases_continue_execution_return`
- ⏭️ NOT RUN `testcases_debug_operation_return.c` → `test_sig35_c_fail_testcases_debug_operation_return`
- ⏭️ NOT RUN `testcases_fix_and_return.c` → `test_sig35_c_fail_testcases_fix_and_return`
- ⏭️ NOT RUN `testcases_flag_and_return.c` → `test_sig35_c_fail_testcases_flag_and_return`
- ⏭️ NOT RUN `testcases_function_call_return.c` → `test_sig35_c_fail_testcases_function_call_return`
- ⏭️ NOT RUN `testcases_global_state_return.c` → `test_sig35_c_fail_testcases_global_state_return`
- ⏭️ NOT RUN `testcases_ignore_exception.c` → `test_sig35_c_fail_testcases_ignore_exception`
- ⏭️ NOT RUN `testcases_implementation_defined_return.c` → `test_sig35_c_fail_testcases_implementation_defined_return`
- ⏭️ NOT RUN `testcases_io_operation_return.c` → `test_sig35_c_fail_testcases_io_operation_return`
- ⏭️ NOT RUN `testcases_library_call_return.c` → `test_sig35_c_fail_testcases_library_call_return`
- ⏭️ NOT RUN `testcases_log_and_return.c` → `test_sig35_c_fail_testcases_log_and_return`
- ⏭️ NOT RUN `testcases_memory_alloc_return.c` → `test_sig35_c_fail_testcases_memory_alloc_return`
- ⏭️ NOT RUN `testcases_multiple_exceptions_return.c` → `test_sig35_c_fail_testcases_multiple_exceptions_return`
- ⏭️ NOT RUN `testcases_nested_handler_return.c` → `test_sig35_c_fail_testcases_nested_handler_return`
- ⏭️ NOT RUN `testcases_process_state_return.c` → `test_sig35_c_fail_testcases_process_state_return`
- ⏭️ NOT RUN `testcases_program_counter_return.c` → `test_sig35_c_fail_testcases_program_counter_return`
- ⏭️ NOT RUN `testcases_recovery_attempt_return.c` → `test_sig35_c_fail_testcases_recovery_attempt_return`
- ⏭️ NOT RUN `testcases_return_from_exception.c` → `test_sig35_c_fail_testcases_return_from_exception`
- ⏭️ NOT RUN `testcases_shared_data_return.c` → `test_sig35_c_fail_testcases_shared_data_return`
- ⏭️ NOT RUN `testcases_sigbus_return.c` → `test_sig35_c_fail_testcases_sigbus_return`
- ⏭️ NOT RUN `testcases_sigfpe_return.c` → `test_sig35_c_fail_testcases_sigfpe_return`
- ⏭️ NOT RUN `testcases_sigill_return.c` → `test_sig35_c_fail_testcases_sigill_return`
- ⏭️ NOT RUN `testcases_signal_delivery_return.c` → `test_sig35_c_fail_testcases_signal_delivery_return`
- ⏭️ NOT RUN `testcases_signal_mask_return.c` → `test_sig35_c_fail_testcases_signal_mask_return`
- ⏭️ NOT RUN `testcases_sigsegv_return.c` → `test_sig35_c_fail_testcases_sigsegv_return`
- ⏭️ NOT RUN `testcases_sigtrap_return.c` → `test_sig35_c_fail_testcases_sigtrap_return`
- ⏭️ NOT RUN `testcases_stack_unwind_return.c` → `test_sig35_c_fail_testcases_stack_unwind_return`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_sig35_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_abort_on_exception.c` → `test_sig35_c_pass_testcases_abort_on_exception`
- ⏭️ NOT RUN `testcases_async_safe_termination.c` → `test_sig35_c_pass_testcases_async_safe_termination`
- ⏭️ NOT RUN `testcases_error_logging_termination.c` → `test_sig35_c_pass_testcases_error_logging_termination`
- ⏭️ NOT RUN `testcases_exit_family_only.c` → `test_sig35_c_pass_testcases_exit_family_only`
- ⏭️ NOT RUN `testcases_exit_on_exception.c` → `test_sig35_c_pass_testcases_exit_on_exception`
- ⏭️ NOT RUN `testcases_exit_termination.c` → `test_sig35_c_pass_testcases_exit_termination`
- ⏭️ NOT RUN `testcases_infinite_loop_termination.c` → `test_sig35_c_pass_testcases_infinite_loop_termination`
- ⏭️ NOT RUN `testcases_quick_exit_termination.c` → `test_sig35_c_pass_testcases_quick_exit_termination`
- ⏭️ NOT RUN `testcases_safe_cleanup_termination.c` → `test_sig35_c_pass_testcases_safe_cleanup_termination`
- ⏭️ NOT RUN `testcases_signal_safe_pattern.c` → `test_sig35_c_pass_testcases_signal_safe_pattern`
- ⏭️ NOT RUN `testcases_state_save_termination.c` → `test_sig35_c_pass_testcases_state_save_termination`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_sig35_c_pass_wiki_compliant_1`

---

### ✅ SIG30-C - Implemented

<a id="rule-sig30c"></a>

**Title:** Call only asynchronous-safe functions within signal handlers

**Description:** Call onlyasynchronous-safe functionswithin signal handlers. Forstrictly
conformingprograms, only the C standard library
functionsabort(),_Exit(),quick_exit(), andsignal()can be safely called from
within a signal handler. The C Standard, 7.14.1.1, paragraph 5 [ISO/IEC
9899:2024], states that if the signal occurs other than as the result of calling
theabort()orraise()function, the behavior isundefinedif Implementations may
define a list of additional asynchronous-safe functions. These functions can
also be called within a signal handler. This restriction applies to library
functions as well as application-defined functions.

**Test Coverage:** 47 tests (33 fail, 14 pass)

**Test Results:** 0/47 passed (0.0%), 47 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_unsafe_atexit_functions.c` → `test_sig30_c_fail_testcases_unsafe_atexit_functions`
- ⏭️ NOT RUN `testcases_unsafe_complex_handler.c` → `test_sig30_c_fail_testcases_unsafe_complex_handler`
- ⏭️ NOT RUN `testcases_unsafe_data_structures.c` → `test_sig30_c_fail_testcases_unsafe_data_structures`
- ⏭️ NOT RUN `testcases_unsafe_directory_ops.c` → `test_sig30_c_fail_testcases_unsafe_directory_ops`
- ⏭️ NOT RUN `testcases_unsafe_environment_functions.c` → `test_sig30_c_fail_testcases_unsafe_environment_functions`
- ⏭️ NOT RUN `testcases_unsafe_error_handling.c` → `test_sig30_c_fail_testcases_unsafe_error_handling`
- ⏭️ NOT RUN `testcases_unsafe_file_io.c` → `test_sig30_c_fail_testcases_unsafe_file_io`
- ⏭️ NOT RUN `testcases_unsafe_formatting_functions.c` → `test_sig30_c_fail_testcases_unsafe_formatting_functions`
- ⏭️ NOT RUN `testcases_unsafe_library_calls.c` → `test_sig30_c_fail_testcases_unsafe_library_calls`
- ⏭️ NOT RUN `testcases_unsafe_locale_functions.c` → `test_sig30_c_fail_testcases_unsafe_locale_functions`
- ⏭️ NOT RUN `testcases_unsafe_logging_functions.c` → `test_sig30_c_fail_testcases_unsafe_logging_functions`
- ⏭️ NOT RUN `testcases_unsafe_malloc_free.c` → `test_sig30_c_fail_testcases_unsafe_malloc_free`
- ⏭️ NOT RUN `testcases_unsafe_math_functions.c` → `test_sig30_c_fail_testcases_unsafe_math_functions`
- ⏭️ NOT RUN `testcases_unsafe_message_queues.c` → `test_sig30_c_fail_testcases_unsafe_message_queues`
- ⏭️ NOT RUN `testcases_unsafe_network_functions.c` → `test_sig30_c_fail_testcases_unsafe_network_functions`
- ⏭️ NOT RUN `testcases_unsafe_printf.c` → `test_sig30_c_fail_testcases_unsafe_printf`
- ⏭️ NOT RUN `testcases_unsafe_process_control.c` → `test_sig30_c_fail_testcases_unsafe_process_control`
- ⏭️ NOT RUN `testcases_unsafe_random_functions.c` → `test_sig30_c_fail_testcases_unsafe_random_functions`
- ⏭️ NOT RUN `testcases_unsafe_regex_functions.c` → `test_sig30_c_fail_testcases_unsafe_regex_functions`
- ⏭️ NOT RUN `testcases_unsafe_resource_limits.c` → `test_sig30_c_fail_testcases_unsafe_resource_limits`
- ⏭️ NOT RUN `testcases_unsafe_shared_memory.c` → `test_sig30_c_fail_testcases_unsafe_shared_memory`
- ⏭️ NOT RUN `testcases_unsafe_signal_manipulation.c` → `test_sig30_c_fail_testcases_unsafe_signal_manipulation`
- ⏭️ NOT RUN `testcases_unsafe_stdio_extensions.c` → `test_sig30_c_fail_testcases_unsafe_stdio_extensions`
- ⏭️ NOT RUN `testcases_unsafe_string_functions.c` → `test_sig30_c_fail_testcases_unsafe_string_functions`
- ⏭️ NOT RUN `testcases_unsafe_terminal_functions.c` → `test_sig30_c_fail_testcases_unsafe_terminal_functions`
- ⏭️ NOT RUN `testcases_unsafe_thread_sync.c` → `test_sig30_c_fail_testcases_unsafe_thread_sync`
- ⏭️ NOT RUN `testcases_unsafe_time_functions.c` → `test_sig30_c_fail_testcases_unsafe_time_functions`
- ⏭️ NOT RUN `testcases_unsafe_timer_functions.c` → `test_sig30_c_fail_testcases_unsafe_timer_functions`
- ⏭️ NOT RUN `testcases_unsafe_user_group_functions.c` → `test_sig30_c_fail_testcases_unsafe_user_group_functions`
- ⏭️ NOT RUN `testcases_unsafe_wide_char_functions.c` → `test_sig30_c_fail_testcases_unsafe_wide_char_functions`
- ⏭️ NOT RUN `wiki_longjmp.c` → `test_sig30_c_fail_wiki_longjmp`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_sig30_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_raise.c` → `test_sig30_c_fail_wiki_raise`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_async_safe_logging.c` → `test_sig30_c_pass_testcases_async_safe_logging`
- ⏭️ NOT RUN `testcases_atomic_operations_only.c` → `test_sig30_c_pass_testcases_atomic_operations_only`
- ⏭️ NOT RUN `testcases_errno_preservation.c` → `test_sig30_c_pass_testcases_errno_preservation`
- ⏭️ NOT RUN `testcases_immediate_exit_handler.c` → `test_sig30_c_pass_testcases_immediate_exit_handler`
- ⏭️ NOT RUN `testcases_minimal_handler.c` → `test_sig30_c_pass_testcases_minimal_handler`
- ⏭️ NOT RUN `testcases_safe_flag_handler.c` → `test_sig30_c_pass_testcases_safe_flag_handler`
- ⏭️ NOT RUN `testcases_self_pipe_trick.c` → `test_sig30_c_pass_testcases_self_pipe_trick`
- ⏭️ NOT RUN `testcases_signal_counting.c` → `test_sig30_c_pass_testcases_signal_counting`
- ⏭️ NOT RUN `testcases_signal_mask_safe.c` → `test_sig30_c_pass_testcases_signal_mask_safe`
- ⏭️ NOT RUN `testcases_signalfd_safe.c` → `test_sig30_c_pass_testcases_signalfd_safe`
- ⏭️ NOT RUN `testcases_write_only_handler.c` → `test_sig30_c_pass_testcases_write_only_handler`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_sig30_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_sig30_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_compliant_3.c` → `test_sig30_c_pass_wiki_compliant_3`

---

## Category: STR

<a id="category-str"></a>

**Implementation Status:** 3 / 16 rules (18.8%)

### 🔶 STR05-C - Not Implemented (has tests)

<a id="rule-str05c"></a>

**Title:** Use pointers to const when referring to string literals

**Description:** The type of a narrow string literal is an array ofchar, and the type of a wide
string literal is an array ofwchar_t. However, string literals (of both types)
are notionally constant and should consequently be protected
byconstqualification. This recommendation is a specialization ofDCL00-C. Const-
qualify immutable objectsand also supportsSTR30-C. Do not attempt to modify
string literals. Addingconstqualification may propagate through a program;
asconstqualifiers are added, still more become necessary. This phenomenon is
sometimes calledconst-poisoning. Const-poisoning can frequently lead to
violations ofEXP05-C. Do not cast away a const qualification.
Althoughconstqualification is a good idea, the costs may outweigh the value in
the remediation of existing code. In this noncompliant code example,
theconstkeyword has been omitted:

**Test Coverage:** 4 tests (2 fail, 2 pass)

**Test Results:** 0/4 passed (0.0%), 4 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_narrow_string_literal.c` → `test_str05_c_fail_wiki_narrow_string_literal`
- ⏭️ NOT RUN `wiki_wide_string_literal.c` → `test_str05_c_fail_wiki_wide_string_literal`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_immutable_strings.c` → `test_str05_c_pass_wiki_immutable_strings`
- ⏭️ NOT RUN `wiki_mutable_strings.c` → `test_str05_c_pass_wiki_mutable_strings`

---

### 🔶 STR10-C - Not Implemented (has tests)

<a id="rule-str10c"></a>

**Title:** Do not concatenate different type of string literals

**Description:** According toMISRA 2008, concatenation of wide and narrow string literals leads
toundefined behavior. This was once considered implicitly undefined behavior
until C90 [ISO/IEC 9899:1990]. However, C99 defined this behavior [ISO/IEC
9899:1999], and C11 further explains in subclause 6.4.5, paragraph 5 [ISO/IEC
9899:2011]: Nonetheless, it is recommended that string literals that are
concatenated should all be the same type so as not to rely onimplementation-
defined behaviororundefined behaviorif compiled on a platform that supports only
C90. This noncompliant code example concatenates wide and narrow string
literals. Although the behavior is undefined in C90, the programmer probably
intended to create a wide string literal.

**Test Coverage:** 3 tests (1 fail, 2 pass)

**Test Results:** 0/3 passed (0.0%), 3 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_c90.c` → `test_str10_c_fail_wiki_c90`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_c90_narrow_string_literals.c` → `test_str10_c_pass_wiki_c90_narrow_string_literals`
- ⏭️ NOT RUN `wiki_c90_wide_string_literals.c` → `test_str10_c_pass_wiki_c90_wide_string_literals`

---

### 🔶 STR37-C - Not Implemented (has tests)

<a id="rule-str37c"></a>

**Title:** Arguments to character-handling functions must be representable as an unsigned char

**Description:** According to the C Standard, 7.4.1 paragraph 1 [ISO/IEC 9899:2024], See
alsoundefined behavior 112. This rule is applicable only to code that runs on
platforms where thechardata type is defined to have the same range,
representation, and behavior assigned char.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_str37_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_str37_c_pass_wiki_compliant_1`

---

### ✅ STR38-C - Implemented

<a id="rule-str38c"></a>

**Title:** Do not confuse narrow and wide character strings and functions

**Description:** Passing narrow string arguments to wide string functions or wide string
arguments to narrow string functions can lead tounexpectedandundefined behavior
151. Scaling problems are likely because of the difference in size between wide
and narrow characters. (SeeARR39-C. Do not add or subtract a scaled integer to a
pointer.)Because wide strings are terminated by a null wide character and can
contain null bytes, determining the length is also problematic.
Becausewchar_tandcharare distinct types, many compilers will produce a warning
diagnostic if an inappropriate function is used. (SeeMSC00-C. Compile cleanly at
high warning levels.) This noncompliant code example incorrectly uses
thestrncpy()function in an attempt to copy up to 10 wide characters. However,
because wide characters can contain null bytes, the copy operation may end
earlier than anticipated, resulting in the truncation of the wide string.

**Test Coverage:** 5 tests (3 fail, 2 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_narrow_strings_with_wide_string_functions.c` → `test_str38_c_fail_wiki_narrow_strings_with_wide_string_functions`
- ⏭️ NOT RUN `wiki_strlen.c` → `test_str38_c_fail_wiki_strlen`
- ⏭️ NOT RUN `wiki_wide_strings_with_narrow_string_functions.c` → `test_str38_c_fail_wiki_wide_strings_with_narrow_string_functions`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_str38_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_str38_c_pass_wiki_compliant_2`

---

### 🔶 STR09-C - Not Implemented (has tests)

<a id="rule-str09c"></a>

**Title:** Don't assume numeric values for expressions with type plain character

**Description:** For portable applications, use only the assignment=operator, the equality
operators==and!=, and the unary&operator on plain-character-typed or plain-wide-
character-typed expressions. This practice is recommended because the C Standard
requires only the digit characters (0–9) to have consecutive numerical values.
Consequently, operations that rely on expected values for plain-character- or
plain-wide-character-typed expressions can lead to unexpected behavior. However,
because of the requirement for digit characters, other operators can be used for
them according to the following restrictions:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_str09_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_str09_c_pass_wiki_compliant_1`

---

### ✅ STR30-C - Implemented

<a id="rule-str30c"></a>

**Title:** Do not attempt to modify string literals

**Description:** According to the C Standard, 6.4.5, paragraph 3 [ISO/IEC 9899:2024]: At compile
time, string literals are used to create an array of static storage duration of
sufficient length to contain the character sequence and a terminating null
character. String literals are usually referred to by a pointer to (or array of)
characters. Ideally, they should be assigned only to pointers to (or arrays
of)const charorconst wchar_t. It is unspecified whether these arrays of string
literals are distinct from each other. The behavior isundefinedif a program
attempts to modify any portion of a string literal. Modifying a string literal
frequently results in an access violation because string literals are typically
stored in read-only memory. (Seeundefined behavior 32.) Avoid assigning a string
literal to a pointer to non-constor casting a string literal to a pointer to
non-const. For the purposes of this rule, a pointer to (or array
of)constcharacters must be treated as a string literal. Similarly, the returned
value of the following library functions must be treated as a string literal if
the first argument is a string literal:

**Test Coverage:** 46 tests (33 fail, 13 pass)

**Test Results:** 0/46 passed (0.0%), 46 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_array_notation_modify.c` → `test_str30_c_fail_testcases_array_notation_modify`
- ⏭️ NOT RUN `testcases_array_of_pointers_modify.c` → `test_str30_c_fail_testcases_array_of_pointers_modify`
- ⏭️ NOT RUN `testcases_cast_away_const.c` → `test_str30_c_fail_testcases_cast_away_const`
- ⏭️ NOT RUN `testcases_direct_modify.c` → `test_str30_c_fail_testcases_direct_modify`
- ⏭️ NOT RUN `testcases_double_pointer_modify.c` → `test_str30_c_fail_testcases_double_pointer_modify`
- ⏭️ NOT RUN `testcases_fgets_to_literal.c` → `test_str30_c_fail_testcases_fgets_to_literal`
- ⏭️ NOT RUN `testcases_function_param_modify.c` → `test_str30_c_fail_testcases_function_param_modify`
- ⏭️ NOT RUN `testcases_gets_to_literal.c` → `test_str30_c_fail_testcases_gets_to_literal`
- ⏭️ NOT RUN `testcases_global_pointer_modify.c` → `test_str30_c_fail_testcases_global_pointer_modify`
- ⏭️ NOT RUN `testcases_loop_modify.c` → `test_str30_c_fail_testcases_loop_modify`
- ⏭️ NOT RUN `testcases_memcpy_to_literal.c` → `test_str30_c_fail_testcases_memcpy_to_literal`
- ⏭️ NOT RUN `testcases_memmove_literal.c` → `test_str30_c_fail_testcases_memmove_literal`
- ⏭️ NOT RUN `testcases_memset_literal.c` → `test_str30_c_fail_testcases_memset_literal`
- ⏭️ NOT RUN `testcases_mkstemp_literal.c` → `test_str30_c_fail_testcases_mkstemp_literal`
- ⏭️ NOT RUN `testcases_pointer_increment_modify.c` → `test_str30_c_fail_testcases_pointer_increment_modify`
- ⏭️ NOT RUN `testcases_return_modified_literal.c` → `test_str30_c_fail_testcases_return_modified_literal`
- ⏭️ NOT RUN `testcases_scanf_to_literal.c` → `test_str30_c_fail_testcases_scanf_to_literal`
- ⏭️ NOT RUN `testcases_snprintf_to_literal.c` → `test_str30_c_fail_testcases_snprintf_to_literal`
- ⏭️ NOT RUN `testcases_sprintf_to_literal.c` → `test_str30_c_fail_testcases_sprintf_to_literal`
- ⏭️ NOT RUN `testcases_strcat_to_literal.c` → `test_str30_c_fail_testcases_strcat_to_literal`
- ⏭️ NOT RUN `testcases_strchr_modify.c` → `test_str30_c_fail_testcases_strchr_modify`
- ⏭️ NOT RUN `testcases_strcpy_to_literal.c` → `test_str30_c_fail_testcases_strcpy_to_literal`
- ⏭️ NOT RUN `testcases_strncat_to_literal.c` → `test_str30_c_fail_testcases_strncat_to_literal`
- ⏭️ NOT RUN `testcases_strncpy_to_literal.c` → `test_str30_c_fail_testcases_strncpy_to_literal`
- ⏭️ NOT RUN `testcases_strpbrk_modify.c` → `test_str30_c_fail_testcases_strpbrk_modify`
- ⏭️ NOT RUN `testcases_strrchr_modify.c` → `test_str30_c_fail_testcases_strrchr_modify`
- ⏭️ NOT RUN `testcases_strstr_modify.c` → `test_str30_c_fail_testcases_strstr_modify`
- ⏭️ NOT RUN `testcases_strtok_literal.c` → `test_str30_c_fail_testcases_strtok_literal`
- ⏭️ NOT RUN `testcases_struct_member_modify.c` → `test_str30_c_fail_testcases_struct_member_modify`
- ⏭️ NOT RUN `testcases_ternary_modify.c` → `test_str30_c_fail_testcases_ternary_modify`
- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_str30_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_posix.c` → `test_str30_c_fail_wiki_posix`
- ⏭️ NOT RUN `wiki_result_ofstrrchr.c` → `test_str30_c_fail_wiki_result_ofstrrchr`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_array_initialization.c` → `test_str30_c_pass_testcases_array_initialization`
- ⏭️ NOT RUN `testcases_array_of_const_pointers.c` → `test_str30_c_pass_testcases_array_of_const_pointers`
- ⏭️ NOT RUN `testcases_const_pointer.c` → `test_str30_c_pass_testcases_const_pointer`
- ⏭️ NOT RUN `testcases_function_param_const.c` → `test_str30_c_pass_testcases_function_param_const`
- ⏭️ NOT RUN `testcases_global_const_pointer.c` → `test_str30_c_pass_testcases_global_const_pointer`
- ⏭️ NOT RUN `testcases_mkstemp_array.c` → `test_str30_c_pass_testcases_mkstemp_array`
- ⏭️ NOT RUN `testcases_strchr_no_modify.c` → `test_str30_c_pass_testcases_strchr_no_modify`
- ⏭️ NOT RUN `testcases_strrchr_const_result.c` → `test_str30_c_pass_testcases_strrchr_const_result`
- ⏭️ NOT RUN `testcases_strtok_array.c` → `test_str30_c_pass_testcases_strtok_array`
- ⏭️ NOT RUN `testcases_struct_const_member.c` → `test_str30_c_pass_testcases_struct_const_member`
- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_str30_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_posix.c` → `test_str30_c_pass_wiki_posix`
- ⏭️ NOT RUN `wiki_result_ofstrrchr.c` → `test_str30_c_pass_wiki_result_ofstrrchr`

---

### 🔶 STR06-C - Not Implemented (has tests)

<a id="rule-str06c"></a>

**Title:** Do not assume that strtok() leaves the parse string unchanged

**Description:** The C functionstrtok()is a string tokenization function that takes two
arguments: an initial string to be parsed and aconst-qualified character
delimiter. It returns a pointer to the first character of a token or to a null
pointer if there is no token. The first timestrtok()is called, the string is
parsed into tokens and a character delimiter. Thestrtok()function parses the
string up to the first instance of the delimiter character, replaces the
character in place with a null byte ('\0'), and returns the address of the first
character in the token. Subsequent calls tostrtok()begin parsing immediately
after the most recently placed null character. Becausestrtok()modifies the
initial string to be parsed, the string is subsequently unsafe and cannot be
used in its original form. If you need to preserve the original string, copy it
into a buffer and pass the address of the buffer tostrtok()instead of the
original string.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_str06_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_str06_c_pass_wiki_compliant_1`

---

### 🔶 STR04-C - Not Implemented (has tests)

<a id="rule-str04c"></a>

**Title:** Use plain char for characters in the basic character set

**Description:** There are threecharacter types:char,signed char, andunsigned char. Compilers
have the latitude to definecharto have the same range, representation, and
behavior as eithersigned charorunsigned char. Irrespective of the choice
made,charis a separate type from the other two and is not compatible with
either. For characters in the basic character set, it does not matter which data
type is used, except for type compatibility. Consequently, it is best to use
plaincharfor character data for compatibility with standard string-handling
functions. In most cases, the only portable operators on plainchartypes are
assignment and equality operators (=,==,!=). An exception is the translation to
and from digits. For example, if thecharcis a digit,c - '0'is a value between 0
and 9.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_str04_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_str04_c_pass_wiki_compliant_1`

---

### 🔶 STR11-C - Not Implemented (has tests)

<a id="rule-str11c"></a>

**Title:** Do not specify the bound of a character array initialized with a string literal

**Description:** The C Standard allows an array variable to be declared both with a bound index
and with an initialization literal. The initialization literal also implies an
array size in the number of elements specified. For strings, the size specified
by a string literal is the number of characters in the literal plus one for the
terminating null character. It is common for an array variable to be initialized
by a string literal and declared with an explicit bound that matches the number
of characters in the string literal. Subclause 6.7.9, paragraph 14, of the C
Standard [ISO/IEC 9899:2011], says: However, if the string is intended to be
used as a null-terminated byte string, then the array will have one too few
characters to hold the string because it does not account for the terminating
null character. Such a sequence of characters has limited utility and has the
potential to causevulnerabilitiesif a null-terminated byte string is assumed.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_str11_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_str11_c_pass_wiki_compliant_1`

---

### 🔶 STR32-C - Not Implemented (has tests)

<a id="rule-str32c"></a>

**Title:** Do not pass a non-null-terminated character sequence to a library function that expects a string

**Description:** Many library functions accept a string or wide string argument with the
constraint that the string they receive is properly null-terminated. Passing a
character sequence or wide character sequence that is not null-terminated to
such a function can result in accessing memory that is outside the bounds of the
object. Do not pass a character sequence or wide character sequence that is not
null-terminated to a library function that expects a string or wide string
argument. This code example is noncompliant because the character
sequencec_strwill not be null-terminated when passed as an argument
toprintf().(SeeSTR11-C. Do not specify the bound of a character array
initialized with a string literalon how to properly initialize character
arrays.) #include <stdio.h> void func(void) { char c_str[3] = "abc";
printf("%s\n", c_str); }

**Test Coverage:** 7 tests (3 fail, 4 pass)

**Test Results:** 0/7 passed (0.0%), 7 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_str32_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_str32_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_strncpy.c` → `test_str32_c_fail_wiki_strncpy`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_str32_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_str32_c_pass_wiki_compliant_2`
- ⏭️ NOT RUN `wiki_copy_without_truncation.c` → `test_str32_c_pass_wiki_copy_without_truncation`
- ⏭️ NOT RUN `wiki_truncation.c` → `test_str32_c_pass_wiki_truncation`

---

### 🔶 STR03-C - Not Implemented (has tests)

<a id="rule-str03c"></a>

**Title:** Do not inadvertently truncate a string

**Description:** Alternative functions that limit the number of bytes copied are often
recommended to mitigate buffer overflowvulnerabilities. Examples include These
functions truncate strings that exceed the specified limits. Additionally, some
functions, such asstrncpy(), do not guarantee that the resulting character
sequence is null-terminated. (SeeSTR32-C. Do not pass a non-null-terminated
character sequence to a library function that expects a string.) Unintentional
truncation results in a loss of data and in some cases leads to software
vulnerabilities.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_str03_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_adequate_space.c` → `test_str03_c_pass_wiki_adequate_space`

---

### 🔶 STR00-C - Not Implemented (has tests)

<a id="rule-str00c"></a>

**Title:** Represent characters using an appropriate type

**Description:** Strings are a fundamental concept in software engineering, but they are not a
built-in type in C. Null-terminated byte strings (NTBS) consist of a contiguous
sequence of characters terminated by and including the first null character and
are supported in C as the format used for string literals. The C programming
language supports single-byte character strings, multibyte character strings,
and wide-character strings. Single-byte and multibyte character strings are both
described as null-terminated byte strings, which are also callednarrowcharacter
strings. A pointer to a null-terminated byte string points to its initial
character. The length of the string is the number of bytes preceding the null
character, and the value of the string is the sequence of the values of the
contained characters, in order. A wide string is a contiguous sequence of wide
characters (of typewchar_t) terminated by and including the first null wide
character. A pointer to a wide string points to its initial (lowest addressed)
wide character. The length of a wide string is the number of wide characters
preceding the null wide character, and the value of a wide string is the
sequence of code values of the contained wide characters, in order.

**Test Coverage:** 40 tests (30 fail, 10 pass)

**Test Results:** 0/40 passed (0.0%), 40 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_array_indexing_char.c` → `test_str00_c_fail_testcases_array_indexing_char`
- ⏭️ NOT RUN `testcases_buffer_manipulation_types.c` → `test_str00_c_fail_testcases_buffer_manipulation_types`
- ⏭️ NOT RUN `testcases_byte_operations_char.c` → `test_str00_c_fail_testcases_byte_operations_char`
- ⏭️ NOT RUN `testcases_char_for_numeric_values.c` → `test_str00_c_fail_testcases_char_for_numeric_values`
- ⏭️ NOT RUN `testcases_char_vs_int_eof.c` → `test_str00_c_fail_testcases_char_vs_int_eof`
- ⏭️ NOT RUN `testcases_character_arithmetic_operations.c` → `test_str00_c_fail_testcases_character_arithmetic_operations`
- ⏭️ NOT RUN `testcases_character_classification_loops.c` → `test_str00_c_fail_testcases_character_classification_loops`
- ⏭️ NOT RUN `testcases_character_constants_wrong_type.c` → `test_str00_c_fail_testcases_character_constants_wrong_type`
- ⏭️ NOT RUN `testcases_character_encoding_issues.c` → `test_str00_c_fail_testcases_character_encoding_issues`
- ⏭️ NOT RUN `testcases_configuration_parsing_errors.c` → `test_str00_c_fail_testcases_configuration_parsing_errors`
- ⏭️ NOT RUN `testcases_ctype_functions_misuse.c` → `test_str00_c_fail_testcases_ctype_functions_misuse`
- ⏭️ NOT RUN `testcases_escape_sequence_handling.c` → `test_str00_c_fail_testcases_escape_sequence_handling`
- ⏭️ NOT RUN `testcases_file_io_character_types.c` → `test_str00_c_fail_testcases_file_io_character_types`
- ⏭️ NOT RUN `testcases_function_parameters_wrong_types.c` → `test_str00_c_fail_testcases_function_parameters_wrong_types`
- ⏭️ NOT RUN `testcases_int_for_string_operations.c` → `test_str00_c_fail_testcases_int_for_string_operations`
- ⏭️ NOT RUN `testcases_locale_dependent_operations.c` → `test_str00_c_fail_testcases_locale_dependent_operations`
- ⏭️ NOT RUN `testcases_memory_operations_chars.c` → `test_str00_c_fail_testcases_memory_operations_chars`
- ⏭️ NOT RUN `testcases_mixed_char_types.c` → `test_str00_c_fail_testcases_mixed_char_types`
- ⏭️ NOT RUN `testcases_network_protocol_parsing.c` → `test_str00_c_fail_testcases_network_protocol_parsing`
- ⏭️ NOT RUN `testcases_pointer_arithmetic_chars.c` → `test_str00_c_fail_testcases_pointer_arithmetic_chars`
- ⏭️ NOT RUN `testcases_printf_format_mismatch.c` → `test_str00_c_fail_testcases_printf_format_mismatch`
- ⏭️ NOT RUN `testcases_regex_pattern_matching.c` → `test_str00_c_fail_testcases_regex_pattern_matching`
- ⏭️ NOT RUN `testcases_signed_char_string_literal.c` → `test_str00_c_fail_testcases_signed_char_string_literal`
- ⏭️ NOT RUN `testcases_string_comparison_issues.c` → `test_str00_c_fail_testcases_string_comparison_issues`
- ⏭️ NOT RUN `testcases_string_concatenation_types.c` → `test_str00_c_fail_testcases_string_concatenation_types`
- ⏭️ NOT RUN `testcases_struct_member_char_types.c` → `test_str00_c_fail_testcases_struct_member_char_types`
- ⏭️ NOT RUN `testcases_token_parsing_wrong_types.c` → `test_str00_c_fail_testcases_token_parsing_wrong_types`
- ⏭️ NOT RUN `testcases_unicode_handling_errors.c` → `test_str00_c_fail_testcases_unicode_handling_errors`
- ⏭️ NOT RUN `testcases_unsigned_char_string_literal.c` → `test_str00_c_fail_testcases_unsigned_char_string_literal`
- ⏭️ NOT RUN `testcases_wide_char_misuse.c` → `test_str00_c_fail_testcases_wide_char_misuse`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_consistent_types_functions.c` → `test_str00_c_pass_testcases_consistent_types_functions`
- ⏭️ NOT RUN `testcases_correct_int_for_eof.c` → `test_str00_c_pass_testcases_correct_int_for_eof`
- ⏭️ NOT RUN `testcases_file_io_appropriate_types.c` → `test_str00_c_pass_testcases_file_io_appropriate_types`
- ⏭️ NOT RUN `testcases_proper_char_usage.c` → `test_str00_c_pass_testcases_proper_char_usage`
- ⏭️ NOT RUN `testcases_proper_ctype_usage.c` → `test_str00_c_pass_testcases_proper_ctype_usage`
- ⏭️ NOT RUN `testcases_proper_string_operations.c` → `test_str00_c_pass_testcases_proper_string_operations`
- ⏭️ NOT RUN `testcases_safe_character_arithmetic.c` → `test_str00_c_pass_testcases_safe_character_arithmetic`
- ⏭️ NOT RUN `testcases_secure_input_validation.c` → `test_str00_c_pass_testcases_secure_input_validation`
- ⏭️ NOT RUN `testcases_unsigned_char_for_bytes.c` → `test_str00_c_pass_testcases_unsigned_char_for_bytes`
- ⏭️ NOT RUN `testcases_wide_char_proper_usage.c` → `test_str00_c_pass_testcases_wide_char_proper_usage`

---

### 🔶 STR02-C - Not Implemented (has tests)

<a id="rule-str02c"></a>

**Title:** Sanitize data passed to complex subsystems

**Description:** String data passed to complex subsystems may contain special characters that can
trigger commands or actions, resulting in a softwarevulnerability. As a result,
it is necessary tosanitizeall string data passed to complex subsystems so that
the resulting string is innocuous in the context in which it will be
interpreted. These are some examples of complex subsystems: Data sanitization
requires an understanding of the data being passed and the capabilities of the
subsystem. John Viega and Matt Messier provide an example of an application that
inputs an email address to a buffer and then uses this string as an argument in
a call tosystem()[Viega 2003]:

**Test Coverage:** 5 tests (3 fail, 2 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_str02_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2_2.c` → `test_str02_c_fail_wiki_noncompliant_2_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_str02_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_str02_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_str02_c_pass_wiki_compliant_2`

---

### ⚫ STR01-C - Not Implemented (no tests)

<a id="rule-str01c"></a>

**Title:** Adopt and implement a consistent plan for managing strings

**Description:** There are two basic approaches for managing strings in C programs: the first is
to maintain strings in statically allocated arrays; the second is to dynamically
allocate memory as required. Each approach has advantages and disadvantages.
However, it generally makes sense to select a single approach to managing
strings and apply it consistently across a project. Otherwise, the decision is
left to individual programmers who are likely to make different, inconsistent
choices. Statically allocated strings assume a fixed-size character array,
meaning that it is impossible to add data after the buffer is filled. Because
the static approach discards excess data, actual program data can be lost.
Consequently, the resulting string must be fully validated. Dynamically
allocated buffers dynamically resize as additional memory is required. Dynamic
approaches scale better and do not discard excess data. The major disadvantage
is that, if inputs are not limited, they can exhaust memory on a machine and
consequently be used indenial-of-serviceattacks.

**Test Coverage:** 0 tests (0 fail, 0 pass)

---

### 🔶 STR34-C - Not Implemented (has tests)

<a id="rule-str34c"></a>

**Title:** Cast characters to unsigned char before converting to larger integer sizes

**Description:** Signed character data must be converted tounsigned charbefore being assigned or
converted to a larger signed type. This rule applies to bothsigned charand
(plain)charcharacters on implementations wherecharis defined to have the same
range, representation, and behaviors assigned char. However, this rule is
applicable only in cases where the character data may contain values that can be
misinterpreted as negative numbers. For example, if thechartype is represented
by a two's complement 8-bit value, any character value greater than +127 is
interpreted as a negative value. This rule is a generalization ofSTR37-C.
Arguments to character-handling functions must be representable as an unsigned
char.

**Test Coverage:** 5 tests (3 fail, 2 pass)

**Test Results:** 0/5 passed (0.0%), 5 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_str34_c_fail_wiki_noncompliant_1`
- ⏭️ NOT RUN `wiki_noncompliant_2.c` → `test_str34_c_fail_wiki_noncompliant_2`
- ⏭️ NOT RUN `wiki_noncompliant_3.c` → `test_str34_c_fail_wiki_noncompliant_3`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_str34_c_pass_wiki_compliant_1`
- ⏭️ NOT RUN `wiki_compliant_2.c` → `test_str34_c_pass_wiki_compliant_2`

---

### ✅ STR31-C - Implemented

<a id="rule-str31c"></a>

**Title:** Guarantee that storage for strings has sufficient space for character data and the null terminator

**Description:** Copying data to a buffer that is not large enough to hold that data results in a
buffer overflow. Buffer overflows occur frequently when manipulating strings
[Seacord 2013b]. To prevent such errors, either limit copies through truncation
or, preferably, ensure that the destination is of sufficient size to hold the
character data to be copied and the null-termination character. (SeeSTR03-C. Do
not inadvertently truncate a string.) When strings live on the heap, this rule
is a specific instance ofMEM35-C. Allocate sufficient memory for an object.
Because strings are represented as arrays of characters, this rule is related to
bothARR30-C. Do not form or use out-of-bounds pointers or array
subscriptsandARR38-C. Guarantee that library functions do not form invalid
pointers. This noncompliant code example demonstrates anoff-by-oneerror [Dowd
2006]. The loop copies data fromsrctodest. However, because the loop does not
account for the null-termination character, it may be incorrectly written 1 byte
past the end ofdest.

**Test Coverage:** 58 tests (38 fail, 20 pass)

**Test Results:** 0/58 passed (0.0%), 58 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `testcases_array_bounds.c` → `test_str31_c_fail_testcases_array_bounds`
- ⏭️ NOT RUN `testcases_array_exact.c` → `test_str31_c_fail_testcases_array_exact`
- ⏭️ NOT RUN `testcases_buffer_reuse.c` → `test_str31_c_fail_testcases_buffer_reuse`
- ⏭️ NOT RUN `testcases_cmd_args.c` → `test_str31_c_fail_testcases_cmd_args`
- ⏭️ NOT RUN `testcases_double_free.c` → `test_str31_c_fail_testcases_double_free`
- ⏭️ NOT RUN `testcases_env_var.c` → `test_str31_c_fail_testcases_env_var`
- ⏭️ NOT RUN `testcases_file_read.c` → `test_str31_c_fail_testcases_file_read`
- ⏭️ NOT RUN `testcases_format_long.c` → `test_str31_c_fail_testcases_format_long`
- ⏭️ NOT RUN `testcases_gets_unsafe.c` → `test_str31_c_fail_testcases_gets_unsafe`
- ⏭️ NOT RUN `testcases_loop_nobounds.c` → `test_str31_c_fail_testcases_loop_nobounds`
- ⏭️ NOT RUN `testcases_macro_expand.c` → `test_str31_c_fail_testcases_macro_expand`
- ⏭️ NOT RUN `testcases_malloc_small.c` → `test_str31_c_fail_testcases_malloc_small`
- ⏭️ NOT RUN `testcases_memcpy_bad.c` → `test_str31_c_fail_testcases_memcpy_bad`
- ⏭️ NOT RUN `testcases_multi_concat.c` → `test_str31_c_fail_testcases_multi_concat`
- ⏭️ NOT RUN `testcases_nested_calls.c` → `test_str31_c_fail_testcases_nested_calls`
- ⏭️ NOT RUN `testcases_off_by_one.c` → `test_str31_c_fail_testcases_off_by_one`
- ⏭️ NOT RUN `testcases_path_toolong.c` → `test_str31_c_fail_testcases_path_toolong`
- ⏭️ NOT RUN `testcases_pointer_arith.c` → `test_str31_c_fail_testcases_pointer_arith`
- ⏭️ NOT RUN `testcases_scanf_overflow.c` → `test_str31_c_fail_testcases_scanf_overflow`
- ⏭️ NOT RUN `testcases_sprintf_long.c` → `test_str31_c_fail_testcases_sprintf_long`
- ⏭️ NOT RUN `testcases_stack_array.c` → `test_str31_c_fail_testcases_stack_array`
- ⏭️ NOT RUN `testcases_strcat_overflow.c` → `test_str31_c_fail_testcases_strcat_overflow`
- ⏭️ NOT RUN `testcases_strcpy_small.c` → `test_str31_c_fail_testcases_strcpy_small`
- ⏭️ NOT RUN `testcases_strncpy_bad.c` → `test_str31_c_fail_testcases_strncpy_bad`
- ⏭️ NOT RUN `testcases_struct_field.c` → `test_str31_c_fail_testcases_struct_field`
- ⏭️ NOT RUN `testcases_temp_buffer.c` → `test_str31_c_fail_testcases_temp_buffer`
- ⏭️ NOT RUN `testcases_token_parse.c` → `test_str31_c_fail_testcases_token_parse`
- ⏭️ NOT RUN `testcases_unicode_wide.c` → `test_str31_c_fail_testcases_unicode_wide`
- ⏭️ NOT RUN `testcases_user_input.c` → `test_str31_c_fail_testcases_user_input`
- ⏭️ NOT RUN `testcases_var_args.c` → `test_str31_c_fail_testcases_var_args`
- ⏭️ NOT RUN `wiki_argv.c` → `test_str31_c_fail_wiki_argv`
- ⏭️ NOT RUN `wiki_argv_2.c` → `test_str31_c_fail_wiki_argv_2`
- ⏭️ NOT RUN `wiki_fscanf.c` → `test_str31_c_fail_wiki_fscanf`
- ⏭️ NOT RUN `wiki_getchar.c` → `test_str31_c_fail_wiki_getchar`
- ⏭️ NOT RUN `wiki_getenv.c` → `test_str31_c_fail_wiki_getenv`
- ⏭️ NOT RUN `wiki_gets.c` → `test_str31_c_fail_wiki_gets`
- ⏭️ NOT RUN `wiki_off_by_one_error.c` → `test_str31_c_fail_wiki_off_by_one_error`
- ⏭️ NOT RUN `wiki_sprintf.c` → `test_str31_c_fail_wiki_sprintf`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `testcases_array_init.c` → `test_str31_c_pass_testcases_array_init`
- ⏭️ NOT RUN `testcases_dynamic_alloc.c` → `test_str31_c_pass_testcases_dynamic_alloc`
- ⏭️ NOT RUN `testcases_path_concat.c` → `test_str31_c_pass_testcases_path_concat`
- ⏭️ NOT RUN `testcases_realloc_safe.c` → `test_str31_c_pass_testcases_realloc_safe`
- ⏭️ NOT RUN `testcases_snprintf_safe.c` → `test_str31_c_pass_testcases_snprintf_safe`
- ⏭️ NOT RUN `testcases_sprintf_safe.c` → `test_str31_c_pass_testcases_sprintf_safe`
- ⏭️ NOT RUN `testcases_strcat_safe.c` → `test_str31_c_pass_testcases_strcat_safe`
- ⏭️ NOT RUN `testcases_strcpy_safe.c` → `test_str31_c_pass_testcases_strcpy_safe`
- ⏭️ NOT RUN `testcases_strncat_safe.c` → `test_str31_c_pass_testcases_strncat_safe`
- ⏭️ NOT RUN `testcases_strncpy_safe.c` → `test_str31_c_pass_testcases_strncpy_safe`
- ⏭️ NOT RUN `wiki_argv.c` → `test_str31_c_pass_wiki_argv`
- ⏭️ NOT RUN `wiki_fgets.c` → `test_str31_c_pass_wiki_fgets`
- ⏭️ NOT RUN `wiki_fscanf.c` → `test_str31_c_pass_wiki_fscanf`
- ⏭️ NOT RUN `wiki_getchar.c` → `test_str31_c_pass_wiki_getchar`
- ⏭️ NOT RUN `wiki_getenv.c` → `test_str31_c_pass_wiki_getenv`
- ⏭️ NOT RUN `wiki_getline_posix.c` → `test_str31_c_pass_wiki_getline_posix`
- ⏭️ NOT RUN `wiki_off_by_one_error.c` → `test_str31_c_pass_wiki_off_by_one_error`
- ⏭️ NOT RUN `wiki_snprintf.c` → `test_str31_c_pass_wiki_snprintf`
- ⏭️ NOT RUN `wiki_sprintf.c` → `test_str31_c_pass_wiki_sprintf`
- ⏭️ NOT RUN `wiki_sprintf_2.c` → `test_str31_c_pass_wiki_sprintf_2`

---

## Category: WIN

<a id="category-win"></a>

**Implementation Status:** 2 / 6 rules (33.3%)

### 🔶 WIN30-C - Not Implemented (has tests)

<a id="rule-win30c"></a>

**Title:** Properly pair allocation and deallocation functions

**Description:** Windows provides several APIs for allocating memory. While some of these
functions have converged over time, it is still important to always properly
pair allocations and deallocations. The following table shows the proper
pairings. AllocatorDeallocatormalloc()free()realloc()free()LocalAlloc()LocalFree
()LocalReAlloc()LocalFree()GlobalAlloc()GlobalFree()GlobalReAlloc()GlobalFree()V
irtualAlloc()VirtualFree()VirtualAllocEx()VirtualFreeEx()VirtualAllocExNuma()Vir
tualFreeEx()AllocateUserPhysicalPages()FreeUserPhysicalPages()AllocateUserPhysic
alPagesNuma()FreeUserPhysicalPages()HeapAlloc()HeapFree()HeapReAlloc()HeapFree()
In this example, theFormatMessage()function allocates a buffer and stores it in
thebufparameter. From the documentation ofFORMAT_MESSAGE_ALLOCATE_BUFFER[MSDN]:

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_win30_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_win30_c_pass_wiki_compliant_1`

---

### 🔶 WIN00-C - Not Implemented (has tests)

<a id="rule-win00c"></a>

**Title:** Be specific when dynamically loading libraries

**Description:** TheLoadLibrary()orLoadLibraryEx()function calls [MSDN] allow you to dynamically
load a library at runtime and use a specific algorithm to locate the library
within the file system [MSDN]. It is possible for an attacker to place a file on
the DLL search path such that your application inadvertently loads and executes
arbitrary source code. #include <Windows.h> void func(void) { HMODULE hMod =
LoadLibrary(TEXT("MyLibrary.dll")); if (hMod != NULL) { typedef void (__cdecl
func_type)(void); func_type *fn = (func_type *)GetProcAddress(hMod,
"MyFunction"); if (fn != NULL) fn(); } } If an attacker were to place a
malicious DLL named MyLibrary.dll higher on the search path than where the
library resides, she could trigger arbitrary code to execute either via
theDllMain()entrypoint (which is called automatically by the system loader) or
by providing an implementation forMyFunction(), either of which would run within
the security context of your application. If your application runs with elevated
privileges (such as a service application), an escalation of privileges could
result.

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_win00_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_win00_c_pass_wiki_compliant_1`

---

### 🔶 WIN03-C - Not Implemented (has tests)

<a id="rule-win03c"></a>

**Title:** Understand HANDLE inheritance

**Description:** Securable resources such as access tokens, events, files, threads, and others
are represented viaHANDLEobjects on Windows [MSDN]. Handle inheritance is a two-
step process. When obtaining aHANDLE, an option is given to specify whether the
object is inheritable or not. This option is usually in the form of
aBOOLparameter (as in the case ofOpenMutex()), or aSECURITY_DESCRIPTORparameter
(as in the case ofCreateFile()). When creating a process via
theCreateProcess()family of APIs, a parameter is given specifying whether the
spawned process will inherit handles previously flagged as being inheritable.
Any handles that were opened as being inheritable will be opened in the child
process using the same handle value and access privileges as in the parent
process. The parent process can then alert the child process of the handle
values via an inter-process communication mechanism, and the child process can
use those values as though it had opened the handle [MSDN]. When opening handles
to securable resources or spawning child processes, prohibit handle inheritance
by default to prevent accidental information leakage. If obtaining an inherited
handle from a parent process, prevent leakage to subsequent child processes by
duplicating the handle without inheritance. This noncompliant code example
attempts to open an existing mutex handle that can be inherited by a child
process:

**Test Coverage:** 6 tests (3 fail, 3 pass)

**Test Results:** 0/6 passed (0.0%), 6 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_fopen.c` → `test_win03_c_fail_wiki_fopen`
- ⏭️ NOT RUN `wiki_further_inheritance.c` → `test_win03_c_fail_wiki_further_inheritance`
- ⏭️ NOT RUN `wiki_mutex.c` → `test_win03_c_fail_wiki_mutex`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_fopen.c` → `test_win03_c_pass_wiki_fopen`
- ⏭️ NOT RUN `wiki_further_inheritance.c` → `test_win03_c_pass_wiki_further_inheritance`
- ⏭️ NOT RUN `wiki_mutex.c` → `test_win03_c_pass_wiki_mutex`

---

### 🔶 WIN04-C - Not Implemented (has tests)

<a id="rule-win04c"></a>

**Title:** Consider encrypting function pointers

**Description:** If an attacker can overwrite memory containing function pointers, they may be
able to execute arbitrary code. To mitigate the effects of such attacks,
pointers to functions can be encrypted at runtime on the basis of some
characteristics of the execution process so that only a running process will be
able to decode them. This is only required for stored function pointers stored
to writable memory, including the stack. The Microsoft SDL [Microsoft 2012]
recommends encoding long-lived pointers in your code. This noncompliant code
example assigns the address of theprintf()function to thelog_fnfunction pointer,
which can be allocated in the stack or data segment: int (*log_fn)(const char *,
...) = printf; /* ... */ log_fn("foo");

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_win04_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_windows.c` → `test_win04_c_pass_wiki_windows`

---

### ✅ WIN02-C - Implemented

<a id="rule-win02c"></a>

**Title:** Restrict privileges when spawning child processes

**Description:** The principle of least privilege states that every program and every user of the
system should operate using the least set of privileges necessary to complete
the job [Saltzer 1974,Saltzer 1975]. The Build Security In website [DHS 2006]
provides additional definitions of this principle. Executing with minimal
privileges mitigates against exploitation in case a vulnerability is discovered
in the code. An application may spawn another process as part of its normal
course of action. On Windows, the newly-spawned process automatically receives
the same privileges as the parent process [MSDN]. By allowing the child process
to run in the same security context as the parent process, the attack surface
for the application is extended to the child process. Furthermore, this example
allows the child process to inherit handles from the parent process by
passingTRUEto thebInheritsHandlesparameter. #include <Windows.h> void
launch_notepad(void) { PROCESS_INFORMATION pi; STARTUPINFO si; ZeroMemory(&si,
sizeof(si)); si.cb = sizeof( si ); if
(CreateProcess(TEXT("C:\\Windows\\Notepad.exe"), NULL, NULL, NULL, TRUE, 0,
NULL, NULL, &si, &pi )) { /* Process has been created; work with the process and
wait for it to terminate. */ WaitForSingleObject(pi.hProcess, INFINITE);
CloseHandle(pi.hThread); CloseHandle(pi.hProcess); } }

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_win02_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_solution.c` → `test_win02_c_pass_wiki_compliant_solution`

---

### ✅ WIN01-C - Implemented

<a id="rule-win01c"></a>

**Title:** Do not forcibly terminate execution

**Description:** When a thread terminates under normal conditions, thread-specific resources such
as the initial stack space and thread-specificHANDLEobjects are released
automatically by the system and notifications are sent to other parts of the
application, such asDLL_THREAD_DETACHmessages being sent to DLLs. However, if a
thread is forcibly terminated by callingTerminateThread(), the cleanup and
notifications do not have the chance to run. MSDN states On some platforms (such
as Microsoft Windows XP and Microsoft Windows Server 2003), the thread's initial
stack is not freed, causing a resource leak. Processes behave similar to
threads, and so share the same concerns. Do not use
theTerminateThread()orTerminateProcess()APIs. Instead, you should prefer to exit
threads and processes by returning from the entrypoint, by callingExitThread(),
or by callingExitProcess().

**Test Coverage:** 2 tests (1 fail, 1 pass)

**Test Results:** 0/2 passed (0.0%), 2 not run

#### Fail Tests (Should Detect Violations)

- ⏭️ NOT RUN `wiki_noncompliant_1.c` → `test_win01_c_fail_wiki_noncompliant_1`

#### Pass Tests (Should NOT Detect Violations)

- ⏭️ NOT RUN `wiki_compliant_1.c` → `test_win01_c_pass_wiki_compliant_1`

---

## Summary by Category

| Category | Rules | Implemented | Tests | Avg Tests/Rule |
|----------|-------|-------------|-------|----------------|
| API | 9 | 5 | 62 | 6.9 |
| ARR | 9 | 8 | 490 | 54.4 |
| CON | 23 | 0 | 75 | 3.3 |
| DCL | 31 | 4 | 173 | 5.6 |
| ENV | 8 | 0 | 75 | 9.4 |
| ERR | 11 | 2 | 88 | 8.0 |
| EXP | 31 | 6 | 236 | 7.6 |
| FIO | 35 | 3 | 200 | 5.7 |
| FLP | 13 | 0 | 40 | 3.1 |
| INT | 23 | 3 | 238 | 10.3 |
| MEM | 17 | 3 | 244 | 14.4 |
| MSC | 8 | 1 | 30 | 3.8 |
| POS | 20 | 4 | 55 | 2.8 |
| PRE | 16 | 4 | 189 | 11.8 |
| SIG | 7 | 2 | 314 | 44.9 |
| STR | 16 | 3 | 185 | 11.6 |
| WIN | 6 | 2 | 16 | 2.7 |

