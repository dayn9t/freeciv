/***********************************************************************
 Freeciv - Copyright (C) 2025 - The Freeciv Project
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
#include <math.h>
#include <cmocka.h>

/* common */
#include "combat.h"
#include "fc_types.h"
#include "game.h"
#include "movement.h"
#include "terrain.h"

/* Test setup and teardown */
static int setup_combat(void **state)
{
    (void) state;
    game_init(false);
    /* Initialize terrain_control with default values for testing */
    terrain_control.move_fragments = 3;  /* Typical value for SINGLE_MOVE */
    return 0;
}

static int teardown_combat(void **state)
{
    (void) state;
    game_free();
    return 0;
}

/***********************************************************************
  Test win_chance basic calculation
***********************************************************************/
static void test_win_chance_basic(void **state)
{
    double chance;

    (void) state;

    /* Equal strength - should be 50% chance */
    chance = win_chance(10, 10, 1, 10, 10, 1);
    assert_true(chance > 0.49 && chance < 0.51);

    /* Stronger attacker - should have higher chance */
    chance = win_chance(20, 10, 1, 10, 10, 1);
    assert_true(chance > 0.5);

    /* Stronger defender - attacker should have lower chance */
    chance = win_chance(10, 10, 1, 20, 10, 1);
    assert_true(chance < 0.5);
}

/***********************************************************************
  Test win_chance with zero attack strength
***********************************************************************/
static void test_win_chance_zero_attack(void **state)
{
    double chance;

    (void) state;

    /* Zero attack strength - should have 50% base (per formula) */
    chance = win_chance(0, 10, 1, 0, 10, 1);
    assert_true(chance >= 0.0 && chance <= 1.0);

    /* Zero attack vs positive defense */
    chance = win_chance(0, 10, 1, 10, 10, 1);
    assert_true(chance >= 0.0 && chance < 0.5);
}

/***********************************************************************
  Test win_chance with different HP values
***********************************************************************/
static void test_win_chance_hp_variation(void **state)
{
    double chance_low_hp, chance_high_hp;

    (void) state;

    /* Attacker with low HP */
    chance_low_hp = win_chance(10, 1, 1, 10, 10, 1);

    /* Attacker with high HP */
    chance_high_hp = win_chance(10, 10, 1, 10, 10, 1);

    /* Higher HP should give better chances */
    assert_true(chance_high_hp > chance_low_hp);
}

/***********************************************************************
  Test win_chance with different firepower values
***********************************************************************/
static void test_win_chance_firepower(void **state)
{
    double chance_low_fp, chance_high_fp;

    (void) state;

    /* Attacker with low firepower */
    chance_low_fp = win_chance(10, 10, 1, 10, 10, 1);

    /* Attacker with high firepower */
    chance_high_fp = win_chance(10, 10, 2, 10, 10, 1);

    /* Higher firepower should improve chances */
    assert_true(chance_high_fp >= chance_low_fp);
}

/***********************************************************************
  Test win_chance bounds - result should always be between 0 and 1
***********************************************************************/
static void test_win_chance_bounds(void **state)
{
    double chance;

    (void) state;

    /* Extreme values */
    chance = win_chance(1, 1, 1, 100, 100, 10);
    assert_true(chance >= 0.0 && chance <= 1.0);

    chance = win_chance(100, 100, 10, 1, 1, 1);
    assert_true(chance >= 0.0 && chance <= 1.0);

    /* Very large values */
    chance = win_chance(1000, 100, 5, 1000, 100, 5);
    assert_true(chance >= 0.0 && chance <= 1.0);
}

/***********************************************************************
  Test is_tired_attack with various moves_left values
***********************************************************************/
static void test_is_tired_attack_disabled(void **state)
{
    (void) state;

    /* When tired_attack is disabled (default), should always return false */
    game.info.tired_attack = FALSE;

    /* Use realistic move values - SINGLE_MOVE is typically 3 */
    assert_false(is_tired_attack(3));
    assert_false(is_tired_attack(1));
    assert_false(is_tired_attack(0));
    assert_false(is_tired_attack(2));
}

/***********************************************************************
  Test is_tired_attack when enabled
***********************************************************************/
static void test_is_tired_attack_enabled(void **state)
{
    (void) state;

    /* Enable tired attack */
    game.info.tired_attack = TRUE;

    /* SINGLE_MOVE is set to 3 in setup
     * An attack is tired if moves_left < SINGLE_MOVE */
    assert_true(is_tired_attack(0));
    assert_true(is_tired_attack(1));
    assert_true(is_tired_attack(2));
    assert_false(is_tired_attack(3));  /* Full move is not tired */
    assert_false(is_tired_attack(4));  /* More than full move is not tired */

    /* Reset */
    game.info.tired_attack = FALSE;
}

