#include <stdbool.h>
#include "box3f.h"

bool
box3fContainsVec3f(Box3f b, Vec3f v) {
    return (v.x >= b.min.x && v.x <= b.max.x) &&
           (v.y >= b.min.y && v.y <= b.max.y) &&
           (v.z >= b.min.z && v.z <= b.max.z);
}

Box3f
box3fMoveVec3f(Box3f b, Vec3f v) {
    b.position = vec3fAdd(b.position, v);
    b.min = vec3fAdd(b.min, v);
    b.max = vec3fAdd(b.max, v);
    return b;
}

Box3f
box3fCreate(Vec3f position, Vec3f size) {
    Box3f ret;
    ret.position = position;
    ret.size = size;
    Vec3f halfSize = vec3fDivf(size, 2.0f);
    ret.min = vec3fSub(position, halfSize);
    ret.max = vec3fAdd(position, halfSize);
    return ret;
}
