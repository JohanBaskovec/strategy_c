#define FIXED_ARRAY_SIZE 10000
#include "array.h"

typedef struct TestNode {
    int nextFree;
    int keep;
    int value;
    int id;
} TestNode;
typedef struct TestFixedArray {
    int length;
    int nextFree;
    TestNode data[FIXED_ARRAY_SIZE];
} TestFixedArray;

TEST_START(fixedArrayTest)
    TestFixedArray a;
    fixedArrayInit(&a, FIXED_ARRAY_SIZE);

    ASSERT_INT_EQUAL(0, a.length);
    ASSERT_INT_EQUAL(0, a.nextFree);
    TestNode n0 = {0, 1, 6};
    TestNode n1 = {0, 1, 7};
    TestNode n2 = {0, 1, 8};
    TestNode n3 = {0, 1, 9};
    int i;
    fixedArrayAdd(&a, n0, FIXED_ARRAY_SIZE, i)
    fixedArrayAdd(&a, n1, FIXED_ARRAY_SIZE, i)
    fixedArrayAdd(&a, n2, FIXED_ARRAY_SIZE, i)
    ASSERT_INT_EQUAL(3, a.length);
    ASSERT_INT_EQUAL(6, a.data[0].value);
    ASSERT_INT_EQUAL(7, a.data[1].value);
    ASSERT_INT_EQUAL(8, a.data[2].value);

    fixedArrayAdd(&a, n3, FIXED_ARRAY_SIZE, i)
    ASSERT_INT_EQUAL(4, a.length);
    ASSERT_INT_EQUAL(9, a.data[3].value);

    fixedArrayRemove(&a, 0);
    ASSERT_INT_EQUAL(4, a.length);
    ASSERT_INT_EQUAL(0, a.data[0].keep);
    fixedArrayRemove(&a, 1);
    ASSERT_INT_EQUAL(4, a.length);

    fixedArrayAdd(&a, n2, FIXED_ARRAY_SIZE, i)
    fixedArrayAdd(&a, n2, FIXED_ARRAY_SIZE, i)
    ASSERT_INT_EQUAL(4, a.length);
    ASSERT_INT_EQUAL(1, a.data[0].keep);
    ASSERT_INT_EQUAL(8, a.data[0].value);
    ASSERT_INT_EQUAL(1, a.data[1].keep);
    ASSERT_INT_EQUAL(8, a.data[1].value);

    fixedArrayAdd(&a, n2, FIXED_ARRAY_SIZE, i)
    ASSERT_INT_EQUAL(5, a.length);
    ASSERT_INT_EQUAL(1, a.data[4].keep);
    ASSERT_INT_EQUAL(8, a.data[4].value);

TEST_END
