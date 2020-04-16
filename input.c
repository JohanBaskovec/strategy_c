#include <SDL.h>
#include "input.h"
#include "camera.h"
#include "world.h"
#include "ai_system.h"
#include "ai_component.h"
#include "object_type.h"
#include <math.h>

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
switchInputMode() {
    if (input.mode == INPUT_MODE_FPS) {
        input.mode = INPUT_MODE_RTS;
    } else {
        input.mode = INPUT_MODE_FPS;
    }
    cameraSwitchMode();
}

void
inputInit() {
    input.tempObj = -1;
    input.creationModeEnabled = true;
    input.object = OBJECT_NONE;
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

    input.keyMapping[INPUT_MODE_ALL][SDL_SCANCODE_C] = KEY_SWITCH_CAMERA_MODE;
    input.timeLimit[KEY_SWITCH_CAMERA_MODE] = 500;

    input.keyMapping[INPUT_MODE_ALL][SDL_SCANCODE_1] = KEY_SELECT_DELETE_BLOCK;
    input.keyMapping[INPUT_MODE_ALL][SDL_SCANCODE_2] = KEY_SELECT_DIRT;
    input.keyMapping[INPUT_MODE_ALL][SDL_SCANCODE_3] = KEY_SELECT_NONE;

    input.mouseMapping[1] = KEY_SELECT;
    input.timeLimit[KEY_SELECT] = 500;
    input.mouseMapping[3] = KEY_GIVE_MOVE_ORDER;
    input.timeLimit[KEY_GIVE_MOVE_ORDER] = 500;

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
creationModeSetObject(ObjectType t) {
    SDL_Log("Creation mode: %d", t);
    input.object = t;
}

void
revealSurroundingBlocks(Entity *e) {
    Vec3f p = e->box.position;
    Vec3i pi = {p.x, p.y, p.z};
    MinMaxVeci mmv = worldGetMinMaxPosi(pi);
    Vec3i min = mmv.min;
    Vec3i max = mmv.max;
    for (int z = min.z ; z <= max.z ; z++) {
        for (int x = min.x ; x <= max.x ; x++) {
            for (int y = min.y ; y <= max.y ; y++) {
                Vec3f v = {x, y, z};
                Entity *revealedEntity = worldGetTileAlignedEntity(v);
                vec3fPrint(v);
                printf("\n");
                if (revealedEntity != NULL) {
                    revealedEntity->reachable = true;
                    Sprite *s = graphicsGetEntitySprite(revealedEntity);
                    s->visible = true;
                }
            }
        }
    }
}

void
pressSelectButton() {
    if (input.hoveredEntity != -1) {
        if (input.object == OBJECT_EMPTINESS) {
            Entity *e = worldGetEntity(input.hoveredEntity);
            if (e->gridAligned) {
                revealSurroundingBlocks(e);
            }
            worldRemoveEntity(input.hoveredEntity);
            input.hoveredEntity = -1;
        } else if(input.object != OBJECT_NONE) {
            Entity *tempEntity = worldGetEntity(input.tempObj);
            tempEntity->isTemp = false;
            input.tempObj = -1;
        }
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
            target.z = target.z + 1;
            SDL_Log("moving selected entity %d to=%f:%f", input.selectedEntity, target.x, target.y);
            aiComponentSetPathTarget(ai, target);
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

    DO_WITH_MINIMUM_DELAY(KEY_SWITCH_CAMERA_MODE, switchInputMode)

    if (input.pressedKeysThisFrame[KEY_SELECT]) {
        pressSelectButton();
    }
    if (input.pressedKeysThisFrame[KEY_SELECT_DIRT]) {
        creationModeSetObject(OBJECT_WALL);
    } else if (input.pressedKeysThisFrame[KEY_SELECT_NONE]) {
        creationModeSetObject(OBJECT_NONE);
    } else if (input.pressedKeysThisFrame[KEY_SELECT_DELETE_BLOCK]) {
        creationModeSetObject(OBJECT_EMPTINESS);
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
hoverEntity(Entity *e) {
    if (input.creationModeEnabled
            && input.object != OBJECT_NONE
            && input.object != OBJECT_EMPTINESS) {
        Sprite *s = graphicsGetEntitySprite(e);
        // find closest empty space to place temporary object
        float shortestDist = 1000;
        Vec3f closestPos;
        bool found = false;
        Vec3f p = s->box.position;
        MinMaxVec mmv = worldGetMinMaxPos(p);
        Vec3f min = mmv.min;
        Vec3f max = mmv.max;
        for (int z = min.z ; z < max.z ; z++) {
            for (int x = min.x ; x < max.x ; x++) {
                for (int y = min.y ; y < max.y ; y++) {
                    Vec3f v = {x, y, z};
                    Vec3f diff = vec3fSub(v, p);
                    float dist = vec3fLength(diff);
                    if (dist < shortestDist) {
                        Entity *e = worldGetTileAlignedEntity(v);
                        if (e == NULL) {
                            shortestDist = dist;
                            closestPos = v;
                            found = true;
                        }
                    }
                }
            }
        }
        if (found) {
            if (input.tempObj != -1) {
                worldRemoveEntity(input.tempObj);
            }
            input.tempObj = worldCreateAndAddEntity(closestPos , input.object, true);
            Entity *e = worldGetEntity(input.tempObj);
            e->isTemp = true;
        }
    }
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
    input.hoveredCoords = objcoord;

    #ifdef LOG_HOVER
    printf("On pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u\n",
         input.mouseX, input.mouseY, color[0], color[1], color[2], color[3], depth, index);
    printf("Coordinates in object space: %f, %f, %f\n",
         objcoord.x, objcoord.y, objcoord.z);
    #endif

    float shortestDist = 1.0f;
    int entityI = -1;

    if (input.closestEntity != NULL) {

        int entityI = input.closestEntity->id;
        if (entityI != input.hoveredEntity) {
            Entity *e = worldGetEntity(entityI);
            if (!e->isTemp) {
                input.hoveredEntity = entityI;
                hoverEntity(e);
            }
        }

        input.closestEntity->hovered = true;
        #ifdef LOG_HOVER
        SDL_Log("Hover %d", entityI);
        #endif
        if (input.pressedKeysThisFrame[KEY_SELECT]
                && input.object == OBJECT_NONE) {
            Entity *currentlySelected = getSelectedEntity();
            if (currentlySelected != NULL) {
                currentlySelected->selected = false;
                Sprite *s = graphicsGetEntitySprite(currentlySelected);
                spriteSetColorAdd(s, vec3fZero);
            }

            Box3f b = input.closestEntity->box;

            input.selectedEntity = entityI;
            input.closestEntity->selected = true;

            Sprite *s = graphicsGetEntitySprite(input.closestEntity);
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

