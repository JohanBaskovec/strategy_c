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
#include "vec3i.h"
#include "box3i.h"

#define ENTITY_MAX_NUMBER 200000
typedef struct EntityFixedArray {
    int length;
    Entity data[ENTITY_MAX_NUMBER];
    int nextFree;
} EntityFixedArray;

typedef struct World {
    int widthTimesHeight;
    int tilesN;
    EntityFixedArray entities;
    bool end;
    int *entityTiles;
    Box3i box;
} World;

extern World world;

void
worldInit();

void
worldUpdate();

void
worldMoveRandom();

float
worldGetDifficulty(Vec3f v);

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

#define MAP_INDEX(ax, ay, az) ((int)(-(az)) * world.widthTimesHeight + (int)ax * world.box.size.y + (int)ay)

#define MAP_INDEXV(v) ((int)(-(v.z)) * world.widthTimesHeight + (int)v.x * world.box.size.y + (int)v.y)

typedef struct MinMaxVec {
    Vec3f min;
    Vec3f max;
} MinMaxVec;

MinMaxVec
worldGetMinMaxPos(Vec3f p);

typedef struct MinMaxVeci {
    Vec3i min;
    Vec3i max;
} MinMaxVeci;

MinMaxVeci
worldGetMinMaxPosi(Vec3i p);
#endif
