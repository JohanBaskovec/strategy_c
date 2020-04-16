#include <stdbool.h>
#include "entity.h"
#include "array.h"
#include "vec3f.h"
#include "sprite.h"
#include "graphics.h"

Entity
entityCreate(
        Box3f box
        , enum Texture texture
        , int spriteIndex
        , ObjectType type
        , bool gridAligned
        , bool reachable
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
    ret.type = type;
    ret.movementDifficulty = 1.0;
    ret.gridAligned = gridAligned;
    ret.isTemp = false;
    ret.keep = true;
    ret.id = -1;
    ret.reachable = reachable;
    return ret;
}

void
entityUpdate(Entity *e) {
    if (!vec3fIsZero(e->velocity)) {
        e->box = box3fMoveVec3f(e->box, e->velocity);

        Sprite *s = graphicsGetSprite(e->texture, e->spriteIndex);
        spriteMove(s, e->velocity);
    }
}
