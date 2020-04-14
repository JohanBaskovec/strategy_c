#ifndef ST_SPRITE_H
#define ST_SPRITE_H
#include <stdbool.h>
#include "box3f.h"
#include "texture_enum.h"
#include "mat4f.h"

typedef struct Sprite {
    Mat4f modelMatrix;
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

void
spriteMove(Sprite *s, Vec3f v);
#endif
