#include <stdbool.h>
#include "entity.h"
#include "array.h"
#include "vec3f.h"

Entity
entityCreate(
        Box3f box
        , enum Texture texture
        , int spriteIndex
) {
    Entity ret;
    ret.velocity = vec3fZero;
    ret.ai = -1;
    ret.texture = texture;
    ret.spriteIndex = spriteIndex;
    ret.alive = true;
    ret.box = box;
    ret.hovered = false;
    ret.selected = false;
    ret.texture = texture;
    return ret;
}
