/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: coordinate_systems.c
 *
 * This case demonstrates violations where coordinate and dimension
 * values that never change are not const-qualified.
 */

#include <stdio.h>
#include <math.h>

void screen_dimensions(void) {
    /* NON-COMPLIANT: Screen dimensions should be const */
    int SCREEN_WIDTH = 1920;
    int SCREEN_HEIGHT = 1080;
    int REFRESH_RATE = 60;
    int COLOR_DEPTH = 32;

    /* NON-COMPLIANT: Display areas should be const */
    int MENU_HEIGHT = 30;
    int STATUS_BAR_HEIGHT = 25;
    int SIDEBAR_WIDTH = 200;

    printf("Display Configuration:\n");
    printf("  Resolution: %dx%d @ %dHz\n", SCREEN_WIDTH, SCREEN_HEIGHT, REFRESH_RATE);
    printf("  Color depth: %d bits\n", COLOR_DEPTH);

    /* Values used for calculations but never modified */
    int usable_width = SCREEN_WIDTH - SIDEBAR_WIDTH;
    int usable_height = SCREEN_HEIGHT - MENU_HEIGHT - STATUS_BAR_HEIGHT;

    printf("  Usable area: %dx%d pixels\n", usable_width, usable_height);
    printf("  Total pixels: %d\n", SCREEN_WIDTH * SCREEN_HEIGHT);
}

void grid_layout(void) {
    /* NON-COMPLIANT: Grid parameters should be const */
    int GRID_ROWS = 10;
    int GRID_COLS = 12;
    int CELL_WIDTH = 50;
    int CELL_HEIGHT = 40;
    int CELL_PADDING = 5;

    printf("\nGrid Layout:\n");
    printf("  Grid size: %d x %d\n", GRID_ROWS, GRID_COLS);
    printf("  Cell dimensions: %d x %d pixels\n", CELL_WIDTH, CELL_HEIGHT);
    printf("  Padding: %d pixels\n", CELL_PADDING);

    /* Parameters used for layout but never modified */
    int total_cells = GRID_ROWS * GRID_COLS;
    int grid_width = GRID_COLS * (CELL_WIDTH + CELL_PADDING);
    int grid_height = GRID_ROWS * (CELL_HEIGHT + CELL_PADDING);

    printf("  Total cells: %d\n", total_cells);
    printf("  Grid dimensions: %d x %d pixels\n", grid_width, grid_height);
}

void coordinate_bounds(void) {
    /* NON-COMPLIANT: Coordinate bounds should be const */
    double MIN_X = -100.0;
    double MAX_X = 100.0;
    double MIN_Y = -100.0;
    double MAX_Y = 100.0;
    double MIN_Z = 0.0;
    double MAX_Z = 50.0;

    /* NON-COMPLIANT: Origin coordinates should be const */
    double ORIGIN_X = 0.0;
    double ORIGIN_Y = 0.0;
    double ORIGIN_Z = 0.0;

    printf("\n3D Coordinate System:\n");
    printf("  X range: [%.1f, %.1f]\n", MIN_X, MAX_X);
    printf("  Y range: [%.1f, %.1f]\n", MIN_Y, MAX_Y);
    printf("  Z range: [%.1f, %.1f]\n", MIN_Z, MAX_Z);
    printf("  Origin: (%.1f, %.1f, %.1f)\n", ORIGIN_X, ORIGIN_Y, ORIGIN_Z);

    /* Bounds used for validation but never modified */
    double test_x = 50.0;
    double test_y = -30.0;
    double test_z = 25.0;

    printf("\nPoint validation (%.1f, %.1f, %.1f):\n", test_x, test_y, test_z);
    if (test_x >= MIN_X && test_x <= MAX_X) {
        printf("  X coordinate is valid\n");
    }
    if (test_y >= MIN_Y && test_y <= MAX_Y) {
        printf("  Y coordinate is valid\n");
    }
    if (test_z >= MIN_Z && test_z <= MAX_Z) {
        printf("  Z coordinate is valid\n");
    }
}

void map_projection(void) {
    /* NON-COMPLIANT: Map constants should be const */
    double MIN_LATITUDE = -90.0;
    double MAX_LATITUDE = 90.0;
    double MIN_LONGITUDE = -180.0;
    double MAX_LONGITUDE = 180.0;

    /* NON-COMPLIANT: Map scale factors should be const */
    double MAP_SCALE = 1000000.0;  /* 1:1,000,000 */
    double METERS_PER_DEGREE_LAT = 111320.0;
    double METERS_PER_DEGREE_LON = 111320.0;  /* At equator */

    printf("\nMap Projection Parameters:\n");
    printf("  Latitude range: [%.1f°, %.1f°]\n", MIN_LATITUDE, MAX_LATITUDE);
    printf("  Longitude range: [%.1f°, %.1f°]\n", MIN_LONGITUDE, MAX_LONGITUDE);
    printf("  Map scale: 1:%.0f\n", MAP_SCALE);

    /* Constants used for conversion but never modified */
    double lat = 40.7128;  /* New York */
    double lon = -74.0060;

    printf("\nCoordinate conversion:\n");
    printf("  Location: %.4f°N, %.4f°W\n", lat, -lon);

    double meters_lat = lat * METERS_PER_DEGREE_LAT;
    double meters_lon = lon * METERS_PER_DEGREE_LON * cos(lat * M_PI / 180.0);

    printf("  Meters from equator: %.0f\n", meters_lat);
    printf("  Meters from prime meridian: %.0f\n", meters_lon);
}

int main(void) {
    /* NON-COMPLIANT: Window dimensions should be const */
    int WINDOW_MIN_WIDTH = 800;
    int WINDOW_MIN_HEIGHT = 600;
    int WINDOW_MAX_WIDTH = 2560;
    int WINDOW_MAX_HEIGHT = 1440;

    printf("Window Constraints:\n");
    printf("  Minimum: %d x %d\n", WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
    printf("  Maximum: %d x %d\n", WINDOW_MAX_WIDTH, WINDOW_MAX_HEIGHT);

    screen_dimensions();
    grid_layout();
    coordinate_bounds();
    map_projection();

    return 0;
}