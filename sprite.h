#ifndef ST_SPRITE_H
#define ST_SPRITE_H
#include <stdbool.h>
#include "box3f.h"
#include "texture_enum.h"

typedef struct Sprite {
    Box3f box;
    bool selected;
} Sprite;

Sprite
spriteCreate(Box3f box);
typedef struct SpriteArray {
    int length;
    int allocated;
    Sprite *data;
} SpriteArray;
#endif
