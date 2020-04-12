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

World world;

Floor dirtFloor;

int
aiComponentArrayAdd(AiComponent e);

float
worldGetDifficulty(int x, int y);

void
worldSetDifficulty(int x, int y, float value);

void
addWall(int x, int y) {
    worldSetDifficulty(x, y, 1);
    Entity entity = {
        .texture = TEXTURE_WALL
        , .alive = true
        , .box = {
            .position = {x, y, 1}
            , .size = graphics.tileSize
        }
        , .velocity = vec3fZero
    };
    Sprite sprite = {
        .alive = true,
        .box = entity.box
    };
    int spriteI = graphicsAddSprite(entity.texture, sprite);
    entity.spriteIndex = spriteI;
    int entityI;
    ARRAY_ADD(world.entities, entity, entityI);
}

void
worldInit() {
    world.width = 10;
    world.height = 10;
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
            enum Texture texture;
            if (rand() % 5 == 0) {
                addWall(x, y);
            }
            Entity entity = {
                .texture = TEXTURE_WOOD_FLOOR
                , .alive = true
                , .box = {
                    .position = {x, y, 0}
                    , .size = graphics.tileSize
                }
                , .velocity = vec3fZero
            };
            Sprite sprite = {
                .alive = true,
                .box = entity.box
            };
            int spriteI = graphicsAddSprite(entity.texture, sprite);
            entity.spriteIndex = spriteI;
            int entityI;
            ARRAY_ADD(world.entities, entity, entityI);
        }
    }


    {
        AiComponent ac = {
            .hasTarget = false
            , .movementSpeed = 0.05
        };
        int aci = aiComponentArrayAdd(ac);
        enum Texture texture = TEXTURE_DIRT_FLOOR;
        Box3f box = {
            .position = {1, 1, 1}
            , .size = graphics.tileSize
        };
        Sprite sprite = {
            .alive = true
            , .box = box
        };
        int spriteI = graphicsAddSprite(texture, sprite);

        Entity entity = {
            .texture = texture
            , .alive = true
            , .box = box
            , .ai = aci
            , .spriteIndex = spriteI
            , .velocity = vec3fZero
        };
        int entityI;
        ARRAY_ADD(world.entities, entity, entityI);
        world.aiComponents.data[aci].entity = entityI;
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
        entity->box.position = vec3fAdd(entity->box.position, entity->velocity);

        Sprite *sprite = graphicsGetSprite(entity->texture, entity->spriteIndex);
        sprite->box.position = entity->box.position;
    }
    for (int i = 0 ; i < world.aiComponents.length ; i++) {
        AiComponent *e = &world.aiComponents.data[i];
        aiUpdate(e);
    }
}

int
aiComponentArrayAdd(AiComponent e) {
    int newIndex = arrayAdd((Array*)&world.aiComponents, sizeof(AiComponent));
    world.aiComponents.data[newIndex] = e;
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
