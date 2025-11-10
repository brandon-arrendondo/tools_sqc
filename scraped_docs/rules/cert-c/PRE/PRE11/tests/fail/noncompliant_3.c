#define INCREMOD(x, max) ((x) = ((x) + 1) % (max));

int index = 0;
int value;
value = INCREMOD(index, 10) + 2;
/* ... */