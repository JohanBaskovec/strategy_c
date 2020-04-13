#ifndef ARRAY_TEST_H
#define ARRAY_TEST_H
#include <mn_test.h>
#include "array.h"

typedef struct TestArray {
    int length;
    int allocated;
    int *data;
} TestArray;


TEST_START(arrayNewTest)
    TestArray ta = arrayCreate();
    ASSERT_NULL(ta.data);
    ASSERT_INT_EQUAL(0, ta.length);
    ASSERT_INT_EQUAL(0, ta.allocated);
TEST_END

TEST_START(arrayAddTest)
    TestArray ta = arrayCreate();
    arrayAdd(ta, 0);
    arrayAdd(ta, 1);
    arrayAdd(ta, 2);
    arrayAdd(ta, 3);
    arrayAdd(ta, 3);
    arrayAdd(ta, 3);
    arrayAdd(ta, 3);
    ASSERT_INT_EQUAL(0, ta.data[0]);
    ASSERT_INT_EQUAL(1, ta.data[1]);
    ASSERT_INT_EQUAL(2, ta.data[2]);
    ASSERT_INT_EQUAL(3, ta.data[3]);
    ASSERT_INT_EQUAL(3, ta.data[4]);
    ASSERT_INT_EQUAL(3, ta.data[5]);
    ASSERT_INT_EQUAL(3, ta.data[6]);
TEST_END

TEST_START(arrayRemoveTest)
    TestArray ta = arrayCreate();
    arrayAdd(ta, 0);
    arrayAdd(ta, 1);
    arrayAdd(ta, 2);
    arrayAdd(ta, 3);
    arrayAdd(ta, 3);
    arrayAdd(ta, 3);
    arrayAdd(ta, 3);
    arrayRemove(ta, 0);
    arrayRemove(ta, 1);

    ASSERT_INT_EQUAL(1, ta.data[0]);
    ASSERT_INT_EQUAL(3, ta.data[1]);
    ASSERT_INT_EQUAL(3, ta.data[2]);
    ASSERT_INT_EQUAL(3, ta.data[3]);
    ASSERT_INT_EQUAL(3, ta.data[4]);
TEST_END
#endif
