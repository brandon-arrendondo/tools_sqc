int insert(int index, int *list, int size, int value) {
  if (size != 0) {
    index = (index + 1) % size;
    list[index] = value;
    return index;
  }
  else {
    return -1;
  }
}