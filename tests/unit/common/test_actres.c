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

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <setjmp.h>
#include <cmocka.h>

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

/* common */
#include "actres.h"
#include "game.h"

/* Test setup and teardown */
static int setup_actres(void **state)
{
    (void) state;
    game_init(false);
    actres_init();
    return 0;
}

static int teardown_actres(void **state)
{
    (void) state;
    actres_free();
    game_free();
    return 0;
}

/* Test actres_init and actres_free */
static void test_actres_init_free(void **state)
{
    (void) state;

    /* actres_init and actres_free should not crash */
    actres_init();
    actres_free();
}

/* Test actres_target_compl_calc for various action results */
static void test_actres_target_compl_calc(void **state)
{
    (void) state;

    /* Test ACTRES_NONE returns simple target complexity */
    assert_int_equal(actres_target_compl_calc(ACTRES_NONE),
                     ACT_TGT_COMPL_SIMPLE);

    /* Test ACTRES_ESTABLISH_EMBASSY - simple target complexity */
    assert_int_equal(actres_target_compl_calc(ACTRES_ESTABLISH_EMBASSY),
                     ACT_TGT_COMPL_SIMPLE);

    /* Test ACTRES_SPY_TARGETED_SABOTAGE_CITY - mandatory target complexity */
    assert_int_equal(actres_target_compl_calc(ACTRES_SPY_TARGETED_SABOTAGE_CITY),
                     ACT_TGT_COMPL_MANDATORY);

    /* Test ACTRES_PILLAGE - flexible target complexity */
    assert_int_equal(actres_target_compl_calc(ACTRES_PILLAGE),
                     ACT_TGT_COMPL_FLEXIBLE);

    /* Test ACTRES_ROAD - mandatory target complexity */
    assert_int_equal(actres_target_compl_calc(ACTRES_ROAD),
                     ACT_TGT_COMPL_MANDATORY);
}

/* Test actres_get_battle_kind for various action results */
static void test_actres_get_battle_kind(void **state)
{
    (void) state;

    /* Test ACTRES_NONE returns no battle */
    assert_int_equal(actres_get_battle_kind(ACTRES_NONE), ABK_NONE);

    /* Test ACTRES_ESTABLISH_EMBASSY - no battle */
    assert_int_equal(actres_get_battle_kind(ACTRES_ESTABLISH_EMBASSY), ABK_NONE);

    /* Test ACTRES_SPY_POISON - diplomatic battle */
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_POISON), ABK_DIPLOMATIC);

    /* Test ACTRES_SPY_BRIBE_UNIT - diplomatic battle */
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_BRIBE_UNIT), ABK_DIPLOMATIC);

    /* Test ACTRES_ATTACK - standard battle */
    assert_int_equal(actres_get_battle_kind(ACTRES_ATTACK), ABK_STANDARD);

    /* Test ACTRES_COLLECT_RANSOM - standard battle */
    assert_int_equal(actres_get_battle_kind(ACTRES_COLLECT_RANSOM), ABK_STANDARD);
}

/* Test actres_is_hostile for various action results */
static void test_actres_is_hostile(void **state)
{
    (void) state;

    /* Test ACTRES_NONE is not hostile */
    assert_false(actres_is_hostile(ACTRES_NONE));

    /* Test ACTRES_ESTABLISH_EMBASSY is not hostile */
    assert_false(actres_is_hostile(ACTRES_ESTABLISH_EMBASSY));

    /* Test ACTRES_SPY_INVESTIGATE_CITY is hostile */
    assert_true(actres_is_hostile(ACTRES_SPY_INVESTIGATE_CITY));

    /* Test ACTRES_SPY_POISON is hostile */
    assert_true(actres_is_hostile(ACTRES_SPY_POISON));

    /* Test ACTRES_ATTACK is hostile */
    assert_true(actres_is_hostile(ACTRES_ATTACK));

    /* Test ACTRES_TRADE_ROUTE is not hostile */
    assert_false(actres_is_hostile(ACTRES_TRADE_ROUTE));

    /* Test ACTRES_FOUND_CITY is not hostile */
    assert_false(actres_is_hostile(ACTRES_FOUND_CITY));
}

