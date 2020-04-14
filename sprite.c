#include "sprite.h"
#include "array.h"

Sprite
spriteCreate(Box3f box) {
    Sprite s;
    s.box = box;
    s.colorMul = vec3fOne;
    s.colorAdd = vec3fZero;
    return s;
}

void
spriteMove(Sprite *s, Vec3f v) {
    s->box = box3fMoveVec3f(s->box, v);

    s->modelMatrix = mat4fVec3fTranslate(mat4fIdentity(), s->box.position);
    s->modelMatrix = mat4fScale(s->modelMatrix, s->box.size);
}

void
spriteSetColorAdd(Sprite *s, Vec3f v) {
    s->colorAdd = v;
}

void
spriteUnmulColor(Sprite *s) {
    s->colorMul = vec3fOne;
}

void
spriteMulColor(Sprite *s, float f) {
    Vec3f v = {f, f, f};
    s->colorMul = v;
}
