#ifndef ST_SHADER
#define ST_SHADER
#include "defines.h"

GLuint
programCreate(
        char const *vertexShaderFileName
        , char const *fragmentShaderFileName
        , char const *geometricShaderFileName
);

GLint
getUniformLocation(GLuint program, char const *name);

GLint
getAttribLocation(GLuint program, char const *name);

#define TO_STR(a) TO_STR2(a)
#define TO_STR2(a) #a
#define SET_UNIFORM_LOCATION(name) p.name = getUniformLocation(p.id, #name);
#define SET_ATTRIB_LOCATION(name) p.name = getAttribLocation(p.id, #name);
#define SET_UNIFORM_LOCATION_ARRAY(name, property) \
    snprintf(str, bufferSize, #name "[%d]." #property, i);\
    p.name[i].property = getUniformLocation(p.id, str);\


#endif
