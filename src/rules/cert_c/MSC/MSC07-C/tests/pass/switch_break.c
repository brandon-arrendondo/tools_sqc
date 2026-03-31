// Test: code after switch-with-break is reachable

#include <stdio.h>

void classify(int x) {
    switch (x) {
    case 0:
        printf("zero\n");
        break;
    case 1:
        printf("one\n");
        break;
    default:
        printf("other\n");
        break;
    }
    printf("done\n");  // reachable after switch
}
