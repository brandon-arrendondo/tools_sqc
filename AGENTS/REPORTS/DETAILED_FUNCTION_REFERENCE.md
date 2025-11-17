# Detailed Function Reference: arr00_c.rs

## Complete Function Catalog with Line Numbers and Complexity Analysis

### PRIMARY RULE CHECK FUNCTIONS (8 functions)

| # | Function Name | Lines | Complexity | Purpose |
|---|---|---|---|---|
| 1 | `check()` | 42-154 | 8 | Main router - dispatches to 14+ specialized checks |
| 2 | `check_array_assignment()` | 160-190 | 3 | Detects direct array-to-array assignment |
| 3 | `check_sizeof_misuse()` | 192-213 | 3 | Detects sizeof() on array parameters |
| 4 | `check_vla_declaration()` | 247-329 | 13 | Analyzes Variable Length Array declarations |
| 5 | `check_dangerous_functions()` | 496-595 | 12 | Detects dangerous functions (gets, scanf, etc.) |
| 6 | `check_obvious_string_overflow()` | 597-719 | 14 | Detects obvious buffer overflows |
| 7 | `check_memcpy_size_mismatch()` | 721-817 | 11 | Analyzes memcpy/memmove size arguments |
| 8 | `check_memory_operation_overflow()` | 818-918 | 10 | Detects memory ops exceeding buffer |

### ADVANCED ANALYSIS FUNCTIONS (4 functions)

| # | Function Name | Lines | Complexity | Purpose |
|---|---|---|---|---|
| 9 | `check_loop_exceeds_allocation()` | 994-1080 | 9 | Analyzes for loops with malloc allocation |
| 10 | `check_loop_bound_exceeds_array()` | 1125-1216 | 8 | Checks loop bounds against array size |
| 11 | `check_loop_array_access()` | 1247-1323 | 9 | Validates loop-based array access |
| 12 | `check_subscript_bounds()` | 1442-1516 | 7 | Validates array subscript indices |

### ARRAY STATE ANALYSIS FUNCTIONS (2 functions)

| # | Function Name | Lines | Complexity | Purpose |
|---|---|---|---|---|
| 13 | `check_uninitialized_array_read()` | 1518-1620 | 8 | Detects reads from uninitialized arrays |
| 14 | `check_use_after_free()` | 1671-1715 | 7 | Detects array access after memory free |

### POINTER & EXPRESSION ANALYSIS FUNCTIONS (3 functions)

| # | Function Name | Lines | Complexity | Purpose |
|---|---|---|---|---|
| 15 | `check_pointer_arithmetic()` | 2079-2158 | 6 | Analyzes pointer arithmetic bounds |
| 16 | `check_pointer_subtraction()` | 2159-2226 | 5 | Validates pointer subtraction safety |
| 17 | `check_array_comparison()` | 2326-2369 | 4 | Detects improper array comparisons |

### SPECIAL PATTERN FUNCTIONS (2 functions)

| # | Function Name | Lines | Complexity | Purpose |
|---|---|---|---|---|
| 18 | `check_comma_in_subscript()` | 1716-1783 | 6 | Detects comma operator in subscripts |
| 19 | `check_constant_out_of_bounds()` | 1784-1864 | 8 | Detects constant out-of-bounds indices |

### RETURN VALUE ANALYSIS (1 function)

| # | Function Name | Lines | Complexity | Purpose |
|---|---|---|---|---|
| 20 | `check_return_local_array()` | 1982-2078 | 7 | Detects return of pointer to local array |

### INDEX & BOUNDARY VALUE ANALYSIS (1 function)

| # | Function Name | Lines | Complexity | Purpose |
|---|---|---|---|---|
| 21 | `check_boundary_value_index()` | 1894-1981 | 8 | Detects boundary value index issues |

---

## HELPER FUNCTIONS BY CATEGORY

### Parametric Analysis Helpers (5 functions) - DUPLICATES IN ast_utils.rs

```
Lines 2370-2505 - These functions are EXACTLY duplicated in ast_utils.rs
Action Required: Delete from arr00_c.rs and import from ast_utils
```

