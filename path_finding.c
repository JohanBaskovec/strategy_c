#include <SDL.h>
#include "path_finding.h"
#include "error.h"
#include "world.h"
#include "array.h"

#define MAP_INDEX(x, y) ((int)x * world.height + (int)y)

PathFindingNode
pathFindingNodeCreate(Vec3f position) {
    PathFindingNode node = {
        .position = position
    };
    return node;
}

float length(float aX, float aY, float bX, float bY) {
    float x = bX - aX;
    float y = bY - aY;
    return sqrt(x * x + y * y);
}

Vec3fArray
reconstructPath(PathFindingNode start, PathFindingNode *cameFrom, PathFindingNode current) {
    Vec3fArray path = arrayCreate();
    PathFindingNode backtrackNode = current;
    #ifdef ST_PATHFINDING_LOG
    SDL_Log("reconstructPath: start=%f:%f",start.position.x, start.position.y);
    SDL_Log("reconstructPath, adding %f:%f", current.position.x, current.position.y);
    #endif
    arrayAdd(path, backtrackNode.position);
    while (backtrackNode.position.x != start.position.x
            || backtrackNode.position.y != start.position.y) {
        int mi = MAP_INDEX(backtrackNode.position.x, backtrackNode.position.y);
        backtrackNode = cameFrom[mi];
        arrayAdd(path, backtrackNode.position);
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("reconstructPath, adding %f:%f", backtrackNode.position.x, backtrackNode.position.y);
        #endif
    }
    return path;
}

void
doTile(
        PathFinding *pathFinding
        , PathFindingNode current
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
            PathFindingNode newNode = pathFindingNodeCreate(newNodePosition);
            arrayAdd(pathFinding->open, newNode);
        }
    }
}

Vec3fArray
pathFindingFindPath(PathFinding *pathFinding, Vec3f start, Vec3f goal) {
    // convert to map grid
    start.x = roundf(start.x);
    start.y = roundf(start.y);
    goal.x =  roundf(goal.x);
    goal.y =  roundf(goal.y);

    #ifdef ST_PATHFINDING_LOG
    SDL_Log("pathFind: from %f:%f to %f:%f", start.x, start.y, goal.x, goal.y);
    #endif

    Vec3fArray ret = arrayCreate();
    int goalX = goal.x;
    int goalY = goal.y;
    if (worldGetDifficulty(goalX, goalY) == 1) {
        SDL_Log("%d:%d is impassable.", goalX, goalY);
        return ret;
    }

    arrayFree(pathFinding->open);
    for (int i = 0 ; i < world.tilesN ; i++) {
        pathFinding->gScores[i] = -1;
        pathFinding->fScores[i] = -1;
    }
    pathFinding->goalX = goal.x;
    pathFinding->goalY = goal.y;

    PathFindingNode startNode = pathFindingNodeCreate(start);
    arrayAdd(pathFinding->open, startNode);
    int currentIndex = MAP_INDEX(start.x, start.y);
    #ifdef ST_PATHFINDING_LOG
    SDL_Log("gScores[%d]=0", currentIndex);
    #endif

    for (int i = 0 ; i < world.tilesN ; i++) {
        pathFinding->gScores[i] = -1;
        pathFinding->fScores[i] = -1;
    }
    pathFinding->fScores[currentIndex] = length(start.x, start.y, goal.x, goal.y);
    pathFinding->gScores[currentIndex] = 0.1;

    int count = 0;
    while (pathFinding->open.length != 0) {
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("Moving to %d %d\n", goalX, goalY);
        SDL_Log("Loop %d\n", count);
        #endif
        count++;
        float minfScore = 9999999;
        int minOpenI = -1;
        // TODO: use min heap or priority queue
        for (int i = 0; i < pathFinding->open.length ; i++) {
            PathFindingNode n = pathFinding->open.data[i];

            int aI = MAP_INDEX(n.position.x, n.position.y);
            float fScore = pathFinding->fScores[aI];
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
        PathFindingNode current = pathFinding->open.data[minOpenI];
        currentIndex = MAP_INDEX(current.position.x, current.position.y);
        if (current.position.x == goalX && current.position.y == goalY) {
            #ifdef ST_PATHFINDING_LOG
            SDL_Log("Found target after %d loops!", count);
            #endif

            return reconstructPath(startNode, pathFinding->cameFrom, current);
        }
        float gScoreCurrent = pathFinding->gScores[currentIndex];
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("Current: %f:%f at index %d, gScore =%f", current.position.x, current.position.y, minOpenI, gScoreCurrent);
        #endif
        if (gScoreCurrent == -1) {
            fatalError("Current node has no score!!");
        }
        arrayRemove(pathFinding->open, minOpenI);
        for (int i = 0; i < pathFinding->open.length ; i++) {
            PathFindingNode n = pathFinding->open.data[i];

            int aI = MAP_INDEX(n.position.x, n.position.y);
            float fScore = pathFinding->fScores[aI];
            #ifdef ST_PATHFINDING_LOG
            SDL_Log("Open after removal: %f:%f, fScore: %f", n.position.x, n.position.y, fScore);
            #endif
        }
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("Open length: %d", pathFinding->open.length);
        #endif

        Vec3f p = current.position;
        float cornerDifficultyLeft = worldGetDifficulty(p.x-1, p.y);
        float cornerDifficultyBottom = worldGetDifficulty(p.x, p.y-1);
        float cornerDifficultyRight = worldGetDifficulty(p.x+1, p.y);
        float cornerDifficultyTop = worldGetDifficulty(p.x, p.y+1);
        //bottom left
        if (cornerDifficultyLeft != 1 && cornerDifficultyBottom != 1) {
            doTile(pathFinding, current, p.x-1, p.y-1);
        }
        // left
        doTile(pathFinding, current, p.x-1, p.y);
        // top left
        if (cornerDifficultyLeft != 1 && cornerDifficultyTop != 1) {
            doTile(pathFinding, current, p.x-1, p.y+1);
        }
        // bottom
        doTile(pathFinding, current, p.x,   p.y-1);
        // top
        doTile(pathFinding, current, p.x,   p.y+1);
        // bottom right
        if (cornerDifficultyRight != 1 && cornerDifficultyBottom != 1) {
            doTile(pathFinding, current, p.x+1, p.y-1);
        }
        // right
        doTile(pathFinding, current, p.x+1, p.y);
        // top right
        if (cornerDifficultyRight != 1 && cornerDifficultyTop != 1) {
            doTile(pathFinding, current, p.x+1, p.y+1);
        }

    }
    #ifdef ST_PATHFINDING_LOG
    SDL_Log("Open set is empty but goal not reached!!");
    #endif
    return ret;

}

void
pathFindingFree(PathFinding *pathFinding) {
    free(pathFinding->cameFrom);
    free(pathFinding->gScores);
    free(pathFinding->fScores);
    arrayFree(pathFinding->open);
}

PathFinding
pathFindingCreate(int worldSize) {
    PathFindingNodeArray open = arrayCreate();
    PathFinding ret = {
        .cameFrom = malloc(sizeof(PathFindingNode) * world.tilesN)
        , .open = open
        , .gScores = malloc(sizeof(float) * world.tilesN)
        , .fScores = malloc(sizeof(float) * world.tilesN)
    };
    return ret;
}
