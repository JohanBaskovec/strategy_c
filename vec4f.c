#include "vec4f.h"

Vec4f
vec4fMulVec4f(Vec4f a, Vec4f b) {
    Vec4f ret = {
        .x = a.x * b.x,
        .y = a.y * b.y,
        .z = a.z * b.z,
        .w = a.w * b.w
    };
    return ret;
}

Vec4f
vec4fAddVec4f(Vec4f a, Vec4f b) {
    Vec4f ret = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
        .w = a.w + b.w
    };
    return ret;
}

Vec4f
vec4fSubVec4f(Vec4f a, Vec4f b) {
    Vec4f ret = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
        .w = a.w - b.w
    };
    return ret;
}
