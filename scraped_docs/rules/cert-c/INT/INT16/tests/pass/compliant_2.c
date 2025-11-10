unsigned int value;

if (scanf("%u", &value) == 1) {
  if (value & 0x1 != 0) {
    /* Take action if value is odd */
  }
}