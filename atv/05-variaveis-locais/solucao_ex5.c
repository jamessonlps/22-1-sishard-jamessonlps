#include <stdio.h>

/**
 * @brief Exercicio 5
 * 0xc(%rsp) => int a
 *
 * @return int
 */

int ex5_solucao() {
    int retval, ebx = 0, a = 1;
    while (a > 0) {
        scanf("%d", &a);
        retval = a;
        if (retval > 0) {
            ebx += retval;
        }
    }
    retval = ebx;
    return retval;
}
