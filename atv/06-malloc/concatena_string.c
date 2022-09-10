#include "macros_correcoes.h"
#include <stdio.h>
#include <stdlib.h>

/* Recebe duas strings e concatena em uma terceira */
char *mystrcat(char *str1, char *str2) {
    int size_str1 = 0, size_str2 = 0, k = 0;
    char i = '\n', j = '\n';

    while (i != '\0' && str1[0] != 0) {
        i = str1[k];
        if (i != '\0') {
            size_str1++;
            k++;
        }
    }

    k = 0;
    while (j != '\0' && str2[0] != 0) {
        j = str2[k];
        if (j != '\0') {
            size_str2++;
            k++;
        }
    }

    char *str_concat = malloc((size_str1 + size_str2 + 1) * sizeof(char));

    for (int i = 0; i < size_str1; i++) {
        str_concat[i] = str1[i];
    }

    for (int i = 0; i < size_str2; i++) {
        str_concat[i + size_str1] = str2[i];
    }

    str_concat[size_str1 + size_str2] = '\0';

    return str_concat;
}

int main(int argc, char *argv[]) {
#define teste1 "str1"
#define teste2 "str2"
#define teste3 "0 str 3"

    char *res = mystrcat(teste1, teste2);
    assertEquals("teste 1", strcmp(res, teste1 teste2), 0);
    free(res);

    res = mystrcat(teste1, teste1);
    assertEquals("teste 2", strcmp(res, teste1 teste1), 0);
    free(res);

    res = mystrcat(teste1, teste3);
    assertEquals("teste 3", strcmp(res, teste1 teste3), 0);
    free(res);

    res = mystrcat(teste3, teste1);
    assertEquals("teste 4", strcmp(res, teste3 teste1), 0);
    free(res);

    res = mystrcat("", teste2);
    assertEquals("teste vazio 1", strcmp(res, teste2), 0);
    free(res);

    res = mystrcat(teste1, "");
    assertEquals("teste vazio 2", strcmp(res, teste1), 0);
    free(res);

    res = mystrcat("", "");
    assertEquals("teste vazio 3", strcmp(res, ""), 0);
    free(res);

    printSummary

        return 0;
}
