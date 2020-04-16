#ifndef ST_INPUT
#define ST_INPUT
#include <stdbool.h>
#include <SDL.h>
#include "object_type.h"
#include "vec3f.h"
#include "entity.h"

typedef enum Key {
    KEY_NONE
    , KEY_ESCAPE

    , KEY_CAMERA_MOVE_LEFT
    , KEY_CAMERA_MOVE_RIGHT
    , KEY_CAMERA_MOVE_FRONT
    , KEY_CAMERA_MOVE_BACK
    , KEY_CAMERA_MOVE_DOWN
    , KEY_CAMERA_MOVE_UP

    , KEY_MOVE_TO_RANDOM_LOCATION
    , KEY_GIVE_MOVE_ORDER

    , KEY_SWITCH_CAMERA_MODE
    , KEY_SELECT_DELETE_BLOCK
    , KEY_SELECT_DIRT
    , KEY_SELECT_NONE

    , KEY_SELECT
    , KEY_NUMBER
} Key;

typedef enum InputMode {
    INPUT_MODE_RTS
    , INPUT_MODE_FPS
    , INPUT_MODE_ALL
    , INPUT_MODE_NUMBER
} InputMode;

typedef enum MouseWheelInput {
    MOUSE_WHEEL_UP
    , MOUSE_WHEEL_DOWN
    , MOUSE_WHEEL_NUMBER
} MouseWheelInput;

typedef struct Input {
    bool pressedKeys[KEY_NUMBER];
    bool pressedKeysThisFrame[KEY_NUMBER];
    Key keyMapping[INPUT_MODE_NUMBER][SDL_NUM_SCANCODES];
    Key mouseMapping[4];
    Key mouseWheelMapping[INPUT_MODE_NUMBER][MOUSE_WHEEL_NUMBER];
    Uint32 timeLimit[KEY_NUMBER];
    Uint32 lastPress[KEY_NUMBER];
    int hoveredEntity;
    int selectedEntity;
    InputMode mode;
    int mouseX;
    int mouseY;
    bool creationModeEnabled;
    ObjectType object;
    Vec3f hoveredCoords;
    int tempObj;
    Entity *closestEntity;

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
