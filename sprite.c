#include "sprite.h"
#include "array.h"

Sprite
spriteCreate(Box3f box) {
    Sprite s;
    s.box = box;
    return s;
}
