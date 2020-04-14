#include <SDL.h>
#include "input.h"
#include "camera.h"
#include "world.h"

float mouseSensitivity = 0.1f;
Input input;
void
findHoveredObject();

void
inputInit() {
    input.hoveredEntity = -1;
    input.selectedEntity = -1;
    for (int i = 0 ; i < KEY_NUMBER ; i++) {
        input.timeLimit[i] = 0;
        input.lastPress[i] = 0;
    }

    input.keyMapping[SDL_SCANCODE_ESCAPE] = KEY_ESCAPE;

    input.keyMapping[SDL_SCANCODE_UP] =    KEY_CAMERA_MOVE_FRONT;
    input.keyMapping[SDL_SCANCODE_RIGHT] = KEY_CAMERA_MOVE_RIGHT;
    input.keyMapping[SDL_SCANCODE_DOWN] =  KEY_CAMERA_MOVE_BACK;
    input.keyMapping[SDL_SCANCODE_LEFT] =  KEY_CAMERA_MOVE_LEFT;

    input.keyMapping[SDL_SCANCODE_R] =  KEY_MOVE_TO_RANDOM_LOCATION;
    input.timeLimit[KEY_MOVE_TO_RANDOM_LOCATION] = 1000;

    input.mouseMapping[1] = KEY_SELECT;
    input.mouseMapping[3] = KEY_GIVE_MOVE_ORDER;
}

void
mouseButtonDown(SDL_MouseButtonEvent *e) {
    Key k = input.mouseMapping[e->button];
    input.pressedKeys[k] = true;
    input.pressedKeysThisFrame[k] = true;
    printf("%d\n", e->button);
}

void
mouseButtonUp(SDL_MouseButtonEvent *e) {
    Key k = input.mouseMapping[e->button];
    input.pressedKeys[k] = false;
}

void
inputPollEvents() {
    for (int i = 0 ; i < KEY_NUMBER ; i++) {
        input.pressedKeysThisFrame[i] = false;
    }
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                input.pressedKeys[KEY_ESCAPE] = true;
                break;
            case SDL_KEYDOWN:
                keyDown(&e.key);
                break;
            case SDL_KEYUP:
                keyUp(&e.key);
                break;
            case SDL_MOUSEMOTION:
                mouseMove(e.motion);
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouseButtonDown(&e.button);
                break;
            case SDL_MOUSEBUTTONUP:
                mouseButtonUp(&e.button);
                break;
            default:
                // do nothing
                break;
        }
    }
    findHoveredObject();

    if (input.pressedKeysThisFrame[KEY_GIVE_MOVE_ORDER]) {
        Entity *hoveredEntity = &world.entities.data[input.hoveredEntity];
        Entity *selectedEntity = &world.entities.data[input.selectedEntity];

        Vec3f target = hoveredEntity->box.position;
        target.z = 1;
        SDL_Log("moving selected entity to=%f:%f", target.x, target.y);

        AiComponent *ai = &world.aiComponents.data[selectedEntity->ai];
        ai->target = target;
        ai->hasTarget = true;
    }
}


Entity*
getHoveredEntity() {
    if (input.hoveredEntity == -1) {
        return NULL;
    }
    return worldGetEntity(input.hoveredEntity);
}

Entity*
getSelectedEntity() {
    if (input.selectedEntity == -1) {
        return NULL;
    }
    return worldGetEntity(input.selectedEntity);
}

void
findHoveredObject() {
    #ifdef OBJECT_SELECTION_LOG
    SDL_Log("Look vector is %f:%f:%f", camera.front.x, camera.front.y, camera.front.z);
    #endif
    Vec3f startingPos = camera.position;
    Vec3f rayStep = vec3fDivf(camera.front, 30.0);
    float rayStepLength = vec3fLength(rayStep);
    double rayLength = 0;
    Vec3f rayPos = startingPos;
    bool intersect = false;
    Entity *currentlyHovered = getHoveredEntity();
    if (currentlyHovered != NULL) {
        input.hoveredEntity = -1;
        currentlyHovered->hovered = false;
    }

    while (rayLength < 10) {
        rayPos = vec3fAdd(rayPos, rayStep);
        for (int i = 0 ; i < world.entities.length ; i++) {
            Entity *e = &world.entities.data[i];
            if (box3fContainsVec3f(e->box, rayPos)) {
                //SDL_Log("box: %d %f:%f:%f", i, e->box.position.x, e->box.position.y, e->box.position.z);
                input.hoveredEntity = i;
                e->hovered = true;
                #ifdef OBJECT_SELECTION_LOG
                SDL_Log("Hover %d", i);
                #endif
                if (input.pressedKeysThisFrame[KEY_SELECT]) {
                    Entity *currentlySelected = getSelectedEntity();
                    if (currentlySelected != NULL) {
                        currentlySelected->selected = false;
                        Sprite *s = graphicsGetEntitySprite(currentlySelected);
                        spriteSetColorAdd(s, vec3fZero);
                    }

                    Box3f b = e->box;
                    Vec3f v = rayPos;

                    input.selectedEntity = i;
                    e->selected = true;

                    Sprite *s = graphicsGetEntitySprite(e);
                    Vec3f colorAdd = {0.3, 0.1, 0.3};
                    spriteSetColorAdd(s, colorAdd);

                    SDL_Log("ray pos: %f:%f:%f", rayPos.x, rayPos.y, rayPos.z);
                    SDL_Log("Selected %d %f %d %f:%f:%f", e->texture, e->box.min.x, i, s->box.position.x, s->box.position.y, s->box.position.z);
                }
                intersect = true;
                goto end;

            }
        }
        Vec3f rev = vec3fSub(rayPos, startingPos);
        rayLength = vec3fLength(rev);
    }
end:
    if (!intersect) {
        Entity *selectedEntity = getSelectedEntity();
        if (selectedEntity != NULL) {
            if (input.pressedKeysThisFrame[KEY_SELECT]) {
                selectedEntity->selected = false;
                Sprite *s = graphicsGetEntitySprite(selectedEntity);
                spriteSetColorAdd(s, vec3fZero);
                input.selectedEntity = -1;
            }
        }
    }
}

void
mouseMove(SDL_MouseMotionEvent e) {
    float xOffset = e.xrel * mouseSensitivity;
    float yOffset = -e.yrel * mouseSensitivity;
    camera.yaw += xOffset;
    camera.pitch += yOffset;
    updateCameraVector();
}

void
keyDown(SDL_KeyboardEvent *e) {
    Key pressedKey = input.keyMapping[e->keysym.scancode];
    input.pressedKeys[pressedKey] = true;
    input.pressedKeysThisFrame[pressedKey] = true;
}

void
keyUp(SDL_KeyboardEvent *e) {
    Key releasedKey = input.keyMapping[e->keysym.scancode];
    input.pressedKeys[releasedKey] = false;
}