/* Test actres_activity_result for various action results */
static void test_actres_activity_result(void **state)
{
    (void) state;

    /* Test ACTRES_NONE returns ACTIVITY_LAST */
    assert_int_equal(actres_activity_result(ACTRES_NONE), ACTIVITY_LAST);

    /* Test ACTRES_ESTABLISH_EMBASSY has no activity */
    assert_int_equal(actres_activity_result(ACTRES_ESTABLISH_EMBASSY), ACTIVITY_LAST);

    /* Test ACTRES_TRANSFORM_TERRAIN has transform activity */
    assert_int_equal(actres_activity_result(ACTRES_TRANSFORM_TERRAIN),
                     ACTIVITY_TRANSFORM);

    /* Test ACTRES_CULTIVATE has cultivate activity */
    assert_int_equal(actres_activity_result(ACTRES_CULTIVATE), ACTIVITY_CULTIVATE);

    /* Test ACTRES_PLANT has plant activity */
    assert_int_equal(actres_activity_result(ACTRES_PLANT), ACTIVITY_PLANT);

    /* Test ACTRES_PILLAGE has pillage activity */
    assert_int_equal(actres_activity_result(ACTRES_PILLAGE), ACTIVITY_PILLAGE);

    /* Test ACTRES_FORTIFY has fortifying activity */
    assert_int_equal(actres_activity_result(ACTRES_FORTIFY), ACTIVITY_FORTIFYING);

    /* Test ACTRES_ROAD has gen_road activity */
    assert_int_equal(actres_activity_result(ACTRES_ROAD), ACTIVITY_GEN_ROAD);

    /* Test ACTRES_CONVERT has convert activity */
    assert_int_equal(actres_activity_result(ACTRES_CONVERT), ACTIVITY_CONVERT);

    /* Test ACTRES_BASE has base activity */
    assert_int_equal(actres_activity_result(ACTRES_BASE), ACTIVITY_BASE);

    /* Test ACTRES_MINE has mine activity */
    assert_int_equal(actres_activity_result(ACTRES_MINE), ACTIVITY_MINE);

    /* Test ACTRES_IRRIGATE has irrigate activity */
    assert_int_equal(actres_activity_result(ACTRES_IRRIGATE), ACTIVITY_IRRIGATE);

    /* Test ACTRES_CLEAN has clean activity */
    assert_int_equal(actres_activity_result(ACTRES_CLEAN), ACTIVITY_CLEAN);
}

/* Test actres_dice_type for various action results */
static void test_actres_dice_type(void **state)
{
    (void) state;

    /* Test ACTRES_NONE returns no dice roll */
    assert_int_equal(actres_dice_type(ACTRES_NONE), DRT_NONE);

    /* Test ACTRES_ESTABLISH_EMBASSY has no dice roll */
    assert_int_equal(actres_dice_type(ACTRES_ESTABLISH_EMBASSY), DRT_NONE);

    /* Test ACTRES_SPY_POISON has diplchance dice roll */
    assert_int_equal(actres_dice_type(ACTRES_SPY_POISON), DRT_DIPLCHANCE);

    /* Test ACTRES_SPY_STEAL_GOLD has diplchance dice roll */
    assert_int_equal(actres_dice_type(ACTRES_SPY_STEAL_GOLD), DRT_DIPLCHANCE);

    /* Test ACTRES_SPY_SABOTAGE_CITY has diplchance dice roll */
    assert_int_equal(actres_dice_type(ACTRES_SPY_SABOTAGE_CITY), DRT_DIPLCHANCE);

    /* Test ACTRES_SPY_INCITE_CITY has diplchance dice roll */
    assert_int_equal(actres_dice_type(ACTRES_SPY_INCITE_CITY), DRT_DIPLCHANCE);

    /* Test ACTRES_ATTACK has no dice roll (uses standard battle) */
    assert_int_equal(actres_dice_type(ACTRES_ATTACK), DRT_NONE);
}

