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
#include "input.h"

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
        , bool visible
) {
    ObjectConfig config = objectConfigs[type];
    enum Texture texture = config.texture;
    Box3f b = box3fCreate(position, graphics.tileSize);
    Sprite sprite = spriteCreate(b, visible);
    int spriteI = graphicsAddSprite(texture, sprite);
    Entity entity = entityCreate(b, texture, spriteI, type, config.isGridAligned);
    int entityI = worldAddEntity(entity);
    Sprite *s = graphicsGetSprite(texture, spriteI);
    s->entity = entityI;
    if (config.isGridAligned) {
        int i = MAP_INDEX(position.x, position.y, position.z);
        /*
        SDL_Log("%f %f %f", position.x, position.y, position.z);
        */
    SDL_Log("world.entityTiles[%d] = %d", i, entityI);
        world.entityTiles[i] = entityI;
    }
    return entityI;
}

void
addWall(int x, int y) {
    Vec3f p = {x, y, 0};
    worldCreateAndAddEntity(p, OBJECT_WALL, true);
}

void
addTree(int x, int y) {
    Vec3f p = {x, y, 0};
    worldCreateAndAddEntity(p, OBJECT_TREE, true);
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
        SDL_Log("Removed grid aligned entity at %d", mapI);
    }
    fixedArrayRemove(&world.entities, index);
}

void
worldInit() {
    world.end = false;
    world.width = 20;
    world.height = 20;
    world.widthTimesHeight = world.width * world.height;
    world.depth = 20;
    world.tilesN = world.width * world.height * world.depth;
    world.entityTiles = malloc(world.tilesN * sizeof(int));
    for (int i = 0 ; i < world.tilesN ; i++) {
        world.entityTiles[i] = -1;
    }
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
        Vec3f p = {0, 1, 0};
        int entityI = worldCreateAndAddEntity(p, OBJECT_HUMAN, true);
        AiComponent ac = aiComponentCreate(entityI);
        int aci = aiSystemAddAiComponent(ac);
        Entity *e = worldGetEntity(entityI);
        e->ai = aci;
    }
    for (int z = 1 ; z < world.depth ; z++) {
        for (int x = 0 ; x < world.width ; x++) {
            for (int y = 0 ; y < world.height ; y++) {
                if (x == 1 && z == 1) {
                    continue;
                }
                Vec3f p = {x, y, -z};
                worldCreateAndAddEntity(p, OBJECT_DIRT_BLOCK, true);
            }
        }
    }
    /*
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
    */
    SDL_Log("Entity memory = %ldb", ENTITY_MAX_NUMBER * sizeof(Entity));
    SDL_Log("Sprites memory = %ldb", SPRITE_MAX_NUMBER * TEXTURE_NUMBER * sizeof(Sprite));

}

float
worldGetDifficulty(Vec3f v) {
    MinMaxVec mm = worldGetMinMaxPos(v);
    if (v.x < mm.min.x || v.x > mm.max.x
            || v.y < mm.min.y || v.y > mm.max.y
            || v.z < mm.min.z || v.z > mm.max.z) {
        return 1;
    }
    int index = MAP_INDEXV(v);
    int entityIndex = world.entityTiles[index];
    if (entityIndex == -1) {
        return 0.1;
    }
    Entity *e = &world.entities.data[entityIndex];
    vec3fPrint(mm.min);
    printf("\n");
    SDL_Log("worldGetDifficulty(%f, %f, %f): index=%d, entityIndex=%d, e=%p", v.x, v.y, v.z, index, entityIndex, e);
    if (e == NULL) {
        return 0;
    }
    return e->movementDifficulty;
}

void
worldUpdate() {
    float shortestDist = 10;
    for (int i = 0 ; i < world.entities.length ; i++) {
        Entity *e = &world.entities.data[i];
        Sprite *s = graphicsGetEntitySprite(e);
        if (s->keep && s->visible) {
            Vec3f diff = vec3fSub(e->box.position, input.hoveredCoords);
            float dist = vec3fRelativeLength(diff);
            if (dist < shortestDist) {
                shortestDist = dist;
                input.closestEntity = e;
            }
        }
        entityUpdate(e);
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
}

MinMaxVec
worldGetMinMaxPos(Vec3f p) {
    MinMaxVec m;

    Vec3f min;
    min.x = fmax(0, p.x - 1.1);
    min.y = fmax(0, p.y - 1.1);
    min.z = fmax(-world.depth + 1, p.z - 1.1);

    Vec3f max;
    max.x = fmin(world.width, p.x + 1.1);
    max.y = fmin(world.height, p.y + 1.1);
    max.z = fmin(0, p.z + 1.1);

    m.min = min;
    m.max = max;
    return m;
}
