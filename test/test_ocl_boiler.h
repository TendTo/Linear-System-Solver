#include <check.h>
#include "ocl_boiler.h"

START_TEST(test_select_platform)
{
    cl_platform_id p = select_platform();
    ck_assert(p);
}
END_TEST

START_TEST(test_select_device)
{
    cl_platform_id p = select_platform();
    ck_assert(p);
    cl_device_id d = select_device(p);
    ck_assert(d);
}
END_TEST

START_TEST(test_create_context)
{
    cl_platform_id p = select_platform();
    ck_assert(p);
    cl_device_id d = select_device(p);
    ck_assert(d);
    cl_context ctx = create_context(p, d);
    ck_assert(ctx);
}
END_TEST

START_TEST(test_create_queue)
{
    cl_platform_id p = select_platform();
    ck_assert(p);
    cl_device_id d = select_device(p);
    ck_assert(d);
    cl_context ctx = create_context(p, d);
    ck_assert(ctx);
    cl_command_queue que = create_queue(ctx, d);
    ck_assert(que);
}
END_TEST

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

Suite *ocl_boiler_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("ocl_boiler");
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_select_platform);
    tcase_add_test(tc_core, test_select_device);
    tcase_add_test(tc_core, test_create_context);
    tcase_add_test(tc_core, test_create_queue);
    tcase_add_test(tc_core, test_round_mul_up);

    suite_add_tcase(s, tc_core);

    return s;
}