/* Test actres_min_range_default for various action results */
static void test_actres_min_range_default(void **state)
{
    (void) state;

    /* Test ACTRES_NONE returns default min range */
    assert_int_equal(actres_min_range_default(ACTRES_NONE),
                     RS_DEFAULT_ACTION_MIN_RANGE);

    /* Test ACTRES_NUKE returns default min range */
    assert_int_equal(actres_min_range_default(ACTRES_NUKE),
                     RS_DEFAULT_ACTION_MIN_RANGE);

    /* Test ACTRES_NUKE_UNITS returns default min range */
    assert_int_equal(actres_min_range_default(ACTRES_NUKE_UNITS),
                     RS_DEFAULT_ACTION_MIN_RANGE);

    /* Test ACTRES_TELEPORT returns default move min range */
    assert_int_equal(actres_min_range_default(ACTRES_TELEPORT),
                     RS_DEFAULT_MOVE_MIN_RANGE);

    /* Test ACTRES_TELEPORT_CONQUER returns default move min range */
    assert_int_equal(actres_min_range_default(ACTRES_TELEPORT_CONQUER),
                     RS_DEFAULT_MOVE_MIN_RANGE);
}

/* Test actres_max_range_default for various action results */
static void test_actres_max_range_default(void **state)
{
    (void) state;

    /* Test ACTRES_NONE returns 0 */
    assert_int_equal(actres_max_range_default(ACTRES_NONE), 0);

    /* Test ACTRES_NUKE returns unlimited distance */
    assert_int_equal(actres_max_range_default(ACTRES_NUKE),
                     ACTION_DISTANCE_UNLIMITED);

    /* Test ACTRES_NUKE_UNITS returns unlimited distance */
    assert_int_equal(actres_max_range_default(ACTRES_NUKE_UNITS),
                     ACTION_DISTANCE_UNLIMITED);

    /* Test ACTRES_TELEPORT returns unlimited distance */
    assert_int_equal(actres_max_range_default(ACTRES_TELEPORT),
                     ACTION_DISTANCE_UNLIMITED);

    /* Test ACTRES_TELEPORT_CONQUER returns unlimited distance */
    assert_int_equal(actres_max_range_default(ACTRES_TELEPORT_CONQUER),
                     ACTION_DISTANCE_UNLIMITED);
}

/* Test actres_legal_target_kind for various action results */
static void test_actres_legal_target_kind(void **state)
{
    (void) state;

    /* Test ACTRES_ESTABLISH_EMBASSY targets cities */
    assert_true(actres_legal_target_kind(ACTRES_ESTABLISH_EMBASSY, ATK_CITY));
    assert_false(actres_legal_target_kind(ACTRES_ESTABLISH_EMBASSY, ATK_UNIT));

    /* Test ACTRES_SPY_BRIBE_UNIT targets units */
    assert_true(actres_legal_target_kind(ACTRES_SPY_BRIBE_UNIT, ATK_UNIT));
    assert_false(actres_legal_target_kind(ACTRES_SPY_BRIBE_UNIT, ATK_CITY));

    /* Test ACTRES_FOUND_CITY targets tiles */
    assert_true(actres_legal_target_kind(ACTRES_FOUND_CITY, ATK_TILE));
    assert_false(actres_legal_target_kind(ACTRES_FOUND_CITY, ATK_CITY));

    /* Test ACTRES_DISBAND_UNIT targets self */
    assert_true(actres_legal_target_kind(ACTRES_DISBAND_UNIT, ATK_SELF));

    /* Test ACTRES_CAPTURE_UNITS targets stacks */
    assert_true(actres_legal_target_kind(ACTRES_CAPTURE_UNITS, ATK_STACK));

    /* Test ACTRES_CONQUER_EXTRAS targets extras */
    assert_true(actres_legal_target_kind(ACTRES_CONQUER_EXTRAS, ATK_EXTRAS));
}

