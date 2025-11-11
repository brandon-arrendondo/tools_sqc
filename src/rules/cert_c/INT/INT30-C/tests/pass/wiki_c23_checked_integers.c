/*
 * Rule: INT30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT30-C violation
 */

#include <stdckdint.h>

/* ... */

pen->num_vertices = _cairo_pen_vertices_needed(
  gstate->tolerance, radius, &gstate->ctm
);

size_t product;
if (ckd_mul(&product, pen->num_vertices, sizeof(cairo_pen_vertex_t))) {
  /* Handle error */
}

pen->vertices = malloc(product);