#include <SDL.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "maths.h"
#include "fs.h"
#include "shader.h"
#include "camera.h"
#include "graphics.h"
#include "input.h"
#include "world.h"
#include "mat4f.h"
#include "ai_system.h"

float movementSpeed = 0.1;

#define DO_WITH_MINIMUM_DELAY(key_name, func)\
        if (input.pressedKeys[key_name]) {\
            Uint32 minTime = input.timeLimit[key_name];\
            if (ticks - input.lastPress[key_name] > minTime) {\
                func();\
                input.lastPress[key_name] = ticks;\
            }\
        }\

int main(int argc, char *argv[]) {
    srand(time(NULL));
    cameraInit();
    inputInit();
    graphicsInit();
    worldInit();
    aiSystemInit();

    Uint32 lastFrameTicks = 0;
    Uint32 secondTimer = 0;
    int fps = 0;
    while (true) {
        graphicsRender();

        inputPollEvents();

        worldUpdate();
        Uint32 ticks = SDL_GetTicks();
        Uint32 ticksSinceLastFrame = ticks - lastFrameTicks;

        if (input.pressedKeys[KEY_ESCAPE]) {
            goto end;
        }
        if (input.pressedKeys[KEY_CAMERA_MOVE_BACK]) {
            Vec3f v = vec3fMulf(camera.front, -movementSpeed);
            cameraMove(v);
            /*
            camera.position.x -= movementSpeed * camera.front.x;
            camera.position.y -= movementSpeed * camera.front.y;
            camera.position.z -= movementSpeed * camera.front.z;
            */
        } else if (input.pressedKeys[KEY_CAMERA_MOVE_FRONT]) {
            Vec3f v = vec3fMulf(camera.front, movementSpeed);
            cameraMove(v);
            /*
            camera.position.x += movementSpeed * camera.front.x;
            camera.position.y += movementSpeed * camera.front.y;
            camera.position.z += movementSpeed * camera.front.z;
            */
        }
        if (input.pressedKeys[KEY_CAMERA_MOVE_LEFT]) {
            Vec3f v = vec3fMulf(camera.right, -movementSpeed);
            cameraMove(v);
            /*
            camera.position.x -= movementSpeed * camera.right.x;
            camera.position.y -= movementSpeed * camera.right.y;
            camera.position.z -= movementSpeed * camera.right.z;
            */

        } else if (input.pressedKeys[KEY_CAMERA_MOVE_RIGHT]) {
            Vec3f v = vec3fMulf(camera.right, movementSpeed);
            cameraMove(v);
            /*
            camera.position.x += movementSpeed * camera.right.x;
            camera.position.y += movementSpeed * camera.right.y;
            camera.position.z += movementSpeed * camera.right.z;
            */
        }

        DO_WITH_MINIMUM_DELAY(KEY_MOVE_TO_RANDOM_LOCATION, worldMoveRandom);

        secondTimer += ticksSinceLastFrame;
        if (secondTimer >= 1000) {
            fps = 1000.0 / ticksSinceLastFrame;
            SDL_Log("FPS: %d", fps);
            secondTimer = 0;
        }
        lastFrameTicks = ticks;
    }

end:

    aiSystemFree();
    worldFree();
    graphicsFree();
    SDL_Quit();

    return 0;
}

