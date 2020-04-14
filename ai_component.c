#include <SDL.h>
#include "entity.h"
#include "array.h"
#include "world.h"
#include "error.h"
#include <math.h>
#include "path_finding.h"
#include "ai_system.h"

#define EQUAL_DELTA(a, b, delta) (fabs(a - b) < delta)

void
aiComponentFree(AiComponent *ai) {
    arrayFree(ai->path);
}

void
aiUpdate(AiComponent *ai) {
    Entity *entity = &world.entities.data[ai->entity];
    if (ai->hasTarget) {
        Vec3fArray path = pathFindingFindPath(
                &aiSystem.pathFinding
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
    }

    if (ai->isMoving) {
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("Moving, pathIndex=%d", ai->pathIndex);
        #endif
        if (ai->pathIndex == -1) {
            ai->isMoving = false;
            entity->velocity = vec3fZero;
            SDL_Log("Final position: %f:%f", entity->box.position.x, entity->box.position.y);
        } else {
            Vec3f pos = entity->box.position;
            Vec3f target = ai->path.data[ai->pathIndex];
            #ifdef ST_PATHFINDING_LOG
            SDL_Log("Position: %f %f", pos.x, pos.y);
            SDL_Log("Moving to next target:: %f %f", target.x, target.y);
            #endif

            float diffX = pos.x - target.x;
            float absDiffX = fabs(diffX);
            bool sameX = absDiffX < 0.001;

            float diffY = pos.y - target.y;
            float absDiffY = fabs(diffY);
            bool sameY = absDiffY < 0.001;
            // if the current position is "close enough" (to take floating
            // point error into account), we move to the next node
            if (sameX && sameY) {
                ai->pathIndex--;

                target = ai->path.data[ai->pathIndex];
                entity->velocity = vec3fSub(target, pos);
                entity->velocity = vec3fNormalize(entity->velocity);
                entity->velocity = vec3fMulf(entity->velocity, ai->movementSpeed);
            } else {
                bool almostSameX = absDiffX < ai->movementSpeed;
                bool almostSameY = absDiffY < ai->movementSpeed;

                // if the distance from the target is less than the movement speed,
                // set the velocity to the distance (so that the end position is
                // exactly the target)
                if (almostSameY && almostSameX) {
                    entity->velocity.y = -diffY;
                    entity->velocity.x = -diffX;
                }
            }

        }

    }
}


