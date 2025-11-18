CODE RULES
****************************************************************

Specifying and elaborating on code rules is essential for maintaining 
consistency and quality in software development. This document outlines the 
key rules and guidelines that should be followed by all developers working on 
BISSELL Electronics projects.

Code violations do not necessarily mean that the code is incorrect or will not
work, but they do indicate that the code does not conform to the established
standards and practices. This can lead to issues such as reduced readability,
maintainability, and potential bugs in the future. Therefore, it is important to
address these violations to ensure that the codebase remains clean and
manageable.

This has two aims:

1. To develop a set of rules to objectively evaluate code quality, of
   BISSELL-developed code and code from suppliers or third-parties.

2. To drive the development of tools to automate the evaluation of code quality
   against these rules, where possible.

The aim of a PR should be to automatically evaluate code against these rules to
quickly provide feedback in a way that is easy to understand and actionable to
the developer.  To the extent tools can help with this, they should be used.

BISSELL-Specific Code Rules
===========================

While the code rules outlined in this document are generally applicable to all
BISSELL Electronics projects, regardless of who writes the code, the following
rules are applicable to code written by BISSELL Electronics software developers,
in addition to the more general rules:

- **BRULE-001**: Every source file must contain the standard BISSELL copyright
  verbiage at the top of the file. 
  
  For C files, an example copyright notice is:

.. code-block:: c

    /**
     * @copyright Copyright 2025, BISSELL Homecare, Inc.
     * All Rights Reserved.
     *
     * This is UNPUBLISHED PROPRIETARY SOURCE CODE of BISSELL Homecare, Inc.
     * the contents of this file may not be disclosed to third parties, copied
     * or duplicated in any form, in whole or in part, without the prior
     * written permission of BISSELL Homecare, Inc.
     */

.

  For Python files, an example copyright notice is:

.. code-block:: python

    # Copyright 2025, BISSELL Homecare Inc.
    # All Rights Reserved.
    #
    # This is UNPUBLISHED PROPRIETARY SOURCE CODE of BISSELL Homecare Inc.
    # the contents of this file may not be disclosed to third parties, copied
    # or duplicated in any form, in whole or in part, without the prior
    # written permission of BISSELL Homecare Inc.

- **BRULE-002**: C source will conform to the coding conventions outlines by
  the RIOT OS coding conventions, which can be found at the following URL:

    https://github.com/RIOT-OS/RIOT/blob/master/CODING_CONVENTIONS.md

  This is verified by this uncrustify configuration file:

.. code-block:: text

    indent_with_tabs        = 0                 # 1=indent to level only, 2=indent with tabs
    input_tab_size          = 4                 # original tab size
    output_tab_size         = 4                 # new tab size
    indent_columns          = output_tab_size   #
    indent_label            = 1                 # pos: absolute col, neg: relative column
    indent_switch_case      = 4                 # number
    indent_ternary_operator = 2                 # When the `:` is a continuation, indent it under `?`

    #
    # inter-symbol newlines
    #

    nl_enum_brace          = remove   # "enum {" vs "enum \n {"
    nl_union_brace         = remove   # "union {" vs "union \n {"
    nl_struct_brace        = remove   # "struct {" vs "struct \n {"
    nl_do_brace            = remove   # "do {" vs "do \n {"
    nl_if_brace            = remove   # "if () {" vs "if () \n {"
    nl_for_brace           = remove   # "for () {" vs "for () \n {"
    nl_else_brace          = remove   # "else {" vs "else \n {"
    nl_while_brace         = remove   # "while () {" vs "while () \n {"
    nl_switch_brace        = remove   # "switch () {" vs "switch () \n {"
    nl_brace_while         = remove   # "} while" vs "} \n while" - cuddle while
    nl_brace_else          = add      # "} \n else" vs "} else"
    nl_func_var_def_blk    = 1        #
    nl_fcall_brace         = remove   # "list_for_each() {" vs "list_for_each()\n{"
    nl_fdef_brace          = add      # "int foo() {" vs "int foo()\n{"
    nl_collapse_empty_body = true     # set while(){\n} to while(){}
    nl_end_of_file         = add      # fix no newline at end of file
    nl_end_of_file_min     = 1        #

    #
    # Source code modifications
    #

    mod_paren_on_return        = ignore   # "return 1;" vs "return (1);"
    mod_full_brace_if          = add      # "if() { } else { }" vs "if() else"
    mod_full_brace_while       = force    # force while(); to while(){ \n ; }
    mod_full_brace_for         = force    # force for(); to for(){ \n ; }
    mod_remove_extra_semicolon = true     # remove superfluous semicolons.

    #
    # inter-character spacing options
    #

    sp_sizeof_paren         = remove   # "sizeof (int)" vs "sizeof(int)"
    sp_before_sparen        = force    # "if (" vs "if("
    sp_after_sparen         = force    # "if () {" vs "if (){"
    sp_inside_braces        = add      # "{ 1 }" vs "{1}"
    sp_inside_braces_struct = add      # "{ 1 }" vs "{1}"
    sp_inside_braces_enum   = add      # "{ 1 }" vs "{1}"
    sp_assign               = add      #
    sp_arith                = add      #
    sp_bool                 = add      #
    sp_compare              = add      #
    sp_assign               = add      #
    sp_after_comma          = add      #
    sp_func_def_paren       = remove   # "int foo (){" vs "int foo(){"
    sp_func_call_paren      = remove   # "foo (" vs "foo("
    sp_func_proto_paren     = remove   # "int foo ();" vs "int foo();"
    sp_else_brace           = add      # ignore/add/remove/force
    sp_before_ptr_star      = add      # ignore/add/remove/force
    sp_after_ptr_star       = remove   # ignore/add/remove/force
    sp_between_ptr_star     = remove   # ignore/add/remove/force
    sp_inside_paren         = remove   # remove spaces inside parens
    sp_paren_paren          = remove   # remove spaces between nested parens
    sp_inside_sparen        = remove   # remove spaces inside parens for if, while and the like
    sp_inside_braces_empty  = remove   # force while(){ } to while(){}

    #
    # Aligning stuff
    #

    align_with_tabs        = FALSE     # use tabs to align
    align_on_tabstop       = TRUE      # align on tabstops
    align_enum_equ_span    = 4         # '=' in enum definition
    align_struct_init_span = 0         # align stuff in a structure init '= { }'
    align_right_cmt_span   = 3         #

    #
    # Special cases
    #

    set PROTO_WRAP ISR   # Wrap ISR macros like functions

