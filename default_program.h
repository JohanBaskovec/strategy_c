#ifndef ST_DEFAULT_PROGRAM
#define ST_DEFAULT_PROGRAM
#include "defines.h"

typedef struct DefaultProgram {
    GLuint id;

    GLint aPos;
    GLint aTexCoords;

    GLint model;
    GLint view;
    GLint projection;

    GLint diffuse;
} DefaultProgram;

DefaultProgram
defaultProgramCreate();
#endif