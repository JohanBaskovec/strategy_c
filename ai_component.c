#include <SDL.h>
#include "entity.h"
#include "array.h"
#include "world.h"
#include "error.h"
#include <math.h>

Vec3fArray
pathFind(Vec3f start, Vec3f goal);

#define EQUAL_DELTA(a, b, delta) (fabs(a - b) < delta)
void
aiUpdate(AiComponent *ai) {
    Entity *entity = &world.entities.data[ai->entity];
    if (ai->hasTarget) {
        Vec3fArray path = pathFind(entity->box.position, ai->target);
        for (int i = path.length - 1 ; i >= 0 ; i--) {
            Vec3f tile = path.data[i];
            SDL_Log("Path: %f %f", tile.x, tile.y);
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
        ai->path = path;
        ai->isMoving = true;

        Vec3f pos = entity->box.position;
        Vec3f target = ai->path.data[ai->pathIndex];
        entity->velocity = vec3fSub(target, pos);
        SDL_Log("Velocity 1: %f:%f", entity->velocity.x, entity->velocity.y);
        entity->velocity = vec3fNormalize(entity->velocity);
        SDL_Log("Velocity 2: %f:%f", entity->velocity.x, entity->velocity.y);
        entity->velocity = vec3fMulf(entity->velocity, ai->movementSpeed);
        SDL_Log("Velocity 3: %f:%f", entity->velocity.x, entity->velocity.y);
    }

    if (ai->isMoving) {
        SDL_Log("Moving, pathIndex=%d", ai->pathIndex);
        if (ai->pathIndex == -1) {
            ai->isMoving = false;
            entity->velocity = vec3fZero;
        } else {
            Vec3f pos = entity->box.position;
            Vec3f target = ai->path.data[ai->pathIndex];
            SDL_Log("Position: %f %f", pos.x, pos.y);
            SDL_Log("Moving to next target:: %f %f", target.x, target.y);
            bool sameX = EQUAL_DELTA(pos.x, target.x, 0.01+ai->movementSpeed);
            bool sameY = EQUAL_DELTA(pos.y, target.y, 0.01+ai->movementSpeed);
            if (sameX && sameY) {
                ai->pathIndex--;

                target = ai->path.data[ai->pathIndex];
                entity->velocity = vec3fSub(target, pos);
                SDL_Log("Velocity 1: %f:%f", entity->velocity.x, entity->velocity.y);
                entity->velocity = vec3fNormalize(entity->velocity);
                SDL_Log("Velocity 2: %f:%f", entity->velocity.x, entity->velocity.y);
                entity->velocity = vec3fMulf(entity->velocity, ai->movementSpeed);
                SDL_Log("Velocity 3: %f:%f", entity->velocity.x, entity->velocity.y);
            }
        }

    }
}

#define MAP_INDEX(x, y) ((int)x * world.height + (int)y)

typedef struct Node {
    Vec3f position;
} Node;

Node
nodeCreate(Vec3f position) {
    Node node = {
        .position = position
    };
    return node;
}

typedef struct NodeArray {
    int length;
    int allocated;
    Node *data;
} NodeArray;

float length(float aX, float aY, float bX, float bY) {
    float x = bX - aX;
    float y = bY - aY;
    return sqrt(x * x + y * y);
}

Vec3fArray
reconstructPath(Node start, Node *cameFrom, Node current) {
    Vec3fArray path;
    Node backtrackNode = current;
    int i;
    SDL_Log("reconstructPath: start=%f:%f",start.position.x, start.position.y);
    SDL_Log("reconstructPath, adding %f:%f", current.position.x, current.position.y);
    ARRAY_ADD(path, backtrackNode.position, i);
    while (backtrackNode.position.x != start.position.x
            || backtrackNode.position.y != start.position.y) {
        int mi = MAP_INDEX(backtrackNode.position.x, backtrackNode.position.y);
        backtrackNode = cameFrom[mi];
        ARRAY_ADD(path, backtrackNode.position, i);
        SDL_Log("reconstructPath, adding %f:%f", backtrackNode.position.x, backtrackNode.position.y);
    }
    return path;
}

Vec3fArray
pathFind(Vec3f start, Vec3f goal) {
    // convert to map grid
    start.x = roundf(start.x);
    start.y = roundf(start.y);
    goal.x =  roundf(goal.x);
    goal.y =  roundf(goal.y);

    SDL_Log("pathFind: from %f:%f to %f:%f", start.x, start.y, goal.x, goal.y);

    Vec3fArray ret = ARRAY_NEW;
    int goalX = goal.x;
    int goalY = goal.y;
    if (worldGetDifficulty(goalX, goalY) == 1) {
        SDL_Log("%d:%d is impassable.", goalX, goalY);
        return ret;
    }

    NodeArray open = ARRAY_NEW;
    Node startNode = nodeCreate(start);
    int nodeI;
    ARRAY_ADD(open, startNode, nodeI);
    Node cameFrom[world.tilesN];
    float gScores[world.tilesN];
    int currentIndex = MAP_INDEX(start.x, start.y);
    SDL_Log("gScores[%d]=0", currentIndex);

    float fScores[world.tilesN];
    for (int i = 0 ; i < world.tilesN ; i++) {
        gScores[i] = -1;
        fScores[i] = -1;
    }
    fScores[currentIndex] = length(start.x, start.y, goal.x, goal.y);
    gScores[currentIndex] = 0.1;


    int count = 0;
    while (open.length != 0) {
        SDL_Log("Moving to %d %d\n", goalX, goalY);
        SDL_Log("Loop %d\n", count);
        count++;
        int t = SDL_GetTicks();
        if (t > 60000) {
            exit(1);
        }
        float minfScore = 9999999;
        int minOpenI = -1;
        // TODO: use min heap or priority queue
        for (int i = 0; i < open.length ; i++) {
            Node n = open.data[i];

            int aI = MAP_INDEX(n.position.x, n.position.y);
            float fScore = fScores[aI];
            SDL_Log("Open: %f:%f, fScore: %f", n.position.x, n.position.y, fScore);
            if (fScore < minfScore) {
                minOpenI = i;
                minfScore = fScore;
            }
        }
        if (minOpenI == -1) {
            fatalError("minOpenI == -1");
        }
        Node current = open.data[minOpenI];
        currentIndex = MAP_INDEX(current.position.x, current.position.y);
        if (current.position.x == goalX && current.position.y == goalY) {
            SDL_Log("Found target after %d loops!", count);

            return reconstructPath(startNode, cameFrom, current);
        }
        float gScoreCurrent = gScores[currentIndex];
        SDL_Log("Current: %f:%f at index %d, gScore =%f", current.position.x, current.position.y, minOpenI, gScoreCurrent);
        if (gScoreCurrent == -1) {
            fatalError("Current node has no score!!");
        }
        ARRAY_REMOVE(open, minOpenI);
        for (int i = 0; i < open.length ; i++) {
            Node n = open.data[i];

            int aI = MAP_INDEX(n.position.x, n.position.y);
            float fScore = fScores[aI];
            SDL_Log("Open after removal: %f:%f, fScore: %f", n.position.x, n.position.y, fScore);
        }
        SDL_Log("Open length: %d", open.length);

        int minX = current.position.x-1;
        int maxX = current.position.x+1;
        int minY = current.position.y-1;
        int maxY = current.position.y+1;
        for (int x = minX ; x <= maxX ; x++) {
            for (int y = minY ; y <= maxY ; y++) {
                if ((x == current.position.x && y == current.position.y)
                        || x < 0 || y < 0
                        || x >= world.width || y >= world.height) {
                    continue;
                }
                SDL_Log("Visiting %d:%d\n", x, y);
                int targetIndex = MAP_INDEX(x, y);
                float difficulty = worldGetDifficulty(x, y);
                if (difficulty == 1) {
                    SDL_Log("found impassable wall at %d:%d", x, y);
                    continue;
                }
                // difficulty is always 0 except when there's a wall
                float gScoreTentative = gScoreCurrent + difficulty; // + difficulty
                float gScoreTarget = gScores[targetIndex];
                float fScoreTarget = fScores[targetIndex];
                SDL_Log("gScoreTarget = %f, fScoreTarget = %f", gScoreTarget, fScoreTarget);
                SDL_Log("gScoreTentative = %f", gScoreTentative);
                if (gScoreTarget == -1 || gScoreTarget > gScoreTentative) {
                    SDL_Log("We keep the target.");
                    cameFrom[targetIndex] = current;
                    gScores[targetIndex] = gScoreTentative;
                    fScores[targetIndex] = gScoreTentative 
                        + length(x, y, goalX, goalY);

                    bool inOpen = false;
                    int k = 0;
                    for (; k < open.length ; k++) {
                        Vec3f position = open.data[k].position;
                        if (position.x == x && position.y == y) {
                            inOpen = true;
                        }
                    }
                    if (!inOpen) {
                        Vec3f newNodePosition = {x, y, 1};
                        SDL_Log("Adding node %d:%d to open", x, y);
                        Node newNode = nodeCreate(newNodePosition);
                        int newNodeI;
                        ARRAY_ADD(open, newNode, newNodeI);
                    }
                }
            }
        }

    }
    SDL_Log("Open set is empty but goal not reached!!");
    return ret;

}