/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: color_definitions.c
 *
 * This case demonstrates violations where color definitions
 * and graphics constants are not const-qualified.
 */

#include <stdio.h>

void rgb_colors(void) {
    /* NON-COMPLIANT: RGB color values should be const */
    unsigned char red_r = 255, red_g = 0, red_b = 0;
    unsigned char green_r = 0, green_g = 255, green_b = 0;
    unsigned char blue_r = 0, blue_g = 0, blue_b = 255;
    unsigned char white_r = 255, white_g = 255, white_b = 255;
    unsigned char black_r = 0, black_g = 0, black_b = 0;

    /* NON-COMPLIANT: Packed RGB values should be const */
    unsigned int color_red = 0xFF0000;
    unsigned int color_green = 0x00FF00;
    unsigned int color_blue = 0x0000FF;
    unsigned int color_yellow = 0xFFFF00;
    unsigned int color_cyan = 0x00FFFF;
    unsigned int color_magenta = 0xFF00FF;

    printf("RGB Color Components:\\n");
    printf("  Red: (%d, %d, %d)\\n", red_r, red_g, red_b);
    printf("  Green: (%d, %d, %d)\\n", green_r, green_g, green_b);
    printf("  Blue: (%d, %d, %d)\\n", blue_r, blue_g, blue_b);
    printf("  White: (%d, %d, %d)\\n", white_r, white_g, white_b);
    printf("  Black: (%d, %d, %d)\\n", black_r, black_g, black_b);

    printf("\\nPacked RGB Values:\\n");
    printf("  Red: 0x%06X\\n", color_red);
    printf("  Green: 0x%06X\\n", color_green);
    printf("  Blue: 0x%06X\\n", color_blue);
    printf("  Yellow: 0x%06X\\n", color_yellow);
    printf("  Cyan: 0x%06X\\n", color_cyan);
    printf("  Magenta: 0x%06X\\n", color_magenta);

    /* Color values used for graphics but never modified */
    unsigned int background_color = color_white;
    unsigned int text_color = color_black;
    printf("\\nDisplay: Background=0x%06X, Text=0x%06X\\n", background_color, text_color);
}

void palette_colors(void) {
    /* NON-COMPLIANT: Color palette should be const */
    unsigned int palette_16[] = {
        0x000000, 0x800000, 0x008000, 0x808000,
        0x000080, 0x800080, 0x008080, 0xC0C0C0,
        0x808080, 0xFF0000, 0x00FF00, 0xFFFF00,
        0x0000FF, 0xFF00FF, 0x00FFFF, 0xFFFFFF
    };

    /* NON-COMPLIANT: Color names should be const */
    char color_names[][12] = {
        "Black", "Maroon", "Green", "Olive",
        "Navy", "Purple", "Teal", "Silver",
        "Gray", "Red", "Lime", "Yellow",
        "Blue", "Fuchsia", "Aqua", "White"
    };

    printf("\\n16-Color Palette:\\n");
    for (int i = 0; i < 16; i++) {
        printf("  %2d: %-8s 0x%06X\\n", i, color_names[i], palette_16[i]);
    }

    /* Palette used for graphics rendering but never modified */
    unsigned int selected_color = palette_16[9];  /* Red */
    printf("\\nSelected color: 0x%06X\\n", selected_color);
}

int main(void) {
    /* NON-COMPLIANT: Graphics configuration should be const */
    int color_depth = 24;
    int alpha_bits = 8;
    char color_space[] = "sRGB";
    double gamma_value = 2.2;

    printf("Graphics Configuration:\\n");
    printf("  Color depth: %d bits\\n", color_depth);
    printf("  Alpha channel: %d bits\\n", alpha_bits);
    printf("  Color space: %s\\n", color_space);
    printf("  Gamma: %.1f\\n", gamma_value);

    rgb_colors();
    palette_colors();

    return 0;
}