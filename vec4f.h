#ifndef ST_VEC4F
#define ST_VEC4F

typedef struct Vec4f {
    float x;
    float y;
    float z;
    float w;
} Vec4f;

Vec4f
vec4fMulVec4f(Vec4f a, Vec4f b);

Vec4f
vec4fAddVec4f(Vec4f a, Vec4f b);

Vec4f
vec4fSubVec4f(Vec4f a, Vec4f b);

Vec4f
vec4fSubf(Vec4f a, float f);

Vec4f
vec4fMulf(Vec4f a, float f);

Vec4f
vec4fDivf(Vec4f a, float f);

#endif
