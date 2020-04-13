#ifndef BOX3F_TEST_H
#define BOX3F_TEST_H
#include "box3f.h"
#include "vec3f.h"

TEST_START(box3fCreateTest)
    Vec3f position = {0, 0, 0};
    Vec3f size = {1, 2, 3};
    Box3f b = box3fCreate(position, size);
    float delta = 0.0001;
    ASSERT_FLOAT_EQUAL(0.0f, b.position.x, delta);
    ASSERT_FLOAT_EQUAL(0.0f, b.position.y, delta);
    ASSERT_FLOAT_EQUAL(0.0f, b.position.z, delta);

    ASSERT_FLOAT_EQUAL(1.0f, b.size.x, delta);
    ASSERT_FLOAT_EQUAL(2.0f, b.size.y, delta);
    ASSERT_FLOAT_EQUAL(3.0f, b.size.z, delta);

    ASSERT_FLOAT_EQUAL(0.5f, b.max.x, delta);
    ASSERT_FLOAT_EQUAL(1.0f, b.max.y, delta);
    ASSERT_FLOAT_EQUAL(1.5f, b.max.z, delta);

    ASSERT_FLOAT_EQUAL(-0.5f, b.min.x, delta);
    ASSERT_FLOAT_EQUAL(-1.0f, b.min.y, delta);
    ASSERT_FLOAT_EQUAL(-1.5f, b.min.z, delta);
TEST_END

TEST_START(box3fMoveVec3fTest)
    Vec3f position = {0, 0, 0};
    Vec3f size = {1, 2, 3};
    Box3f b = box3fCreate(position, size);
    float delta = 0.0001;
    Vec3f velocity = {1, 2, 3};

    b = box3fMoveVec3f(b, velocity);
    ASSERT_FLOAT_EQUAL(1.0f, b.position.x, delta);
    ASSERT_FLOAT_EQUAL(2.0f, b.position.y, delta);
    ASSERT_FLOAT_EQUAL(3.0f, b.position.z, delta);

    ASSERT_FLOAT_EQUAL(1.0f, b.size.x, delta);
    ASSERT_FLOAT_EQUAL(2.0f, b.size.y, delta);
    ASSERT_FLOAT_EQUAL(3.0f, b.size.z, delta);

    ASSERT_FLOAT_EQUAL(1.5f, b.max.x, delta);
    ASSERT_FLOAT_EQUAL(3.0f, b.max.y, delta);
    ASSERT_FLOAT_EQUAL(4.5f, b.max.z, delta);

    ASSERT_FLOAT_EQUAL(0.5f, b.min.x, delta);
    ASSERT_FLOAT_EQUAL(1.0f, b.min.y, delta);
    ASSERT_FLOAT_EQUAL(1.5f, b.min.z, delta);
TEST_END

TEST_START(box3fContainsVec3fTest)
    Vec3f position = {0, 0, 0};
    Vec3f size = {1, 2, 3};
    Box3f b = box3fCreate(position, size);
    float delta = 0.0001;
    Vec3f p0 = {0.5, 1.0, 1.5};
    bool c0 = box3fContainsVec3f(b, p0);
    ASSERT(c0);

    Vec3f p1 = {1, 2, 3};
    bool c1 = box3fContainsVec3f(b, p1);
    ASSERT(!c1);

    Vec3f p2 = {0, 0, 0};
    bool c2 = box3fContainsVec3f(b, p2);
    ASSERT(c2);
TEST_END

#endif
