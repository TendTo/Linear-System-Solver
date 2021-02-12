#include <check.h>
#include "ocl_boiler.h"
#include "utility.h"

START_TEST(test_round_mul_up)
{
    size_t result;
    result = round_mul_up(1, 10);
    ck_assert_int_eq(result, 10);
    result = round_mul_up(10, 10);
    ck_assert_int_eq(result, 10);
    result = round_mul_up(12, 10);
    ck_assert_int_eq(result, 20);
    result = round_mul_up(11, 9);
    ck_assert_int_eq(result, 18);
}
END_TEST

START_TEST(test_simple_cl_application)
{
    double expected[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0};
    cl_status status;
    cl_int rows = 3, cols = 4;
    size_t memsize = sizeof(double) * rows * cols;

    clCreateStatus(&status, "test/test_kernel.ocl");
    ck_assert(status.prog);

    cl_int err;
    cl_kernel init_mat_k = clCreateKernel(status.prog, "init_mat", &err);
    ocl_check(err, "create Kernel");
    ck_assert_int_eq(err, CL_SUCCESS);

    cl_mem d_mat = clCreateBuffer(status.ctx, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                  memsize, NULL, &err);

    size_t gws[] = {cols, rows};

    err = clSetKernelArg(init_mat_k, 0, sizeof(cl_int), &rows);
    ocl_check(err, "set arg 0 for init_mat_k");
    ck_assert_int_eq(err, CL_SUCCESS);
    err = clSetKernelArg(init_mat_k, 1, sizeof(cl_int), &cols);
    ocl_check(err, "set arg 1 for init_mat_k");
    ck_assert_int_eq(err, CL_SUCCESS);
    err = clSetKernelArg(init_mat_k, 2, sizeof(d_mat), &d_mat);
    ocl_check(err, "set arg 2 for init_mat_k");
    ck_assert_int_eq(err, CL_SUCCESS);

    cl_event init_evt, read_evt;
    err = clEnqueueNDRangeKernel(status.que, init_mat_k,
                                 2, NULL, gws, NULL,
                                 0, NULL, &init_evt);

    ocl_check(err, "enqueue init_mat");
    ck_assert_int_eq(err, CL_SUCCESS);

    double *h_mat = clEnqueueMapBuffer(status.que, d_mat, CL_TRUE,
                                       CL_MAP_READ, 0, memsize,
                                       1, &init_evt, &read_evt, &err);
    ocl_check(err, "enqueue read_mat");
    ck_assert_int_eq(err, CL_SUCCESS);

    ck_assert(compare_arr(h_mat, expected, rows * cols));

    clFreeStatus(&status);
    ck_assert(1);
}
END_TEST

Suite *ocl_boiler_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("ocl_boiler");
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_round_mul_up);
    tcase_add_test(tc_core, test_simple_cl_application);

    suite_add_tcase(s, tc_core);

    return s;
}