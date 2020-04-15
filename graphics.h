#ifndef ST_GRAPHICS
#define ST_GRAPHICS
#include <SDL.h>
#include <GL/glew.h>
#include <stdbool.h>
#include "vec3f.h"
#include "box3f.h"
#include "sprite.h"
#include "texture_enum.h"
#include "entity.h"

#define ZNEAR (0.1)
#define ZFAR (100.0)

typedef struct TextureConfig {
    char const *name;
    GLenum  format;
} TextureConfig;


#define SPRITE_MAX_NUMBER 30000

typedef struct SpriteFixedArray {
    int length;
    Sprite data[SPRITE_MAX_NUMBER];
} SpriteFixedArray;

typedef struct Graphics {
    Vec3f tileSize;
    SpriteFixedArray sprites[TEXTURE_NUMBER];
    int screenWidth;
    int screenHeight;
    Mat4f projectionMatrix;
} Graphics;

extern Graphics graphics;

#define GRAPHICS_MAX_SPRITES 4096

void
graphicsInit();

void
graphicsRender();

void
graphicsFree();

int
graphicsAddSprite(enum Texture texture, Sprite sprite);

void
graphicsDeleteSprite(enum Texture texture, int index);

Sprite*
graphicsGetSprite(enum Texture texture, int i);

Sprite*
graphicsGetEntitySprite(Entity *e);

void
graphicsInitBufferData();

#endif
