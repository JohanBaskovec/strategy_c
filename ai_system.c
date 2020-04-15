#include "ai_system.h"
#include "world.h"
#include "ai_component.h"
#include "path_finding.h"

AiSystem aiSystem;

void
aiSystemInit() {
}

void
aiSystemFree() {
    for (int i = 0 ; i < aiSystem.aiComponents.length ; i++) {
        aiComponentFree(&aiSystem.aiComponents.data[i]);
    }
    arrayFree(aiSystem.aiComponents);
}

void
aiSystemUpdate() {
    for (int i = 0 ; i < aiSystem.aiComponents.length ; i++) {
        AiComponent *e = &aiSystem.aiComponents.data[i];
        aiUpdate(e);
    }
}

int
aiSystemAddAiComponent(AiComponent e) {
    int newIndex = aiSystem.aiComponents.length;
    arrayAdd(&aiSystem.aiComponents, e);
    return newIndex;
}

AiComponent*
aiSystemGetEntityAiComponent(Entity *e) {
    if (e->ai == -1) {
        return NULL;
    }
    return &aiSystem.aiComponents.data[e->ai];
}
