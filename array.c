#include "array.h"
#include <SDL.h>
#include <stdlib.h>

int
arrayAdd(Array *a, size_t sizeOfElement) {
    SDL_Log("Allocated: %d, length: %d", a->allocated, a->length);
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

void
arrayRemove(Array *a, int i, size_t sizeOfElement) {
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
