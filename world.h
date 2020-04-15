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

typedef struct World {
    int width;
    int height;
    int depth;
    int tilesN;
    float *difficultyMap;
    EntityArray entities;
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
worldGetDifficulty(int x, int y);

void
worldSetDifficulty(int x, int y, float value);

Entity*
worldGetEntity(int i);

void
worldFree();

#define MAP_INDEX(x, y) ((int)x * world.height + (int)y)
#endif
