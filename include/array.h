#ifndef DYNAMICLIST_H
#define DYNAMICLIST_H

#include <stdlib.h>

/**
 * Generic element type stored in the dynamic array.
 */
typedef void* TElem;

/**
 * Function pointer type for destroying elements.
 */
typedef void(*DestroyF)(TElem);

/**
 * Dynamic array structure.
 */
typedef struct {
    int cap;                 /**< Current allocated capacity. */
    int size;                /**< Current number of elements. */
    TElem* elems;            /**< Array of element pointers. */
    DestroyF destroyElement; /**< Function to destroy elements. */
} array_t;

/**
 * Creates a new dynamic array.
 * @param destroyElements Function to free individual elements, or NULL if not needed.
 * @return Pointer to a newly allocated array_t.
 */
array_t* create_array_t(DestroyF destroyElements);

/**
 * Frees memory associated with the dynamic array and its elements.
 * @param v Pointer to the array_t to destroy.
 */
void destroy_array_t(array_t* v);

/**
 * Resizes the array when capacity is reached.
 * Doubles the capacity to accommodate new elements.
 * @param v Pointer to the array_t to resize.
 */
void resize(array_t* v);

/**
 * Adds a new element to the dynamic array.
 * @param v Pointer to the array_t.
 * @param e The element to add.
 */
void add(array_t* v, TElem e);

/**
 * Updates the element at a given position.
 * @param v Pointer to the array_t.
 * @param e Pointer to the new element.
 * @param position Index of the element to update (0-based).
 */
void update(array_t* v, TElem* e, int position);

/**
 * Deletes the element at a given position.
 * Calls destroyElement on it if defined.
 * @param v Pointer to the array_t.
 * @param position Index of the element to remove (0-based).
 */
void del(array_t* v, int position);

/**
 * Checks if the array is empty.
 * @param v Pointer to the array_t.
 * @return 1 if empty, 0 otherwise.
 */
int empty(array_t* v);

#endif

