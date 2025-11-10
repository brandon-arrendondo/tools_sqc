/* (Incorrect) Set all bits in mask to 1 */
const unsigned long mask = 0xFFFFFFFF;

unsigned long flipbits(unsigned long x) {
  return x ^ mask;
}