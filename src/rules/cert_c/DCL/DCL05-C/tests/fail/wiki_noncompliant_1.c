/*
 * Rule: DCL05-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL05-C violation
 */

struct obj {
  int i;
  float f;
};
typedef struct obj *ObjectPtr;
 
void func(const ObjectPtr o) {
  /* Can actually modify o's contents, against expectations */
}