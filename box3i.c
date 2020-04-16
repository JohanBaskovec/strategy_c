#include "box3i.h"
#include "vec3f.h"

bool
box3iContainsVec3i(Box3i b, Vec3i v) {
    bool c = (v.x >= b.min.x && v.x <= b.max.x) &&
           (v.y >= b.min.y && v.y <= b.max.y) &&
           (v.z >= b.min.z && v.z <= b.max.z);
    return c;
}

bool
box3iContainsVec3f(Box3i b, Vec3f v) {
    bool c = (v.x >= b.min.x && v.x <= b.max.x) &&
           (v.y >= b.min.y && v.y <= b.max.y) &&
           (v.z >= b.min.z && v.z <= b.max.z);
    return c;
}

Box3i
box3iCreate(Vec3i position, Vec3i size) {
    Box3i ret;
    // 0 1 2
    ret.position = position;
    ret.size = size;
    if (size.x % 2 != 0 || size.y % 2 != 0 || size.z % 2 != 0) {
        printf("box3iCreate error: vector size must be divisible by 2");
        exit(1);
    }
    Vec3i halfSize = vec3iDivi(size, 2);
    ret.min = vec3iSub(position, halfSize);
    ret.max = vec3iAdd(position, halfSize);
    return ret;
}
