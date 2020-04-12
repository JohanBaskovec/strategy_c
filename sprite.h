#ifndef ST_SPRITE_H
#define ST_SPRITE_H
#include <stdbool.h>
#include "box3f.h"
#include "texture_enum.h"

typedef struct Sprite {
    bool alive;
    Box3f box;
} Sprite;

typedef struct SpriteArray {
    int length;
    int allocated;
    Sprite *data;
} SpriteArray;

int
spriteArrayAdd(
        SpriteArray *a
        , Sprite sprite
);

SpriteArray
spriteArrayCreate();
#endif
