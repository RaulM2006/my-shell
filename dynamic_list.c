#include "dynamic_list.h"

vector* create_vector() {
    vector* v = malloc(sizeof(vector));

    v->cap = 1;
    v->size = 0;
    v->elems = malloc(v->cap * sizeof(TElem));

    return v;
}

void destroy_vector(vector* v) {
    for (int i = 0; i < v->size; ++i) {
        free(v->elems[i]);
    }
    free(v->elems);
    free(v);
}

void resize(vector* v) {
    int new_cap = v->cap * 2;
    TElem* aux = malloc(new_cap * sizeof(TElem));

    for (int i = 0; i < v->size; ++i) {
        aux[i] = v->elems[i];
    }
    free(v->elems);
    v->elems = aux;
    v->cap = new_cap;
}

void push_back(vector* v, TElem e) {
    if (v->size == v->cap) {
        resize(v);
    }
    v->elems[v->size] = e;
    (v->size)++;
}

void pop_back(vector* v) {
    if (v->size > 0) {
        free(v->elems[v->size - 1]);
        (v->size)--;
    }
}

TElem element(vector* v, int index) {
    if (index < 0 || index >= v->size) {
        return NULL;
    }
    return v->elems[index];
}

int empty(vector* v) {
    if (v->size == 0) {
        return 1;
    }
    return 0;
}
