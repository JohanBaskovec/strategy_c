#include "array_test.h"
#include "vec3f_test.h"
#include "box3f_test.h"

TEST_PROGRAM_START
    RUN_TEST(arrayNewTest)
    RUN_TEST(arrayAddTest)
    RUN_TEST(arrayRemoveTest)

    RUN_TEST(vec3fDivfTest);
    RUN_TEST(vec3fSubfTest);

    RUN_TEST(box3fCreateTest);
    RUN_TEST(box3fContainsVec3fTest);
    RUN_TEST(box3fMoveVec3fTest);
TEST_PROGRAM_END
