#include "maths_test.h"

int main()
{
    enum test_status
    (*tests[])() = {
        &testRadiansToDegrees
        , &testDegreesToRadians
        , &testMat4fVec3fTranslate
    };
    int n_tests = sizeof tests / sizeof tests[0];
    return run_tests(tests, n_tests);
}
