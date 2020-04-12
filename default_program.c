#include <SDL.h>
#include <GL/glew.h>
#include "default_program.h"
#include "shader.h"

DefaultProgram
defaultProgramCreate() {
    DefaultProgram p;
    SDL_Log("Creating default program.\n");
    p.id = programCreate("default.vs", "default.fs", NULL);
    glUseProgram(p.id);

    SET_ATTRIB_LOCATION(aPos);
    SET_ATTRIB_LOCATION(aTexCoords);

    SET_UNIFORM_LOCATION(model);
    SET_UNIFORM_LOCATION(projection);
    SET_UNIFORM_LOCATION(view);

    SET_UNIFORM_LOCATION(diffuse);

    glUseProgram(0);
    SDL_Log("Default program created.\n");
    return p;
}
