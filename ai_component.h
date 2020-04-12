#ifndef ST_AI_COMPONENT_H
#define ST_AI_COMPONENT_H
#include <stdbool.h>
#include "vec3f.h"

typedef struct Vec3fArray {
    int length;
    int allocated;
    Vec3f *data;
} Vec3fArray;

typedef struct AiComponent {
    int entity;
    bool hasTarget;
    Vec3f target;
    bool isMoving;
    Vec3f movingTo;
    Vec3fArray path;
    int pathIndex;
    float movementSpeed;
} AiComponent;

void
aiUpdate(AiComponent *ai);

#endif
