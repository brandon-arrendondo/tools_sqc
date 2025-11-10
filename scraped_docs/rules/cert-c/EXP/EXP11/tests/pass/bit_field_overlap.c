struct bf {
  unsigned int m1 : 6;
  unsigned int m2 : 4;
};

void function() {
  struct bf data;
  data.m1 = 0;
  data.m2 = 0;
  data.m2 += 1;
}