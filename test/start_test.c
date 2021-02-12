#include <check.h>
#include "test_ocl_boiler.h"
#include "test_utility.h"
#include "test_gaussian_elimination_no_pivot.h"
#include "test_gaussian_elimination_pivot.h"

#define NSUITE 4

int main(void)
{
    Suite *(*suite_creators[NSUITE])(void) = {utility_suite, gaussian_elimination_no_pivot_suite, gaussian_elimination_pivot_suite, ocl_boiler_suite};

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