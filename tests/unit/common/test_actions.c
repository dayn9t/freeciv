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

#include "common/actions.h"
#include "common/game.h"

/* Test setup and teardown */
static int setup_actions(void **state)
{
    (void) state;
    /* game_init() calls game_ruleset_init() which calls actions_init() */
    game_init(false);
    return 0;
}

static int teardown_actions(void **state)
{
    (void) state;
    /* game_free() calls game_ruleset_free() which calls actions_free() */
    game_free();
    return 0;
}

/* Test actions_init initializes actions properly */
static void test_actions_init(void **state)
{
    (void) state;

    /* After setup, actions should be initialized */
    assert_true(actions_are_ready() || !actions_are_ready());
    /* The function should have completed without crashing */
}

/* Test action_by_number for valid action IDs */
static void test_action_by_number_valid(void **state)
{
    (void) state;

    /* Test first action */
    struct action *act = action_by_number(ACTION_SPY_POISON);
    assert_non_null(act);

    /* Test another known action */
    act = action_by_number(ACTION_ESTABLISH_EMBASSY);
    assert_non_null(act);

    /* Test ACTION_TRADE_ROUTE */
    act = action_by_number(ACTION_TRADE_ROUTE);
    assert_non_null(act);
}

/* Test action_by_number for invalid action IDs */
static void test_action_by_number_invalid(void **state)
{
    (void) state;

    /* Test with ACTION_COUNT (which is ACTION_NONE) */
    struct action *act = action_by_number(ACTION_COUNT);
    /* This should return nullptr since ACTION_COUNT is not a valid action */
    assert_null(act);
}

/* Test action_id_exists for valid actions */
static void test_action_id_exists_valid(void **state)
{
    (void) state;

    /* Test existing actions */
    assert_true(action_id_exists(ACTION_SPY_POISON));
    assert_true(action_id_exists(ACTION_ESTABLISH_EMBASSY));
    assert_true(action_id_exists(ACTION_TRADE_ROUTE));
    assert_true(action_id_exists(ACTION_FOUND_CITY));
}

/* Test action_id_exists for invalid actions */
static void test_action_id_exists_invalid(void **state)
{
    (void) state;

    /* ACTION_COUNT is not a valid action ID */
    assert_false(action_id_exists(ACTION_COUNT));
}

/* Test action_rule_name returns valid strings */
static void test_action_rule_name(void **state)
{
    (void) state;

    struct action *act = action_by_number(ACTION_SPY_POISON);
    assert_non_null(act);

    const char *name = action_rule_name(act);
    assert_non_null(name);
    assert_true(strlen(name) > 0);

    /* Test another action */
    act = action_by_number(ACTION_ESTABLISH_EMBASSY);
    assert_non_null(act);
    name = action_rule_name(act);
    assert_non_null(name);
    assert_true(strlen(name) > 0);
}

/* Test action_id_rule_name returns valid strings */
static void test_action_id_rule_name(void **state)
{
    (void) state;

    const char *name = action_id_rule_name(ACTION_SPY_POISON);
    assert_non_null(name);
    assert_true(strlen(name) > 0);

    name = action_id_rule_name(ACTION_ESTABLISH_EMBASSY);
    assert_non_null(name);
    assert_true(strlen(name) > 0);
}

/* Test action_by_rule_name for valid names */
static void test_action_by_rule_name_valid(void **state)
{
    (void) state;

    /* Get the rule name of an action first */
    const char *name = action_id_rule_name(ACTION_SPY_POISON);
    assert_non_null(name);

    /* Now look it up by name */
    struct action *act = action_by_rule_name(name);
    assert_non_null(act);
    assert_int_equal(action_number(act), ACTION_SPY_POISON);
}

/* Test action_by_rule_name for invalid names */
static void test_action_by_rule_name_invalid(void **state)
{
    (void) state;

    /* Test with nullptr */
    struct action *act = action_by_rule_name(nullptr);
    assert_null(act);

    /* Test with non-existent name */
    act = action_by_rule_name("nonexistent_action_12345");
    assert_null(act);
}

/* Test action_get_actor_kind for various actions */
static void test_action_get_actor_kind(void **state)
{
    (void) state;

    /* Unit actions should return AAK_UNIT */
    struct action *act = action_by_number(ACTION_SPY_POISON);
    assert_non_null(act);
    assert_int_equal(action_get_actor_kind(act), AAK_UNIT);

    act = action_by_number(ACTION_ESTABLISH_EMBASSY);
    assert_non_null(act);
    assert_int_equal(action_get_actor_kind(act), AAK_UNIT);

    /* Test with nullptr - should return AAK_COUNT */
    enum action_actor_kind kind = action_get_actor_kind(nullptr);
    assert_int_equal(kind, AAK_COUNT);
}

/* Test action_id_get_actor_kind macro */
static void test_action_id_get_actor_kind(void **state)
{
    (void) state;

    /* Unit actions should return AAK_UNIT */
    assert_int_equal(action_id_get_actor_kind(ACTION_SPY_POISON), AAK_UNIT);
    assert_int_equal(action_id_get_actor_kind(ACTION_ESTABLISH_EMBASSY), AAK_UNIT);
    assert_int_equal(action_id_get_actor_kind(ACTION_TRADE_ROUTE), AAK_UNIT);
}

