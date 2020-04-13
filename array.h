#ifndef ST_ARRAY_H
#define ST_ARRAY_H
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
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
    // replace dead entity
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


#define arrayAdd(arr, element)\
    do {\
        int newIndex = voidArrayAdd((Array*)&arr, sizeof(element));\
        arr.data[newIndex] = element;\
    } while(0);

#define arrayPtrAdd(arr, element)\
    do {\
        int newIndex = voidArrayAdd((Array*)arr, sizeof(element));\
        arr->data[newIndex] = element;\
    } while(0);

#define arrayRemove(arr, i)\
    do {\
        voidArrayRemove((Array*)&arr, i, sizeof(*arr.data));\
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
    a.allocated = 0;

#endif
