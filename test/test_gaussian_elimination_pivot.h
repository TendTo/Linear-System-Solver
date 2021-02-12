#include <check.h>
#include "test_const.h"
#include "gaussian_elimination_pivot.h"

START_TEST(test_Gaussian_elimination_pivot_3_int)
{
    const int n = 3;
    double *x;
    x = Gaussian_elimination_pivot(A_3_int, b_3_int, n);
    ck_assert(compare_arr(x, expected_3_int, n));
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_pivot_3_comma)
{
    const int n = 3;
    double *x;
    x = Gaussian_elimination_pivot(A_3_comma, b_3_comma, n);
    ck_assert(compare_arr(x, expected_3_comma, n));
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_pivot_5)
{
    const int n = 5;
    double *x;
    x = Gaussian_elimination_pivot(A_5, b_5, n);
    ck_assert(compare_arr(x, expected_5, n));
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_pivot_10)
{
    const int n = 10;
    double *x;
    x = Gaussian_elimination_pivot(U_10, NULL, n);
    ck_assert(compare_arr(x, expected_10, n));
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_pivot_1000)
{
    const int n = 1000;
    double *x;
    x = Gaussian_elimination_pivot(U_1000, NULL, n);
    ck_assert(compare_arr(x, expected_1000, n));
    free(x);
}
END_TEST

Suite *gaussian_elimination_pivot_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("gaussian_elimination_pivot");
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_Gaussian_elimination_pivot_3_int);
    tcase_add_test(tc_core, test_Gaussian_elimination_pivot_3_comma);
    tcase_add_test(tc_core, test_Gaussian_elimination_pivot_5);
    tcase_add_test(tc_core, test_Gaussian_elimination_pivot_10);
    tcase_add_test(tc_core, test_Gaussian_elimination_pivot_1000);

    suite_add_tcase(s, tc_core);

    return s;
}