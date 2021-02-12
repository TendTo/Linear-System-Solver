#include <check.h>
#include "test_const.h"
#include "test_ocl_boiler.h"
#include "test_utility.h"
#include "test_gaussian_elimination_no_pivot.h"
#include "test_gaussian_elimination_pivot.h"

#define NSUITE 4

void initialize_consts()
{
    srand(123);
    for (int i = 109; i >= 0; --i)
        U_10[i] = i * i - 2 * i - 1000 + i % 4 * 5 - i % 5 * i;
    for (size_t i = 0; i < 1001000; ++i)
        U_1000[i] = (rand() / (double)RAND_MAX - 0.5) * 1000;
}

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