1. **`find_containing_function()`** (Lines 2370-2379)
   - Finds the function_definition node containing a given node
   - Used by: check_if_array_parameter (line 219), check_vla_size_validation (line 334)
   - Exact duplicate: ast_utils.rs line 26

2. **`get_function_parameters()`** (Lines 2381-2391)
   - Extracts function parameter list as (name, type) tuples
   - Used by: check_if_array_parameter (line 222)
   - Exact duplicate: ast_utils.rs line 121

3. **`extract_parameters()`** (Lines 2393-2419)
   - Helper for get_function_parameters
   - Used by: get_function_parameters (line 2386)
   - Exact duplicate: ast_utils.rs line 134

4. **`extract_parameter_info()`** (Lines 2421-2441)
   - Extracts (name, type) from parameter_declaration node
   - Used by: extract_parameters (line 2404)
   - Nearly identical: ast_utils.rs line 163

5. **`find_identifier_in_declarator()`** (Lines 2443-2457)
   - Recursively finds identifier in declarator hierarchy
   - Used by: extract_parameter_info (line 2429)
   - Nearly identical: ast_utils.rs line 99

### Type Checking Helpers (2 functions) - MINOR DUPLICATES

6. **`is_array_parameter_type()`** (Lines 2463-2468)
   - Checks if parameter type indicates array
   - Used by: check_if_array_parameter (line 226)
   - Very similar: ast_utils.rs line 216 (identical logic)

7. **`is_function_parameter()`** (Lines 387-409)
   - Checks if variable name is in function parameters
   - Used by: check_subscript_bounds (line 1471), check_vla_size_validation (line 387)
   - Similar but different: ast_utils.rs line 186 (public version available)

### Context Detection Helpers (3 functions) - MAJOR DUPLICATES

8. **`is_inside_loop()`** (Lines 1621-1640)
   - Checks if node is inside for/while/do loop
   - Used by: check_uninitialized_array_read (line 1565)
   - Exact duplicate: ast_utils.rs line 38

9. **`is_write_context()`** (Lines 1641-1670)
   - Checks if subscript is on left side of assignment
   - Used by: check_uninitialized_array_read (line 1613)
   - Exact duplicate: ast_utils.rs line 330

10. **`is_loop_variable()`** (Lines 411-475)
    - Complex parsing - detects if variable defined in for loop init
    - Used by: check_subscript_bounds (line 1466)
    - No duplicate found (unique complex logic)

### Variable Validation Helpers (4 functions) - HIGH REUSABILITY

11. **`has_bounds_validation()`** (Lines 1865-1893)
    - Checks for bounds validation patterns (if conditions)
    - Used by: check_subscript_bounds (line 1473), check_subscript_bounds (line 1495)
    - Reusability: HIGH - Used by arr30_c.rs, str31_c.rs, mem33_c.rs (suspected)
    - Recommended Action: Move to variable_analysis.rs

12. **`is_user_input_variable()`** (Lines 1364-1377)
    - Detects if variable comes from scanf/input functions
    - Used by: check_loop_array_access (line 1279), check_subscript_bounds (line 1495)
    - Reusability: HIGH
    - Recommended Action: Move to variable_analysis.rs

13. **`has_validation_before_loop()`** (Lines 1379-1401)
    - Checks if loop variable validated between scanf and loop
    - Used by: check_loop_array_access (line 1289)
    - Reusability: HIGH
    - Recommended Action: Move to variable_analysis.rs

14. **`is_uninitialized_variable()`** (Lines 1403-1440)
    - Detects uninitialized variables in declarations
    - Used by: check_loop_array_access (line 1300)
    - Reusability: MEDIUM
    - Recommended Action: Move to variable_analysis.rs

### Size Extraction Helpers (4 functions) - HIGH REUSABILITY

15. **`find_element_size()`** (Lines 919-958)
    - Extracts element type size from preceding context
    - Used by: check_memcpy_size_mismatch (line 773)
    - Reusability: MEDIUM - Similar logic in arr30_c.rs
    - Recommended Action: Move to size_analysis.rs

