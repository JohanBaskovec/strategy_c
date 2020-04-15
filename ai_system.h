#ifndef ST_AI_SYSTEM_H
#define ST_AI_SYSTEM_H
#include "path_finding.h"
#include "vec3f.h"
#include "ai_component.h"
#include "entity.h"

typedef struct AiSystem {
    AiComponentArray aiComponents;
} AiSystem;

Vec3fArray
aiSystemFindPath(Vec3f start, Vec3f goal);

extern AiSystem aiSystem;

void
aiSystemInit();

void
aiSystemUpdate();

void
aiSystemFree();

int
aiSystemAddAiComponent(AiComponent e);

AiComponent*
aiSystemGetEntityAiComponent(Entity *e);
#endif
