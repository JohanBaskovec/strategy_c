#ifndef VEC3F_TEST_H
#define VEC3F_TEST_H
#include "vec3f.h"

TEST_START(vec3fDivfTest)
    Vec3f v = {1, 2, 3};
    Vec3f divided = vec3fDivf(v, 2.0f);
    float delta = 0.0001f;
    ASSERT_FLOAT_EQUAL(0.5f, divided.x, delta);
    ASSERT_FLOAT_EQUAL(1.0f, divided.y, delta);
    ASSERT_FLOAT_EQUAL(1.5f, divided.z, delta);
TEST_END

TEST_START(vec3fSubfTest)
    Vec3f v = {1, 2, 3};
    Vec3f r = vec3fSubf(v, 2.0f);
    float delta = 0.0001f;
    ASSERT_FLOAT_EQUAL(-1.0f, r.x, delta);
    ASSERT_FLOAT_EQUAL( 0.0f, r.y, delta);
    ASSERT_FLOAT_EQUAL( 1.0f, r.z, delta);
TEST_END
#endif
