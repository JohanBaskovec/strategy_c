#include "ai_system.h"
#include "world.h"
#include "ai_component.h"
#include "path_finding.h"

AiSystem aiSystem;

void
aiSystemInit() {
    aiSystem.pathFinding = pathFindingCreate(world.tilesN);
}

void
aiSystemFree() {
    pathFindingFree(&aiSystem.pathFinding);
    for (int i = 0 ; i < world.aiComponents.length ; i++) {
        aiComponentFree(&world.aiComponents.data[i]);
    }
    arrayFree(world.aiComponents);
}

void
aiSystemUpdate() {
    //TODO: move ai components to a AI system struct
    for (int i = 0 ; i < world.aiComponents.length ; i++) {
        AiComponent *e = &world.aiComponents.data[i];
        aiUpdate(e);
    }

}

Vec3fArray
aiSystemFindPath(Vec3f start, Vec3f goal) {
    return pathFindingFindPath(&aiSystem.pathFinding, start, goal);
}
