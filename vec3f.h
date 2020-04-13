#ifndef ST_VEC3F
#define ST_VEC3F

typedef struct Vec3f {
    float x;
    float y;
    float z;
} Vec3f;

#define Vec3f_size (3 * sizeof(float))

extern Vec3f vec3fZero;

float
vec3fLength(Vec3f vec);

Vec3f
vec3fNormalize(Vec3f vec);

Vec3f
vec3fAdd(Vec3f a, Vec3f b);

Vec3f
vec3fSub(Vec3f a, Vec3f b);

Vec3f
vec3fMulf(Vec3f vec, float f);

float
vec3fDotProduct(Vec3f a, Vec3f b);

Vec3f
vec3fCrossProduct(Vec3f a, Vec3f b);

Vec3f
vec3fDivf(Vec3f vec, float f);

Vec3f
vec3fSubf(Vec3f vec, float f);

#endif