16. **`find_allocation_size()`** (Lines 1082-1123)
    - Finds malloc/realloc allocation size
    - Used by: check_loop_exceeds_allocation (line 1046)
    - Reusability: HIGH - Similar in arr30_c.rs
    - Recommended Action: Move to size_analysis.rs

17. **`find_string_literal_length()`** (Lines 959-993)
    - Finds string literal length in variable initialization
    - Used by: check_obvious_string_overflow (line 661)
    - Reusability: MEDIUM - Similar in str31_c.rs
    - Recommended Action: Move to size_analysis.rs or string_analysis.rs

18. **`find_array_size()`** (Lines 2278-2325)
    - Extracts array size from preceding text
    - Used by: check_obvious_string_overflow (line 681)
    - Reusability: HIGH - Already exists in ast_utils.rs (line 279)
    - Recommended Action: Use ast_utils.rs version or consolidate

### Identifier Classification Helpers (4 functions) - MEDIUM REUSABILITY

19. **`is_array_identifier()`** (Lines 2470-2492)
    - Heuristic check if identifier is used as array
    - Used by: check_array_assignment (line 166)
    - Reusability: MEDIUM - No symbol table limitation
    - Recommended Action: Move to ast_utils.rs as public

20. **`is_subscript()`** (Lines 2494-2496)
    - Simple check for subscript_expression kind
    - Used by: check_array_assignment (line 166)
    - Reusability: LOW - Trivial helper
    - Recommended Action: Keep inline or in ast_utils

21. **`is_function_call_name()`** (Lines 2498-2505)
    - Checks if identifier is function in call expression
    - Used by: is_array_identifier (line 2475)
    - Reusability: MEDIUM - Useful pattern
    - Recommended Action: Move to ast_utils.rs as public

### VLA-Specific Helpers (2 functions)

22. **`check_vla_size_validation()`** (Lines 331-386)
    - Validates VLA size variables
    - Used by: check_vla_declaration (line 323)
    - Reusability: LOW - VLA-specific logic
    - Recommended Action: Keep in arr00_c.rs

23. **`check_if_array_parameter()`** (Lines 215-245)
    - Specific check for sizeof on array parameters
    - Used by: check_sizeof_misuse (lines 201, 207)
    - Reusability: MEDIUM - Pattern might repeat in other checks
    - Recommended Action: Keep in arr00_c.rs

### Pattern Analysis Helpers (6 functions)

24. **`has_size_validation_before()`** (Lines 477-494)
    - Checks for common validation patterns on size variables
    - Used by: check_vla_size_validation (line 359)
    - Reusability: MEDIUM
    - Recommended Action: Move to pattern_matching.rs

25. **`is_initialized_to_boundary_value()`** (Lines 1943-1981)
    - Checks if variable initialized to boundary value
    - Used by: check_boundary_value_index (line 1925)
    - Reusability: LOW - Specific to boundary analysis
    - Recommended Action: Keep in arr00_c.rs

26. **`find_pointer_source_array()`** (Lines 2227-2277)
    - Finds array from which pointer was derived
    - Used by: check_pointer_subtraction (line 2195)
    - Reusability: MEDIUM
    - Recommended Action: Move to size_analysis.rs

27. **`extract_array_name_from_subscript()`** (Lines 1217-1246)
    - Extracts array name from subscript expressions
    - Used by: check_loop_bound_exceeds_array (line 1178)
    - Reusability: MEDIUM
    - Recommended Action: Move to size_analysis.rs

28. **`extract_loop_bound_variable()`** (Lines 1324-1346)
    - Extracts variable name from loop condition
    - Used by: check_loop_array_access (line 1270)
    - Reusability: MEDIUM
    - Recommended Action: Move to variable_analysis.rs

29. **`contains_array_access()`** (Lines 1347-1363)
    - Checks if node tree contains array access
    - Used by: check_loop_array_access (line 1283)
    - Reusability: MEDIUM
    - Recommended Action: Keep in arr00_c.rs or move to ast_utils

---

## FUNCTION DEPENDENCY GRAPH

