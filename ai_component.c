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

typedef struct PathFinding {
    float *gScores;
    float *fScores;
    Node *cameFrom;
    NodeArray open;
    int goalX;
    int goalY;
} PathFinding;

Vec3fArray
reconstructPath(Node start, Node *cameFrom, Node current) {
    Vec3fArray path = ARRAY_NEW;
    Node backtrackNode = current;
    int i;
    #ifdef ST_PATHFINDING_LOG
    SDL_Log("reconstructPath: start=%f:%f",start.position.x, start.position.y);
    SDL_Log("reconstructPath, adding %f:%f", current.position.x, current.position.y);
    #endif
    ARRAY_ADD(path, backtrackNode.position, i);
    while (backtrackNode.position.x != start.position.x
            || backtrackNode.position.y != start.position.y) {
        int mi = MAP_INDEX(backtrackNode.position.x, backtrackNode.position.y);
        backtrackNode = cameFrom[mi];
        ARRAY_ADD(path, backtrackNode.position, i);
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("reconstructPath, adding %f:%f", backtrackNode.position.x, backtrackNode.position.y);
        #endif
    }
    return path;
}

void
doTile(
        PathFinding *pathFinding
        , Node current
        , int x
        , int y
) {
    if ((x == current.position.x && y == current.position.y)
            || x < 0 || y < 0
            || x >= world.width || y >= world.height) {
        return;
    }

    int currentIndex = MAP_INDEX(current.position.x, current.position.y);

    float gScoreCurrent = pathFinding->gScores[currentIndex];
    #ifdef ST_PATHFINDING_LOG
    SDL_Log("Visiting %d:%d\n", x, y);
    #endif
    int targetIndex = MAP_INDEX(x, y);
    float difficulty = worldGetDifficulty(x, y);

    if (difficulty == 1) {
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("found impassable wall at %d:%d", x, y);
        #endif
        return;
    }
    // difficulty is always 0 except when there's a wall
    float gScoreTentative = gScoreCurrent + difficulty; // + difficulty
    float gScoreTarget = pathFinding->gScores[targetIndex];
    float fScoreTarget = pathFinding->fScores[targetIndex];
    #ifdef ST_PATHFINDING_LOG
    SDL_Log("gScoreTarget = %f, fScoreTarget = %f", gScoreTarget, fScoreTarget);
    SDL_Log("gScoreTentative = %f", gScoreTentative);
    #endif

    if (gScoreTarget == -1 || gScoreTarget > gScoreTentative) {
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("We keep the target.");
        #endif
        pathFinding->cameFrom[targetIndex] = current;
        pathFinding->gScores[targetIndex] = gScoreTentative;
        pathFinding->fScores[targetIndex] = gScoreTentative 
            + length(x, y, pathFinding->goalX, pathFinding->goalY);

        bool inOpen = false;
        int k = 0;
        for (; k < pathFinding->open.length ; k++) {
            Vec3f position = pathFinding->open.data[k].position;
            if (position.x == x && position.y == y) {
                inOpen = true;
            }
        }
        if (!inOpen) {
            Vec3f newNodePosition = {x, y, 1};
            #ifdef ST_PATHFINDING_LOG
            SDL_Log("Adding node %d:%d to open", x, y);
            #endif
            Node newNode = nodeCreate(newNodePosition);
            int newNodeI;
            ARRAY_ADD(pathFinding->open, newNode, newNodeI);
        }
    }
}

