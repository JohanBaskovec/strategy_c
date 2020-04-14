#include "sprite.h"
#include "array.h"

Sprite
spriteCreate(Box3f box) {
    Sprite s;
    s.box = box;
    s.selected = false;
    return s;
}

void
spriteMove(Sprite *s, Vec3f v) {
    s->box = box3fMoveVec3f(s->box, v);

    s->modelMatrix = mat4fVec3fTranslate(mat4fIdentity(), s->box.position);
    s->modelMatrix = mat4fScale(s->modelMatrix, s->box.size);
}
