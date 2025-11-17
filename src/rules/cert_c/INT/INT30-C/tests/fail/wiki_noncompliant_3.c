/*
 * Rule: INT30-C
 * Source: wiki
 * Status: FAIL - Should trigger INT30-C violation
 */

pen->num_vertices = _cairo_pen_vertices_needed(
  gstate->tolerance, radius, &gstate->ctm
);
pen->vertices = malloc(
  pen->num_vertices * sizeof(cairo_pen_vertex_t)
);