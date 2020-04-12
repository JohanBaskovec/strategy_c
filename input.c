#include <SDL.h>
#include "input.h"
#include "camera.h"

float mouseSensitivity = 0.1f;
Input input;

void
inputInit() {
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
}

void
inputPollEvents() {
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
            default:
                // do nothing
                break;
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
}

void
keyUp(SDL_KeyboardEvent *e) {
    Key releasedKey = input.keyMapping[e->keysym.scancode];
    input.pressedKeys[releasedKey] = false;
}

