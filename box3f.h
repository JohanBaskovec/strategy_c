#ifndef ST_BOX3F
#define ST_BOX3F
#include "vec3f.h"

typedef struct Box3f {
    Vec3f position;
    Vec3f size;
    Vec3f min;
    Vec3f max;
} Box3f;

bool
box3fContainsVec3f(Box3f b, Vec3f v);

Box3f
box3fMoveVec3f(Box3f b, Vec3f v);

Box3f
box3fCreate(Vec3f position, Vec3f size);
#endif
