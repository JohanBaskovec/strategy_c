#include <SDL.h>
#include <GL/glew.h>
#include <stdbool.h>
#include "error.h"
#include "fs.h"
#include "shader.h"

void
readShaderSource(char const *name, char **target) {
    size_t length;
    ErrorCode err = getContentsPosix(name, target , &length);
    if (err) {
        fatalError("Error while reading shader source %s, %d %s\n", name,
                err, errorGetString(err));
    }

}

GLuint createShader(
        GLenum shaderType
        , GLchar **source
        , GLuint program
) {
    GLuint shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, (GLchar const* const *)source, NULL);

    glCompileShader(shader);

    GLint compiled = false;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (compiled) {
        glAttachShader(program, shader);
        return shader;
    }

    char errorMessage[1024];
    GLsizei errorMessageLength = 0;
    glGetShaderInfoLog(shader, 1024, &errorMessageLength, errorMessage);
    fatalError("glCompileShader error: %s\n", errorMessage);
    return 0;
}


GLuint
programCreate(
        char const *vertexShaderFileName
        , char const *fragmentShaderFileName
        , char const *geometricShaderFileName
) {
    char *vsSrc = NULL;
    char *fsSrc = NULL;
    char *gsSrc = NULL;

    readShaderSource(vertexShaderFileName, &vsSrc);
    readShaderSource(fragmentShaderFileName, &fsSrc);

    GLuint program = glCreateProgram();

    SDL_Log("Creating vertex shader from %s...", vertexShaderFileName);
    GLuint vertexShader = createShader(
            GL_VERTEX_SHADER
            , &vsSrc
            , program
    );
    SDL_Log("Created vertex shader from %s.", vertexShaderFileName);

    SDL_Log("Creating fragment shader from %s...", fragmentShaderFileName);
    GLuint fragmentShader = createShader(
            GL_FRAGMENT_SHADER
            , &fsSrc
            , program
    );
    SDL_Log("Created fragment shader from %s.", fragmentShaderFileName);

    GLuint geometricShader;
    if (geometricShaderFileName != NULL) {
        readShaderSource(geometricShaderFileName, &gsSrc);
        SDL_Log("Creating geometric shader from %s...", geometricShaderFileName);
        geometricShader = createShader(
                GL_GEOMETRY_SHADER
                , &gsSrc
                , program
        );
        SDL_Log("Created geometric shader from %s.", geometricShaderFileName);
    }

    glLinkProgram(program);

    GLint linkSuccess = false;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
    if (!linkSuccess) {
        char errorMessage[1024];
        GLsizei errorMessageLength = 0;
        glGetProgramInfoLog(program, 1024, &errorMessageLength, errorMessage);
        fatalError("glLinkProgram: error: %s\n", errorMessage);
    }

    glDeleteShader(vertexShader);
    free(vsSrc);
    free(gsSrc);
    free(fsSrc);
    glDeleteShader(fragmentShader);
    if (geometricShaderFileName != NULL) {
        glDeleteShader(geometricShader);
    }

    SDL_Log("Created program %d.\n", program);
    return program;
}

GLint
getUniformLocation(GLuint program, char const *name) {
    GLint var = glGetUniformLocation(program, name);
    if (var == -1) {
        fatalError("Uniform %s not found in shader.\n", name);
    }
    return var;
}

GLint
getAttribLocation(GLuint program, char const *name) {
    GLint var = glGetAttribLocation(program, name);
    if (var == -1) {
        fatalError("Attribute %s not found in shader.\n", name);
    }
    return var;
}



