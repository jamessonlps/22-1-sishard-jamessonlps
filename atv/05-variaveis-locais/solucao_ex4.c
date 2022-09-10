#include <stdio.h>

int ex4_solucao(int d) {
    int b = 12;
    int c = 8;
    scanf("%d %d", &b, &c);
    int retval = b + (2 * c) + d;
    return retval;
}