- **BRULE-003**: Python source will conform to the PEP 8 coding style, which can
  be found at the following URL:

    https://www.python.org/dev/peps/pep-0008/

  This is verified by the `flake8` tool, which checks for PEP 8 compliance and
  other code quality issues.  The `ruff` tool can also be used to enforce PEP 8
  compliance and catch additional issues.

- **BRULE-004**: YAML and JSON files will conform to the following rules:

  - YAML files will be checked for syntax errors using the `yamllint` tool.
  - JSON files will be checked for syntax errors using the `jsonlint` tool.
  - Both YAML and JSON files should use spaces for indentation, with a
    consistent number of spaces (usually 2 or 4) per level of indentation.
  - Comments in YAML files should start with a `#` character and be placed on
    their own line or at the end of a line.
  - Prettifier tools like `prettier` can be used to format YAML and JSON files
    consistently.

- **BRULE-005**: All source code must be stored in ADO.

  This means that all source code must be checked into the ADO Git repository.
  This is to ensure that all code is versioned, backed up, and accessible to all
  developers. It also allows for collaboration and code review through pull
  requests.  PRs should be used to propose changes, with the main branch protected
  by branch policies and code reviews.

- **BRULE-006**: All public interface functions should be documented with doxygen
  headers.

- **BRULE-007**: Any source-file scoped variables or functions that are static
  should be marked as STATIC (macro defined in common_defs.h), so test code can
  easily access them (by turning them into global scope during testing).

- **BRULE-008**: Header guards should not begin with an underscore
  (e.g. `#ifndef _MY_HEADER_H_` is not allowed).  The C standard does not allow
  identifiers that begin with an underscore to be defined in the global namespace,
  so using an underscore in header guards can lead to conflicts with system headers
  and other libraries.

- **BRULE-010**: All source files should end with:

.. code-block:: c

        /// (C) COPYRIGHT BISSELL Homecare, Inc. ----------- END OF FILE
.

  This helps our license parsing tools to identify files.


General Code Rules
==================

Code to be reviewed should be checked against the following rules.  Through past 
experience, code will generally fail one or more of these rules and exceptions
can be made on a case-by-case basis.  Overall the goal is to communicate expectations
and have a metric to gauge the quality of code.  A 1-10 spectrum can be used for
code quality, with 10 being ideal and 1 being unacceptable.

Not all code rules are equivalent in weighting.

A rough  scoring criteria is to start at 10.  Subtract 2 points for each critical-level
rule that is violated, 0.5 points for each warning-level rule that is violated,
and 1 point total if any number above zero of the nice-to-have rules are violated.

Most supplier code historically has been below 4 and the expectation for BISSELL
code is to be above 7.

Code Artifacts (Nice to Have)
-----------------------------

- **BRULE-011**: For any code release, a changelog should be provided that
  describes the changes made in the release. This should include a summary of
  the changes, any new features, bug fixes, and any known issues. The changelog
  should be written in a clear and concise manner, and should be easy to read
  and understand.

