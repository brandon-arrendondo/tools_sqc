/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: license_constants.c
 *
 * This case demonstrates violations where license information
 * and legal constants are not const-qualified.
 */

#include <stdio.h>

void license_info(void) {
    /* NON-COMPLIANT: License text should be const */
    char license_name[] = "MIT License";
    char license_version[] = "1.0";
    char license_url[] = "https://opensource.org/licenses/MIT";

    /* NON-COMPLIANT: Copyright information should be const */
    char copyright_holder[] = "Example Corporation";
    char copyright_year[] = "2024";
    char copyright_notice[] = "Copyright (c) 2024 Example Corporation";

    /* NON-COMPLIANT: License text body should be const */
    char license_text[] =
        "Permission is hereby granted, free of charge, to any person "
        "obtaining a copy of this software and associated documentation "
        "files, to deal in the Software without restriction...";

    printf("License Information:\\n");
    printf("  Name: %s v%s\\n", license_name, license_version);
    printf("  URL: %s\\n", license_url);
    printf("  %s\\n", copyright_notice);
    printf("  Holder: %s\\n", copyright_holder);
    printf("  Year: %s\\n", copyright_year);

    printf("\\nLicense Text (excerpt):\\n");
    printf("  %s\\n", license_text);

    /* License info used for display but never modified */
    char full_notice[256];
    sprintf(full_notice, "%s - %s", copyright_notice, license_name);
    printf("\\nFull notice: %s\\n", full_notice);
}

void software_info(void) {
    /* NON-COMPLIANT: Software metadata should be const */
    char software_name[] = "Example Application";
    char software_version[] = "2.1.0";
    char build_date[] = "2024-01-15";
    char build_time[] = "14:30:25";
    char build_number[] = "2024.01.15.1430";

    /* NON-COMPLIANT: Author information should be const */
    char primary_author[] = "John Developer";
    char author_email[] = "john@example.com";
    char organization[] = "Example Software Inc.";
    char website[] = "https://www.example.com";

    /* NON-COMPLIANT: Third-party components should be const */
    char component1_name[] = "OpenSSL";
    char component1_version[] = "1.1.1";
    char component1_license[] = "OpenSSL License";

    char component2_name[] = "zlib";
    char component2_version[] = "1.2.11";
    char component2_license[] = "zlib License";

    printf("\\nSoftware Information:\\n");
    printf("  Name: %s\\n", software_name);
    printf("  Version: %s\\n", software_version);
    printf("  Build: %s %s (Build %s)\\n", build_date, build_time, build_number);

    printf("\\nAuthor Information:\\n");
    printf("  Primary Author: %s <%s>\\n", primary_author, author_email);
    printf("  Organization: %s\\n", organization);
    printf("  Website: %s\\n", website);

    printf("\\nThird-party Components:\\n");
    printf("  %s v%s (%s)\\n", component1_name, component1_version, component1_license);
    printf("  %s v%s (%s)\\n", component2_name, component2_version, component2_license);

    /* Software info used for about dialog but never modified */
    char about_text[512];
    sprintf(about_text, "%s v%s\\nBuilt on %s\\nBy %s",
            software_name, software_version, build_date, primary_author);
    printf("\\nAbout text: %s\\n", about_text);
}

void legal_notices(void) {
    /* NON-COMPLIANT: Legal disclaimers should be const */
    char warranty_disclaimer[] =
        "THIS SOFTWARE IS PROVIDED 'AS IS' WITHOUT WARRANTY OF ANY KIND";
    char liability_disclaimer[] =
        "IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DAMAGES";
    char trademark_notice[] =
        "All trademarks are property of their respective owners";

    /* NON-COMPLIANT: Compliance statements should be const */
    char export_control[] =
        "This software may be subject to export control regulations";
    char privacy_statement[] =
        "This software does not collect personal information";
    char accessibility_statement[] =
        "We strive to make our software accessible to all users";

    printf("\\nLegal Notices:\\n");
    printf("  Warranty: %s\\n", warranty_disclaimer);
    printf("  Liability: %s\\n", liability_disclaimer);
    printf("  Trademarks: %s\\n", trademark_notice);

    printf("\\nCompliance Statements:\\n");
    printf("  Export Control: %s\\n", export_control);
    printf("  Privacy: %s\\n", privacy_statement);
    printf("  Accessibility: %s\\n", accessibility_statement);

    /* Legal text used for documentation but never modified */
    printf("\\nLegal footer generated from disclaimers\\n");
}

void attribution_info(void) {
    /* NON-COMPLIANT: Attribution requirements should be const */
    char attribution_required[] = "Attribution required for redistribution";
    char attribution_format[] = "Powered by %s - %s";
    char source_available[] = "Source code available at: %s";
    char repository_url[] = "https://github.com/example/project";

    /* NON-COMPLIANT: Contributor information should be const */
    char contributors[] = "John Doe, Jane Smith, Bob Johnson";
    char maintainer[] = "Development Team";
    char contact_email[] = "support@example.com";

    /* NON-COMPLIANT: Project metadata should be const */
    char project_status[] = "Active Development";
    char last_updated[] = "2024-01-15";
    char supported_until[] = "2026-01-15";

    printf("\\nAttribution Information:\\n");
    printf("  Requirement: %s\\n", attribution_required);
    printf("  Format: %s\\n", attribution_format);
    printf("  Source: %s\\n", repository_url);

    printf("\\nProject Information:\\n");
    printf("  Contributors: %s\\n", contributors);
    printf("  Maintainer: %s\\n", maintainer);
    printf("  Contact: %s\\n", contact_email);
    printf("  Status: %s\\n", project_status);
    printf("  Last Updated: %s\\n", last_updated);
    printf("  Supported Until: %s\\n", supported_until);

    /* Attribution used for credits display but never modified */
    char attribution_text[256];
    sprintf(attribution_text, attribution_format, "Example Library", "v1.0");
    printf("\\nSample attribution: %s\\n", attribution_text);
}

int main(void) {
    /* NON-COMPLIANT: Application metadata should be const */
    char app_title[] = "License and Legal Information Demo";
    char app_description[] = "Demonstrates software licensing and legal notices";
    char terms_of_service[] = "By using this software, you agree to our terms";
    char privacy_policy[] = "Your privacy is important to us";

    printf("Application: %s\\n", app_title);
    printf("Description: %s\\n", app_description);
    printf("Terms: %s\\n", terms_of_service);
    printf("Privacy: %s\\n", privacy_policy);

    license_info();
    software_info();
    legal_notices();
    attribution_info();

    return 0;
}