#ifndef ST_PATH_FINDING_H
#define ST_PATH_FINDING_H
#include "vec3f.h"
#include "array.h"

typedef struct PathFindingNode {
    Vec3f position;
} PathFindingNode;

ARRAY_DECLARE(PathFindingNode, PathFindingNodeArray)

typedef struct PathFinding {
    // TODO: use Array struct
    float *gScores;
    float *fScores;
    PathFindingNode *cameFrom;

    PathFindingNodeArray open;

    // TODO: use Vec2f struct
    int goalX;
    int goalY;
} PathFinding;

Vec3fArray
pathFindingFindPath(PathFinding *pathFinding, Vec3f start, Vec3f goal);

void
pathFindingFree(PathFinding *pathFinding);

PathFinding
pathFindingCreate(int worldSize);
#endif
