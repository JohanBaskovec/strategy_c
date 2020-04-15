#ifndef ST_AI_COMPONENT_H
#define ST_AI_COMPONENT_H
#include <stdbool.h>
#include "vec3f.h"
#include "path_finding.h"
#include "attribute.h"
#include "skill.h"
#include "job_type.h"

typedef struct AiComponent {
    int entity;
    bool hasTarget;
    Vec3f target;
    bool isMoving;
    Vec3f movingTo;
    Vec3fArray path;
    int pathIndex;
    float movementSpeed;
    float hunger;
    PathFinding pathFinding;
    float attributes[ATTRIBUTE_NUMBER];
    float skills[SKILL_NUMBER];
    float jobPriorities[JOB_NUMBER];
} AiComponent;

ARRAY_DECLARE(AiComponent, AiComponentArray);

AiComponent
aiComponentCreate();

void
aiUpdate(AiComponent *ai);

void
aiComponentFree(AiComponent *ai);

#endif
