#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include "world.h"
#include <stdbool.h>
#include "maths.h"
#include <math.h>
#include "error.h"
#include "graphics.h"
#include "array.h"
#include "error.h"
#include "ai_component.h"
#include "ai_system.h"

World world;

Floor dirtFloor;

float
worldGetDifficulty(int x, int y);

void
worldSetDifficulty(int x, int y, float value);

int
worldAddEntity(Entity entity) {
    int i = world.entities.length;
    arrayAdd(world.entities, entity);
    return i;
}

int
createAndAddEntity(Vec3f position, enum Texture texture, bool isWall) {
    Box3f b = box3fCreate(position, graphics.tileSize);
    Sprite sprite = spriteCreate(b);
    int spriteI = graphicsAddSprite(texture, sprite);
    Entity entity = entityCreate(b, texture, spriteI);
    int entityI = worldAddEntity(entity);
    Sprite *s = graphicsGetSprite(texture, spriteI);
    s->entity = entityI;
    if (isWall) {
        if (position.z == 1) {
            worldSetDifficulty(position.x, position.y, 1);
        }
        int i = MAP_INDEX(position.x, position.y);
        world.entityTiles[i] = entityI;
    }
    return entityI;
}

void
addWall(int x, int y) {
    Vec3f p = {x, y, 1};
    createAndAddEntity(p, TEXTURE_WALL, true);
}

Entity*
worldGetEntity(int i) {
    return &world.entities.data[i];
}

void
worldInit() {
    world.end = false;
    world.width = 100;
    world.height = 100;
    world.tilesN = world.width * world.height;
    world.difficultyMap = malloc(world.tilesN * sizeof(float));
    world.entityTiles = malloc(world.tilesN * sizeof(int));
    for (int x = 0 ; x < world.width ; x++) {
        for (int y = 0 ; y < world.height ; y++) {
            worldSetDifficulty(x, y, 0.1);
        }
    }
    world.entities.length = 0;
    world.entities.data = NULL;

    for (int x = 0 ; x < world.width ; x++) {
        for (int y = 0 ; y < world.height ; y++) {
            if (rand() % 5 == 0) {
                addWall(x, y);
            }
            Vec3f p = {x, y, 0};
            createAndAddEntity(p, TEXTURE_WOOD_FLOOR, true);
        }
    }


    {
        enum Texture texture = TEXTURE_DIRT_FLOOR;
        Vec3f p = {1, 1, 1};
        int entityI = createAndAddEntity(p, texture, false);
        AiComponent ac = aiComponentCreate(entityI);
        int aci = aiSystemAddAiComponent(ac);
        Entity *e = worldGetEntity(entityI);
        e->ai = aci;
    }
}

float
worldGetDifficulty(int x, int y) {
    return world.difficultyMap[x * world.height + y];
}

void
worldSetDifficulty(int x, int y, float value) {
    world.difficultyMap[x * world.height + y] = value;
}

void
worldUpdate() {
    for (int i = 0 ; i < world.entities.length ; i++) {
        Entity *entity = &world.entities.data[i];
        entityUpdate(entity);
    }
    aiSystemUpdate();
}

void
worldMoveRandom() {
    int goalX = rand() % world.width;
    int goalY = rand() % world.height;
    //int goalX = 1;
    //int goalY = 3;

    Vec3f target = {goalX, goalY, 1};
    //SDL_Log("worldMoveRandom, target=%d:%d", goalX, goalY);
    aiSystem.aiComponents.data[0].target = target;
    aiSystem.aiComponents.data[0].hasTarget = true;
}

void
worldFree() {
    arrayFree(world.entities);
    free(world.difficultyMap);
}
