#ifndef ST_ARRAY_H
#define ST_ARRAY_H
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
typedef struct ArrayElement {
    bool alive;
} ArrayElement;

typedef struct Array {
    int length;
    int allocated;
    void *data;
} Array;

static inline int
voidArrayAdd(Array *a, size_t sizeOfElement) {
    // init empty array
    if (a->allocated == 0) {
        a->allocated = 128;
        a->data = malloc(sizeOfElement * a->allocated);
        a->length++;
        return 0;
    }
    if (a->length < a->allocated) {
        int newIndex = a->length;
        a->length++;
        return newIndex;
    }
    // no free space, allocate new memory
    int newIndex = a->length;
    a->length++;
    a->allocated *= 2;
    a->data = realloc(
            a->data
            , a->allocated * sizeOfElement
    );
    return newIndex;
}

static inline void
voidArrayRemove(Array *a, int i, size_t sizeOfElement) {
    // init empty array
    if (a->allocated == 0) {
        return;
    }
    // if removing the last element, we don't have to copy elements
    if (a->length != i + 1) {
        void *dest = a->data + (i * sizeOfElement);
        void *source = a->data + (i * sizeOfElement) + sizeOfElement;
        size_t count = sizeOfElement * (a->length - i - 1);
        memcpy(dest, source, count);
    }
    a->length--;
}

static inline int
fixedVoidArrayAdd(Array *a, size_t sizeOfElement) {
    if (a->length < a->allocated) {
        int newIndex = a->length;
        a->length++;
        return newIndex;
    }
    // no free space, crash
    fatalError("fixedVoidArrayAdd: No free space in array.");
    return 0;
}

static inline void
fixedVoidArrayRemove(Array *a, int i, size_t sizeOfElement) {
    // if removing the last element, we don't have to copy elements
    if (a->length != i + 1) {
        void *dest = a->data + (i * sizeOfElement);
        void *source = a->data + (i * sizeOfElement) + sizeOfElement;
        size_t count = sizeOfElement * (a->length - i - 1);
        memcpy(dest, source, count);
    }
    a->length--;
}


#define arrayAdd(arr, element)\
    do {\
        int newIndex = voidArrayAdd((Array*)arr, sizeof(element));\
        (arr)->data[newIndex] = element;\
    } while(0);

#define arrayRemove(arr, i)\
    do {\
        voidArrayRemove((Array*)arr, i, sizeof(*((arr)->data)));\
    } while(0);

#define arrayCreate() \
    {\
        .length = 0\
        , .allocated = 0\
        , .data = NULL\
    }

#define arrayFree(a)\
    free(a.data);\
    a.length = 0;\
    a.allocated = 0;\
    a.data = NULL;

#define ARRAY_DECLARE(type, name)\
    typedef struct name {\
        int length;\
        int allocated;\
        type *data;\
    } name;

typedef struct VoidFixedArray {
    int length;
    int nextFree;
    void *data[100];
} VoidFixedArray;

// TODO: allow dynamic expanding of the array (but not shrinking)
#define fixedArrayAdd(arr, elementToAdd, size, nextFreeIndex) \
    do {\
        if ((arr)->nextFree == -1) {\
            printf("Error: array is full.\n");\
            exit(1);\
        }\
        nextFreeIndex = (arr)->nextFree;\
        (arr)->nextFree = (arr)->data[nextFreeIndex].nextFree;\
        elementToAdd.id = nextFreeIndex;\
        (arr)->data[nextFreeIndex] = elementToAdd;\
        if ((arr)->length < nextFreeIndex+1) {\
            (arr)->length = nextFreeIndex+1;\
        }\
    } while (0);

#define fixedArrayRemove(a, index) \
    {\
        printf("deleting\n");\
        (a)->data[index].nextFree = (a)->nextFree;\
        (a)->data[index].keep = false;\
        (a)->nextFree = index;\
    }

#define fixedArrayInit(a, size) \
    {\
        (a)->length = 0;\
        (a)->nextFree = 0;\
        for (int k = 0 ; k < size - 1 ; k++) {\
            (a)->data[k].nextFree = k+1;\
        }\
        (a)->data[size - 1].nextFree = -1;\
    }

#endif
