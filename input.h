#ifndef ST_INPUT
#define ST_INPUT
#include <stdbool.h>
#include <SDL.h>

typedef enum Key {
    KEY_NONE
    , KEY_ESCAPE

    , KEY_CAMERA_MOVE_LEFT
    , KEY_CAMERA_MOVE_RIGHT
    , KEY_CAMERA_MOVE_FRONT
    , KEY_CAMERA_MOVE_BACK

    , KEY_MOVE_TO_RANDOM_LOCATION

    , KEY_NUMBER
} Key;

typedef struct Input {
    bool pressedKeys[KEY_NUMBER];
    Key keyMapping[SDL_NUM_SCANCODES];
    Uint32 timeLimit[KEY_NUMBER];
    Uint32 lastPress[KEY_NUMBER];
} Input;

extern Input input;

void
inputInit();

void
keyDown(SDL_KeyboardEvent *e);

void
keyUp(SDL_KeyboardEvent *e);

void
mouseMove(SDL_MouseMotionEvent e);

void
inputPollEvents();

#endif
