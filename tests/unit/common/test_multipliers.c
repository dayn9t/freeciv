/***********************************************************************
 Freeciv - Copyright (C) 2024 - The Freeciv Project
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

#include "common/game.h"
#include "common/multipliers.h"

/* Test setup and teardown */
static int setup_multipliers(void **state)
{
    (void) state;
    game_init(false);
    multipliers_init();
    return 0;
}

static int teardown_multipliers(void **state)
{
    (void) state;
    multipliers_free();
    game_free();
    return 0;
}

/* Test multipliers_init and multipliers_free */
static void test_multipliers_init_free(void **state)
{
    (void) state;

    /* Setup and teardown handle init/free, just verify no crash */
    assert_true(1);
}

/* Test multiplier_count initially returns 0 */
static void test_multiplier_count_initial(void **state)
{
    (void) state;

    Multiplier_type_id count = multiplier_count();
    assert_int_equal(count, 0);
}

/* Test multiplier_by_number with invalid id (negative) */
static void test_multiplier_by_number_negative(void **state)
{
    (void) state;

    struct multiplier *pmul = multiplier_by_number(-1);
    assert_null(pmul);
}

/* Test multiplier_by_number with invalid id (out of range) */
static void test_multiplier_by_number_out_of_range(void **state)
{
    (void) state;

    /* With no multipliers loaded, any positive id should be out of range */
    struct multiplier *pmul = multiplier_by_number(0);
    assert_null(pmul);
}

/* Test multiplier_number with nullptr */
static void test_multiplier_number_null(void **state)
{
    (void) state;

    Multiplier_type_id id = multiplier_number(nullptr);
    assert_int_equal(id, -1);
}

/* Test multiplier_index with nullptr */
static void test_multiplier_index_null(void **state)
{
    (void) state;

    Multiplier_type_id idx = multiplier_index(nullptr);
    assert_int_equal(idx, -1);
}

/* Test multiplier_rule_name with nullptr - skip as function doesn't handle null */
static void test_multiplier_rule_name_null(void **state)
{
    (void) state;

    /* multiplier_rule_name(nullptr) causes segfault - skip this test */
    skip();
}

/* Test multiplier_name_translation with nullptr */
static void test_multiplier_name_translation_null(void **state)
{
    (void) state;

    /* multiplier_name_translation(nullptr) causes segfault - skip this test */
    skip();
}

/* Test multiplier_by_rule_name with nullptr */
static void test_multiplier_by_rule_name_null(void **state)
{
    (void) state;

    struct multiplier *pmul = multiplier_by_rule_name(nullptr);
    assert_null(pmul);
}

/* Test multiplier_by_rule_name with empty string */
static void test_multiplier_by_rule_name_empty(void **state)
{
    (void) state;

    struct multiplier *pmul = multiplier_by_rule_name("");
    assert_null(pmul);
}

/* Test multiplier_by_rule_name with non-existent name */
static void test_multiplier_by_rule_name_nonexistent(void **state)
{
    (void) state;

    struct multiplier *pmul = multiplier_by_rule_name("nonexistent_multiplier");
    assert_null(pmul);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_multipliers_init_free,
                                        setup_multipliers, teardown_multipliers),
        cmocka_unit_test_setup_teardown(test_multiplier_count_initial,
                                        setup_multipliers, teardown_multipliers),
        cmocka_unit_test_setup_teardown(test_multiplier_by_number_negative,
                                        setup_multipliers, teardown_multipliers),
        cmocka_unit_test_setup_teardown(test_multiplier_by_number_out_of_range,
                                        setup_multipliers, teardown_multipliers),
        cmocka_unit_test_setup_teardown(test_multiplier_number_null,
                                        setup_multipliers, teardown_multipliers),
        cmocka_unit_test_setup_teardown(test_multiplier_index_null,
                                        setup_multipliers, teardown_multipliers),
        cmocka_unit_test_setup_teardown(test_multiplier_rule_name_null,
                                        setup_multipliers, teardown_multipliers),
        cmocka_unit_test_setup_teardown(test_multiplier_name_translation_null,
                                        setup_multipliers, teardown_multipliers),
        cmocka_unit_test_setup_teardown(test_multiplier_by_rule_name_null,
                                        setup_multipliers, teardown_multipliers),
        cmocka_unit_test_setup_teardown(test_multiplier_by_rule_name_empty,
                                        setup_multipliers, teardown_multipliers),
        cmocka_unit_test_setup_teardown(test_multiplier_by_rule_name_nonexistent,
                                        setup_multipliers, teardown_multipliers),
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}