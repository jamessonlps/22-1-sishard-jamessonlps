#include <stdio.h>
#include <stdlib.h>

#include "vec_int.h"

typedef struct _vec_int {
    int *data;
    int size;
    int capacity;
} vec_int;

vec_int *vec_int_create() {
    vec_int *v = malloc(sizeof(vec_int)); // tamanho = 16
    v->capacity = 2;
    v->size = 0;
    v->data = malloc((v->capacity) * sizeof(int));
    return v;
}

void vec_int_destroy(vec_int **_v) {
    free((*_v)->data); // libera memória alocada para o data
    free(*_v);         // libera memória alocada para vec_int
    *_v = NULL;
}

int vec_int_size(vec_int *v) {
    return v->size;
}

int vec_int_at(vec_int *v, int pos, int *val) {
    if (v->size == 0 || pos < 0 || pos >= v->size) {
        return 0;
    }
    *val = v->data[pos];
    return 1;
}

int vec_int_insert(vec_int *v, int pos, int val) {
    if (pos < 0 || pos > v->size) {
        return 0;
    }
    // Expande vetor se ja estiver no tamanho maximo
    if (v->size == v->capacity - 1) {
        int *new_data;
        new_data = realloc(v->data, (v->capacity) * sizeof(int) * 2);
        if (new_data == NULL) {
            return 0;
        }
        v->data = new_data;
        v->capacity *= 2;
    }

    // Desloca os elementos do vetor
    for (int i = v->size - 1; i >= pos; i--) {
        v->data[i + 1] = v->data[i];
    }

    // Grava novo valor e atualiza tamanho
    v->data[pos] = val;
    v->size++;
    return 1;
}

int vec_int_remove(vec_int *v, int pos) {
    // Checa se posição é válida
    if (pos < 0 || pos >= v->size) {
        return 0;
    }

    // Desloca os elementos do vetor para eliminiar o escolhido
    for (int i = pos; i < v->size - 1; i++) {
        v->data[i] = v->data[i + 1];
    }

    if (v->size == (v->capacity) / 4) {
        int *new_data;
        new_data = realloc(v->data, (v->capacity) * sizeof(int) / 2);
        if (new_data == NULL) {
            return 0;
        }
        v->data = new_data;
        v->capacity /= 2;
    }

    v->size--;
    return 1;
}
