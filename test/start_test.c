#include <check.h>
#include "test_ocl_boiler.h"
#include "test_utility.h"
#include "test_gaussian_reduction.h"

#define NSUITE 2

int main(void)
{
    Suite *(*suite_creators[NSUITE])(void) = {utility_suite, gaussian_reduction_suite};

    int no_failed = 0;
    Suite *s;
    SRunner *runner;

    for (size_t i = 0; i < NSUITE; ++i)
    {
        s = suite_creators[i]();
        runner = srunner_create(s);

        srunner_run_all(runner, CK_NORMAL);
        no_failed += srunner_ntests_failed(runner);
        srunner_free(runner);
    }

    return (no_failed == 0) ? 0 : 1;
}