### High-Level Call Flow:
```
check() (main router)
  ├─→ check_array_assignment()
  ├─→ check_sizeof_misuse()
  │   └─→ check_if_array_parameter()
  │       ├─→ find_containing_function() [DUP]
  │       ├─→ get_function_parameters() [DUP]
  │       └─→ is_array_parameter_type() [DUP]
  ├─→ check_array_comparison()
  ├─→ check_pointer_arithmetic()
  ├─→ check_pointer_subtraction()
  │   └─→ find_pointer_source_array()
  │   └─→ find_array_size()
  ├─→ check_vla_declaration()
  │   ├─→ check_vla_size_validation()
  │   │   ├─→ find_containing_function() [DUP]
  │   │   └─→ has_size_validation_before()
  │   └─→ extract_parameter_info()
  ├─→ check_dangerous_functions()
  ├─→ check_obvious_string_overflow()
  │   ├─→ find_string_literal_length()
  │   ├─→ find_containing_function() [DUP]
  │   └─→ find_array_size()
  ├─→ check_memcpy_size_mismatch()
  │   └─→ find_element_size()
  ├─→ check_memory_operation_overflow()
  ├─→ check_loop_exceeds_allocation()
  │   ├─→ find_containing_function() [DUP]
  │   └─→ find_allocation_size()
  ├─→ check_loop_bound_exceeds_array()
  │   ├─→ extract_array_name_from_subscript()
  │   └─→ find_containing_function() [DUP]
  ├─→ check_loop_array_access()
  │   ├─→ extract_loop_bound_variable()
  │   ├─→ contains_array_access()
  │   ├─→ is_user_input_variable()
  │   ├─→ is_uninitialized_variable()
  │   ├─→ has_validation_before_loop()
  │   ├─→ find_containing_function() [DUP]
  │   └─→ is_function_parameter()
  ├─→ check_return_local_array()
  │   └─→ find_containing_function() [DUP]
  ├─→ check_subscript_bounds()
  │   ├─→ find_containing_function() [DUP]
  │   ├─→ is_function_parameter()
  │   ├─→ is_user_input_variable()
  │   ├─→ has_bounds_validation()
  │   └─→ is_loop_variable()
  ├─→ check_uninitialized_array_read()
  │   ├─→ is_inside_loop() [DUP]
  │   ├─→ is_write_context() [DUP]
  │   └─→ find_containing_function() [DUP]
  ├─→ check_use_after_free()
  ├─→ check_comma_in_subscript()
  ├─→ check_constant_out_of_bounds()
  │   └─→ has_bounds_validation()
  └─→ check_boundary_value_index()
      ├─→ is_initialized_to_boundary_value()
      └─→ find_containing_function() [DUP]
```

---

## COMPLEXITY DISTRIBUTION

### By Complexity Level:

**HIGH (10+):** 5 functions
- check_obvious_string_overflow() - 14
- check_vla_declaration() - 13
- check_dangerous_functions() - 12
- check_memcpy_size_mismatch() - 11
- check_memory_operation_overflow() - 10

**MODERATE (7-9):** 8 functions
- check_loop_exceeds_allocation() - 9
- check_loop_array_access() - 9
- check_loop_bound_exceeds_array() - 8
- check_uninitialized_array_read() - 8
- check_constant_out_of_bounds() - 8
- check_boundary_value_index() - 8
- check_subscript_bounds() - 7
- check_return_local_array() - 7

**LOW (≤6):** 7 functions
- check() router - 8 (but this is expected for dispatcher)
- check_use_after_free() - 7
- check_comma_in_subscript() - 6
- check_pointer_arithmetic() - 6
- check_array_comparison() - 4
- check_pointer_subtraction() - 5
- check_sizeof_misuse() - 3
- check_array_assignment() - 3

---

## REFACTORING IMPACT SUMMARY

| Action | Functions Removed | Functions Moved | Functions Added | Lines Saved |
|--------|---|---|---|---|
| Remove ast_utils duplicates | 8 | 0 | 0 | 150 |
| Create variable_analysis.rs | 0 | 5 | 5 | 0* |
| Create size_analysis.rs | 0 | 4 | 4 | 0* |
| Reduce complexity splits | 5 | 0 | 5+ | -100* |
| **TOTAL** | **8** | **9** | **14+** | **~50 net** |

*Lines moved but net code count depends on implementation consolidation