/* Test actres_target_kind_default for various action results */
static void test_actres_target_kind_default(void **state)
{
    (void) state;

    /* Test ACTRES_ESTABLISH_EMBASSY default target is city */
    assert_int_equal(actres_target_kind_default(ACTRES_ESTABLISH_EMBASSY),
                     ATK_CITY);

    /* Test ACTRES_SPY_BRIBE_UNIT default target is unit */
    assert_int_equal(actres_target_kind_default(ACTRES_SPY_BRIBE_UNIT),
                     ATK_UNIT);

    /* Test ACTRES_SPY_BRIBE_STACK default target is stack */
    assert_int_equal(actres_target_kind_default(ACTRES_SPY_BRIBE_STACK),
                     ATK_STACK);

    /* Test ACTRES_FOUND_CITY default target is tile */
    assert_int_equal(actres_target_kind_default(ACTRES_FOUND_CITY),
                     ATK_TILE);

    /* Test ACTRES_DISBAND_UNIT default target is self */
    assert_int_equal(actres_target_kind_default(ACTRES_DISBAND_UNIT),
                     ATK_SELF);

    /* Test ACTRES_CONQUER_EXTRAS default target is extras */
    assert_int_equal(actres_target_kind_default(ACTRES_CONQUER_EXTRAS),
                     ATK_EXTRAS);
}

/* Test actres_sub_target_kind_default for various action results */
static void test_actres_sub_target_kind_default(void **state)
{
    (void) state;

    /* Test ACTRES_NONE returns no sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_NONE), ASTK_NONE);

    /* Test ACTRES_ESTABLISH_EMBASSY returns no sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_ESTABLISH_EMBASSY),
                     ASTK_NONE);

    /* Test ACTRES_SPY_TARGETED_SABOTAGE_CITY returns building sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_TARGETED_SABOTAGE_CITY),
                     ASTK_BUILDING);

    /* Test ACTRES_SPY_TARGETED_STEAL_TECH returns tech sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_TARGETED_STEAL_TECH),
                     ASTK_TECH);

    /* Test ACTRES_ROAD returns extra sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_ROAD),
                     ASTK_EXTRA);

    /* Test ACTRES_BASE returns extra sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_BASE),
                     ASTK_EXTRA);

    /* Test ACTRES_MINE returns extra sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_MINE),
                     ASTK_EXTRA);

    /* Test ACTRES_IRRIGATE returns extra sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_IRRIGATE),
                     ASTK_EXTRA);

    /* Test ACTRES_PILLAGE returns extra sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_PILLAGE),
                     ASTK_EXTRA);

    /* Test ACTRES_CLEAN returns extra sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_CLEAN),
                     ASTK_EXTRA);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_actres_init_free,
                                        setup_actres, teardown_actres),
        cmocka_unit_test_setup_teardown(test_actres_target_compl_calc,
                                        setup_actres, teardown_actres),
        cmocka_unit_test_setup_teardown(test_actres_get_battle_kind,
                                        setup_actres, teardown_actres),
        cmocka_unit_test_setup_teardown(test_actres_is_hostile,
                                        setup_actres, teardown_actres),
        cmocka_unit_test_setup_teardown(test_actres_activity_result,
                                        setup_actres, teardown_actres),
        cmocka_unit_test_setup_teardown(test_actres_dice_type,
                                        setup_actres, teardown_actres),
        cmocka_unit_test_setup_teardown(test_actres_min_range_default,
                                        setup_actres, teardown_actres),
        cmocka_unit_test_setup_teardown(test_actres_max_range_default,
                                        setup_actres, teardown_actres),
        cmocka_unit_test_setup_teardown(test_actres_legal_target_kind,
                                        setup_actres, teardown_actres),
        cmocka_unit_test_setup_teardown(test_actres_target_kind_default,
                                        setup_actres, teardown_actres),
        cmocka_unit_test_setup_teardown(test_actres_sub_target_kind_default,
                                        setup_actres, teardown_actres),
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}