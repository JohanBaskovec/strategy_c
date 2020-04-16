#include <math.h>
#include "vec3f.h"

Vec3f vec3fZero = {0, 0, 0};
Vec3f vec3fOne = {1, 1, 1};

float
vec3fLength(Vec3f vec) {
    return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

float
vec3fDistance(Vec3f a, Vec3f b) {
    Vec3f diff = vec3fSub(a, b);
    return vec3fLength(diff);
}

float
vec3fRelativeLength(Vec3f vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

bool
vec3fIsZero(Vec3f v) {
    return v.x == 0 && v.z == 0 && v.y == 0;
}

Vec3f
vec3fNormalize(Vec3f vec) {
    float length = vec3fLength(vec);
    Vec3f ret = {
        .x = vec.x / length,
        .y = vec.y / length,
        .z = vec.z / length
    };
    return ret;
}

Vec3f
vec3fAdd(Vec3f a, Vec3f b) {
    Vec3f ret = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };
    return ret;
}

Vec3f
vec3fSub(Vec3f a, Vec3f b) {
    Vec3f ret = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };
    return ret;
}

float
vec3fDotProduct(Vec3f a, Vec3f b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3f
vec3fCrossProduct(Vec3f a, Vec3f b) {
    Vec3f ret = {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
    return ret;
}

Vec3f
vec3fMulf(Vec3f vec, float f) {
    Vec3f ret = {
        .x = vec.x * f,
        .y = vec.y * f,
        .z = vec.z * f
    };
    return ret;
}

Vec3f
vec3fDivf(Vec3f vec, float f) {
    Vec3f ret = {
        .x = vec.x / f,
        .y = vec.y / f,
        .z = vec.z / f
    };
    return ret;
}

Vec3f
vec3fSubf(Vec3f vec, float f) {
    Vec3f ret = {
        .x = vec.x - f,
        .y = vec.y - f,
        .z = vec.z - f
    };
    return ret;
}

bool
vec3fEqual(Vec3f a, Vec3f b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

void
vec3fPrint(Vec3f v) {
    printf("{.x: %f, .y: %f, .z: %f}", v.x, v.y, v.z);
}
