/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: file_paths.c
 *
 * This case demonstrates violations where file paths and directory
 * names that never change are not const-qualified.
 */

#include <stdio.h>
#include <string.h>

void system_paths(void) {
    /* NON-COMPLIANT: System paths should be const */
    char config_dir[] = "/etc";
    char home_dir[] = "/home";
    char temp_dir[] = "/tmp";
    char log_dir[] = "/var/log";
    char bin_dir[] = "/usr/bin";

    printf("System Directory Paths:\n");
    printf("  Config: %s\n", config_dir);
    printf("  Home: %s\n", home_dir);
    printf("  Temp: %s\n", temp_dir);
    printf("  Logs: %s\n", log_dir);
    printf("  Binaries: %s\n", bin_dir);

    /* Paths are used for file operations but never modified */
    char full_path[256];
    sprintf(full_path, "%s/myapp.conf", config_dir);
    printf("  Config file: %s\n", full_path);

    sprintf(full_path, "%s/myapp.log", log_dir);
    printf("  Log file: %s\n", full_path);
}

void application_files(void) {
    /* NON-COMPLIANT: Application file names should be const */
    char config_file[] = "app.conf";
    char log_file[] = "application.log";
    char pid_file[] = "app.pid";
    char lock_file[] = "app.lock";
    char data_file[] = "data.db";

    /* NON-COMPLIANT: File extensions should be const */
    char txt_extension[] = ".txt";
    char log_extension[] = ".log";
    char conf_extension[] = ".conf";
    char backup_extension[] = ".bak";

    printf("\nApplication Files:\n");
    printf("  Config: %s\n", config_file);
    printf("  Log: %s\n", log_file);
    printf("  PID: %s\n", pid_file);
    printf("  Lock: %s\n", lock_file);
    printf("  Database: %s\n", data_file);

    printf("\nFile Extensions:\n");
    printf("  Text: %s\n", txt_extension);
    printf("  Log: %s\n", log_extension);
    printf("  Config: %s\n", conf_extension);
    printf("  Backup: %s\n", backup_extension);

    /* Extensions used for file type checking but never modified */
    char test_file[] = "document.txt";
    if (strstr(test_file, txt_extension) != NULL) {
        printf("  %s is a text file\n", test_file);
    }
}

void resource_paths(void) {
    /* NON-COMPLIANT: Resource paths should be const */
    char image_path[] = "/assets/images/";
    char css_path[] = "/assets/css/";
    char js_path[] = "/assets/js/";
    char font_path[] = "/assets/fonts/";
    char icon_path[] = "/assets/icons/";

    /* NON-COMPLIANT: Default resource names should be const */
    char default_css[] = "style.css";
    char default_js[] = "script.js";
    char default_favicon[] = "favicon.ico";
    char default_logo[] = "logo.png";

    printf("\nWeb Resource Paths:\n");
    printf("  Images: %s\n", image_path);
    printf("  CSS: %s\n", css_path);
    printf("  JavaScript: %s\n", js_path);
    printf("  Fonts: %s\n", font_path);
    printf("  Icons: %s\n", icon_path);

    /* Paths used for URL construction but never modified */
    char full_url[256];
    sprintf(full_url, "%s%s", css_path, default_css);
    printf("  Default CSS: %s\n", full_url);

    sprintf(full_url, "%s%s", js_path, default_js);
    printf("  Default JS: %s\n", full_url);

    sprintf(full_url, "%s%s", icon_path, default_favicon);
    printf("  Favicon: %s\n", full_url);
}

void backup_paths(void) {
    /* NON-COMPLIANT: Backup directory patterns should be const */
    char backup_root[] = "/backup";
    char daily_backup[] = "/backup/daily";
    char weekly_backup[] = "/backup/weekly";
    char monthly_backup[] = "/backup/monthly";
    char archive_backup[] = "/backup/archive";

    /* NON-COMPLIANT: Backup filename patterns should be const */
    char daily_pattern[] = "daily_%Y%m%d.tar.gz";
    char weekly_pattern[] = "weekly_%Y_w%U.tar.gz";
    char monthly_pattern[] = "monthly_%Y%m.tar.gz";

    printf("\nBackup Configuration:\n");
    printf("  Root: %s\n", backup_root);
    printf("  Daily: %s\n", daily_backup);
    printf("  Weekly: %s\n", weekly_backup);
    printf("  Monthly: %s\n", monthly_backup);
    printf("  Archive: %s\n", archive_backup);

    printf("\nBackup Filename Patterns:\n");
    printf("  Daily: %s\n", daily_pattern);
    printf("  Weekly: %s\n", weekly_pattern);
    printf("  Monthly: %s\n", monthly_pattern);

    /* Patterns used for file generation but never modified */
    char backup_file[256];
    sprintf(backup_file, "%s/database_%s", daily_backup, "20240101");
    printf("  Sample backup: %s\n", backup_file);
}

int main(void) {
    /* NON-COMPLIANT: Installation paths should be const */
    char install_prefix[] = "/usr/local";
    char install_bindir[] = "/usr/local/bin";
    char install_libdir[] = "/usr/local/lib";
    char install_datadir[] = "/usr/local/share";

    printf("Installation Paths:\n");
    printf("  Prefix: %s\n", install_prefix);
    printf("  Binaries: %s\n", install_bindir);
    printf("  Libraries: %s\n", install_libdir);
    printf("  Data: %s\n", install_datadir);

    system_paths();
    application_files();
    resource_paths();
    backup_paths();

    return 0;
}
