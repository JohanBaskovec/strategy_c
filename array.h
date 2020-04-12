#ifndef ST_ARRAY_H
#define ST_ARRAY_H
#include <stdbool.h>
#include <stddef.h>
typedef struct ArrayElement {
    bool alive;
} ArrayElement;

typedef struct Array {
    int length;
    int allocated;
    void *data;
} Array;

int
arrayAdd(Array *a, size_t sizeOfElement);

void
arrayRemove(Array *a, int i, size_t sizeOfElement);


#define ARRAY_ADD(arr, element, newIndex)\
    do {\
        newIndex = arrayAdd((Array*)&arr, sizeof(element));\
        arr.data[newIndex] = element;\
    } while(0);

#define ARRAY_PTR_ADD(arr, element)\
    do {\
        int newIndex = arrayAdd((Array*)arr, sizeof(element));\
        arr->data[newIndex] = element;\
    } while(0);

#define ARRAY_REMOVE(arr, i)\
    do {\
        arrayRemove((Array*)&arr, i, sizeof(*arr.data));\
    } while(0);

#define ARRAY_NEW \
{\
    .length = 0\
    , .allocated = 0\
    , .data = NULL\
}

#endif
