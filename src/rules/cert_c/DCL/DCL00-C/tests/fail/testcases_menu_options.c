/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: menu_options.c
 *
 * This case demonstrates violations where menu options and
 * user interface constants are not const-qualified.
 */

#include <stdio.h>

void main_menu(void) {
    /* NON-COMPLIANT: Menu option constants should be const */
    int OPTION_NEW = 1;
    int OPTION_OPEN = 2;
    int OPTION_SAVE = 3;
    int OPTION_SAVE_AS = 4;
    int OPTION_PRINT = 5;
    int OPTION_EXIT = 9;

    /* NON-COMPLIANT: Menu text should be const */
    char menu_title[] = "=== Main Menu ===";
    char option_new[] = "1. New Document";
    char option_open[] = "2. Open Document";
    char option_save[] = "3. Save Document";
    char option_save_as[] = "4. Save As...";
    char option_print[] = "5. Print Document";
    char option_exit[] = "9. Exit";

    printf("%s\\n", menu_title);
    printf("%s\\n", option_new);
    printf("%s\\n", option_open);
    printf("%s\\n", option_save);
    printf("%s\\n", option_save_as);
    printf("%s\\n", option_print);
    printf("%s\\n", option_exit);

    /* Menu constants used for selection but never modified */
    int user_choice = 3;
    if (user_choice == OPTION_SAVE) {
        printf("User selected: Save Document\\n");
    } else if (user_choice == OPTION_EXIT) {
        printf("User selected: Exit\\n");
    }
}

void settings_menu(void) {
    /* NON-COMPLIANT: Settings categories should be const */
    char category_display[] = "Display";
    char category_audio[] = "Audio";
    char category_network[] = "Network";
    char category_security[] = "Security";
    char category_advanced[] = "Advanced";

    /* NON-COMPLIANT: Setting option IDs should be const */
    int DISPLAY_RESOLUTION = 101;
    int DISPLAY_BRIGHTNESS = 102;
    int DISPLAY_CONTRAST = 103;
    int AUDIO_VOLUME = 201;
    int AUDIO_QUALITY = 202;
    int NETWORK_TIMEOUT = 301;

    printf("\\nSettings Menu:\\n");
    printf("Categories: %s, %s, %s, %s, %s\\n",
           category_display, category_audio, category_network,
           category_security, category_advanced);

    printf("Display options: %d, %d, %d\\n",
           DISPLAY_RESOLUTION, DISPLAY_BRIGHTNESS, DISPLAY_CONTRAST);
    printf("Audio options: %d, %d\\n", AUDIO_VOLUME, AUDIO_QUALITY);
    printf("Network options: %d\\n", NETWORK_TIMEOUT);

    /* Settings used for configuration but never modified */
    int current_setting = DISPLAY_BRIGHTNESS;
    printf("Current setting ID: %d\\n", current_setting);
}

void toolbar_buttons(void) {
    /* NON-COMPLIANT: Button IDs should be const */
    int BTN_NEW = 1001;
    int BTN_OPEN = 1002;
    int BTN_SAVE = 1003;
    int BTN_CUT = 1004;
    int BTN_COPY = 1005;
    int BTN_PASTE = 1006;
    int BTN_UNDO = 1007;
    int BTN_REDO = 1008;

    /* NON-COMPLIANT: Button labels should be const */
    char label_new[] = "New";
    char label_open[] = "Open";
    char label_save[] = "Save";
    char label_cut[] = "Cut";
    char label_copy[] = "Copy";
    char label_paste[] = "Paste";
    char label_undo[] = "Undo";
    char label_redo[] = "Redo";

    /* NON-COMPLIANT: Tooltip text should be const */
    char tooltip_new[] = "Create a new document";
    char tooltip_open[] = "Open an existing document";
    char tooltip_save[] = "Save the current document";

    printf("\\nToolbar Configuration:\\n");
    printf("Button IDs: %d, %d, %d, %d, %d, %d, %d, %d\\n",
           BTN_NEW, BTN_OPEN, BTN_SAVE, BTN_CUT,
           BTN_COPY, BTN_PASTE, BTN_UNDO, BTN_REDO);

    printf("Labels: %s, %s, %s, %s\\n", label_new, label_open, label_save, label_cut);
    printf("Tooltips: %s, %s, %s\\n", tooltip_new, tooltip_open, tooltip_save);

    /* Button IDs used for event handling but never modified */
    int clicked_button = BTN_SAVE;
    if (clicked_button == BTN_SAVE) {
        printf("Save button clicked: %s\\n", tooltip_save);
    }
}

void dialog_boxes(void) {
    /* NON-COMPLIANT: Dialog button constants should be const */
    int DLG_OK = 1;
    int DLG_CANCEL = 2;
    int DLG_YES = 3;
    int DLG_NO = 4;
    int DLG_APPLY = 5;
    int DLG_CLOSE = 6;

    /* NON-COMPLIANT: Dialog messages should be const */
    char msg_confirm_exit[] = "Are you sure you want to exit?";
    char msg_save_changes[] = "Do you want to save your changes?";
    char msg_delete_confirm[] = "This action cannot be undone. Continue?";
    char msg_error_generic[] = "An error occurred. Please try again.";

    /* NON-COMPLIANT: Dialog titles should be const */
    char title_confirmation[] = "Confirmation";
    char title_warning[] = "Warning";
    char title_error[] = "Error";
    char title_information[] = "Information";

    printf("\\nDialog Box Configuration:\\n");
    printf("Button codes: OK=%d, Cancel=%d, Yes=%d, No=%d\\n",
           DLG_OK, DLG_CANCEL, DLG_YES, DLG_NO);

    printf("Titles: %s, %s, %s, %s\\n",
           title_confirmation, title_warning, title_error, title_information);

    printf("Messages:\\n");
    printf("  Exit: %s\\n", msg_confirm_exit);
    printf("  Save: %s\\n", msg_save_changes);
    printf("  Delete: %s\\n", msg_delete_confirm);

    /* Dialog constants used for display but never modified */
    int user_response = DLG_YES;
    if (user_response == DLG_YES) {
        printf("User confirmed action\\n");
    }
}

int main(void) {
    /* NON-COMPLIANT: Window dimensions should be const */
    int WINDOW_WIDTH = 800;
    int WINDOW_HEIGHT = 600;
    int MIN_WINDOW_WIDTH = 640;
    int MIN_WINDOW_HEIGHT = 480;

    /* NON-COMPLIANT: Color scheme IDs should be const */
    int THEME_LIGHT = 1;
    int THEME_DARK = 2;
    int THEME_HIGH_CONTRAST = 3;

    printf("UI Configuration:\\n");
    printf("Window: %dx%d (min: %dx%d)\\n",
           WINDOW_WIDTH, WINDOW_HEIGHT, MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);
    printf("Themes: Light=%d, Dark=%d, High Contrast=%d\\n",
           THEME_LIGHT, THEME_DARK, THEME_HIGH_CONTRAST);

    main_menu();
    settings_menu();
    toolbar_buttons();
    dialog_boxes();

    return 0;
}