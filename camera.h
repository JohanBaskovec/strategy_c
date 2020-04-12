#ifndef ST_CAMERA
#define ST_CAMERA
#include "vec3f.h"

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
} Camera;

extern Camera camera;

void
cameraInit();

void
updateCameraVector();

#endif
