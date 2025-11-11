/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: bit_masks.c
 *
 * This case demonstrates violations where bit masks and flags
 * that are never modified are not const-qualified.
 */

#include <stdio.h>

void permission_flags(void) {
    /* NON-COMPLIANT: Permission flags should be const */
    unsigned int READ_PERMISSION = 0x04;
    unsigned int WRITE_PERMISSION = 0x02;
    unsigned int EXECUTE_PERMISSION = 0x01;
    unsigned int ALL_PERMISSIONS = 0x07;

    /* NON-COMPLIANT: User type masks should be const */
    unsigned int USER_MASK = 0x1C0;  /* Bits 6-8 */
    unsigned int GROUP_MASK = 0x038;  /* Bits 3-5 */
    unsigned int OTHER_MASK = 0x007;  /* Bits 0-2 */

    unsigned int file_perms = 0755;  /* rwxr-xr-x in octal */

    printf("Permission Flags:\n");
    printf("  READ:    0x%02X\n", READ_PERMISSION);
    printf("  WRITE:   0x%02X\n", WRITE_PERMISSION);
    printf("  EXECUTE: 0x%02X\n", EXECUTE_PERMISSION);
    printf("  ALL:     0x%02X\n", ALL_PERMISSIONS);

    /* Using masks but never modifying them */
    if (file_perms & USER_MASK) {
        printf("User permissions set\n");
    }
    if (file_perms & GROUP_MASK) {
        printf("Group permissions set\n");
    }
    if (file_perms & OTHER_MASK) {
        printf("Other permissions set\n");
    }
}

void hardware_registers(void) {
    /* NON-COMPLIANT: Hardware register masks should be const */
    unsigned int STATUS_READY = 0x0001;
    unsigned int STATUS_BUSY = 0x0002;
    unsigned int STATUS_ERROR = 0x0004;
    unsigned int STATUS_INTERRUPT = 0x0008;

    /* NON-COMPLIANT: Control register bits should be const */
    unsigned int CTRL_ENABLE = 0x0001;
    unsigned int CTRL_RESET = 0x0002;
    unsigned int CTRL_MODE_0 = 0x0010;
    unsigned int CTRL_MODE_1 = 0x0020;

    unsigned int status_reg = 0x0005;  /* READY and ERROR */
    unsigned int control_reg = 0x0011;  /* ENABLE and MODE_0 */

    printf("\nHardware Register Masks:\n");

    /* Masks are used for bit testing but never modified */
    printf("Status Register (0x%04X):\n", status_reg);
    if (status_reg & STATUS_READY) printf("  - Ready\n");
    if (status_reg & STATUS_BUSY) printf("  - Busy\n");
    if (status_reg & STATUS_ERROR) printf("  - Error\n");
    if (status_reg & STATUS_INTERRUPT) printf("  - Interrupt\n");

    printf("Control Register (0x%04X):\n", control_reg);
    if (control_reg & CTRL_ENABLE) printf("  - Enabled\n");
    if (control_reg & CTRL_RESET) printf("  - Reset\n");
    if (control_reg & CTRL_MODE_0) printf("  - Mode 0\n");
    if (control_reg & CTRL_MODE_1) printf("  - Mode 1\n");
}

void color_masks(void) {
    /* NON-COMPLIANT: Color component masks should be const */
    unsigned int RED_MASK = 0xFF0000;
    unsigned int GREEN_MASK = 0x00FF00;
    unsigned int BLUE_MASK = 0x0000FF;
    unsigned int ALPHA_MASK = 0xFF000000;

    /* NON-COMPLIANT: Bit shift amounts should be const */
    int RED_SHIFT = 16;
    int GREEN_SHIFT = 8;
    int BLUE_SHIFT = 0;
    int ALPHA_SHIFT = 24;

    unsigned int color = 0xFF4080C0;  /* ARGB color */

    printf("\nColor Component Extraction:\n");
    printf("Color value: 0x%08X\n", color);

    /* Masks and shifts are used but never modified */
    unsigned int red = (color & RED_MASK) >> RED_SHIFT;
    unsigned int green = (color & GREEN_MASK) >> GREEN_SHIFT;
    unsigned int blue = (color & BLUE_MASK) >> BLUE_SHIFT;
    unsigned int alpha = (color & ALPHA_MASK) >> ALPHA_SHIFT;

    printf("  Alpha: 0x%02X (%d)\n", alpha, alpha);
    printf("  Red:   0x%02X (%d)\n", red, red);
    printf("  Green: 0x%02X (%d)\n", green, green);
    printf("  Blue:  0x%02X (%d)\n", blue, blue);
}

int main(void) {
    /* NON-COMPLIANT: File attribute flags should be const */
    unsigned short ATTR_READONLY = 0x0001;
    unsigned short ATTR_HIDDEN = 0x0002;
    unsigned short ATTR_SYSTEM = 0x0004;
    unsigned short ATTR_DIRECTORY = 0x0010;
    unsigned short ATTR_ARCHIVE = 0x0020;

    unsigned short file_attrs = 0x0021;  /* READONLY | ARCHIVE */

    printf("File Attributes (0x%04X):\n", file_attrs);

    /* Flags are used for testing but never modified */
    if (file_attrs & ATTR_READONLY) printf("  [R] Read-only\n");
    if (file_attrs & ATTR_HIDDEN) printf("  [H] Hidden\n");
    if (file_attrs & ATTR_SYSTEM) printf("  [S] System\n");
    if (file_attrs & ATTR_DIRECTORY) printf("  [D] Directory\n");
    if (file_attrs & ATTR_ARCHIVE) printf("  [A] Archive\n");

    permission_flags();
    hardware_registers();
    color_masks();

    return 0;
}