#include <SDL.h>
#include "input.h"
#include "camera.h"
#include "world.h"
#include "ai_system.h"
#include "ai_component.h"

#define DO_WITH_MINIMUM_DELAY(key_name, func)\
        if (input.pressedKeys[key_name]) {\
            Uint32 minTime = input.timeLimit[key_name];\
            if (ticks - input.lastPress[key_name] > minTime) {\
                func();\
                input.lastPress[key_name] = ticks;\
            }\
        }

float mouseSensitivity = 0.1f;
Input input;
void
findHoveredObject();

void
pressKeyThisFrame(Key k) {
    input.pressedKeys[k] = true;
    input.pressedKeysThisFrame[k] = true;
}

void
releaseKeyThisFrame(Key k) {
    input.pressedKeys[k] = false;
    input.pressedKeysThisFrame[k] = false;
}

void
inputInit() {
    input.mode = INPUT_MODE_RTS;
    input.hoveredEntity = -1;
    input.selectedEntity = -1;
    for (int i = 0 ; i < KEY_NUMBER ; i++) {
        input.timeLimit[i] = 0;
        input.lastPress[i] = 0;
    }

    input.keyMapping[INPUT_MODE_ALL][SDL_SCANCODE_ESCAPE] = KEY_ESCAPE;

    input.keyMapping[INPUT_MODE_FPS][SDL_SCANCODE_UP] =    KEY_CAMERA_MOVE_FRONT;
    input.keyMapping[INPUT_MODE_FPS][SDL_SCANCODE_RIGHT] = KEY_CAMERA_MOVE_RIGHT;
    input.keyMapping[INPUT_MODE_FPS][SDL_SCANCODE_DOWN] =  KEY_CAMERA_MOVE_BACK;
    input.keyMapping[INPUT_MODE_FPS][SDL_SCANCODE_LEFT] =  KEY_CAMERA_MOVE_LEFT;

    input.keyMapping[INPUT_MODE_RTS][SDL_SCANCODE_UP] =    KEY_CAMERA_MOVE_UP;
    input.keyMapping[INPUT_MODE_RTS][SDL_SCANCODE_RIGHT] = KEY_CAMERA_MOVE_RIGHT;
    input.keyMapping[INPUT_MODE_RTS][SDL_SCANCODE_DOWN] =  KEY_CAMERA_MOVE_DOWN;
    input.keyMapping[INPUT_MODE_RTS][SDL_SCANCODE_LEFT] =  KEY_CAMERA_MOVE_LEFT;

    input.keyMapping[INPUT_MODE_ALL][SDL_SCANCODE_R] =  KEY_MOVE_TO_RANDOM_LOCATION;
    input.timeLimit[KEY_MOVE_TO_RANDOM_LOCATION] = 1000;

    input.mouseMapping[1] = KEY_SELECT;
    input.mouseMapping[3] = KEY_GIVE_MOVE_ORDER;

    input.mouseWheelMapping[INPUT_MODE_RTS][MOUSE_WHEEL_UP] = KEY_CAMERA_MOVE_FRONT;
    input.mouseWheelMapping[INPUT_MODE_RTS][MOUSE_WHEEL_DOWN] = KEY_CAMERA_MOVE_BACK;
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
mouseWheel(SDL_MouseWheelEvent *e) {
    MouseWheelInput i;
    if(e->y == -1) {
       i = MOUSE_WHEEL_DOWN;
    } else if (e->y == 1) {
       i = MOUSE_WHEEL_UP;
    } else {
        // unsupported
        return;
    }

    Key k = input.mouseWheelMapping[input.mode][i];
    pressKeyThisFrame(k);

    k = input.mouseWheelMapping[INPUT_MODE_ALL][i];
    pressKeyThisFrame(k);
}

void
noMouseWheel() {
    for (int i = 0 ; i < MOUSE_WHEEL_NUMBER ; i++) {
        Key k = input.mouseWheelMapping[input.mode][i];
        releaseKeyThisFrame(k);

        k = input.mouseWheelMapping[INPUT_MODE_ALL][i];
        releaseKeyThisFrame(k);
    }
}

void
inputPollEvents(Uint32 ticks) {
    for (int i = 0 ; i < KEY_NUMBER ; i++) {
        input.pressedKeysThisFrame[i] = false;
    }
    SDL_Event e;

    bool hasMouseWheelEvent = false;
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
            case SDL_MOUSEWHEEL:
                mouseWheel(&e.wheel);
                hasMouseWheelEvent = true;
                break;
            default:
                // do nothing
                break;
        }
    }
    if (!hasMouseWheelEvent) {
        noMouseWheel();
    }
    findHoveredObject();

    if (input.pressedKeysThisFrame[KEY_GIVE_MOVE_ORDER]) {
        Entity *selectedEntity = &world.entities.data[input.selectedEntity];

        AiComponent *ai = aiSystemGetEntityAiComponent(selectedEntity);
        if (ai != NULL) {
            Entity *hoveredEntity = &world.entities.data[input.hoveredEntity];
            Vec3f target = hoveredEntity->box.position;
            target.z = 1;
            SDL_Log("moving selected entity %d to=%f:%f", input.selectedEntity, target.x, target.y);
            ai->target = target;
            ai->hasTarget = true;
        }
    }

    if (input.pressedKeys[KEY_ESCAPE]) {
        world.end = true;
    }
    if (input.pressedKeys[KEY_CAMERA_MOVE_BACK]) {
        cameraMoveBack();
    } else if (input.pressedKeys[KEY_CAMERA_MOVE_FRONT]) {
        cameraMoveFront();
    }
    if (input.pressedKeys[KEY_CAMERA_MOVE_LEFT]) {
        cameraMoveLeft();
    } else if (input.pressedKeys[KEY_CAMERA_MOVE_RIGHT]) {
        cameraMoveRight();
    }
    if (input.pressedKeys[KEY_CAMERA_MOVE_UP]) {
        cameraMoveUp();
    } else if (input.pressedKeys[KEY_CAMERA_MOVE_DOWN]) {
        cameraMoveDown();
    }

    DO_WITH_MINIMUM_DELAY(KEY_MOVE_TO_RANDOM_LOCATION, worldMoveRandom);
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
    GLbyte color[4];
    GLfloat depth;
    GLuint index;

    glReadPixels(input.mouseX, graphics.screenHeight - input.mouseY - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
    glReadPixels(input.mouseX, graphics.screenHeight - input.mouseY - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    glReadPixels(input.mouseX, graphics.screenHeight - input.mouseY - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);


    Vec4f viewport = {0, 0, graphics.screenWidth, graphics.screenHeight};
    Vec3f wincoord = {input.mouseX, graphics.screenHeight - input.mouseY, depth};
    Vec3f objcoord = mat4fUnproject(wincoord, camera.viewMatrix, graphics.projectionMatrix, viewport);

    // objcoord is right at the surface of the object, push it a liiittllee
    objcoord = vec3fAdd(objcoord, vec3fDivf(camera.front, 100.0));
    #ifdef LOG_HOVER
    printf("On pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u\n",
         input.mouseX, input.mouseY, color[0], color[1], color[2], color[3], depth, index);
    printf("Coordinates in object space: %f, %f, %f\n",
         objcoord.x, objcoord.y, objcoord.z);
    #endif

    float shortestDist = 1.0f;
    Entity *closestEntity = NULL;
    int entityI = -1;

    // TODO: move this to main loop to not loop over every entity multiple times
    for (int i = 0 ; i < world.entities.length ; i++) {
        Entity *e = &world.entities.data[i];
        Vec3f diff = vec3fSub(e->box.position, objcoord);
        float dist = vec3fLength(diff);
        if (dist < shortestDist) {
            shortestDist = dist;
            closestEntity = e;
            entityI = i;
        }
    }
    if (closestEntity != NULL) {
        input.hoveredEntity = entityI;
        closestEntity->hovered = true;
        #ifdef LOG_HOVER
        SDL_Log("Hover %d", entityI);
        #endif
        if (input.pressedKeysThisFrame[KEY_SELECT]) {
            Entity *currentlySelected = getSelectedEntity();
            if (currentlySelected != NULL) {
                currentlySelected->selected = false;
                Sprite *s = graphicsGetEntitySprite(currentlySelected);
                spriteSetColorAdd(s, vec3fZero);
            }

            Box3f b = closestEntity->box;

            input.selectedEntity = entityI;
            closestEntity->selected = true;

            Sprite *s = graphicsGetEntitySprite(closestEntity);
            Vec3f colorAdd = {0.3, 0.1, 0.3};
            spriteSetColorAdd(s, colorAdd);
        }
    }
}

void
mouseMove(SDL_MouseMotionEvent e) {
    input.mouseX = e.x;
    input.mouseY = e.y;
    if (input.mode == INPUT_MODE_FPS) {
        float xOffset = e.xrel * mouseSensitivity;
        float yOffset = -e.yrel * mouseSensitivity;
        camera.yaw += xOffset;
        camera.pitch += yOffset;
        updateCameraVector();
    }
}

void
keyDownInMode(SDL_KeyboardEvent *e, InputMode mode) {
    Key k = input.keyMapping[mode][e->keysym.scancode];
    pressKeyThisFrame(k);
}
void
keyDown(SDL_KeyboardEvent *e) {
    keyDownInMode(e, input.mode);
    keyDownInMode(e, INPUT_MODE_ALL);
}

void
keyUpInMode(SDL_KeyboardEvent *e, InputMode mode) {
    Key k = input.keyMapping[mode][e->keysym.scancode];
    releaseKeyThisFrame(k);
}
void
keyUp(SDL_KeyboardEvent *e) {
    keyUpInMode(e, input.mode);
    keyUpInMode(e, INPUT_MODE_ALL);
}

