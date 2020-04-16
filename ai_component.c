#include <SDL.h>
#include "assert.h"
#include "entity.h"
#include "array.h"
#include "world.h"
#include "error.h"
#include <math.h>
#include "path_finding.h"
#include "ai_system.h"
#include "job_priority_queue.h"

#define EQUAL_DELTA(a, b, delta) (fabs(a - b) < delta)

AiComponent
aiComponentCreate(int entityI) {
    AiComponent ai = {
        .hasTarget = false
        , .movementSpeed = 0.5
        , .entity = entityI
        , .pathFinding = pathFindingCreate(world.tilesN)
        , .jobPriorities = jobPriorityQueueCreate()
        , .state = AI_STATE_IDLE
    };
    return ai;
}

void
aiComponentFree(AiComponent *ai) {
    arrayFree(ai->path);
    pathFindingFree(&ai->pathFinding);
    jobPriorityQueueFree(&ai->jobPriorities);
}

void
aiComponentSetPathTarget(AiComponent *ai , Vec3f target) {
    ai->target = target;
    ai->hasTarget = true;
}

void
findTileFreeNextToEntity(Entity *e) {
}

typedef struct Vec3fOptional {
    Vec3f value;
    bool valid;
} Vec3fOptional;

Vec3fOptional
findClosestEmptyTileNextToGridAlignedEntity(Entity *ai, Entity *e) {
    Vec3fOptional ret = {.valid = false};
    assert(e->gridAligned);

    Vec3f pos = e->box.position;
    Vec3i posi = {pos.x, pos.y, pos.z};
    MinMaxVeci mmv = worldGetMinMaxPosi(posi);
    Vec3i min = mmv.min;
    Vec3i max = mmv.max;

    float shortestDist = 1000;
    Vec3f target;
    for (int z = min.z; z <= max.z ; z++) {
        for (int x = min.x ; x <= max.x ; x++) {
            for (int y = min.y ; y <= max.y ; y++) {
                int mapI = MAP_INDEX(x, y, z);
                int entityI = world.entityTiles[mapI];
                if (entityI == -1) {
                    int belowI = MAP_INDEX(x, y, z - 1);
                    int belowEntityI = world.entityTiles[belowI];
                    //printf("free space at %d %d %d\n %d %d\n", x, y, z, belowI, belowEntityI);
                    if (belowEntityI == -1) {
                        //printf("nothing to stand on!!\n");
                        // nothing to stand on!
                        continue;
                    }

                    float distance = vec3fDistance(e->box.position, ai->box.position);
                    if (distance < shortestDist) {
                        shortestDist = distance;
                        Vec3f v = {x, y, z};
                        ret.value = v;
                        ret.valid = true;
                    }
                    //printf("Distance: %f. Shortest distance: %f\n", distance, shortestDist);
                }
            }
        }
    }
    /*
    if (ret.valid) {
        printf("Moving to");
        vec3fPrint(ret.value);
        printf("\n");
    } else {
        printf("No empty space next to entity\n");
    }
    */
    return ret;
}

void
lookForTreeToCut(AiComponent *ai) {
    float closestDist = 1000;
    Entity *closestTree;
    bool foundTree = false;
    SDL_Log("looking for tree");
    Entity *aiEntity = worldGetEntity(ai->entity);
    Vec3f aiPosition = aiEntity->box.position;
    for (int i = 0 ; i < world.entities.length ; i++) {
        Entity *tree = &world.entities.data[i];
        if (tree->reachable && tree->keep && tree->type == OBJECT_TREE) {
            float dist = vec3fDistance(aiPosition, tree->box.position);
            if (closestDist > dist) {
                closestDist = dist;
                closestTree = tree;
            }
        }
    }
    if (closestTree == NULL) {
        //printf("Found not tree :(\n");
        return;
    }
    //printf("Found a tree at");
    if (closestTree->id == ai->entityTarget) {
        //printf("already cutting this tree\n");
        return;
    }

    ai->entityTarget = closestTree->id;
    /*
    vec3fPrint(closestTree->box.position);
    printf("\n");
    */
    Vec3fOptional target = findClosestEmptyTileNextToGridAlignedEntity(aiEntity, closestTree);
    if (target.valid) {
        ai->state = AI_STATE_MOVING_TO_WORK;
        aiComponentSetPathTarget(ai, target.value);
    }
}

