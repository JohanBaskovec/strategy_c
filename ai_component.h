#ifndef ST_AI_COMPONENT_H
#define ST_AI_COMPONENT_H
#include <stdbool.h>
#include "vec3f.h"
#include "path_finding.h"
#include "attribute.h"
#include "skill.h"
#include "job_type.h"
#include "job_priority_queue.h"

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
    JobPriorityQueue jobPriorities;
} AiComponent;

ARRAY_DECLARE(AiComponent, AiComponentArray);

AiComponent
aiComponentCreate();

void
aiUpdate(AiComponent *ai);

void
aiComponentFree(AiComponent *ai);

void
aiComponentSetPathTarget(AiComponent *ai , Vec3f target);
#endif