- **BRULE-012**: For any code release, a requirements specification should be
  provided that describes the requirements for the code. This should include a
  description of the functionality, performance, and any other requirements that
  the code must meet. The requirements specification should be written in a clear
  and concise manner, and should be easy to read and understand.

- **BRULE-013**: For any code release, a test plan should be provided that
  describes the tests that will be performed on the code. This should include a
  description of the test cases, the expected results, and any other information
  that is relevant to the testing process. The test plan should be written in a
  clear and concise manner, and should be easy to read and understand.

- **BRULE-014**: For any code release, a test results report should be
  provided that describes the results of the tests performed on the code. This
  should include a summary of the test results, any issues found, and any
  recommendations for further testing or improvements. The test results report
  should be written in a clear and concise manner, and should be easy to read
  and understand.

- **BRULE-015**: For any code release, a software design document should be
  provided that describes the design of the code. This should include a
  description of the architecture, the design patterns used, and any other
  information that is relevant to the design process. The software design
  document should be written in a clear and concise manner, and should be easy
  to read and understand.

- **BRULE-016**: Test plans should include:
    - Unit tests (bounds of loops, function inputs, outputs, bounds of arrays)
    - Hardware/software in the loop tests
    - Fault-injection tests
    - Timing Analysis

- **BRULE-017**: Test results should include:
    - Pass/fail results for each test case
    - Any issues found during testing, including severity and impact
    - Recommendations for further testing or improvements
    - Coverage analysis, if applicable (line, branch, file coverage)

- **BRULE-018**: For any code release, the firmware version should be provided
  with steps to confirm version running on product.

- **BRULE-019**: For any code release, build configuration should be provided
  if source code is provided.  This should include relevant details such as:
    - Compiler version and flags
    - Build scripts and configurations
    - Dependencies and libraries used
    - Any other relevant build information
    This is to ensure that the code can be built and run in the same way as it
    was tested, and to facilitate debugging and further development.

Code Language and Format (Nice to Have)
---------------------------------------

- **BRULE-020**: Code and configuration files should be in ASCII text format, not UTF-8 or
  other Unicode formats. This is to ensure compatibility with compilers and
  tools that may not fully support Unicode. ASCII is a more universally supported
  character set, ensuring that the code can be compiled and run on a wide range
  of systems without issues.

- **BRULE-021**: Any code comments in non-English languages should be followed directly
  by an English translation. This is to ensure that all code comments are
  understandable by all developers, regardless of their native language. The
  English translation should be clear and concise, and should accurately reflect
  the meaning of the original comment.

Code Architecture and Organization (Nice to Have)
-------------------------------------------------

- **BRULE-022**: Code should be clearly organized or structured on the filesystem 
  by module or functional level.

- **BRULE-023**: Code should be modular, with each module having a clear
  responsibility and interface. This is to ensure that the code is easy to
  understand, maintain, and test. Each module should be self-contained and
  should not have unnecessary dependencies on other modules. This allows for
  easier testing, debugging, and reuse of code across different projects.

- **BRULE-024**: Code should be written in a way that is easy to read and
  understand. This includes using clear and descriptive variable and function
  names, consistent formatting, and appropriate comments. Code should be
  structured in a way that makes it easy to follow the flow of execution and
  understand the logic behind it. This is to ensure that the code is maintainable
  and can be easily understood by other developers.

- **BRULE-025**: Functions should be small and focused on a single task, having
  minimal side effects.

Code Quality (Warning-Level)
----------------------------

- **BRULE-026**: Code should be free of syntax errors and warnings.  This assumes
  maximal use of compiler warnings (e.g. -Wall -Wextra -Werror -Wpedantic for GCC).

- **BRULE-027**: C code should pass all MISRA checks.  We currently use MISRA C:2012
  as the standard for C code.  This is to ensure that the code adheres to best
  practices for safety and reliability in embedded systems development.

- **BRULE-028**: There should be a single point of exit from each function.

- **BRULE-029**: Use of strong typing and type-checking, including the use of
  size-based integers (uint8_t, etc).

- **BRULE-030**: No constant values embedded in code (magic numbers).  All
  constant values should be defined as macros or constants with descriptive names.
  This does not mean: #define FORTY_TWO 42, but rather #define PWM_MAX_DUTY_CYCLE 42.

- **BRULE-031**: Conditionals should have a max nest value of 2.  This is to
  ensure that the code is easy to read and understand, and to avoid deep nesting
  that can make the code difficult to follow.

- **BRULE-032**: Variables should be initialized at the point of definition to
  prevent usage of uninitialized values.

- **BRULE-033**: Useful comments should be provided at the file-level and
  function-level, where needed, to aid understanding and maintenance.

