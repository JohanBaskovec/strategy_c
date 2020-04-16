#include <SDL.h>
#include "path_finding.h"
#include "error.h"
#include "world.h"
#include "array.h"

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

#define ST_PATHFINDING_LOG
Vec3fArray
reconstructPath(PathFindingNode start, PathFindingNode *cameFrom, PathFindingNode current) {
    Vec3fArray path = arrayCreate();
    PathFindingNode backtrackNode = current;
    #ifdef ST_PATHFINDING_LOG
    SDL_Log("reconstructPath: start=%f:%f:%f",start.position.x, start.position.y, start.position.z);
    SDL_Log("reconstructPath, adding %f:%f:%f", current.position.x, current.position.y, current.position.z);
    #endif
    arrayAdd(&path, backtrackNode.position);
    while (backtrackNode.position.x != start.position.x
            || backtrackNode.position.y != start.position.y
            || backtrackNode.position.z != start.position.z) {
        int mi = MAP_INDEX(backtrackNode.position.x, backtrackNode.position.y, backtrackNode.position.z);
        backtrackNode = cameFrom[mi];
        arrayAdd(&path, backtrackNode.position);
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("reconstructPath, adding %f:%f:%f", backtrackNode.position.x, backtrackNode.position.y, backtrackNode.position.z);
        #endif
    }
    return path;
}

void
doTile(
        PathFinding *pathFinding
        , PathFindingNode current
        , Vec3f target
) {
    if ((vec3fEqual(target, current.position))
            || target.x < 0 || target.y < 0 || target.z > 0
            || target.x >= world.width || target.y >= world.height || target.z < -world.depth ) {
        return;
    }

    Vec3f belowTarget = target;
    belowTarget.z -= 1;
    float difficultyBelow = worldGetDifficulty(belowTarget);
    if (difficultyBelow < 0.8) {
        SDL_Log("nothing solid below %f:%f:%f, can't move there", target.x, target.y, target.z);
        return;
    }

    int currentIndex = MAP_INDEX(current.position.x, current.position.y, current.position.z);

    float gScoreCurrent = pathFinding->gScores[currentIndex];
    #ifdef ST_PATHFINDING_LOG
    SDL_Log("Visiting %f:%f:%f\n", target.x, target.y, target.z);
    #endif
    int targetIndex = MAP_INDEX(target.x, target.y, target.z);
    float difficulty = worldGetDifficulty(target);

    if (difficulty == 1) {
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("found impassable wall at %f:%f:%f", target.x, target.y, target.z);
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
        printf("pathFinding->cameFrom[%d] = ", targetIndex);
        vec3fPrint(current.position);
        printf("\n");

        #endif
        pathFinding->cameFrom[targetIndex] = current;
        pathFinding->gScores[targetIndex] = gScoreTentative;
        Vec3f diffFromGoal = vec3fSub(target, pathFinding->goal);
        pathFinding->fScores[targetIndex] = gScoreTentative + vec3fLength(diffFromGoal);

        bool inOpen = false;
        int k = 0;
        for (; k < pathFinding->open.length ; k++) {
            Vec3f position = pathFinding->open.data[k].position;
            if (vec3fEqual(position, target)) {
                inOpen = true;
            }
        }
        if (!inOpen) {
            #ifdef ST_PATHFINDING_LOG
            SDL_Log("Adding node %f:%f:%f to open", target.x, target.y, target.z);
            #endif
            PathFindingNode newNode = pathFindingNodeCreate(target);
            arrayAdd(&pathFinding->open, newNode);
        }
    }
}

