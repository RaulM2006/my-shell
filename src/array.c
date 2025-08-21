#include "array.h"

array_t* create_array_t(DestroyF destroyElements) {
    array_t* v = malloc(sizeof(array_t));

    v->cap = 1;
    v->size = 0;
    v->elems = malloc(v->cap * sizeof(TElem));
    v->destroyElement = destroyElements;

    return v;
}

void destroy_array_t(array_t* v) {
    for (int i = 0; i < v->size; ++i) {
        if (v->elems[i] != NULL) {
            v->destroyElement(v->elems[i]);
        }
    }
    free(v->elems);
    free(v);
}

void resize(array_t* v) {
    int new_cap = v->cap * 2;
    TElem* aux = malloc(new_cap * sizeof(TElem));

    for (int i = 0; i < v->size; ++i) {
        aux[i] = v->elems[i];
    }
    free(v->elems);
    v->elems = aux;
    v->cap = new_cap;
}

void add(array_t* v, TElem e) {
    if (v->size == v->cap) {
        resize(v);
    }
    v->elems[v->size] = e;
    (v->size)++;
}

void update(array_t* v, TElem* e, int position) {
    v->destroyElement(v->elems[position]);
    v->elems[position] = e;
}

void del(array_t* v, int position) {
    v->destroyElement(v->elems[position]);
    for (int i = position; i < v->size - 1; ++i) {
        v->elems[i] = v->elems[i + 1];
    }
    (v->size)--;
}

int empty(array_t* v) {
    if (v->size == 0) {
        return 1;
    }
    return 0;
}
