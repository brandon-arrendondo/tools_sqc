/*
 * Rule: DCL05-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL05-C violation
 */

struct obj {
  int i;
  float f;
};
typedef struct obj Object;
 
void func(const Object *o) {
  /* Cannot modify o's contents */
}