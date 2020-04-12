#ifndef ST_MATH_TEST_H
#define ST_MATH_TEST_H

#include <stdbool.h>
#include "mn_test.h"
#include "maths.h"

enum test_status testRadiansToDegrees()
{
    ASSERT_FLOAT_EQUAL(57.2957795, radiansToDegrees(1.0), 0.0001);
    ASSERT_FLOAT_EQUAL(28385.647, radiansToDegrees(495.423), 0.0001);
    ASSERT_FLOAT_EQUAL(0.0, radiansToDegrees(0.0), 0.0001);
    return TEST_STATUS_SUCCESS;
}

enum test_status testDegreesToRadians() {
    ASSERT_FLOAT_EQUAL(1.0, degreesToRadians(57.2957795), 0.0001);
    ASSERT_FLOAT_EQUAL(495.423, degreesToRadians(28385.647), 0.0001);
    ASSERT_FLOAT_EQUAL(0.0, degreesToRadians(0.0), 0.0001);
    return TEST_STATUS_SUCCESS;
}

enum test_status testMat4fVec3fTranslate() {
    return TEST_STATUS_SUCCESS;
}

#endif