Vec3fArray
pathFindingFindPath(PathFinding *pathFinding, Vec3f start, Vec3f goal) {
    // convert to map grid
    #ifdef ST_PATHFINDING_LOG
    SDL_Log("pathFind: from %f:%f:%f to %f:%f:%f", start.x, start.y, start.z, goal.x, goal.y, goal.z);
    #endif

    start.x = roundf(start.x);
    start.y = roundf(start.y);
    start.z = roundf(start.z);

    Vec3fArray ret = arrayCreate();
    int goalX = goal.x;
    int goalY = goal.y;
    int goalZ = goal.z;
    if (worldGetDifficulty(goal) == 1) {
        SDL_Log("%d:%d:%d is impassable.", goalX, goalY, goalZ);
        return ret;
    }

    arrayFree(pathFinding->open);
    for (int i = 0 ; i < world.tilesN ; i++) {
        pathFinding->gScores[i] = -1;
        pathFinding->fScores[i] = -1;
    }
    pathFinding->goal = goal;

    PathFindingNode startNode = pathFindingNodeCreate(start);
    arrayAdd(&pathFinding->open, startNode);
    int currentIndex = MAP_INDEX(start.x, start.y, start.z);
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
        SDL_Log("Moving to %d %d %d\n", goalX, goalY, goalZ);
        SDL_Log("Loop %d\n", count);
        #endif
        count++;
        float minfScore = 9999999;
        int minOpenI = -1;
        // TODO: use min heap or priority queue
        for (int i = 0; i < pathFinding->open.length ; i++) {
            PathFindingNode n = pathFinding->open.data[i];

            int aI = MAP_INDEX(n.position.x, n.position.y, n.position.z);
            float fScore = pathFinding->fScores[aI];
            #ifdef ST_PATHFINDING_LOG
            SDL_Log("Open: %f:%f:%f, fScore: %f", n.position.x, n.position.y, n.position.z, fScore);
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
        currentIndex = MAP_INDEX(current.position.x, current.position.y, current.position.z);
        if (vec3fEqual(current.position, goal)) {
            #ifdef ST_PATHFINDING_LOG
            SDL_Log("Found target after %d loops!", count);
            #endif

            return reconstructPath(startNode, pathFinding->cameFrom, current);
        }
        float gScoreCurrent = pathFinding->gScores[currentIndex];
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("Current: %f:%f:%f at index %d, gScore =%f", current.position.x, current.position.y, current.position.z, minOpenI, gScoreCurrent);
        #endif
        if (gScoreCurrent == -1) {
            fatalError("Current node has no score!!");
        }
        arrayRemove(&pathFinding->open, minOpenI);
        for (int i = 0; i < pathFinding->open.length ; i++) {
            PathFindingNode n = pathFinding->open.data[i];

            int aI = MAP_INDEX(n.position.x, n.position.y, n.position.z);
            float fScore = pathFinding->fScores[aI];
            #ifdef ST_PATHFINDING_LOG
            SDL_Log("Open after removal: %f:%f:%f, fScore: %f", n.position.x, n.position.y, n.position.z, fScore);
            #endif
        }
        #ifdef ST_PATHFINDING_LOG
        SDL_Log("Open length: %d", pathFinding->open.length);
        #endif

        Vec3f p = current.position;

        Vec3f xMin1 = {p.x-1, p.y, p.z};
        float xMin1Diff = worldGetDifficulty(xMin1);

        Vec3f xPlus1 = {p.x+1, p.y, p.z};
        float xPlus1Diff = worldGetDifficulty(xPlus1);

        Vec3f yMin1 = {p.x, p.y-1, p.z};
        float yMin1Diff = worldGetDifficulty(yMin1);

        Vec3f yPlus1 = {p.x, p.y+1, p.z};
        float yPlus1Diff = worldGetDifficulty(yPlus1);

        Vec3f zPlus1 = {p.x, p.y, p.z+1};
        float zPlus1Diff = worldGetDifficulty(zPlus1);

        Vec3f zMin1 = {p.x, p.y, p.z-1};
        float zMin1Diff = worldGetDifficulty(zPlus1);

        //bottom left
        if (xMin1Diff != 1 && yMin1Diff != 1) {
            Vec3f target = {p.x-1, p.y-1, p.z};
            doTile(pathFinding, current, target);
        }
        // left
        {
            Vec3f target = {p.x-1, p.y, p.z};
            doTile(pathFinding, current, target);
        }
        // top left
        if (xMin1Diff != 1 && yPlus1Diff != 1) {
            Vec3f target = {p.x-1, p.y+1, p.z};
            doTile(pathFinding, current, target);
        }
        // bottom
        {
            Vec3f target = {p.x, p.y-1, p.z};
            doTile(pathFinding, current, target);
        }
        // top
        {
            Vec3f target = {p.x, p.y+1, p.z};
            doTile(pathFinding, current, target);
        }
        // bottom right
        if (xPlus1Diff != 1 && yMin1Diff != 1) {
            Vec3f target = {p.x+1, p.y-1, p.z};
            doTile(pathFinding, current, target);
        }
        // right
        {
            Vec3f target = {p.x+1, p.y, p.z};
            doTile(pathFinding, current, target);
        }
        // top right
        if (xPlus1Diff != 1 && yPlus1Diff != 1) {
            Vec3f target = {p.x+1, p.y+1, p.z};
            doTile(pathFinding, current, target);
        }

        //z - 1
        //
        //bottom left z - 1
        if (xMin1Diff != 1 && yMin1Diff != 1) {
            Vec3f target = {p.x-1, p.y-1, p.z-1};
            doTile(pathFinding, current, target);
        }
        // left z - 1
        {
            Vec3f target = {p.x-1, p.y, p.z-1};
            doTile(pathFinding, current, target);
        }
        // top left z - 1
        if (xMin1Diff != 1 && yPlus1Diff != 1) {
            Vec3f target = {p.x-1, p.y+1, p.z-1};
            doTile(pathFinding, current, target);
        }
        // bottom z - 1
        {
            Vec3f target = {p.x, p.y-1, p.z-1};
            doTile(pathFinding, current, target);
        }
        // top z - 1
        {
            Vec3f target = {p.x, p.y+1, p.z-1};
            doTile(pathFinding, current, target);
        }
        // bottom right z - 1
        if (xPlus1Diff != 1 && yMin1Diff != 1) {
            Vec3f target = {p.x+1, p.y-1, p.z-1};
            doTile(pathFinding, current, target);
        }
        // right z - 1
        {
            Vec3f target = {p.x+1, p.y, p.z-1};
            doTile(pathFinding, current, target);
        }
        // top right z - 1
        if (xPlus1Diff != 1 && yPlus1Diff != 1) {
            Vec3f target = {p.x+1, p.y+1, p.z-1};
            doTile(pathFinding, current, target);
        }

        //z + 1
        //
        //bottom left z - 1
        if (xMin1Diff != 1 && yMin1Diff != 1) {
            Vec3f target = {p.x-1, p.y-1, p.z+1};
            doTile(pathFinding, current, target);
        }
        // left z + 1
        {
            Vec3f target = {p.x-1, p.y, p.z+1};
            doTile(pathFinding, current, target);
        }
        // top left z + 1
        if (xMin1Diff != 1 && yPlus1Diff != 1) {
            Vec3f target = {p.x-1, p.y+1, p.z+1};
            doTile(pathFinding, current, target);
        }
        // bottom z + 1
        {
            Vec3f target = {p.x, p.y-1, p.z+1};
            doTile(pathFinding, current, target);
        }
        // top z + 1
        {
            Vec3f target = {p.x, p.y+1, p.z+1};
            doTile(pathFinding, current, target);
        }
        // bottom right z + 1
        if (xPlus1Diff != 1 && yMin1Diff != 1) {
            Vec3f target = {p.x+1, p.y-1, p.z+1};
            doTile(pathFinding, current, target);
        }
        // right z + 1
        {
            Vec3f target = {p.x+1, p.y, p.z+1};
            doTile(pathFinding, current, target);
        }
        // top right z + 1
        if (xPlus1Diff != 1 && yPlus1Diff != 1) {
            Vec3f target = {p.x+1, p.y+1, p.z+1};
            doTile(pathFinding, current, target);
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
    for (int i = 0 ; i < world.tilesN ; i++) {
        ret.fScores[i] = -1;
    }
    for (int i = 0 ; i < world.tilesN ; i++) {
        ret.gScores[i] = -1;
    }
    return ret;
}
