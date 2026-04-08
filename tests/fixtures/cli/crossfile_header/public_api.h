/*
 * Cross-file header test — public API header.
 * Declares prototypes for functions that are intentionally non-static.
 * DCL15-C should NOT flag these as missing static qualifier when
 * prescan sees the .h prototype.
 */

#ifndef PUBLIC_API_H
#define PUBLIC_API_H

int compute_value(int x);
void print_result(int value);

#endif
