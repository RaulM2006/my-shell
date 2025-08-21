#ifndef DYNAMICLIST_H
#define DYNAMICLIST_H

#include <stdlib.h>

typedef void* TElem;
typedef void(*DestroyF)(TElem);

typedef struct {
    int cap;
    int size;
    TElem* elems;
    DestroyF destroyElement;
} array_t;

// array_t constructor
array_t* create_array_t(DestroyF destroyElements);

// array_t destructor
void destroy_array_t(array_t* v);

// resize the array_t when capacity is filled
void resize(array_t* v);

void add(array_t* v, TElem e);
void update(array_t* v, TElem* e, int position);
void del(array_t* v, int position);

int empty(array_t* v);

#endif