Vec3fArray
pathFind(Vec3f start, Vec3f goal) {
    // convert to map grid
    start.x = roundf(start.x);
    start.y = roundf(start.y);
    goal.x =  roundf(goal.x);
    goal.y =  roundf(goal.y);

    #ifdef ST_PATHFINDING_LOG
    SDL_Log("pathFind: from %f:%f to %f:%f", start.x, start.y, goal.x, goal.y);
    #endif

    Vec3fArray ret = ARRAY_NEW;
    int goalX = goal.x;
    int goalY = goal.y;
    if (worldGetDifficulty(goalX, goalY) == 1) {
        SDL_Log("%d:%d is impassable.", goalX, goalY);
        return ret;
    }

    PathFinding pathFinding = {
        .cameFrom = malloc(sizeof(Node) * world.tilesN)
        , .open = ARRAY_NEW
        , .gScores = malloc(sizeof(Node) * world.tilesN)
        , .fScores = malloc(sizeof(Node) * world.tilesN)
        , .goalX = goal.x
        , .goalY = goal.y

    };
    Node startNode = nodeCreate(start);
    int nodeI;
    ARRAY_ADD(pathFinding.open, startNode, nodeI);
    int currentIndex = MAP_INDEX(start.x, start.y);
    #ifdef ST_PATHFINDING_LOG
    SDL_Log("gScores[%d]=0", currentIndex);
    #endif

    for (int i = 0 ; i < world.tilesN ; i++) {
        pathFinding.gScores[i] = -1;
        pathFinding.fScores[i] = -1;
    }
    pathFinding.fScores[currentIndex] = length(start.x, start.y, goal.x, goal.y);
    pathFinding.gScores[currentIndex] = 0.1;


    int count = 0;
    while (pathFinding.open.length != 0) {
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("Moving to %d %d\n", goalX, goalY);
        SDL_Log("Loop %d\n", count);
        #endif
        count++;
        float minfScore = 9999999;
        int minOpenI = -1;
        // TODO: use min heap or priority queue
        for (int i = 0; i < pathFinding.open.length ; i++) {
            Node n = pathFinding.open.data[i];

            int aI = MAP_INDEX(n.position.x, n.position.y);
            float fScore = pathFinding.fScores[aI];
            #ifdef ST_PATHFINDING_LOG
            SDL_Log("Open: %f:%f, fScore: %f", n.position.x, n.position.y, fScore);
            #endif
            if (fScore < minfScore) {
                minOpenI = i;
                minfScore = fScore;
            }
        }
        if (minOpenI == -1) {
            fatalError("minOpenI == -1");
        }
        Node current = pathFinding.open.data[minOpenI];
        currentIndex = MAP_INDEX(current.position.x, current.position.y);
        if (current.position.x == goalX && current.position.y == goalY) {
            #ifdef ST_PATHFINDING_LOG
            SDL_Log("Found target after %d loops!", count);
            #endif

            return reconstructPath(startNode, pathFinding.cameFrom, current);
        }
        float gScoreCurrent = pathFinding.gScores[currentIndex];
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("Current: %f:%f at index %d, gScore =%f", current.position.x, current.position.y, minOpenI, gScoreCurrent);
        #endif
        if (gScoreCurrent == -1) {
            fatalError("Current node has no score!!");
        }
        ARRAY_REMOVE(pathFinding.open, minOpenI);
        for (int i = 0; i < pathFinding.open.length ; i++) {
            Node n = pathFinding.open.data[i];

            int aI = MAP_INDEX(n.position.x, n.position.y);
            float fScore = pathFinding.fScores[aI];
            #ifdef ST_PATHFINDING_LOG
            SDL_Log("Open after removal: %f:%f, fScore: %f", n.position.x, n.position.y, fScore);
            #endif
        }
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("Open length: %d", pathFinding.open.length);
        #endif

        Vec3f p = current.position;
        float cornerDifficultyLeft = worldGetDifficulty(p.x-1, p.y);
        float cornerDifficultyBottom = worldGetDifficulty(p.x, p.y-1);
        float cornerDifficultyRight = worldGetDifficulty(p.x+1, p.y);
        float cornerDifficultyTop = worldGetDifficulty(p.x, p.y+1);
        //bottom left
        if (cornerDifficultyLeft != 1 && cornerDifficultyBottom != 1) {
            doTile(&pathFinding, current, p.x-1, p.y-1);
        }
        // left
        doTile(&pathFinding, current, p.x-1, p.y);
        // top left
        if (cornerDifficultyLeft != 1 && cornerDifficultyTop != 1) {
            doTile(&pathFinding, current, p.x-1, p.y+1);
        }
        // bottom
        doTile(&pathFinding, current, p.x,   p.y-1);
        // top
        doTile(&pathFinding, current, p.x,   p.y+1);
        // bottom right
        if (cornerDifficultyRight != 1 && cornerDifficultyBottom != 1) {
            doTile(&pathFinding, current, p.x+1, p.y-1);
        }
        // right
        doTile(&pathFinding, current, p.x+1, p.y);
        // top right
        if (cornerDifficultyRight != 1 && cornerDifficultyTop != 1) {
            doTile(&pathFinding, current, p.x+1, p.y+1);
        }

    }
    #ifdef ST_PATHFINDING_LOG
    SDL_Log("Open set is empty but goal not reached!!");
    #endif
    return ret;

}
