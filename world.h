#ifndef ST_WORLD
#define ST_WORLD
#include <stdbool.h>

#include "vec3f.h"
#include "defines.h"
#include "floor.h"
#include "box3f.h"
#include "graphics.h"
#include "entity.h"
#include "ai_component.h"

#define ENTITY_MAX_NUMBER 200000
typedef struct EntityFixedArray {
    int length;
    Entity data[ENTITY_MAX_NUMBER];
    int nextFree;
} EntityFixedArray;

typedef struct World {
    int width;
    int height;
    int widthTimesHeight;
    int depth;
    int tilesN;
    EntityFixedArray entities;
    bool end;
    int *entityTiles;
} World;

extern World world;

void
worldInit();

void
worldUpdate();

void
worldMoveRandom();

float
worldGetDifficulty(int x, int y, int z);

void
worldSetDifficulty(int x, int y, float value);

Entity*
worldGetEntity(int i);

void
worldFree();

int
worldCreateAndAddEntity(
        Vec3f position
        , ObjectType type
        , bool visible
);

Entity*
worldGetTileAlignedEntity(Vec3f pos);

void
worldRemoveEntity(int index);

#define MAP_INDEX(x, y, z) ((int)-z * world.widthTimesHeight + (int)x * world.height + (int)y)

#define MAP_INDEXV(v) ((int)-v.z * world.widthTimesHeight + (int)v.x * world.height + (int)v.y)
#endif
