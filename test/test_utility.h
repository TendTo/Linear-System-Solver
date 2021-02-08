#include <check.h>
#include "utility.h"

START_TEST(test_create_complete_matrix_lin)
{
    double *U;
    double A[9] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    double b[3] = {10.0, 11.0, 12.0};
    double expected[12] = {1.0, 2.0, 3.0, 10.0, 4.0, 5.0, 6.0, 11.0, 7.0, 8.0, 9.0, 12.0};
    U = create_complete_matrix_lin(A, b, 3);
    ck_assert(compare_arr(U, expected, 12));
    free(U);
}
END_TEST

START_TEST(test_create_complete_matrix_lin_b_null)
{
    double *U;
    double A[12] = {1.0, 2.0, 3.0, 10.0, 4.0, 5.0, 6.0, 11.0, 7.0, 8.0, 9.0, 12.0};
    U = create_complete_matrix_lin(A, NULL, 3);
    ck_assert(compare_arr(U, A, 12));
    free(U);
}
END_TEST

START_TEST(test_swap)
{
    double A[2] = {1.0, 2.0};
    swap(&A[0], &A[1]);
    ck_assert(A[0] - 2.0 < 0.00001);
    ck_assert(A[0] - 2.0 > -0.00001);
    ck_assert(A[1] - 1.0 < 0.00001);
    ck_assert(A[1] - 1.0 > -0.00001);
}
END_TEST

START_TEST(test_read_matrix_from_file)
{
    double *U;
    double expected[12] = {1.0, 3.0, -4.0, 1.0, 3.0, 1.0, 2.0, -7.0, 0.0, -2.0, 3.0, 1.0};
    U = read_matrix_from_file("test/test_matrix.txt");
    ck_assert(compare_arr(U, expected, 12));
    free(U);
}
END_TEST

Suite *utility_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("utility");
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_create_complete_matrix_lin);
    tcase_add_test(tc_core, test_create_complete_matrix_lin_b_null);
    tcase_add_test(tc_core, test_swap);
    tcase_add_test(tc_core, test_read_matrix_from_file);

    suite_add_tcase(s, tc_core);

    return s;
}