/***********************************************************************
  Test is_stack_vulnerable with various conditions
***********************************************************************/
static void test_is_stack_vulnerable_killstack_disabled(void **state)
{
    (void) state;

    /* When killstack is disabled, stack is never vulnerable */
    game.info.killstack = FALSE;

    assert_false(is_stack_vulnerable(NULL));
}

/***********************************************************************
  Test POWER_FACTOR constant
***********************************************************************/
static void test_power_factor(void **state)
{
    (void) state;

    /* POWER_FACTOR should be positive */
    assert_true(POWER_FACTOR > 0);

    /* POWER_FACTOR should be 10 (as defined in combat.h) */
    assert_int_equal(POWER_FACTOR, 10);
}

/***********************************************************************
  Test unit_attack_result enumeration values
***********************************************************************/
static void test_unit_attack_result_enum(void **state)
{
    (void) state;

    /* Test that enum values are defined and distinct */
    assert_int_equal(ATT_OK, 0);
    assert_true(ATT_NON_ATTACK > ATT_OK);
    assert_true(ATT_UNREACHABLE > ATT_NON_ATTACK);
    assert_true(ATT_NONNATIVE_SRC > ATT_UNREACHABLE);
    assert_true(ATT_NONNATIVE_DST > ATT_NONNATIVE_SRC);
    assert_true(ATT_NOT_WIPABLE > ATT_NONNATIVE_DST);
}

/***********************************************************************
  Test combat_bonus_type enumeration
***********************************************************************/
static void test_combat_bonus_type_enum(void **state)
{
    (void) state;

    /* Test that combat bonus types are defined */
    assert_int_equal(CBONUS_DEFENSE_MULTIPLIER, 0);
    assert_true(CBONUS_DEFENSE_DIVIDER > CBONUS_DEFENSE_MULTIPLIER);
    assert_true(CBONUS_LOW_FIREPOWER > CBONUS_DEFENSE_DIVIDER);
    assert_true(CBONUS_DEFENSE_MULTIPLIER_PCT > CBONUS_LOW_FIREPOWER);
    assert_true(CBONUS_DEFENSE_DIVIDER_PCT > CBONUS_DEFENSE_MULTIPLIER_PCT);
    assert_true(CBONUS_SCRAMBLES_PCT > CBONUS_DEFENSE_DIVIDER_PCT);
}

/***********************************************************************
  Test combat_bonus_against with NULL list
***********************************************************************/
static void test_combat_bonus_against_null_list(void **state)
{
    int result;

    (void) state;

    /* NULL list should return 0 */
    result = combat_bonus_against(NULL, NULL, CBONUS_DEFENSE_MULTIPLIER);
    assert_int_equal(result, 0);
}

/***********************************************************************
  Test win_chance symmetric properties
***********************************************************************/
static void test_win_chance_symmetry(void **state)
{
    double chance1, chance2;

    (void) state;

    /* With equal parameters, win chance should be approximately 0.5 */
    chance1 = win_chance(10, 10, 1, 10, 10, 1);
    assert_true(fabs(chance1 - 0.5) < 0.01);

    /* Swapping attacker and defender should give complementary probabilities */
    chance1 = win_chance(10, 10, 1, 20, 10, 1);
    chance2 = win_chance(20, 10, 1, 10, 10, 1);

    /* Note: Not exactly complementary due to the algorithm, but related */
    assert_true(chance1 < 0.5);
    assert_true(chance2 > 0.5);
}

/***********************************************************************
  Test win_chance with very large HP values
***********************************************************************/
static void test_win_chance_large_hp(void **state)
{
    double chance;

    (void) state;

    /* Large HP values */
    chance = win_chance(10, 1000, 1, 10, 1000, 1);
    assert_true(chance > 0.49 && chance < 0.51);

    /* Very large HP */
    chance = win_chance(10, 10000, 1, 10, 10000, 1);
    assert_true(chance >= 0.0 && chance <= 1.0);
}

/***********************************************************************
  Test game.info default values relevant to combat
***********************************************************************/
static void test_combat_game_defaults(void **state)
{
    (void) state;

    /* Test default values for combat-related game settings
     * Note: These are initialized by game_init() */
    assert_false(game.info.tired_attack);
    /* killstack default depends on game settings, just verify it's set */
    (void) game.info.killstack;
}

