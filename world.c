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
#include "object_config.h"

World world;

Floor dirtFloor;

ObjectConfig objectConfigs[OBJECT_NUMBER];

int
worldAddEntity(Entity entity) {
    int i;
    fixedArrayAdd(&world.entities, entity, ENTITY_MAX_NUMBER, i);
    return i;
}

int
worldCreateAndAddEntity(
        Vec3f position
        , ObjectType type
) {
    ObjectConfig config = objectConfigs[type];
    enum Texture texture = config.texture;
    Box3f b = box3fCreate(position, graphics.tileSize);
    Sprite sprite = spriteCreate(b);
    int spriteI = graphicsAddSprite(texture, sprite);
    Entity entity = entityCreate(b, texture, spriteI, type, config.isGridAligned);
    int entityI = worldAddEntity(entity);
    Sprite *s = graphicsGetSprite(texture, spriteI);
    s->entity = entityI;
    if (config.isGridAligned) {
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
    worldCreateAndAddEntity(p, OBJECT_WALL);
}

void
addTree(int x, int y) {
    Vec3f p = {x, y, 0};
    worldCreateAndAddEntity(p, OBJECT_TREE);
}

Entity*
worldGetEntity(int i) {
    return &world.entities.data[i];
}

Entity*
worldGetTileAlignedEntity(Vec3f pos) {
    int i = MAP_INDEX(pos.x, pos.y, pos.z);
    int entityIndex = world.entityTiles[i];
    if (entityIndex == -1) {
        return NULL;
    }
    Entity *e = &world.entities.data[entityIndex];
    return e;
}

void
worldRemoveEntity(int index) {
    Entity *e = worldGetEntity(index);
    graphicsDeleteSprite(e->texture, e->spriteIndex);
    if (e->gridAligned) {
        int mapI = MAP_INDEXV(e->box.position);
        world.entityTiles[mapI] = -1;
    }
    fixedArrayRemove(&world.entities, index);
}

void
worldInit() {
    world.end = false;
    world.width = 50;
    world.height = 50;
    world.widthTimesHeight = world.width * world.height;
    world.depth = 50;
    world.tilesN = world.width * world.height * world.depth;
    world.entityTiles = malloc(world.tilesN * sizeof(int));
    memset(world.entityTiles, -1, world.tilesN);
    fixedArrayInit(&world.entities, ENTITY_MAX_NUMBER);

    objectConfigs[OBJECT_HUMAN].texture = TEXTURE_HUMAN;
    objectConfigs[OBJECT_HUMAN].isGridAligned = false;

    objectConfigs[OBJECT_DIRT_BLOCK].texture = TEXTURE_DIRT_BLOCK;
    objectConfigs[OBJECT_DIRT_BLOCK].isGridAligned = true;

    objectConfigs[OBJECT_TREE].texture = TEXTURE_TREE;
    objectConfigs[OBJECT_TREE].isGridAligned = true;

    objectConfigs[OBJECT_WALL].texture = TEXTURE_WALL;
    objectConfigs[OBJECT_WALL].isGridAligned = true;

    {
        Vec3f p = {1, 1, 0};
        int entityI = worldCreateAndAddEntity(p, OBJECT_HUMAN);
        AiComponent ac = aiComponentCreate(entityI);
        int aci = aiSystemAddAiComponent(ac);
        Entity *e = worldGetEntity(entityI);
        e->ai = aci;
    }
    for (int z = 1 ; z < world.depth ; z++) {
        for (int x = 0 ; x < world.width ; x++) {
            for (int y = 0 ; y < world.height ; y++) {
                Vec3f p = {x, y, -z};
                worldCreateAndAddEntity(p, OBJECT_DIRT_BLOCK);
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
    SDL_Log("Entity memory = %ldb", ENTITY_MAX_NUMBER * sizeof(Entity));
    SDL_Log("Sprites memory = %ldb", SPRITE_MAX_NUMBER * TEXTURE_NUMBER * sizeof(Sprite));
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
}
