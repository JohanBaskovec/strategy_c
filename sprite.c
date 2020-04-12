#include "sprite.h"
#include "array.h"

int
spriteArrayAdd(
        SpriteArray *a
        , Sprite sprite
) {
    int newIndex = arrayAdd((Array*)a, sizeof(Sprite));
    a->data[newIndex] = sprite;
    return newIndex;
}

SpriteArray
spriteArrayCreate() {
    SpriteArray a = {
        .length = 0,
        .data = NULL
    };
    return a;
}