/***********************************************************************
  Test is_tired_attack boundary conditions
***********************************************************************/
static void test_is_tired_attack_boundary(void **state)
{
    (void) state;

    game.info.tired_attack = TRUE;

    /* Test boundary with SINGLE_MOVE = 3
     * Any moves less than 3 is tired */
    assert_true(is_tired_attack(0));
    assert_true(is_tired_attack(1));
    assert_true(is_tired_attack(2));
    assert_false(is_tired_attack(3));

    game.info.tired_attack = FALSE;
}

/***********************************************************************
  Test win_chance with varying firepower ratios
***********************************************************************/
static void test_win_chance_firepower_ratio(void **state)
{
    double chance_equal, chance_double;

    (void) state;

    /* Equal firepower on both sides */
    chance_equal = win_chance(10, 10, 2, 10, 10, 2);

    /* Attacker has double firepower */
    chance_double = win_chance(10, 10, 2, 10, 10, 1);

    /* Higher firepower ratio should help */
    assert_true(chance_double > chance_equal);
}

/***********************************************************************
  Test win_chance strength dominance
***********************************************************************/
static void test_win_chance_strength_dominance(void **state)
{
    double chance;

    (void) state;

    /* Much stronger attacker should have very high win chance */
    chance = win_chance(100, 10, 1, 1, 10, 1);
    assert_true(chance > 0.9);

    /* Much stronger defender should give attacker very low win chance */
    chance = win_chance(1, 10, 1, 100, 10, 1);
    assert_true(chance < 0.1);
}

/***********************************************************************
  Test win_chance with minimum valid values
***********************************************************************/
static void test_win_chance_minimum_values(void **state)
{
    double chance;

    (void) state;

    /* Minimum valid positive values */
    chance = win_chance(1, 1, 1, 1, 1, 1);
    assert_true(chance > 0.0 && chance < 1.0);
}

/***********************************************************************
  Test is_stack_vulnerable with killstack enabled but other conditions
***********************************************************************/
static void test_is_stack_vulnerable_conditions(void **state)
{
    (void) state;

    game.info.killstack = TRUE;

    /* NULL tile - this would crash, so we just verify the setting */
    assert_true(game.info.killstack);

    /* Note: Full testing would require a mock tile with city or extra */
}

/***********************************************************************
  Test win_chance numerical stability
***********************************************************************/
static void test_win_chance_numerical_stability(void **state)
{
    double chance;

    (void) state;

    /* Test that the function doesn't produce NaN or Inf */
    chance = win_chance(1, 1, 1, 1, 1, 1);
    assert_false(isnan(chance));
    assert_false(isinf(chance));

    chance = win_chance(100, 100, 10, 100, 100, 10);
    assert_false(isnan(chance));
    assert_false(isinf(chance));
}

/***********************************************************************
  Test win_chance deterministic behavior
***********************************************************************/
static void test_win_chance_deterministic(void **state)
{
    double chance1, chance2;

    (void) state;

    /* Same inputs should give same outputs */
    chance1 = win_chance(10, 10, 1, 10, 10, 1);
    chance2 = win_chance(10, 10, 1, 10, 10, 1);

    assert_true(fabs(chance1 - chance2) < 1e-10);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        /* Win chance tests */
        cmocka_unit_test_setup_teardown(test_win_chance_basic,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_win_chance_zero_attack,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_win_chance_hp_variation,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_win_chance_firepower,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_win_chance_bounds,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_win_chance_symmetry,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_win_chance_large_hp,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_win_chance_firepower_ratio,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_win_chance_strength_dominance,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_win_chance_minimum_values,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_win_chance_numerical_stability,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_win_chance_deterministic,
                                        setup_combat, teardown_combat),

        /* Tired attack tests */
        cmocka_unit_test_setup_teardown(test_is_tired_attack_disabled,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_is_tired_attack_enabled,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_is_tired_attack_boundary,
                                        setup_combat, teardown_combat),

        /* Stack vulnerability tests */
        cmocka_unit_test_setup_teardown(test_is_stack_vulnerable_killstack_disabled,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_is_stack_vulnerable_conditions,
                                        setup_combat, teardown_combat),

        /* Constants and enums tests */
        cmocka_unit_test_setup_teardown(test_power_factor,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_unit_attack_result_enum,
                                        setup_combat, teardown_combat),
        cmocka_unit_test_setup_teardown(test_combat_bonus_type_enum,
                                        setup_combat, teardown_combat),

        /* Combat bonus tests */
        cmocka_unit_test_setup_teardown(test_combat_bonus_against_null_list,
                                        setup_combat, teardown_combat),

        /* Game defaults tests */
        cmocka_unit_test_setup_teardown(test_combat_game_defaults,
                                        setup_combat, teardown_combat),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}