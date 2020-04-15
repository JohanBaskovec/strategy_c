#ifndef ST_ENTITY_H
#define ST_ENTITY_H
#include <stdbool.h>
#include "texture_enum.h"
#include "box3f.h"
#include "mat4f.h"
#include "object_type.h"

typedef struct Entity {
    bool alive;
    enum Texture texture;
    int spriteIndex;
    Box3f box;
    int ai;
    Vec3f velocity;
    bool hovered;
    bool selected;
    ObjectType type;
    float movementDifficulty;
    bool gridAligned;
    bool isTemp;
} Entity;

Entity
entityCreate(
        Box3f box
        , enum Texture texture
        , int spriteIndex
        , ObjectType type
        , bool gridAligned
);

typedef struct EntityArray {
    int length;
    int allocated;
    Entity *data;
} EntityArray;

void entityUpdate(Entity *e);
#endif
