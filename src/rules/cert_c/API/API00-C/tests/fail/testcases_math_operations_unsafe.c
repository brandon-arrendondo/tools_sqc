/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: math_operations_unsafe.c
 *
 * This case demonstrates violations where mathematical functions
 * don't validate their parameters for domain and range safety.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

/* Mathematical vector structure */
typedef struct {
    double *components;
    size_t dimension;
} Vector;

/* Mathematical matrix structure */
typedef struct {
    double **elements;
    size_t rows;
    size_t columns;
} Matrix;

/* NON-COMPLIANT: No validation of logarithm input */
double safe_logarithm(double value, double base) {
    /* No validation of value or base */
    return log(value) / log(base);  /* value could be <= 0, base could be <= 0 or == 1 */
}

/* NON-COMPLIANT: No validation of square root input */
double safe_square_root(double value) {
    /* No validation of value */
    return sqrt(value);  /* value could be negative */
}

/* NON-COMPLIANT: No validation of arc trigonometric functions */
double safe_arcsine(double value) {
    /* No validation of value range */
    return asin(value);  /* value must be in [-1, 1] */
}

/* NON-COMPLIANT: No validation of vector operations */
double vector_dot_product(const Vector *v1, const Vector *v2) {
    /* No validation of vectors or dimension compatibility */
    double result = 0.0;
    for (size_t i = 0; i < v1->dimension; i++) {  /* v1 could be NULL */
        result += v1->components[i] * v2->components[i];  /* v2 could be NULL or different dimension */
    }
    return result;
}

/* NON-COMPLIANT: No validation of vector creation */
Vector *create_vector(size_t dimension) {
    Vector *vector = malloc(sizeof(Vector));
    /* No validation of dimension */
    vector->dimension = dimension;  /* Could be 0 */
    vector->components = malloc(dimension * sizeof(double));  /* Could be 0-byte allocation */
    return vector;
}

/* NON-COMPLIANT: No validation of matrix operations */
Matrix *matrix_multiply(const Matrix *m1, const Matrix *m2) {
    /* No validation of matrices or dimension compatibility */
    Matrix *result = malloc(sizeof(Matrix));
    result->rows = m1->rows;      /* m1 could be NULL */
    result->columns = m2->columns; /* m2 could be NULL */

    /* No check if m1->columns == m2->rows */
    result->elements = malloc(result->rows * sizeof(double *));
    for (size_t i = 0; i < result->rows; i++) {
        result->elements[i] = malloc(result->columns * sizeof(double));
        for (size_t j = 0; j < result->columns; j++) {
            result->elements[i][j] = 0.0;
            for (size_t k = 0; k < m1->columns; k++) {  /* Could access invalid memory */
                result->elements[i][j] += m1->elements[i][k] * m2->elements[k][j];
            }
        }
    }

    return result;
}

/* NON-COMPLIANT: No validation of statistical functions */
double calculate_mean(const double *values, size_t count) {
    /* No validation of values array or count */
    double sum = 0.0;
    for (size_t i = 0; i < count; i++) {
        sum += values[i];  /* values could be NULL */
    }
    return sum / count;  /* count could be 0 */
}

/* NON-COMPLIANT: No validation of standard deviation calculation */
double calculate_standard_deviation(const double *values, size_t count) {
    /* No validation of parameters */
    double mean = calculate_mean(values, count);  /* values could be NULL, count could be 0 */
    double sum_squared_diff = 0.0;

    for (size_t i = 0; i < count; i++) {
        double diff = values[i] - mean;
        sum_squared_diff += diff * diff;
    }

    return sqrt(sum_squared_diff / (count - 1));  /* count could be <= 1 */
}

/* NON-COMPLIANT: No validation of polynomial evaluation */
double evaluate_polynomial(const double *coefficients, size_t degree, double x) {
    /* No validation of coefficients or degree */
    double result = 0.0;
    double x_power = 1.0;

    for (size_t i = 0; i <= degree; i++) {  /* coefficients could be NULL */
        result += coefficients[i] * x_power;
        x_power *= x;  /* Could overflow for large x */
    }

    return result;
}

/* NON-COMPLIANT: No validation of numerical integration */
double integrate_function(double (*func)(double), double a, double b, int num_intervals) {
    /* No validation of function pointer or parameters */
    double h = (b - a) / num_intervals;  /* num_intervals could be 0 */
    double sum = 0.0;

    for (int i = 0; i < num_intervals; i++) {
        double x = a + i * h;
        sum += func(x);  /* func could be NULL */
    }

    return sum * h;
}

/* NON-COMPLIANT: No validation of root finding */
double find_root_newton(double (*func)(double), double (*derivative)(double), double initial_guess, int max_iterations) {
    /* No validation of function pointers or parameters */
    double x = initial_guess;

    for (int i = 0; i < max_iterations; i++) {  /* max_iterations could be negative */
        double fx = func(x);        /* func could be NULL */
        double fpx = derivative(x); /* derivative could be NULL */

        if (fpx == 0.0) {  /* No handling of zero derivative */
            break;
        }

        x = x - fx / fpx;  /* Could diverge */
    }

    return x;
}

/* NON-COMPLIANT: No validation of complex number operations */
typedef struct {
    double real;
    double imaginary;
} Complex;

Complex complex_divide(Complex a, Complex b) {
    /* No validation of division by zero */
    double denominator = b.real * b.real + b.imaginary * b.imaginary;  /* Could be 0 */
    Complex result;
    result.real = (a.real * b.real + a.imaginary * b.imaginary) / denominator;
    result.imaginary = (a.imaginary * b.real - a.real * b.imaginary) / denominator;
    return result;
}

/* NON-COMPLIANT: No validation of factorial calculation for large numbers */
double factorial_recursive(int n) {
    /* No validation of n */
    if (n < 0) {  /* Partial validation but insufficient */
        return -1;
    }
    if (n <= 1) {
        return 1;
    }
    return n * factorial_recursive(n - 1);  /* Will overflow for large n, infinite recursion risk */
}

int main(void) {
    Vector *null_vector = NULL;
    Matrix *null_matrix = NULL;
    double *null_array = NULL;

    /* Examples of dangerous mathematical operations */
    // safe_logarithm(-5.0, 0.0);  /* Negative value and invalid base */
    // safe_square_root(-10.0);  /* Negative value */
    // safe_arcsine(2.0);  /* Value outside domain */
    // vector_dot_product(null_vector, null_vector);  /* NULL vectors */
    // create_vector(0);  /* Zero dimension */
    // matrix_multiply(null_matrix, null_matrix);  /* NULL matrices */
    // calculate_mean(null_array, 0);  /* NULL array and zero count */
    // calculate_standard_deviation(null_array, 1);  /* Insufficient data points */
    // evaluate_polynomial(null_array, 10, 1e10);  /* NULL coefficients and large x */
    // integrate_function(NULL, 0, 1, 0);  /* NULL function and zero intervals */
    // find_root_newton(NULL, NULL, 0, -5);  /* NULL functions and negative iterations */
    // complex_divide((Complex){1, 0}, (Complex){0, 0});  /* Division by zero */
    // factorial_recursive(200);  /* Will cause overflow */

    printf("Mathematical functions compiled but lack parameter validation\n");
    return 0;
}