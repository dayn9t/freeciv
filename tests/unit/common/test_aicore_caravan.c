/***********************************************************************
 Freeciv - Copyright (C) 2026 - The Freeciv Project
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <setjmp.h>
#include <cmocka.h>

#include "common/aicore/caravan.h"
#include "common/game.h"

/* Test setup and teardown */
static int setup_caravan(void **state)
{
    (void) state;
    game_init(false);
    return 0;
}

static int teardown_caravan(void **state)
{
    (void) state;
    game_free();
    return 0;
}

/* Test caravan_parameter_init_default */
static void test_caravan_parameter_init_default(void **state)
{
    (void) state;

    struct caravan_parameter param;
    caravan_parameter_init_default(&param);

    /* Check default values */
    assert_true(param.consider_windfall);
    assert_true(param.consider_trade);
    assert_true(param.consider_wonders);
    assert_true(param.account_for_broken_routes);
    assert_false(param.ignore_transit_time);
    assert_false(param.convert_trade);
}

/* Test caravan_parameter_is_legal with valid parameters */
static void test_caravan_parameter_is_legal_valid(void **state)
{
    (void) state;

    struct caravan_parameter param;
    caravan_parameter_init_default(&param);

    /* Default parameters should be legal */
    assert_true(caravan_parameter_is_legal(&param));

    /* Test with discount 1.0 and finite horizon */
    param.discount = 1.0;
    param.horizon = 100;
    assert_true(caravan_parameter_is_legal(&param));

    /* Test with discount 0.5 */
    param.discount = 0.5;
    param.horizon = 50;
    assert_true(caravan_parameter_is_legal(&param));
}

/* Test caravan_parameter_is_legal with invalid parameters */
static void test_caravan_parameter_is_legal_invalid(void **state)
{
    (void) state;

    struct caravan_parameter param;
    caravan_parameter_init_default(&param);

    /* Test with discount > 1.0 */
    param.discount = 1.5;
    assert_false(caravan_parameter_is_legal(&param));

    /* Test with discount < 0.0 */
    param.discount = -0.5;
    assert_false(caravan_parameter_is_legal(&param));

    /* Test with discount == 1.0 and infinite horizon */
    param.discount = 1.0;
    /* param.horizon is already FC_INFINITY */
    assert_false(caravan_parameter_is_legal(&param));
}

/* Test caravan_result_init_zero */
static void test_caravan_result_init_zero(void **state)
{
    (void) state;

    struct caravan_result result;
    caravan_result_init_zero(&result);

    assert_null(result.src);
    assert_null(result.dest);
    assert_int_equal(result.arrival_time, 0);
    assert_false(result.help_wonder);
    assert_false(result.required_boat);
}

/* Test caravan_result_compare */
static void test_caravan_result_compare(void **state)
{
    (void) state;

    struct caravan_result a, b;
    caravan_result_init_zero(&a);
    caravan_result_init_zero(&b);

    /* Equal values */
    a.value = 100.0;
    b.value = 100.0;
    a.arrival_time = 10;
    b.arrival_time = 10;
    assert_int_equal(caravan_result_compare(&a, &b), 0);

    /* a has higher value */
    a.value = 150.0;
    assert_true(caravan_result_compare(&a, &b) > 0);

    /* b has higher value */
    a.value = 50.0;
    assert_true(caravan_result_compare(&a, &b) < 0);
}

/* Test caravan_result_compare with different times */
static void test_caravan_result_compare_different_times(void **state)
{
    (void) state;

    struct caravan_result a, b;
    caravan_result_init_zero(&a);
    caravan_result_init_zero(&b);

    a.value = 100.0;
    b.value = 100.0;
    a.arrival_time = 5;
    b.arrival_time = 10;

    /* Faster (smaller time) should be better */
    int cmp = caravan_result_compare(&a, &b);
    assert_true(cmp >= 0);
}

/* Test caravan_parameter_init_from_unit */
static void test_caravan_parameter_init_from_unit(void **state)
{
    (void) state;

    struct caravan_parameter param_default, param_from_unit;

    /* Initialize default parameters */
    caravan_parameter_init_default(&param_default);

    /* Default values should have all considerations enabled */
    assert_true(param_default.consider_windfall);
    assert_true(param_default.consider_trade);
    assert_true(param_default.consider_wonders);

    /* Initialize from unit - just verify it doesn't crash */
    /* Note: Actual behavior depends on unit capabilities */
    caravan_parameter_init_from_unit(&param_from_unit, NULL);

    /* Should still have valid parameters */
    assert_true(param_from_unit.horizon == FC_INFINITY);
    assert_true(param_from_unit.discount == 0.95);
}

/* Test caravan_parameter_log doesn't crash */
static void test_caravan_parameter_log(void **state)
{
    (void) state;

    struct caravan_parameter param;
    caravan_parameter_init_default(&param);

    /* Just verify the logging function doesn't crash */
    caravan_parameter_log(&param, LOG_DEBUG);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_caravan_parameter_init_default,
                                        setup_caravan, teardown_caravan),
        cmocka_unit_test_setup_teardown(test_caravan_parameter_is_legal_valid,
                                        setup_caravan, teardown_caravan),
        cmocka_unit_test_setup_teardown(test_caravan_parameter_is_legal_invalid,
                                        setup_caravan, teardown_caravan),
        cmocka_unit_test(test_caravan_result_init_zero),
        cmocka_unit_test(test_caravan_result_compare),
        cmocka_unit_test(test_caravan_result_compare_different_times),
        cmocka_unit_test_setup_teardown(test_caravan_parameter_init_from_unit,
                                        setup_caravan, teardown_caravan),
        cmocka_unit_test_setup_teardown(test_caravan_parameter_log,
                                        setup_caravan, teardown_caravan),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
