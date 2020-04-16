#ifndef ST_BOX3I_H
#define ST_BOX3I_H
#include "vec3i.h"
#include "vec3f.h"

typedef struct Box3i {
    Vec3i position;
    Vec3i size;
    Vec3i min;
    Vec3i max;
} Box3i;

bool
box3iContainsVec3i(Box3i b, Vec3i v);

bool
box3iContainsVec3f(Box3i b, Vec3f v);

Box3i
box3iCreate(Vec3i position, Vec3i size);
#endif