- **BRULE-034**: Code should not contain redundant, dead, commented out, or
  unused code or variables.

- **BRULE-035**: Proper use of headers is required, with minimal use of
  `extern` in `.c` files.

- **BRULE-036**: Parentheses should be used to avoid any operator precedence
  confusion.

- **BRULE-037**: All `switch` statements must include a `default` clause.

- **BRULE-038**: Code should compile without warnings for all possible C
  `#define` flags.

- **BRULE-039**: Scope for all functions and variables should be minimized;
  global variables should be minimized.

- **BRULE-040**: Floating point operations should only be used when an FPU or
  sufficient memory is available.

- **BRULE-041**: McCabe complexity level of any function should be less than 20.

- **BRULE-042**: A report of static and dynamic memory usage should be provided.

- **BRULE-043**: RAM and ROM utilization should be less than 90%.

- **BRULE-044**: The same source code should compile to the same binary output.


Code Quality (Critical-Level)
-----------------------------

- **BRULE-045**: Code should be free of memory leaks and other resource leaks.

- **BRULE-046**: All external sources and licenses should be clearly defined
  and documented. This includes any third-party libraries, frameworks, or tools
  used in the code. The documentation should include the license type, any
  restrictions or requirements imposed by the license, and any attribution or
  copyright notices required by the license.  In general, there should be strong
  file and directory separation from BISSELL-developed code and third-party code, 
  with both being clearly defined and marked.

- **BRULE-047**: Code should be free of undefined behavior. This includes avoiding
  constructs that can lead to unpredictable behavior, such as dereferencing null
  pointers, accessing out-of-bounds memory, or using uninitialized variables.
  Checks to avoid division by zero, buffer overflows, and other common pitfalls
  should be implemented. This is to ensure that the code is reliable and does not
  exhibit unexpected behavior during execution.

- **BRULE-048**: EEPROM corruption control logic should be implemented in
  all code that writes to EEPROM. This includes checks to ensure that the data
  being written is valid and does not exceed the bounds of the EEPROM memory.
  Additionally, mechanisms should be in place to handle power loss or other
  interruptions during EEPROM writes, to prevent corruption of the stored data.

- **BRULE-049**: Unused interrupt vectors should be directed to a default error 
  handler. This is to ensure that any unexpected interrupts do not cause the
  system to crash or behave unpredictably. The default error handler should log
  the interrupt and take appropriate action, such as resetting the system or
  entering a safe state.

- **BRULE-050**: The watchdog should exist, be initialized correctly, and be used
  appropriately. This is to ensure that the system can recover from unexpected
  conditions or hangs. The watchdog should be configured to trigger a reset if
  the system does not respond within a specified time period, and should be
  periodically refreshed by the code to prevent false triggers.

- **BRULE-051**: Where applicable, concurrency control mechanisms should be
  implemented to ensure that shared resources are accessed safely and
  consistently. This includes using mutexes, semaphores, or other synchronization
  primitives to protect shared data from concurrent access. This is to prevent
  race conditions, deadlocks, and other concurrency-related issues that can
  lead to unpredictable behavior or system crashes.

- **BRULE-052**: A trap should exist for stack overflow detection. This can be
  implemented using a stack guard or sentinel value that is checked at runtime
  to detect stack overflows. If a stack overflow is detected, the system should
  take appropriate action, such as logging the error and resetting the system.
  This is to ensure that the system can recover from stack overflows and prevent
  crashes or undefined behavior.

- **BRULE-053**: No long-running ISR (Interrupt Service Routine) should exist.
  ISRs should be kept short and efficient, with any long-running tasks being
  deferred to the main loop or a separate task. This is to ensure that the system
  can respond to interrupts in a timely manner and avoid blocking other critical
  tasks or interrupts.

- **BRULE-054**: Interrupt priority control should be clearly defined and
  documented. This includes specifying the priority levels of different
  interrupts, ensuring that critical interrupts have higher priority than less
  critical ones, and avoiding priority inversion issues. The interrupt priority
  configuration should be consistent across the codebase and should be clearly
  documented to facilitate understanding and maintenance.

- **BRULE-055**: Code should have clear brownout detection and recovery
  mechanisms. This includes detecting when the system voltage drops below a
  certain threshold and taking appropriate action, such as shutting down safely
  or entering a low-power state. The brownout detection should be implemented in
  hardware or software, depending on the system architecture, and should be
  tested to ensure reliable operation.

- **BRULE-056**: No hard-coded credentials or keys should exist in the code.
  This includes avoiding hard-coded passwords, API keys, or other sensitive
  information that could compromise the security of the system. Instead, such
  information should be stored securely, such as in a secure storage area or
  encrypted configuration file, and should be accessed securely at runtime.

