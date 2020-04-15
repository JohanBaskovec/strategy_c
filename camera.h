#ifndef ST_CAMERA
#define ST_CAMERA
#include "vec3f.h"
#include "mat4f.h"
typedef enum CameraMode {
    CAMERA_MODE_RTS
    , CAMERA_MODE_FPS
} CameraMode;

typedef struct Camera {
    Vec3f position;
    Vec3f direction;
    Vec3f front;
    Vec3f target;
    Vec3f right;
    Vec3f up;
    float roll;
    float pitch;
    float yaw;
    Mat4f viewMatrix;
    CameraMode mode;
    float movementSpeed;
} Camera;

extern Camera camera;

void
cameraInit();

void
updateCameraVector();

void
cameraMove(Vec3f v);

void
cameraMoveBack();

void
cameraMoveFront();

void
cameraMoveUp();

void
cameraMoveLeft();

void
cameraMoveRight();

void
cameraMoveDown();

void
cameraSwitchMode();

#endif
