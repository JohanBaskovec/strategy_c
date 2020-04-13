#include "graphics.h"
#include <stdbool.h>
#include "error.h"
#include "camera.h"
#include "world.h"
#include "mat4f.h"
#include "vec3f.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
#include "maths.h"
#include "defines.h"
#include "default_program.h"
#include "box3f.h"
#include "sprite.h"
#include "array.h"

#include "3d_data.h"

static int screenWidth = 1200;
static int screenHeight = 1000;
static float screenRatio = 0;
static float fovDegree = 90;
static float fovRadians = 0;

GLuint defaultVao;
GLuint defaultVbo;
SDL_Window *window;
GLuint textures[TEXTURE_NUMBER];
TextureConfig textureConfigs[TEXTURE_NUMBER] = {
    {
        .name = "paddle.png",
        .format = GL_RGBA
    }
    ,{
        .name = "wood00.jpg",
        .format = GL_RGB
    }
    ,{
        .name = "wall.png",
        .format = GL_RGBA
    }
};

Graphics graphics;

void
createTexture(GLuint texture, char const *fileName, GLenum format);

int
graphicsAddSprite(enum Texture texture, Sprite sprite) {
    int i = graphics.sprites[texture].length;
    arrayAdd(graphics.sprites[texture], sprite);
    return i;
}

Sprite*
graphicsGetSprite(enum Texture texture, int i) {
    return &graphics.sprites[texture].data[i];
}

Sprite*
graphicsGetEntitySprite(Entity *e) {
    return graphicsGetSprite(e->texture, e->spriteIndex);
}

void
openglDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                           GLenum severity, GLsizei length,
                           const GLchar* message, const void* userParam) {
    SDL_Log(
            "GL CALLBACK: type = 0x%x, severity = 0x%x, message = %s\n"
            , type
            , severity
            , message
    );
}

DefaultProgram defaultProgram;
Mat4f identityMat;

void
graphicsInit() {
    // make sure that sprite array starts empty
    for (int i = 0 ; i < TEXTURE_NUMBER ; i++) {
        SpriteArray a = arrayCreate();
        graphics.sprites[i] = a;
    }
    Vec3f tileSize = {1.0, 1.0, 1.0};
    graphics.tileSize = tileSize;
    identityMat = mat4fIdentity();
    SDL_Log("Opening window...");
    screenRatio = (screenWidth * 1.0) / screenHeight;
    fovRadians = degreesToRadians(fovDegree);
    int err = SDL_Init(SDL_INIT_VIDEO);
    if (err) {
      fatalSdlError("SDL_Init");
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_PROFILE_MASK
            , SDL_GL_CONTEXT_PROFILE_CORE
    );


    window = SDL_CreateWindow(
        "Pong"
        , SDL_WINDOWPOS_CENTERED
        , SDL_WINDOWPOS_CENTERED
        , screenWidth
        , screenHeight
        , SDL_WINDOW_OPENGL
    );

    if (window == NULL) {
        fatalSdlError("SDL_CreateWindow");
    }
    SDL_Log("Window opened.");

    SDL_GL_CreateContext(window);
    if (err) {
        fatalSdlError("SDL_GL_CreateContext");
    }

    glewExperimental = true;
    glewInit();

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);

    glDebugMessageCallback(openglDebugMessageCallback, NULL);

    err = SDL_GL_SetSwapInterval(1);

    if (err) {
      fatalSdlError("SDL_GL_SetSwapInterval");
    }

    defaultProgram = defaultProgramCreate();

    glGenVertexArrays(1, &defaultVao);
    glGenBuffers(1, &defaultVbo);

    glBindVertexArray(defaultVao);
    glBindBuffer(GL_ARRAY_BUFFER, defaultVbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(
            defaultProgram.aPos
            , 3
            , GL_FLOAT
            , GL_FALSE
            , 8 * sizeof(float)
            , NULL
    );
    glEnableVertexAttribArray(defaultProgram.aPos);

    glVertexAttribPointer(
            defaultProgram.aTexCoords
            , 2
            , GL_FLOAT
            , GL_FALSE
            , 8 * sizeof(float)
            , (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(defaultProgram.aTexCoords);

    SDL_Log("Loading textures...");

    stbi_set_flip_vertically_on_load(true);
    glGenTextures(TEXTURE_NUMBER, textures);
    for (int i = 0 ; i < TEXTURE_NUMBER ; i++) {
        createTexture(textures[i], textureConfigs[i].name, textureConfigs[i].format);
    }
    SDL_Log("Textures loaded.");

    glBindVertexArray(0);

    SDL_Log("OpengL: available image units = %d\n",  GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    SDL_SetRelativeMouseMode(true);

    SDL_Log("Loading done.");
}

void
graphicsRender() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render normally
    glViewport(0, 0, screenWidth, screenHeight);
    glUseProgram(defaultProgram.id);

    camera.target = vec3fAdd(camera.position, camera.front);
    Mat4f viewMat = mat4fLookAt(camera.position, camera.target, camera.up);
    glUniformMatrix4fv(defaultProgram.view, 1, false, (GLfloat*)&viewMat);

    Mat4f projectionMat = mat4fPerspective(fovRadians, screenRatio, ZNEAR, ZFAR);
    glUniformMatrix4fv(defaultProgram.projection, 1, false, (GLfloat*)&projectionMat);

    // bind the sampler2d in the shader to the texture GL_TEXTURE0
    glUniform1i(defaultProgram.diffuse, 0);

    glBindVertexArray(defaultVao);
    glActiveTexture(GL_TEXTURE0);

    for (int i = 0 ; i < TEXTURE_NUMBER ; i++) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        SpriteArray *a = &graphics.sprites[i];
        for (int x = 0 ; x < a->length ; x++) {
            Sprite *s = &a->data[x];
            Mat4f modelMat = mat4fVec3fTranslate(identityMat, s->box.position);
            modelMat = mat4fScale(modelMat, s->box.size);

            glUniform1i(defaultProgram.selected, s->selected);
            glUniformMatrix4fv(defaultProgram.model, 1, false, (GLfloat*)&modelMat);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    glBindVertexArray(0);
    glUseProgram(0);
    SDL_GL_SwapWindow(window);
}

void
graphicsFree() {
    SDL_DestroyWindow(window);
}

void
createTexture(GLuint texture, char const *fileName, GLenum format) {
    SDL_Log("Creating texture %d from %s...\n", texture, fileName);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(fileName, &width, &height, &nrChannels, 0);
    if (!data) {
        fatalError("stbi_load error.\n");
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    SDL_Log("Creating texture %d from %s: DONE.\n", texture, fileName);
}