void
aiUpdate(AiComponent *ai) {
    Entity *entity = &world.entities.data[ai->entity];
    // todo: priority queue
    float highestPriority;
    int jobToDo;
    bool findJob = true;
    if (ai->state == AI_STATE_IDLE) {
        ai->state = AI_STATE_LOOKING_FOR_WORK;
        for (int i = 0 ; i < JOB_PRIORITY_NUMBER ; i++ ) {
            JobTypeArray *jobs = &ai->jobPriorities.jobsByPriority[i];
            for (int k = 0 ; k < jobs->length ; k++) {
                JobType job = jobs->data[k];
                if (job == JOB_TREE_CUTTING) {
                    lookForTreeToCut(ai);
                }
            }

        }
    }
    if (ai->state == AI_STATE_WORKING) {
        // todo: cut tree
        // when done, set ai->state to idle
    }
    if (ai->hasTarget) {
        // todo: if moving to work, check that target object still exists
        for (int i = 0 ; i < world.tilesN ; i++) {
            //int targetIndex = MAP_INDEX(target.x, target.y, target.z);
            float fScore =  ai->pathFinding.fScores[i];
            int tileI = world.entityTiles[i];
            if (tileI == -1) {
                continue;
            }
            Entity *tile = worldGetEntity(tileI);
            Sprite *tileSprite = graphicsGetEntitySprite(tile);
            spriteSetColorAdd(tileSprite, vec3fZero);
        }

        Vec3fArray path = pathFindingFindPath(
                &ai->pathFinding
                , entity->box.position
                , ai->target
        );
        for (int i = path.length - 1 ; i >= 0 ; i--) {
            Vec3f tile = path.data[i];
            #ifdef ST_PATHFINDING_LOG
            SDL_Log("Path: %f %f", tile.x, tile.y);
            #endif
        }
        // now we only compute the path once, when it's first set
        // TODO: recompute the path every n ticks
        ai->hasTarget = false;
        if (path.length == 0) {
            SDL_Log("No path");
            ai->isMoving = false;
            return;
        }
        // we iterate over the path positions in reverse order, and ignore
        // the last one because it's the current position
        ai->pathIndex = path.length - 2;

        arrayFree(ai->path);
        ai->path = path;
        ai->isMoving = true;

        Vec3f pos = entity->box.position;
        Vec3f target = ai->path.data[ai->pathIndex];
        entity->velocity = vec3fSub(target, pos);
        entity->velocity = vec3fNormalize(entity->velocity);
        entity->velocity = vec3fMulf(entity->velocity, ai->movementSpeed);

        float maxFScore = 0;
        float maxGScore = 0;
        for (int i = 0 ; i < world.tilesN ; i++) {
            float fScore =  ai->pathFinding.fScores[i];
            float gScore =  ai->pathFinding.gScores[i];
            if (fScore != -1) {
                if (fScore > maxFScore) {
                    maxFScore = fScore;
                }
            }
            if (gScore != -1) {
                if (gScore > maxGScore) {
                    maxGScore = gScore;
                }
            }
        }
        for (int i = 0 ; i < world.tilesN ; i++) {
            float fScore =  ai->pathFinding.fScores[i];
            float gScore =  ai->pathFinding.gScores[i];
            if (gScore != -1) {
                // "temporary" hack: we traverse empty space
                // but colour the block below it
                int tileI = world.entityTiles[i + world.widthTimesHeight];
                if (tileI == -1) {
                    continue;
                }
                Entity *tile = worldGetEntity(tileI);
                Sprite *tileSprite = graphicsGetEntitySprite(tile);

                Vec3f colorAdd = {gScore / maxGScore, 0, 0};
                spriteSetColorAdd(tileSprite, colorAdd);
            }
        }

    }

    if (ai->isMoving) {
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("Moving, pathIndex=%d", ai->pathIndex);
        #endif
        if (ai->pathIndex != -1) {
            Vec3f pos = entity->box.position;
            Vec3f target = ai->path.data[ai->pathIndex];
            #ifdef ST_PATHFINDING_LOG
            SDL_Log("Position: %f %f", pos.x, pos.y);
            SDL_Log("Moving to next target:: %f %f", target.x, target.y);
            #endif

            Vec3f diff = vec3fSub(pos, target);
            float absDiffX = fabs(diff.x);
            bool sameX = absDiffX < 0.001;

            float absDiffY = fabs(diff.y);
            bool sameY = absDiffY < 0.001;
            if (sameX && sameY) {
                ai->pathIndex--;

                if (ai->pathIndex != -1) {
                    target = ai->path.data[ai->pathIndex];
                    entity->velocity = vec3fSub(target, pos);
                    entity->velocity = vec3fNormalize(entity->velocity);
                    entity->velocity = vec3fMulf(entity->velocity, ai->movementSpeed);
                } else {
                    ai->isMoving = false;
                    entity->velocity = vec3fZero;
                    if (ai->state == AI_STATE_MOVING_TO_WORK) {
                        ai->state == AI_STATE_WORKING;
                    }
                }
            } else {
                float length = vec3fLength(diff);

                // if the distance from the target is less than the movement speed,
                // set the velocity to the distance (so that the end position is
                // exactly the target)
                if (length <= ai->movementSpeed) {
                    entity->velocity.y = -diff.y;
                    entity->velocity.x = -diff.x;
                }
            }

        }

    }
}


