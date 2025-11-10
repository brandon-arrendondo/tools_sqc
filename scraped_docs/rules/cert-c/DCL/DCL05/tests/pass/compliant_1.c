struct obj {
  int i;
  float f;
};
typedef struct obj Object;
 
void func(const Object *o) {
  /* Cannot modify o's contents */
}