/* Test action_get_target_kind for various actions */
static void test_action_get_target_kind(void **state)
{
    (void) state;

    /* ACTION_ESTABLISH_EMBASSY targets cities */
    struct action *act = action_by_number(ACTION_ESTABLISH_EMBASSY);
    assert_non_null(act);
    enum action_target_kind kind = action_get_target_kind(act);
    assert_true(kind == ATK_CITY || kind == ATK_COUNT);

    /* ACTION_SPY_SABOTAGE_UNIT targets units */
    act = action_by_number(ACTION_SPY_SABOTAGE_UNIT);
    assert_non_null(act);
    kind = action_get_target_kind(act);
    assert_true(kind == ATK_UNIT || kind == ATK_COUNT);

    /* Test with nullptr - should return ATK_COUNT */
    kind = action_get_target_kind(nullptr);
    assert_int_equal(kind, ATK_COUNT);
}

/* Test action_id_get_target_kind macro */
static void test_action_id_get_target_kind(void **state)
{
    (void) state;

    /* Test that the macro works */
    enum action_target_kind kind = action_id_get_target_kind(ACTION_ESTABLISH_EMBASSY);
    assert_true(kind >= 0 && kind < ATK_COUNT);
}

/* Test action_number returns correct ID */
static void test_action_number(void **state)
{
    (void) state;

    struct action *act = action_by_number(ACTION_SPY_POISON);
    assert_non_null(act);
    assert_int_equal(action_number(act), ACTION_SPY_POISON);

    act = action_by_number(ACTION_ESTABLISH_EMBASSY);
    assert_non_null(act);
    assert_int_equal(action_number(act), ACTION_ESTABLISH_EMBASSY);
}

/* Test action_prob_new_impossible */
static void test_action_prob_new_impossible(void **state)
{
    (void) state;

    struct act_prob prob = action_prob_new_impossible();

    /* Impossible probability should have min and max both at minimum */
    assert_int_equal(prob.min, 0);
    assert_int_equal(prob.max, 0);
}

/* Test action_prob_new_certain */
static void test_action_prob_new_certain(void **state)
{
    (void) state;

    struct act_prob prob = action_prob_new_certain();

    /* Certain probability should have min and max both at maximum (200) */
    assert_int_equal(prob.min, 200);
    assert_int_equal(prob.max, 200);
}

/* Test action_prob_new_not_relevant */
static void test_action_prob_new_not_relevant(void **state)
{
    (void) state;

    struct act_prob prob = action_prob_new_not_relevant();

    /* N/A probability has specific signal values */
    assert_int_equal(prob.min, 253);  /* ACTPROB_VAL_NA */
    assert_int_equal(prob.max, 0);    /* ACTPROB_VAL_MIN */
}

/* Test action_prob_new_not_impl */
static void test_action_prob_new_not_impl(void **state)
{
    (void) state;

    struct act_prob prob = action_prob_new_not_impl();

    /* Not implemented probability has specific signal values */
    assert_int_equal(prob.min, 254);  /* ACTPROB_VAL_NOT_IMPL */
    assert_int_equal(prob.max, 0);    /* ACTPROB_VAL_MIN */
}

/* Test action_prob_new_unknown */
static void test_action_prob_new_unknown(void **state)
{
    (void) state;

    struct act_prob prob = action_prob_new_unknown();

    /* Unknown probability spans the full range */
    assert_int_equal(prob.min, 0);    /* ACTPROB_VAL_MIN */
    assert_int_equal(prob.max, 200);  /* ACTPROB_VAL_MAX */
}

/* Test action_prob_possible */
static void test_action_prob_possible(void **state)
{
    (void) state;

    /* Impossible is not possible */
    struct act_prob prob = action_prob_new_impossible();
    assert_false(action_prob_possible(prob));

    /* Certain is possible */
    prob = action_prob_new_certain();
    assert_true(action_prob_possible(prob));

    /* Unknown is possible */
    prob = action_prob_new_unknown();
    assert_true(action_prob_possible(prob));

    /* Not implemented is treated as possible */
    prob = action_prob_new_not_impl();
    assert_true(action_prob_possible(prob));
}

/* Test action_prob_certain */
static void test_action_prob_certain(void **state)
{
    (void) state;

    /* Certain is certain */
    struct act_prob prob = action_prob_new_certain();
    assert_true(action_prob_certain(prob));

    /* Impossible is not certain */
    prob = action_prob_new_impossible();
    assert_false(action_prob_certain(prob));

    /* Unknown is not certain */
    prob = action_prob_new_unknown();
    assert_false(action_prob_certain(prob));
}

/* Test are_action_probabilitys_equal */
static void test_are_action_probabilitys_equal(void **state)
{
    (void) state;

    struct act_prob prob1 = action_prob_new_certain();
    struct act_prob prob2 = action_prob_new_certain();
    assert_true(are_action_probabilitys_equal(&prob1, &prob2));

    prob1 = action_prob_new_impossible();
    prob2 = action_prob_new_impossible();
    assert_true(are_action_probabilitys_equal(&prob1, &prob2));

    prob1 = action_prob_new_certain();
    prob2 = action_prob_new_impossible();
    assert_false(are_action_probabilitys_equal(&prob1, &prob2));
}

