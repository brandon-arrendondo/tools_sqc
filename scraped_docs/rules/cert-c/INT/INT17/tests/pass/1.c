/* (Correct) Set all bits in mask to 1 */
const unsigned long mask = -1;

unsigned long flipbits(unsigned long x) {
  return x ^ mask;
}