#include <check.h>
#include "gaussian_elimination_no_pivot.h"

START_TEST(test_Gaussian_elimination_no_pivot_3_int)
{
    double *x;
    double A[] = {1.0, 3.0, -4.0, 3.0, 1.0, 2.0, 0.0, -2.0, 3.0};
    double b[] = {1.0, -7.0, 1.0};
    double expected[] = {6.0, -11.0, -7.0};
    x = Gaussian_elimination_no_pivot(A, b, 3);
    ck_assert(compare_arr(x, expected, 3));
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_3_comma)
{
    double *x;
    double A[] = {1.0, 3.0, -4.0, 3.0, 1.0, 2.0, 4.0, -2.0, 3.0};
    double b[] = {1.0, -7.0, 5.0};
    double expected[] = {1.454545, -4.636364, -3.363636};
    x = Gaussian_elimination_no_pivot(A, b, 3);
    ck_assert(compare_arr(x, expected, 3));
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_10)
{
    double *x;
    double U[110];
    for (int i = 109; i >= 0; --i)
        U[i] = i * i - 2 * i - 1000 + i % 4 * 5 - i % 5 * i;
    double expected[] = {1.89524, -1.09934, 1.89551, 0.48337, -4.31887, -0.45298, 1.11972, -1.76043, -0.56855, 3.83117};
    x = Gaussian_elimination_no_pivot(U, NULL, 10);
    ck_assert(compare_arr(x, expected, 10));
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_lmem_3)
{
    double *x;
    double A[] = {1.0, 3.0, -4.0, 3.0, 1.0, 2.0, 0.0, -2.0, 3.0};
    double b[] = {1.0, -7.0, 1.0};
    double expected[] = {6.0, -11.0, -7.0};
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_lmem(A, b, 3, &status);
    ck_assert(compare_arr(x, expected, 3));
    clFreeStatus(&status);
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_lmem_10)
{
    double *x;
    double U[110];
    for (int i = 109; i >= 0; --i)
        U[i] = i * i - 2 * i - 1000 + i % 4 * 5 - i % 5 * i;
    double expected[] = {1.89524, -1.09934, 1.89551, 0.48337, -4.31887, -0.45298, 1.11972, -1.76043, -0.56855, 3.83117};
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_lmem(U, NULL, 10, &status);
    ck_assert(compare_arr(x, expected, 10));
    clFreeStatus(&status);
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_texture_3)
{
    int n = 3;
    float *x;
    float A[] = {1.0f, 3.0f, -4.0f, 1.0f, 3.0f, 1.0f, 2.0f, -7.0f, 0.0f, -2.0f, 3.0f, 1.0f};
    double expected[] = {6.0, -11.0, -7.0};
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_texture(A, NULL, n, &status);
    double *new_x = f_to_d_array(x, n);
    ck_assert(compare_arr(new_x, expected, n));
    clFreeStatus(&status);
    free(x);
    free(new_x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_texture_10)
{
    int n = 10;
    float *x;
    float U[110];
    for (int i = 109; i >= 0; --i)
        U[i] = i * i - 2 * i - 1000 + i % 4 * 5 - i % 5 * i;
    double expected[] = {1.89524, -1.09934, 1.89551, 0.48337, -4.31887, -0.45298, 1.11972, -1.76043, -0.56855, 3.83117};
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_texture(U, NULL, n, &status);
    double *new_x = f_to_d_array(x, n);
    ck_assert(compare_arr(new_x, expected, n));
    clFreeStatus(&status);
    free(x);
    free(new_x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_buffer_3)
{
    double *x;
    double A[] = {1.0f, 3.0f, -4.0f, 1.0f, 3.0f, 1.0f, 2.0f, -7.0f, 0.0f, -2.0f, 3.0f, 1.0f};
    double expected[] = {6.0f, -11.0f, -7.0f};
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_buffer(A, NULL, 3, &status);
    ck_assert(compare_arr(x, expected, 3));
    clFreeStatus(&status);
    free(x);
}
END_TEST

START_TEST(test_Gaussian_elimination_no_pivot_gpu_buffer_10)
{
    double *x;
    double U[110];
    for (int i = 109; i >= 0; --i)
        U[i] = i * i - 2 * i - 1000 + i % 4 * 5 - i % 5 * i;
    double expected[] = {1.89524, -1.09934, 1.89551, 0.48337, -4.31887, -0.45298, 1.11972, -1.76043, -0.56855, 3.83117};
    cl_status status;
    clCreateStatus(&status, "src/ocl/gaussian_elimination_no_pivot.ocl");
    x = Gaussian_elimination_no_pivot_gpu_buffer(U, NULL, 10, &status);
    ck_assert(compare_arr(x, expected, 10));
    clFreeStatus(&status);
    free(x);
}
END_TEST

Suite *gaussian_elimination_no_pivot_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("gaussian_elimination_no_pivot");
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_3_int);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_3_comma);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_10);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_lmem_3);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_lmem_10);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_texture_3);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_texture_10);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_buffer_3);
    tcase_add_test(tc_core, test_Gaussian_elimination_no_pivot_gpu_buffer_10);

    suite_add_tcase(s, tc_core);

    return s;
}