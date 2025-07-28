#ifndef DYNAMICLIST_H
#define DYNAMICLIST_H

#include <stdlib.h>

typedef void* TElem;

typedef struct {
    int cap;
    int size;
    TElem* elems;
} vector;

// vector constructor
vector* create_vector();

// vector destructor
void destroy_vector(vector* v);

// resize the vector when capacity is filled
void resize(vector* v);

void push_back(vector* v, TElem e);
void pop_back(vector* v);
TElem element(vector* v, int index);
int empty(vector* v);
#endif
