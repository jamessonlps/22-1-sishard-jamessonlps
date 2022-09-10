#include "macros_correcoes.h"
#include <stdio.h>
#include <stdlib.h>

/* Copia string e retorna uma nova string */
char *mystrcpy(char *stroriginal) {
    int str_size = 0, k = 0;

    char i;
    while (i != '\0') {
        i = stroriginal[k];
        str_size++;
        k++;
    }
    str_size += 1; // adiciona o '\0'

    char *str_copy = malloc(str_size * sizeof(char));

    if (str_copy == NULL) {
        exit(0);
        return NULL;
    }

    for (int i = 0; i < str_size; i++) {
        str_copy[i] = stroriginal[i];
    }

    return str_copy;
}

int main(int argc, char *argv[]) {
    char *strcopia;

    char *s1 = "AAAAA";
    char *t1 = mystrcpy(s1);
    assertEquals("Teste 1", strcmp(s1, t1), 0);
    assertDifferent("Teste 1 - endereço diferente", s1, t1);
    free(t1);

    s1 = "";
    t1 = mystrcpy(s1);
    assertEquals("Teste 2", strcmp(s1, t1), 0);
    assertDifferent("Teste 2 - endereço diferente", s1, t1);
    free(t1);

    s1 = "AaAa123384739jlkfjdsl";
    t1 = mystrcpy(s1);
    assertEquals("Teste 3", strcmp(s1, t1), 0);
    assertDifferent("Teste 3 - endereço diferente", s1, t1);
    free(t1);

    s1 = "Aa00asdsada";
    t1 = mystrcpy(s1);
    assertEquals("Teste 4", strcmp(s1, t1), 0);
    assertDifferent("Teste 4 - endereço diferente", s1, t1);
    free(t1);

    s1 = "0asgfd";
    t1 = mystrcpy(s1);
    assertEquals("Teste 5", strcmp(s1, t1), 0);
    assertDifferent("Teste 5 - endereço diferente", s1, t1);
    free(t1);

    printSummary

        return 0;
}
