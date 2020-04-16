#ifndef ST_VEC3I
#define ST_VEC3I
#include "array.h"

typedef struct Vec3i {
    int x;
    int y;
    int z;
} Vec3i;

Vec3i
vec3iAdd(Vec3i a, Vec3i b);

Vec3i
vec3iSub(Vec3i a, Vec3i b);

Vec3i
vec3iDivi(Vec3i vec, int f);

void
vec3iPrint(Vec3i v);
#endif
