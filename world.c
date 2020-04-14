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

int
aiComponentArrayAdd(AiComponent e);

World world;

Floor dirtFloor;

float
worldGetDifficulty(int x, int y);

void
worldSetDifficulty(int x, int y, float value);

void
addWall(int x, int y) {
    worldSetDifficulty(x, y, 1);
    Vec3f p = {x, y, 1};
    Box3f b = box3fCreate(p, graphics.tileSize);
    Sprite sprite = spriteCreate(b);
    enum Texture texture = TEXTURE_WALL;
    int spriteI = graphicsAddSprite(texture, sprite);
    Entity entity = entityCreate(b, texture, spriteI);
    arrayAdd(world.entities, entity);
    SDL_Log("Add wall, spriteI = %d", spriteI);
}

Entity*
worldGetEntity(int i) {
    return &world.entities.data[i];
}

void
worldInit() {
    world.width = 20;
    world.height = 20;
    world.tilesN = world.width * world.height;
    world.difficultyMap = malloc(world.tilesN * sizeof(float));
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
            Box3f b = box3fCreate(p, graphics.tileSize);

            Sprite sprite = spriteCreate(b);
            enum Texture texture = TEXTURE_WOOD_FLOOR;
            int spriteI = graphicsAddSprite(texture, sprite);
            Entity entity = entityCreate(b, texture, spriteI);
            arrayAdd(world.entities, entity);
            SDL_Log("Add floor, spriteI = %d, text=%d", spriteI, texture);
        }
    }


    {
        AiComponent ac = {
            .hasTarget = false
            , .movementSpeed = 0.05
        };
        int aci = aiComponentArrayAdd(ac);
        enum Texture texture = TEXTURE_DIRT_FLOOR;
        Vec3f p = {1, 1, 1};
        Box3f box = box3fCreate(p, graphics.tileSize);
        SDL_Log("box of ai: %f", box.min.x);
        Sprite sprite = spriteCreate(box);
        int spriteI = graphicsAddSprite(texture, sprite);
        SDL_Log("sprite i: %d", spriteI);

        Entity entity = entityCreate(box, texture, spriteI);
        entity.ai = aci;
        world.aiComponents.data[aci].entity = world.entities.length;
        arrayAdd(world.entities, entity);
        SDL_Log("Add char, spriteI = %d", spriteI);
    }

    /*
    addWall(0, 0);
    addWall(0, 1);
    addWall(0, 2);
    addWall(1, 2);
    addWall(2, 2);
    addWall(3, 2);
    addWall(4, 2);
    */
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
        entity->box = box3fMoveVec3f(entity->box, entity->velocity);

        Sprite *sprite = graphicsGetSprite(entity->texture, entity->spriteIndex);
        sprite->box = entity->box;
    }
    aiSystemUpdate();
}

int
aiComponentArrayAdd(AiComponent e) {
    int newIndex = world.aiComponents.length;
    arrayAdd(world.aiComponents, e);
    return newIndex;
}

void
worldMoveRandom() {
    int goalX = rand() % world.width;
    int goalY = rand() % world.height;
    //int goalX = 1;
    //int goalY = 3;

    Vec3f target = {goalX, goalY, 1};
    SDL_Log("worldMoveRandom, target=%d:%d", goalX, goalY);
    world.aiComponents.data[0].target = target;
    world.aiComponents.data[0].hasTarget = true;
}

void
worldFree() {
    arrayFree(world.entities);
    free(world.difficultyMap);
}
