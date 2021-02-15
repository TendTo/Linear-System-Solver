#include <check.h>
#include "test_const.h"
#include "gaussian_elimination_no_pivot.h"

START_TEST(test_Gaussian_elimination_no_pivot_3_int)
{
    const int n = 3;
    double *x;
    x = Gaussian_elimination_no_pivot(A_3_int, b_3_int, n);
    ck_assert(compare_arr(x, expected_3_int, n));
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_3_comma)
{
    const int n = 3;
    double *x;
    x = Gaussian_elimination_no_pivot(A_3_comma, b_3_comma, n);
    ck_assert(compare_arr(x, expected_3_comma, n));
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_10)
{
    const int n = 10;
    double *x;
    x = Gaussian_elimination_no_pivot(U_10, NULL, n);
    ck_assert(compare_arr(x, expected_10, n));
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_1000)
{
    const int n = 1000;
    double *x;
    x = Gaussian_elimination_no_pivot(U_1000, NULL, n);
    ck_assert(compare_arr(x, expected_1000, n));
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_lmem_3)
{
    const int n = 3;
    double *x;
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_lmem(A_3_int, b_3_int, n, &status);
    ck_assert(compare_arr(x, expected_3_int, n));
    clFreeStatus(&status);
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_lmem_10)
{
    const int n = 10;
    double *x;
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_lmem(U_10, NULL, n, &status);
    ck_assert(compare_arr(x, expected_10, n));
    clFreeStatus(&status);
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_texture_3)
{
    const int n = 3;
    float *x;
    float U[] = {1.0f, 3.0f, -4.0f, 1.0f, 3.0f, 1.0f, 2.0f, -7.0f, 0.0f, -2.0f, 3.0f, 1.0f};
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_texture(U, NULL, n, &status);
    double *new_x = f_to_d_array(x, n);
    ck_assert(compare_arr(new_x, expected_3_int, n));
    clFreeStatus(&status);
    free(x);
    free(new_x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_texture_10)
{
    const int n = 10;
    float *x;
    float U[110];
    for (int i = 109; i >= 0; --i)
        U[i] = i * i - 2 * i - 1000 + i % 4 * 5 - i % 5 * i;
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_texture(U, NULL, n, &status);
    double *new_x = f_to_d_array(x, n);
    ck_assert(compare_arr(new_x, expected_10, n));
    clFreeStatus(&status);
    free(x);
    free(new_x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_texture_1000)
{
    const int n = 1000;
    float *x;
    float U[n * n + n];
    for (int i = 0; i < n * n + n; ++i)
    {
        U[i] = (rand() / (float)RAND_MAX - 0.5f) * 1000;
    }
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_texture(U, NULL, n, &status);
    double *new_x = f_to_d_array(x, n);
    ck_assert(compare_arr(new_x, expected_1000, n));
    free(x);
    free(new_x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_texture_vec_3)
{
    const int n = 3;
    float *x;
    float U[] = {1.0f, 3.0f, -4.0f, 1.0f, 3.0f, 1.0f, 2.0f, -7.0f, 0.0f, -2.0f, 3.0f, 1.0f};
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_texture_vec(U, NULL, n, &status);
    double *new_x = f_to_d_array(x, n);
    ck_assert(compare_arr(new_x, expected_3_int, n));
    clFreeStatus(&status);
    free(x);
    free(new_x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_buffer_3)
{
    const int n = 3;
    double *x;
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_buffer(U_3, NULL, n, &status);
    ck_assert(compare_arr(x, expected_3_int, n));
    clFreeStatus(&status);
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_buffer_10)
{
    const int n = 10;
    double *x;
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_buffer(U_10, NULL, n, &status);
    ck_assert(compare_arr(x, expected_10, n));
    clFreeStatus(&status);
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_buffer_1000)
{
    const int n = 1000;
    double *x;
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_buffer(U_1000, NULL, n, &status);
    ck_assert(compare_arr(x, expected_1000, n));
    free(x);
}
END_TEST

Suite *gaussian_elimination_no_pivot_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("gaussian_elimination_no_pivot");
    tc_core = tcase_create("core");

    srand(123);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_3_int);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_3_comma);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_10);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_1000);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_lmem_3);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_lmem_10);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_texture_3);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_texture_10);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_texture_1000);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_texture_vec_3);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_buffer_3);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_buffer_10);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_buffer_1000);

    suite_add_tcase(s, tc_core);

    return s;
}