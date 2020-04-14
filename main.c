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
    int frameCount = 0;

    while (!world.end) {
        graphicsRender();

        worldUpdate();
        Uint32 ticks = SDL_GetTicks();
        Uint32 ticksSinceLastFrame = ticks - lastFrameTicks;

        inputPollEvents(ticks);

        secondTimer += ticksSinceLastFrame;
        if (secondTimer >= 1000) {
            SDL_Log("%d frames rendering in %d milliseconds.", frameCount, secondTimer);
            fps = (frameCount / (secondTimer * 1.0)) * 1000;
            SDL_Log("FPS: %d", fps);
            secondTimer = 0;
            frameCount = 0;
        }
        lastFrameTicks = ticks;
        frameCount++;
    }

end:

    aiSystemFree();
    worldFree();
    graphicsFree();
    SDL_Quit();

    return 0;
}