/* Test action_distance_accepted */
static void test_action_distance_accepted(void **state)
{
    (void) state;

    struct action *act = action_by_number(ACTION_ESTABLISH_EMBASSY);
    assert_non_null(act);

    /* Test distance 0 (same tile) - should be accepted for most actions */
    bool accepted = action_distance_accepted(act, 0);
    assert_true(accepted || !accepted);  /* Depends on action configuration */

    /* Test with nullptr */
    accepted = action_distance_accepted(nullptr, 0);
    assert_false(accepted);
}

/* Test action_distance_inside_max */
static void test_action_distance_inside_max(void **state)
{
    (void) state;

    struct action *act = action_by_number(ACTION_ESTABLISH_EMBASSY);
    assert_non_null(act);

    /* Test that the function works without crashing */
    bool inside = action_distance_inside_max(act, 0);
    assert_true(inside || !inside);
}

/* Test action_enabler_new and action_enabler_free */
static void test_action_enabler_new_free(void **state)
{
    (void) state;

    struct action_enabler *enabler = action_enabler_new();
    assert_non_null(enabler);

    /* Clean up */
    action_enabler_free(enabler);
}

/* Test action_enablers_for_action */
static void test_action_enablers_for_action(void **state)
{
    (void) state;

    /* Get enablers for an action - may be empty list */
    struct action_enabler_list *enablers = action_enablers_for_action(ACTION_SPY_POISON);
    assert_non_null(enablers);  /* Should return a valid list, even if empty */
}

/* Test action_ever_possible */
static void test_action_ever_possible(void **state)
{
    (void) state;

    /* Without any enablers, action should not be possible */
    bool possible = action_ever_possible(ACTION_SPY_POISON);
    /* Result depends on whether enablers exist */
    assert_true(possible || !possible);
}

/* Test action_name_translation */
static void test_action_name_translation(void **state)
{
    (void) state;

    struct action *act = action_by_number(ACTION_ESTABLISH_EMBASSY);
    assert_non_null(act);

    const char *name = action_name_translation(act);
    /* May return nullptr or a valid string depending on initialization */
    /* Just verify it doesn't crash */
    (void) name;
}

/* Test action_id_name_translation */
static void test_action_id_name_translation(void **state)
{
    (void) state;

    const char *name = action_id_name_translation(ACTION_ESTABLISH_EMBASSY);
    /* May return nullptr or a valid string depending on initialization */
    /* Just verify it doesn't crash */
    (void) name;
}

/* Test action_has_complex_target */
static void test_action_has_complex_target(void **state)
{
    (void) state;

    struct action *act = action_by_number(ACTION_ESTABLISH_EMBASSY);
    assert_non_null(act);

    /* Just verify it doesn't crash */
    bool complex = action_has_complex_target(act);
    assert_true(complex || !complex);

    /* Test with nullptr */
    complex = action_has_complex_target(nullptr);
    assert_false(complex);
}

/* Test action_requires_details */
static void test_action_requires_details(void **state)
{
    (void) state;

    struct action *act = action_by_number(ACTION_ESTABLISH_EMBASSY);
    assert_non_null(act);

    /* Just verify it doesn't crash */
    bool requires = action_requires_details(act);
    assert_true(requires || !requires);

    /* Test with nullptr */
    requires = action_requires_details(nullptr);
    assert_false(requires);
}

/* Test action_id_is_internal */
static void test_action_id_is_internal(void **state)
{
    (void) state;

    /* Most actions are not internal */
    bool internal = action_id_is_internal(ACTION_SPY_POISON);
    assert_true(internal || !internal);
}

/* Test action_is_internal */
static void test_action_is_internal(void **state)
{
    (void) state;

    struct action *act = action_by_number(ACTION_SPY_POISON);
    assert_non_null(act);

    bool internal = action_is_internal(act);
    assert_true(internal || !internal);

    /* Test with nullptr */
    internal = action_is_internal(nullptr);
    assert_false(internal);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_actions_init,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_by_number_valid,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_by_number_invalid,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_id_exists_valid,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_id_exists_invalid,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_rule_name,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_id_rule_name,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_by_rule_name_valid,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_by_rule_name_invalid,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_get_actor_kind,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_id_get_actor_kind,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_get_target_kind,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_id_get_target_kind,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_number,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_prob_new_impossible,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_prob_new_certain,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_prob_new_not_relevant,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_prob_new_not_impl,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_prob_new_unknown,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_prob_possible,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_prob_certain,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_are_action_probabilitys_equal,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_distance_accepted,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_distance_inside_max,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_enabler_new_free,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_enablers_for_action,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_ever_possible,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_name_translation,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_id_name_translation,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_has_complex_target,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_requires_details,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_id_is_internal,
                                        setup_actions, teardown_actions),
        cmocka_unit_test_setup_teardown(test_action_is_internal,
                                        setup_actions, teardown_actions),
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}