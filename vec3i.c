#include "vec3i.h"

Vec3i
vec3iAdd(Vec3i a, Vec3i b) {
    Vec3i ret = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };
    return ret;
}

Vec3i
vec3iSub(Vec3i a, Vec3i b) {
    Vec3i ret = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };
    return ret;
}


Vec3i
vec3iDivi(Vec3i vec, int f) {
    Vec3i ret = {
        .x = vec.x / f,
        .y = vec.y / f,
        .z = vec.z / f
    };
    return ret;
}

void
vec3iPrint(Vec3i v) {
    printf("{.x: %d, .y: %d, .z: %d}", v.x, v.y, v.z);
}
