/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: compiler_flags.c
 *
 * This case demonstrates violations where compiler flags and
 * build configuration constants are not const-qualified.
 */

#include <stdio.h>

void compiler_options(void) {
    /* NON-COMPLIANT: Compiler flag names should be const */
    char flag_optimize[] = "-O2";
    char flag_debug[] = "-g";
    char flag_warnings[] = "-Wall";
    char flag_extra_warnings[] = "-Wextra";
    char flag_pedantic[] = "-pedantic";
    char flag_std_c11[] = "-std=c11";
    char flag_std_c99[] = "-std=c99";

    /* NON-COMPLIANT: Warning flags should be const */
    char warn_unused[] = "-Wunused";
    char warn_shadow[] = "-Wshadow";
    char warn_conversion[] = "-Wconversion";
    char warn_format[] = "-Wformat";
    char warn_init[] = "-Wuninitialized";

    printf("Compiler Flags:\\n");
    printf("  Optimization: %s\\n", flag_optimize);
    printf("  Debug info: %s\\n", flag_debug);
    printf("  Standards: %s, %s\\n", flag_std_c11, flag_std_c99);
    printf("  Basic warnings: %s, %s, %s\\n",
           flag_warnings, flag_extra_warnings, flag_pedantic);

    printf("\\nSpecific Warnings:\\n");
    printf("  %s, %s, %s, %s, %s\\n",
           warn_unused, warn_shadow, warn_conversion, warn_format, warn_init);

    /* Flags used for command construction but never modified */
    char compile_command[256];
    sprintf(compile_command, "gcc %s %s %s -o program main.c",
           flag_optimize, flag_warnings, flag_std_c11);
    printf("\\nSample command: %s\\n", compile_command);
}

void preprocessor_defines(void) {
    /* NON-COMPLIANT: Preprocessor definitions should be const */
    char define_debug[] = "-DDEBUG=1";
    char define_release[] = "-DRELEASE=1";
    char define_version[] = "-DVERSION=\\\"1.0.0\\\"";
    char define_platform[] = "-DPLATFORM_LINUX=1";
    char define_feature[] = "-DFEATURE_SSL=1";

    /* NON-COMPLIANT: Include paths should be const */
    char include_usr[] = "-I/usr/include";
    char include_local[] = "-I/usr/local/include";
    char include_project[] = "-I./include";
    char include_third_party[] = "-I./third_party/include";

    /* NON-COMPLIANT: Library paths should be const */
    char lib_path_usr[] = "-L/usr/lib";
    char lib_path_local[] = "-L/usr/local/lib";
    char lib_path_project[] = "-L./lib";

    printf("\\nPreprocessor Defines:\\n");
    printf("  Build type: %s, %s\\n", define_debug, define_release);
    printf("  Version: %s\\n", define_version);
    printf("  Platform: %s\\n", define_platform);
    printf("  Features: %s\\n", define_feature);

    printf("\\nInclude Paths:\\n");
    printf("  %s, %s, %s, %s\\n",
           include_usr, include_local, include_project, include_third_party);

    printf("\\nLibrary Paths:\\n");
    printf("  %s, %s, %s\\n", lib_path_usr, lib_path_local, lib_path_project);

    /* Paths used for build script generation but never modified */
    char build_flags[512];
    sprintf(build_flags, "%s %s %s %s",
           define_release, include_project, lib_path_project, define_version);
    printf("\\nBuild flags: %s\\n", build_flags);
}

void linker_options(void) {
    /* NON-COMPLIANT: Library names should be const */
    char lib_math[] = "-lm";
    char lib_pthread[] = "-lpthread";
    char lib_ssl[] = "-lssl";
    char lib_crypto[] = "-lcrypto";
    char lib_curl[] = "-lcurl";
    char lib_sqlite[] = "-lsqlite3";

    /* NON-COMPLIANT: Linker flags should be const */
    char flag_static[] = "-static";
    char flag_shared[] = "-shared";
    char flag_pie[] = "-pie";
    char flag_no_pie[] = "-no-pie";
    char flag_strip[] = "-s";

    /* NON-COMPLIANT: Runtime library paths should be const */
    char rpath_origin[] = "-Wl,-rpath,'$ORIGIN'";
    char rpath_lib[] = "-Wl,-rpath,/usr/local/lib";

    printf("\\nLinker Configuration:\\n");
    printf("  Libraries: %s, %s, %s, %s, %s, %s\\n",
           lib_math, lib_pthread, lib_ssl, lib_crypto, lib_curl, lib_sqlite);
    printf("  Flags: %s, %s, %s, %s, %s\\n",
           flag_static, flag_shared, flag_pie, flag_no_pie, flag_strip);
    printf("  RPATH: %s, %s\\n", rpath_origin, rpath_lib);

    /* Linker options used for executable creation but never modified */
    char link_command[256];
    sprintf(link_command, "gcc -o program main.o %s %s %s",
           lib_math, lib_pthread, flag_pie);
    printf("\\nLink command: %s\\n", link_command);
}

void build_targets(void) {
    /* NON-COMPLIANT: Target names should be const */
    char target_debug[] = "debug";
    char target_release[] = "release";
    char target_test[] = "test";
    char target_clean[] = "clean";
    char target_install[] = "install";

    /* NON-COMPLIANT: Output directories should be const */
    char dir_build[] = "./build";
    char dir_debug[] = "./build/debug";
    char dir_release[] = "./build/release";
    char dir_test[] = "./build/test";

    /* NON-COMPLIANT: Executable names should be const */
    char exe_debug[] = "program_debug";
    char exe_release[] = "program";
    char exe_test[] = "test_runner";

    printf("\\nBuild Targets:\\n");
    printf("  Targets: %s, %s, %s, %s, %s\\n",
           target_debug, target_release, target_test, target_clean, target_install);
    printf("  Directories: %s, %s, %s, %s\\n",
           dir_build, dir_debug, dir_release, dir_test);
    printf("  Executables: %s, %s, %s\\n", exe_debug, exe_release, exe_test);

    /* Build targets used for makefile generation but never modified */
    char makefile_rule[256];
    sprintf(makefile_rule, "%s: main.c\\n\\tgcc -g -o %s/%s main.c",
           target_debug, dir_debug, exe_debug);
    printf("\\nMakefile rule:\\n%s\\n", makefile_rule);
}

int main(void) {
    /* NON-COMPLIANT: Build configuration should be const */
    char build_system[] = "Make";
    char compiler[] = "GCC";
    char compiler_version[] = "9.4.0";
    char target_arch[] = "x86_64";
    char target_os[] = "Linux";

    /* NON-COMPLIANT: Project settings should be const */
    char project_name[] = "MyProject";
    char project_version[] = "1.2.3";
    char output_format[] = "ELF";

    printf("Build Configuration:\\n");
    printf("  System: %s\\n", build_system);
    printf("  Compiler: %s %s\\n", compiler, compiler_version);
    printf("  Target: %s-%s\\n", target_arch, target_os);
    printf("  Project: %s v%s\\n", project_name, project_version);
    printf("  Output: %s format\\n", output_format);

    compiler_options();
    preprocessor_defines();
    linker_options();
    build_targets();

    return 0;
}