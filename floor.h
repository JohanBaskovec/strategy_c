#ifndef ST_FLOOR
#define ST_FLOOR
#include "vec3f.h"

typedef enum FloorType {
    FLOOR_TYPE_GRASS,
    FLOOR_TYPE_SAND,
    FLOOR_TYPE_NUMBER
} FloorType;

typedef struct Floor {
    Vec3f position;
} Floor;

#endif
