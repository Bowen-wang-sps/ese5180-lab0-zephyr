#include <zephyr/ztest.h>
#include "sum_log.h"
ZTEST(sum_log_test_suite, test_sum_log_basic)
{
 // TO DO
 //call sum_log(...) and zassert_equal(...)/zasser_true(....)
    int result = sum_log(2, 3);

    zassert_equal(result, 5);
}
ZTEST(sum_log_test_suite, test_sum_log_negative)
{
 //TO DO
    int result = sum_log(-2, -3);

    zassert_equal(result, -5);
}
ZTEST(sum_log_test_suite, test_sum_log_zero)
{
 // TO DO
    int result = sum_log(0, 0);

    zassert_equal(result, 0);
}
ZTEST_SUITE(sum_log_test_suite, NULL, NULL, NULL, NULL, NULL);