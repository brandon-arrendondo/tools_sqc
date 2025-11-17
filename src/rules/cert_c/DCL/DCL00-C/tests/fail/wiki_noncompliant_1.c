/*
 * Rule: DCL00-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL00-C violation
 */

float pi = 3.14159f;
float degrees;
float radians;
/* ... */
radians = degrees * pi / 180;