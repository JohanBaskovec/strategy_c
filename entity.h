#ifndef ST_ENTITY_H
#define ST_ENTITY_H
#include <stdbool.h>
#include "texture_enum.h"
#include "box3f.h"

typedef struct Entity {
    bool alive;
    enum Texture texture;
    int spriteIndex;
    Box3f box;
    int ai;
    Vec3f velocity;
} Entity;

typedef struct EntityArray {
    int length;
    int allocated;
    Entity *data;
} EntityArray;

#endif
