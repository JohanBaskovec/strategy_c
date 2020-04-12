/**
 * \file
 */

#ifndef TEST_LIBRARY_LIBRARY_H
#define TEST_LIBRARY_LIBRARY_H

#include <string.h>
#include <stdio.h>
#include <math.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define __ASSERT_FAILED_STRING "ASSERT failed in function %s at line %s:%d\n"

#define __FAIL_TEST_IF_TRUE(expected, actual, printfchar, type_name, expected_string, actual_string, bool_to_verify, message)\
    do { \
        if (bool_to_verify) { \
            printf(ANSI_COLOR_RED __ASSERT_FAILED_STRING\
"\t" #type_name "s " message ".\n\
\tExpected = " printfchar ", actual = " printfchar " \n" ANSI_COLOR_RESET,\
__FUNCTION__, __FILE__, __LINE__, expected_string, actual_string); \
        return TEST_STATUS_FAILURE;\
        } \
    } while (0);


/**
 * Assert the values are equal.
 */
#define ASSERT_VAR_EQUAL(expected, actual, printfchar, type_name)\
__FAIL_TEST_IF_TRUE(expected\
        , actual\
        , printfchar\
        , type_name\
        , expected\
        , actual\
        , actual != expected\
        , "aren't equal")

/**
* Assert the values are unequal.
*/
#define ASSERT_VAR_UNEQUAL(unexpected, actual, printfchar, type_name)\
  __FAIL_TEST_IF_TRUE(unexpected\
        , actual\
        , printfchar\
        , type_name\
        , unexpected\
        , actual\
        , actual == unexpected\
        , "are equal")

/**
 * Status of the tests. Each test function must
 * return either one of these values.
 */
enum test_status
{
    /**
     * Indicates that the test ran without any assertion errors.
     */
    TEST_STATUS_SUCCESS,
    /**
     * Indicates that the test ran but encountered an assertion error.
     */
    TEST_STATUS_FAILURE
};


/**
 * Assert the expression is true.
 */
#define ASSERT(expression) \
    do { \
    if (!(expression)) { \
        printf(ANSI_COLOR_RED __ASSERT_FAILED_STRING "\t %s is false!\n"\
        ANSI_COLOR_RESET, __FUNCTION__, __FILE__, __LINE__, #expression); \
        return TEST_STATUS_FAILURE;\
    } \
    } while (0);


/**
 * Assert the booleans are equal.
 */
#define ASSERT_BOOL_EQUAL(expected_bool, actual_bool) \
__FAIL_TEST_IF_TRUE(expected_bool\
        , actual_bool\
        , "%s"\
        , bool\
        , expected_bool?"true":"false"\
        , actual_bool?"true":"false"\
        , (actual_bool != expected_bool)\
        , "aren't equal")

/**
* Assert the booleans are equal.
*/
#define ASSERT_BOOL_UNEQUAL(expected_bool, actual_bool) \
__FAIL_TEST_IF_TRUE(expected_bool\
        , actual_bool\
        , "%s"\
        , bool\
        , expected_bool?"true":"false"\
        , actual_bool?"true":"false"\
        , (actual_bool == expected_bool)\
        , "are equal")

/**
 * Assert the ints are equal.
 */
#define ASSERT_INT_EQUAL(expected_int, actual_int) \
    ASSERT_VAR_EQUAL(expected_int, actual_int, "%i", int)

/**
* Assert the ints are unequal.
*/
#define ASSERT_INT_UNEQUAL(expected_int, actual_int) \
    ASSERT_VAR_UNEQUAL(expected_int, actual_int, "%i", int)
/**
 * Assert the values are almost equal.
 */
#define ASSERT_VAR_ALMOST_EQUAL(expected, actual, printfchar, type_name, delta)\
__FAIL_TEST_IF_TRUE(expected\
        , actual\
        , printfchar\
        , type_name\
        , expected\
        , actual\
        , (fabs(actual - expected) > delta)\
        , "aren't equal")

/**
* Assert the values are unequal.
*/
#define ASSERT_VAR_NOT_ALMOST_UNEQUAL(unexpected, actual, printfchar, type_name, delta)\
  __FAIL_TEST_IF_TRUE(unexpected\
        , actual\
        , printfchar\
        , type_name\
        , unexpected\
        , actual\
        , (fabs(actual - expected) < delta)\
        , "are equal")

/**
* Assert the double are equal.
*/
#define ASSERT_DOUBLE_EQUAL(expected, actual, delta) \
    ASSERT_VAR_ALMOST_EQUAL(expected, actual, "%f", double, delta)

/**
* Assert the double are unequal.
*/
#define ASSERT_DOUBLE_UNEQUAL(unexpected, actual, delta) \
    ASSERT_VAR_NOT_ALMOST_EQUAL(unexpected, actual, "%f", double, delta)


/**
* Assert the floats are equal.
*/
#define ASSERT_FLOAT_EQUAL(expected, actual, delta) \
    ASSERT_VAR_ALMOST_EQUAL(expected, actual, "%f", float, delta)

/**
* Assert the floats are unequal.
*/
#define ASSERT_FLOAT_UNEQUAL(unexpected, actual, delta) \
    ASSERT_VAR_NOT_ALMOST_EQUAL(unexpected, actual, "%f", float, delta)

/**
* Assert the pointer is equal to another pointer.
*/
#define ASSERT_POINTER_EQUAL(expected, actual) \
    ASSERT_VAR_EQUAL(expected, actual, "%p", pointer)

/**
* Assert the pointer is not equal to another pointer.
*/
#define ASSERT_POINTER_UNEQUAL(expected, actual) \
    ASSERT_VAR_UNEQUAL(expected, actual, "%p", pointer)

/**
* Assert the value is null.
*/
#define ASSERT_NULL(actual) \
    ASSERT_POINTER_EQUAL(NULL, actual)

/**
* Assert the value is not null.
*/
#define ASSERT_NOT_NULL(actual) \
    ASSERT_POINTER_UNEQUAL(NULL, actual)

/**
* Assert the size_t are equal.
*/
#define ASSERT_SIZE_T_EQUAL(expected_size_t, actual_size_t) \
    ASSERT_VAR_EQUAL(expected_size_t, actual_size_t, "%zu", size_t)

/**
* Assert the size_t are equal.
*/
#define ASSERT_SIZE_T_UNEQUAL(unexpected_size_t, actual_size_t) \
    ASSERT_VAR_UNEQUAL(unexpected_size_t, actual_size_t, "%zu", size_t)


/**
 * Assert the char are equal.
 */
#define ASSERT_CHAR_EQUAL(expected_char, actual_char) \
    ASSERT_VAR_EQUAL(expected_char, actual_char, "%c", char)

/**
* Assert the char are unequal.
*/
#define ASSERT_CHAR_UNEQUAL(unexpected_char, actual_char) \
    ASSERT_VAR_UNEQUAL(unexpected_char, actual_char, "%c", char)

/**
 * Assert the string are equal.
 */
#define ASSERT_STRING_EQUAL(expected_string, actual_string) \
__FAIL_TEST_IF_TRUE(expected_string\
        , actual_string\
        , "%s"\
        , string\
        , expected_string\
        , actual_string\
        , strcmp(expected_string, actual_string) != 0\
        , "aren't equal")
/**
 * Assert the string are unequal.
 */
#define ASSERT_STRING_UNEQUAL(unexpected_string, actual_string) \
__FAIL_TEST_IF_TRUE(unexpected_string\
        , actual_string\
        , "%s"\
        , string\
        , unexpected_string\
        , actual_string\
        , strcmp(unexpected_string, actual_string) == 0\
        , "are equal")

/**
 * Run \p number_test functions from \p test_functions array,
 *
 * Return ::TEST_STATUS_FAILURE if any of the functions returned
 * ::TEST_STATUS_FAILURE.
 *
 * Return ::TEST_STATUS_SUCCESS if all functions
 * returned ::TEST_STATUS_SUCCESS.
 *
 * If a function doesn't return a value from ::test_status,
 * print an error message and return that value.
 */
extern inline int
run_tests(enum test_status (**test_functions)(), int number_tests)
{
    enum test_status tests_status = TEST_STATUS_SUCCESS;
    for (int i = 0; i < number_tests; i++)
    {
        enum test_status current_test_status = test_functions[i]();
        if (current_test_status == TEST_STATUS_FAILURE)
        {
            tests_status = TEST_STATUS_FAILURE;
        }
        else if (current_test_status != TEST_STATUS_SUCCESS)
        {
            printf(
                    ANSI_COLOR_RED "Error: invalid return value %d from "
                    "the test function at index %d\n. Test functions must "
                    "return either TEST_STATUS_SUCCESS (0) or "
                    "TEST_STATUS_FAILURE (1)."
                    , current_test_status, i
            );
            return current_test_status;
        }
    }
    return tests_status;
}



/**
 * \example test_ASSERT.h

 * \example test_ASSERT_BOOL_EQUAL.h
 * \example test_ASSERT_BOOL_UNEQUAL.h
 * \example test_ASSERT_CHAR_EQUAL.h
 * \example test_ASSERT_CHAR_UNEQUAL.h
 * \example test_ASSERT_INT_EQUAL.h
 * \example test_ASSERT_INT_UNEQUAL.h
 * \example test_ASSERT_NOT_NULL.h
 * \example test_ASSERT_NULL.h
 * \example test_ASSERT_STRING_EQUAL.h
 * \example test_ASSERT_STRING_UNEQUAL.h
 * \example test_ASSERT_VAR_EQUAL.h
 * \example test_ASSERT_VAR_UNEQUAL.h
 * \example tests.c
 */


#endif
