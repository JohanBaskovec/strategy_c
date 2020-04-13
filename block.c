#include "block.h"

Block
blockCreate(
        Box3f box
        , enum Texture texture
        , int spriteIndex
) {
    Block b;
    b.box = box;
    b.texture = texture;
    b.spriteIndex = spriteIndex;
    return b;
}
