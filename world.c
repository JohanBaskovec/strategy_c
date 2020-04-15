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
#include "object_type.h"

World world;

Floor dirtFloor;

int
worldAddEntity(Entity entity) {
    int i = world.entities.length;
    arrayAdd(&world.entities, entity);
    return i;
}

int
createAndAddEntity(
        Vec3f position
        , enum Texture texture
        , bool isWall
        , ObjectType type
) {
    Box3f b = box3fCreate(position, graphics.tileSize);
    Sprite sprite = spriteCreate(b);
    int spriteI = graphicsAddSprite(texture, sprite);
    Entity entity = entityCreate(b, texture, spriteI);
    int entityI = worldAddEntity(entity);
    Sprite *s = graphicsGetSprite(texture, spriteI);
    s->entity = entityI;
    if (isWall) {
        int i = MAP_INDEX(position.x, position.y, position.z);
        if (entityI < 20) {
            SDL_Log("%f %f %f", position.x, position.y, position.z);
            SDL_Log("world.entityTiles[%d] = %d", i, entityI);
        }
        world.entityTiles[i] = entityI;
    }
    return entityI;
}

void
addWall(int x, int y) {
    Vec3f p = {x, y, 0};
    createAndAddEntity(p, TEXTURE_WALL, true, OBJECT_WALL);
}

void
addTree(int x, int y) {
    Vec3f p = {x, y, 0};
    createAndAddEntity(p, TEXTURE_TREE, true, OBJECT_TREE);
}

Entity*
worldGetEntity(int i) {
    return &world.entities.data[i];
}

void
worldInit() {
    world.end = false;
    world.width = 10;
    world.height = 10;
    world.widthTimesHeight = world.width * world.height;
    world.depth = 10;
    world.tilesN = world.width * world.height * world.depth;
    world.entityTiles = malloc(world.tilesN * sizeof(int));
    memset(world.entityTiles, -1, world.tilesN);
    EntityArray entityArray = arrayCreate();
    world.entities = entityArray;

    {
        enum Texture texture = TEXTURE_HUMAN;
        Vec3f p = {1, 1, 0};
        int entityI = createAndAddEntity(p, texture, false, OBJECT_HUMAN);
        AiComponent ac = aiComponentCreate(entityI);
        int aci = aiSystemAddAiComponent(ac);
        Entity *e = worldGetEntity(entityI);
        e->ai = aci;
    }
    for (int z = 1 ; z < world.depth ; z++) {
        for (int x = 0 ; x < world.width ; x++) {
            for (int y = 0 ; y < world.height ; y++) {
                Vec3f p = {x, y, -z};
                createAndAddEntity(p, TEXTURE_DIRT_BLOCK, true, OBJECT_DIRT_BLOCK);
            }
        }
    }
    for (int x = 0 ; x < world.width ; x++) {
        for (int y = 0 ; y < world.height ; y++) {
            if (x != 1 && y != 1) {
                if (rand() % 5 == 0) {
                    addWall(x, y);
                } else if (rand() % 5 == 0) {
                    addTree(x, y);
                }
            }
        }
    }
}

float
worldGetDifficulty(int x, int y, int z) {
    int index = MAP_INDEX(x, y, z);
    int entityIndex = world.entityTiles[index];
    if (entityIndex == -1) {
        return 0.1;
    }
    Entity *e = &world.entities.data[entityIndex];
    return e->movementDifficulty;
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

    Vec3f target = {goalX, goalY, 0};
    //SDL_Log("worldMoveRandom, target=%d:%d", goalX, goalY);
    aiSystem.aiComponents.data[0].target = target;
    aiSystem.aiComponents.data[0].hasTarget = true;
}

void
worldFree() {
    arrayFree(world.entities);
}
