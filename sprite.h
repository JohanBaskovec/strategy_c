#ifndef ST_SPRITE_H
#define ST_SPRITE_H
#include <stdbool.h>
#include "box3f.h"
#include "texture_enum.h"
#include "mat4f.h"

typedef struct Sprite {
    Mat4f modelMatrix;
    Vec3f colorMul;
    Vec3f colorAdd;
    Box3f box;
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

void
spriteMulColor(Sprite *s, float f);

void
spriteUnmulColor(Sprite *s);

void
spriteSetColorAdd(Sprite *s, Vec3f v);
#endif
