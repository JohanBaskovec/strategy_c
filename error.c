#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <SDL.h>
#include "error.h"

void fatalSdlError(const char* funcName) {
    const char *sdlError = SDL_GetError();
    if (sdlError == NULL) {
        sdlError = "No error message from SDL.";
    }
    fatalError("%s: fatal error: %s\n", funcName, sdlError);
}
