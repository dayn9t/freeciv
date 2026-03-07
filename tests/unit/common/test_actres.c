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

/* common */
#include "actres.h"
#include "game.h"
#include "map.h"
#include "tile.h"
#include "unittype.h"
#include "world_object.h"
#include "extras.h"
#include "terrain.h"

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

    /* Test ACTRES_BASE - mandatory target complexity */
    assert_int_equal(actres_target_compl_calc(ACTRES_BASE),
                     ACT_TGT_COMPL_MANDATORY);

    /* Test ACTRES_MINE - mandatory target complexity */
    assert_int_equal(actres_target_compl_calc(ACTRES_MINE),
                     ACT_TGT_COMPL_MANDATORY);

    /* Test ACTRES_IRRIGATE - mandatory target complexity */
    assert_int_equal(actres_target_compl_calc(ACTRES_IRRIGATE),
                     ACT_TGT_COMPL_MANDATORY);

    /* Test ACTRES_SPY_TARGETED_STEAL_TECH - mandatory target complexity */
    assert_int_equal(actres_target_compl_calc(ACTRES_SPY_TARGETED_STEAL_TECH),
                     ACT_TGT_COMPL_MANDATORY);

    /* Test ACTRES_STRIKE_BUILDING - mandatory target complexity */
    assert_int_equal(actres_target_compl_calc(ACTRES_STRIKE_BUILDING),
                     ACT_TGT_COMPL_MANDATORY);

    /* Test ACTRES_CLEAN - flexible target complexity */
    assert_int_equal(actres_target_compl_calc(ACTRES_CLEAN),
                     ACT_TGT_COMPL_FLEXIBLE);

    /* Test various simple target complexity actions */
    assert_int_equal(actres_target_compl_calc(ACTRES_SPY_INVESTIGATE_CITY),
                     ACT_TGT_COMPL_SIMPLE);
    assert_int_equal(actres_target_compl_calc(ACTRES_SPY_POISON),
                     ACT_TGT_COMPL_SIMPLE);
    assert_int_equal(actres_target_compl_calc(ACTRES_TRADE_ROUTE),
                     ACT_TGT_COMPL_SIMPLE);
    assert_int_equal(actres_target_compl_calc(ACTRES_FOUND_CITY),
                     ACT_TGT_COMPL_SIMPLE);
    assert_int_equal(actres_target_compl_calc(ACTRES_ATTACK),
                     ACT_TGT_COMPL_SIMPLE);
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

    /* Test ACTRES_WIPE_UNITS - standard battle */
    assert_int_equal(actres_get_battle_kind(ACTRES_WIPE_UNITS), ABK_STANDARD);

    /* Test additional diplomatic battle actions */
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_STEAL_GOLD), ABK_DIPLOMATIC);
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_SABOTAGE_CITY), ABK_DIPLOMATIC);
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_TARGETED_SABOTAGE_CITY), ABK_DIPLOMATIC);
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_SABOTAGE_CITY_PRODUCTION), ABK_DIPLOMATIC);
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_STEAL_TECH), ABK_DIPLOMATIC);
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_TARGETED_STEAL_TECH), ABK_DIPLOMATIC);
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_INCITE_CITY), ABK_DIPLOMATIC);
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_BRIBE_STACK), ABK_DIPLOMATIC);
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_SABOTAGE_UNIT), ABK_DIPLOMATIC);
    assert_int_equal(actres_get_battle_kind(ACTRES_STEAL_MAPS), ABK_DIPLOMATIC);
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_NUKE), ABK_DIPLOMATIC);
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_SPREAD_PLAGUE), ABK_DIPLOMATIC);
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_ATTACK), ABK_DIPLOMATIC);

    /* Test actions with no battle (ABK_NONE) */
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_INVESTIGATE_CITY), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_TRADE_ROUTE), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_MARKETPLACE), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_HELP_WONDER), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_FOUND_CITY), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_NUKE), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_BOMBARD), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_STRIKE_BUILDING), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_STRIKE_PRODUCTION), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_CAPTURE_UNITS), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_NUKE_UNITS), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_DESTROY_CITY), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_EXPEL_UNIT), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_DISBAND_UNIT_RECOVER), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_DISBAND_UNIT), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_HOME_CITY), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_UPGRADE_UNIT), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_PARADROP), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_AIRLIFT), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_CONQUER_CITY), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_HEAL_UNIT), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_TRANSFORM_TERRAIN), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_CULTIVATE), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_PLANT), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_PILLAGE), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_FORTIFY), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_ROAD), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_CONVERT), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_BASE), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_MINE), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_IRRIGATE), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_TRANSPORT_DEBOARD), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_TRANSPORT_UNLOAD), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_TRANSPORT_DISEMBARK), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_TRANSPORT_BOARD), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_TRANSPORT_EMBARK), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_CONQUER_EXTRAS), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_HUT_ENTER), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_HUT_FRIGHTEN), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_UNIT_MOVE), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_PARADROP_CONQUER), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_HOMELESS), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_SPY_ESCAPE), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_TRANSPORT_LOAD), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_CLEAN), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_TELEPORT), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_TELEPORT_CONQUER), ABK_NONE);
    assert_int_equal(actres_get_battle_kind(ACTRES_ENABLER_CHECK), ABK_NONE);
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

    /* Test additional hostile actions */
    assert_true(actres_is_hostile(ACTRES_SPY_STEAL_GOLD));
    assert_true(actres_is_hostile(ACTRES_SPY_SABOTAGE_CITY));
    assert_true(actres_is_hostile(ACTRES_SPY_TARGETED_SABOTAGE_CITY));
    assert_true(actres_is_hostile(ACTRES_SPY_SABOTAGE_CITY_PRODUCTION));
    assert_true(actres_is_hostile(ACTRES_SPY_STEAL_TECH));
    assert_true(actres_is_hostile(ACTRES_SPY_TARGETED_STEAL_TECH));
    assert_true(actres_is_hostile(ACTRES_SPY_INCITE_CITY));
    assert_true(actres_is_hostile(ACTRES_SPY_BRIBE_UNIT));
    assert_true(actres_is_hostile(ACTRES_SPY_BRIBE_STACK));
    assert_true(actres_is_hostile(ACTRES_SPY_SABOTAGE_UNIT));
    assert_true(actres_is_hostile(ACTRES_CAPTURE_UNITS));
    assert_true(actres_is_hostile(ACTRES_STEAL_MAPS));
    assert_true(actres_is_hostile(ACTRES_BOMBARD));
    assert_true(actres_is_hostile(ACTRES_SPY_NUKE));
    assert_true(actres_is_hostile(ACTRES_NUKE));
    assert_true(actres_is_hostile(ACTRES_NUKE_UNITS));
    assert_true(actres_is_hostile(ACTRES_DESTROY_CITY));
    assert_true(actres_is_hostile(ACTRES_EXPEL_UNIT));
    assert_true(actres_is_hostile(ACTRES_WIPE_UNITS));
    assert_true(actres_is_hostile(ACTRES_COLLECT_RANSOM));
    assert_true(actres_is_hostile(ACTRES_CONQUER_CITY));
    assert_true(actres_is_hostile(ACTRES_PILLAGE));
    assert_true(actres_is_hostile(ACTRES_SPY_ATTACK));
    assert_true(actres_is_hostile(ACTRES_SPY_SPREAD_PLAGUE));
    assert_true(actres_is_hostile(ACTRES_CONQUER_EXTRAS));
    assert_true(actres_is_hostile(ACTRES_STRIKE_BUILDING));
    assert_true(actres_is_hostile(ACTRES_STRIKE_PRODUCTION));

    /* Test additional non-hostile actions */
    assert_false(actres_is_hostile(ACTRES_MARKETPLACE));
    assert_false(actres_is_hostile(ACTRES_HELP_WONDER));
    assert_false(actres_is_hostile(ACTRES_JOIN_CITY));
    assert_false(actres_is_hostile(ACTRES_DISBAND_UNIT_RECOVER));
    assert_false(actres_is_hostile(ACTRES_DISBAND_UNIT));
    assert_false(actres_is_hostile(ACTRES_HOME_CITY));
    assert_false(actres_is_hostile(ACTRES_HOMELESS));
    assert_false(actres_is_hostile(ACTRES_UPGRADE_UNIT));
    assert_false(actres_is_hostile(ACTRES_PARADROP));
    assert_false(actres_is_hostile(ACTRES_PARADROP_CONQUER));
    assert_false(actres_is_hostile(ACTRES_AIRLIFT));
    assert_false(actres_is_hostile(ACTRES_HEAL_UNIT));
    assert_false(actres_is_hostile(ACTRES_TRANSFORM_TERRAIN));
    assert_false(actres_is_hostile(ACTRES_CULTIVATE));
    assert_false(actres_is_hostile(ACTRES_PLANT));
    assert_false(actres_is_hostile(ACTRES_FORTIFY));
    assert_false(actres_is_hostile(ACTRES_ROAD));
    assert_false(actres_is_hostile(ACTRES_CONVERT));
    assert_false(actres_is_hostile(ACTRES_BASE));
    assert_false(actres_is_hostile(ACTRES_MINE));
    assert_false(actres_is_hostile(ACTRES_IRRIGATE));
    assert_false(actres_is_hostile(ACTRES_TRANSPORT_DEBOARD));
    assert_false(actres_is_hostile(ACTRES_TRANSPORT_UNLOAD));
    assert_false(actres_is_hostile(ACTRES_TRANSPORT_LOAD));
    assert_false(actres_is_hostile(ACTRES_TRANSPORT_DISEMBARK));
    assert_false(actres_is_hostile(ACTRES_TRANSPORT_BOARD));
    assert_false(actres_is_hostile(ACTRES_TRANSPORT_EMBARK));
    assert_false(actres_is_hostile(ACTRES_HUT_ENTER));
    assert_false(actres_is_hostile(ACTRES_HUT_FRIGHTEN));
    assert_false(actres_is_hostile(ACTRES_UNIT_MOVE));
    assert_false(actres_is_hostile(ACTRES_TELEPORT));
    assert_false(actres_is_hostile(ACTRES_TELEPORT_CONQUER));
    assert_false(actres_is_hostile(ACTRES_ENABLER_CHECK));
    assert_false(actres_is_hostile(ACTRES_SPY_ESCAPE));
    assert_false(actres_is_hostile(ACTRES_CLEAN));
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

    /* Test additional actions with no activity */
    assert_int_equal(actres_activity_result(ACTRES_SPY_INVESTIGATE_CITY), ACTIVITY_LAST);
    assert_int_equal(actres_activity_result(ACTRES_SPY_POISON), ACTIVITY_LAST);
    assert_int_equal(actres_activity_result(ACTRES_TRADE_ROUTE), ACTIVITY_LAST);
    assert_int_equal(actres_activity_result(ACTRES_FOUND_CITY), ACTIVITY_LAST);
    assert_int_equal(actres_activity_result(ACTRES_ATTACK), ACTIVITY_LAST);
    assert_int_equal(actres_activity_result(ACTRES_DISBAND_UNIT), ACTIVITY_LAST);
    assert_int_equal(actres_activity_result(ACTRES_NUKE), ACTIVITY_LAST);
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

    /* Test additional diplchance dice roll actions */
    assert_int_equal(actres_dice_type(ACTRES_SPY_TARGETED_SABOTAGE_CITY), DRT_DIPLCHANCE);
    assert_int_equal(actres_dice_type(ACTRES_SPY_SABOTAGE_CITY_PRODUCTION), DRT_DIPLCHANCE);
    assert_int_equal(actres_dice_type(ACTRES_SPY_STEAL_TECH), DRT_DIPLCHANCE);
    assert_int_equal(actres_dice_type(ACTRES_SPY_TARGETED_STEAL_TECH), DRT_DIPLCHANCE);
    assert_int_equal(actres_dice_type(ACTRES_STEAL_MAPS), DRT_DIPLCHANCE);
    assert_int_equal(actres_dice_type(ACTRES_SPY_NUKE), DRT_DIPLCHANCE);
    assert_int_equal(actres_dice_type(ACTRES_STRIKE_BUILDING), DRT_DIPLCHANCE);
    assert_int_equal(actres_dice_type(ACTRES_STRIKE_PRODUCTION), DRT_DIPLCHANCE);
    assert_int_equal(actres_dice_type(ACTRES_SPY_SPREAD_PLAGUE), DRT_DIPLCHANCE);

    /* Test additional no dice roll actions */
    assert_int_equal(actres_dice_type(ACTRES_SPY_INVESTIGATE_CITY), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_TRADE_ROUTE), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_MARKETPLACE), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_HELP_WONDER), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_SPY_BRIBE_UNIT), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_SPY_BRIBE_STACK), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_SPY_SABOTAGE_UNIT), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_CAPTURE_UNITS), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_FOUND_CITY), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_JOIN_CITY), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_BOMBARD), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_NUKE), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_NUKE_UNITS), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_DESTROY_CITY), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_EXPEL_UNIT), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_DISBAND_UNIT_RECOVER), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_DISBAND_UNIT), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_HOME_CITY), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_HOMELESS), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_UPGRADE_UNIT), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_PARADROP), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_PARADROP_CONQUER), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_AIRLIFT), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_WIPE_UNITS), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_COLLECT_RANSOM), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_CONQUER_CITY), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_HEAL_UNIT), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_TRANSFORM_TERRAIN), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_CULTIVATE), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_PLANT), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_PILLAGE), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_CLEAN), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_FORTIFY), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_ROAD), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_CONVERT), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_BASE), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_MINE), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_IRRIGATE), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_TRANSPORT_DEBOARD), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_TRANSPORT_UNLOAD), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_TRANSPORT_LOAD), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_TRANSPORT_DISEMBARK), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_TRANSPORT_BOARD), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_TRANSPORT_EMBARK), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_SPY_ATTACK), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_CONQUER_EXTRAS), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_HUT_ENTER), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_HUT_FRIGHTEN), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_UNIT_MOVE), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_TELEPORT), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_TELEPORT_CONQUER), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_ENABLER_CHECK), DRT_NONE);
    assert_int_equal(actres_dice_type(ACTRES_SPY_ESCAPE), DRT_NONE);
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

    /* Test ACTRES_NONE returns default max range */
    assert_int_equal(actres_max_range_default(ACTRES_NONE),
                     RS_DEFAULT_ACTION_MAX_RANGE);

    /* Test ACTRES_NUKE returns default explode nuclear max range */
    assert_int_equal(actres_max_range_default(ACTRES_NUKE),
                     RS_DEFAULT_EXPLODE_NUCLEAR_MAX_RANGE);

    /* Test ACTRES_NUKE_UNITS returns default max range */
    assert_int_equal(actres_max_range_default(ACTRES_NUKE_UNITS),
                     RS_DEFAULT_ACTION_MAX_RANGE);

    /* Test ACTRES_TELEPORT returns unlimited distance */
    assert_int_equal(actres_max_range_default(ACTRES_TELEPORT),
                     ACTION_DISTANCE_UNLIMITED);

    /* Test ACTRES_TELEPORT_CONQUER returns unlimited distance */
    assert_int_equal(actres_max_range_default(ACTRES_TELEPORT_CONQUER),
                     ACTION_DISTANCE_UNLIMITED);

    /* Test ACTRES_AIRLIFT returns unlimited distance */
    assert_int_equal(actres_max_range_default(ACTRES_AIRLIFT),
                     ACTION_DISTANCE_UNLIMITED);

    /* Test ACTRES_HELP_WONDER returns default action max range */
    assert_int_equal(actres_max_range_default(ACTRES_HELP_WONDER),
                     RS_DEFAULT_ACTION_MAX_RANGE);

    /* Test ACTRES_DISBAND_UNIT_RECOVER returns default action max range */
    assert_int_equal(actres_max_range_default(ACTRES_DISBAND_UNIT_RECOVER),
                     RS_DEFAULT_ACTION_MAX_RANGE);

    /* Test ACTRES_BOMBARD returns default action max range */
    assert_int_equal(actres_max_range_default(ACTRES_BOMBARD),
                     RS_DEFAULT_ACTION_MAX_RANGE);
}

/* Test actres_legal_target_kind for various action results */
static void test_actres_legal_target_kind(void **state)
{
    (void) state;

    /* Test ACTRES_ESTABLISH_EMBASSY targets cities */
    assert_true(actres_legal_target_kind(ACTRES_ESTABLISH_EMBASSY, ATK_CITY));
    assert_false(actres_legal_target_kind(ACTRES_ESTABLISH_EMBASSY, ATK_UNIT));
    assert_false(actres_legal_target_kind(ACTRES_ESTABLISH_EMBASSY, ATK_TILE));
    assert_false(actres_legal_target_kind(ACTRES_ESTABLISH_EMBASSY, ATK_STACK));
    assert_false(actres_legal_target_kind(ACTRES_ESTABLISH_EMBASSY, ATK_EXTRAS));
    assert_false(actres_legal_target_kind(ACTRES_ESTABLISH_EMBASSY, ATK_SELF));

    /* Test ACTRES_SPY_BRIBE_UNIT targets units */
    assert_true(actres_legal_target_kind(ACTRES_SPY_BRIBE_UNIT, ATK_UNIT));
    assert_false(actres_legal_target_kind(ACTRES_SPY_BRIBE_UNIT, ATK_CITY));
    assert_false(actres_legal_target_kind(ACTRES_SPY_BRIBE_UNIT, ATK_TILE));

    /* Test ACTRES_FOUND_CITY targets tiles */
    assert_true(actres_legal_target_kind(ACTRES_FOUND_CITY, ATK_TILE));
    assert_false(actres_legal_target_kind(ACTRES_FOUND_CITY, ATK_CITY));
    assert_false(actres_legal_target_kind(ACTRES_FOUND_CITY, ATK_UNIT));

    /* Test ACTRES_DISBAND_UNIT targets self */
    assert_true(actres_legal_target_kind(ACTRES_DISBAND_UNIT, ATK_SELF));
    assert_false(actres_legal_target_kind(ACTRES_DISBAND_UNIT, ATK_CITY));
    assert_false(actres_legal_target_kind(ACTRES_DISBAND_UNIT, ATK_TILE));

    /* Test ACTRES_CAPTURE_UNITS targets stacks */
    assert_true(actres_legal_target_kind(ACTRES_CAPTURE_UNITS, ATK_STACK));
    assert_false(actres_legal_target_kind(ACTRES_CAPTURE_UNITS, ATK_CITY));
    assert_false(actres_legal_target_kind(ACTRES_CAPTURE_UNITS, ATK_UNIT));

    /* Test ACTRES_CONQUER_EXTRAS targets extras */
    assert_true(actres_legal_target_kind(ACTRES_CONQUER_EXTRAS, ATK_EXTRAS));
    assert_false(actres_legal_target_kind(ACTRES_CONQUER_EXTRAS, ATK_CITY));
    assert_false(actres_legal_target_kind(ACTRES_CONQUER_EXTRAS, ATK_TILE));

    /* Test ACTRES_PILLAGE - flexible target (tile or extras) */
    assert_true(actres_legal_target_kind(ACTRES_PILLAGE, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_PILLAGE, ATK_EXTRAS));
    assert_false(actres_legal_target_kind(ACTRES_PILLAGE, ATK_CITY));
    assert_false(actres_legal_target_kind(ACTRES_PILLAGE, ATK_UNIT));
    assert_false(actres_legal_target_kind(ACTRES_PILLAGE, ATK_STACK));
    assert_false(actres_legal_target_kind(ACTRES_PILLAGE, ATK_SELF));

    /* Test ACTRES_NUKE - flexible target (tile or city) */
    assert_true(actres_legal_target_kind(ACTRES_NUKE, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_NUKE, ATK_CITY));
    assert_false(actres_legal_target_kind(ACTRES_NUKE, ATK_UNIT));
    assert_false(actres_legal_target_kind(ACTRES_NUKE, ATK_STACK));
    assert_false(actres_legal_target_kind(ACTRES_NUKE, ATK_EXTRAS));
    assert_false(actres_legal_target_kind(ACTRES_NUKE, ATK_SELF));

    /* Test ACTRES_NONE - works with all target kinds */
    assert_true(actres_legal_target_kind(ACTRES_NONE, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_NONE, ATK_UNIT));
    assert_true(actres_legal_target_kind(ACTRES_NONE, ATK_STACK));
    assert_true(actres_legal_target_kind(ACTRES_NONE, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_NONE, ATK_EXTRAS));
    assert_true(actres_legal_target_kind(ACTRES_NONE, ATK_SELF));

    /* Test ATK_UNIT target actions */
    assert_true(actres_legal_target_kind(ACTRES_SPY_SABOTAGE_UNIT, ATK_UNIT));
    assert_true(actres_legal_target_kind(ACTRES_EXPEL_UNIT, ATK_UNIT));
    assert_true(actres_legal_target_kind(ACTRES_HEAL_UNIT, ATK_UNIT));
    assert_true(actres_legal_target_kind(ACTRES_TRANSPORT_DEBOARD, ATK_UNIT));
    assert_true(actres_legal_target_kind(ACTRES_TRANSPORT_UNLOAD, ATK_UNIT));
    assert_true(actres_legal_target_kind(ACTRES_TRANSPORT_LOAD, ATK_UNIT));
    assert_true(actres_legal_target_kind(ACTRES_TRANSPORT_BOARD, ATK_UNIT));
    assert_true(actres_legal_target_kind(ACTRES_TRANSPORT_EMBARK, ATK_UNIT));

    /* Test ATK_STACK target actions */
    assert_true(actres_legal_target_kind(ACTRES_SPY_BRIBE_STACK, ATK_STACK));
    assert_true(actres_legal_target_kind(ACTRES_BOMBARD, ATK_STACK));
    assert_true(actres_legal_target_kind(ACTRES_NUKE_UNITS, ATK_STACK));
    assert_true(actres_legal_target_kind(ACTRES_ATTACK, ATK_STACK));
    assert_true(actres_legal_target_kind(ACTRES_WIPE_UNITS, ATK_STACK));
    assert_true(actres_legal_target_kind(ACTRES_SPY_ATTACK, ATK_STACK));
    assert_true(actres_legal_target_kind(ACTRES_COLLECT_RANSOM, ATK_STACK));

    /* Test ATK_TILE target actions */
    assert_true(actres_legal_target_kind(ACTRES_PARADROP, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_PARADROP_CONQUER, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_TRANSFORM_TERRAIN, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_CULTIVATE, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_PLANT, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_CLEAN, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_ROAD, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_BASE, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_MINE, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_IRRIGATE, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_TRANSPORT_DISEMBARK, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_HUT_ENTER, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_HUT_FRIGHTEN, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_UNIT_MOVE, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_TELEPORT, ATK_TILE));
    assert_true(actres_legal_target_kind(ACTRES_TELEPORT_CONQUER, ATK_TILE));

    /* Test ATK_SELF target actions */
    assert_true(actres_legal_target_kind(ACTRES_CONVERT, ATK_SELF));
    assert_true(actres_legal_target_kind(ACTRES_FORTIFY, ATK_SELF));
    assert_true(actres_legal_target_kind(ACTRES_HOMELESS, ATK_SELF));
    assert_true(actres_legal_target_kind(ACTRES_ENABLER_CHECK, ATK_SELF));

    /* Test ATK_CITY target actions */
    assert_true(actres_legal_target_kind(ACTRES_SPY_INVESTIGATE_CITY, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_SPY_POISON, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_SPY_STEAL_GOLD, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_SPY_SABOTAGE_CITY, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_SPY_TARGETED_SABOTAGE_CITY, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_SPY_SABOTAGE_CITY_PRODUCTION, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_SPY_STEAL_TECH, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_SPY_TARGETED_STEAL_TECH, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_SPY_INCITE_CITY, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_TRADE_ROUTE, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_MARKETPLACE, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_HELP_WONDER, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_JOIN_CITY, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_STEAL_MAPS, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_SPY_NUKE, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_DESTROY_CITY, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_DISBAND_UNIT_RECOVER, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_HOME_CITY, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_UPGRADE_UNIT, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_AIRLIFT, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_STRIKE_BUILDING, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_STRIKE_PRODUCTION, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_CONQUER_CITY, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_SPY_SPREAD_PLAGUE, ATK_CITY));
    assert_true(actres_legal_target_kind(ACTRES_SPY_ESCAPE, ATK_CITY));
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

    /* Test ACTRES_ATTACK default target is stack */
    assert_int_equal(actres_target_kind_default(ACTRES_ATTACK),
                     ATK_STACK);

    /* Test ACTRES_BOMBARD default target is stack */
    assert_int_equal(actres_target_kind_default(ACTRES_BOMBARD),
                     ATK_STACK);

    /* Test ACTRES_NUKE default target is tile */
    assert_int_equal(actres_target_kind_default(ACTRES_NUKE),
                     ATK_TILE);

    /* Test ACTRES_NUKE_UNITS default target is stack */
    assert_int_equal(actres_target_kind_default(ACTRES_NUKE_UNITS),
                     ATK_STACK);

    /* Test ACTRES_TRANSFORM_TERRAIN default target is tile */
    assert_int_equal(actres_target_kind_default(ACTRES_TRANSFORM_TERRAIN),
                     ATK_TILE);

    /* Test ACTRES_CULTIVATE default target is tile */
    assert_int_equal(actres_target_kind_default(ACTRES_CULTIVATE),
                     ATK_TILE);

    /* Test ACTRES_PLANT default target is tile */
    assert_int_equal(actres_target_kind_default(ACTRES_PLANT),
                     ATK_TILE);

    /* Test ACTRES_PILLAGE default target is tile */
    assert_int_equal(actres_target_kind_default(ACTRES_PILLAGE),
                     ATK_TILE);

    /* Test ACTRES_CLEAN default target is tile */
    assert_int_equal(actres_target_kind_default(ACTRES_CLEAN),
                     ATK_TILE);

    /* Test ACTRES_FORTIFY default target is self */
    assert_int_equal(actres_target_kind_default(ACTRES_FORTIFY),
                     ATK_SELF);

    /* Test ACTRES_ROAD default target is tile */
    assert_int_equal(actres_target_kind_default(ACTRES_ROAD),
                     ATK_TILE);

    /* Test ACTRES_CONVERT default target is self */
    assert_int_equal(actres_target_kind_default(ACTRES_CONVERT),
                     ATK_SELF);

    /* Test ACTRES_BASE default target is tile */
    assert_int_equal(actres_target_kind_default(ACTRES_BASE),
                     ATK_TILE);

    /* Test ACTRES_MINE default target is tile */
    assert_int_equal(actres_target_kind_default(ACTRES_MINE),
                     ATK_TILE);

    /* Test ACTRES_IRRIGATE default target is tile */
    assert_int_equal(actres_target_kind_default(ACTRES_IRRIGATE),
                     ATK_TILE);

    /* Test ACTRES_TELEPORT default target is tile */
    assert_int_equal(actres_target_kind_default(ACTRES_TELEPORT),
                     ATK_TILE);

    /* Test ACTRES_TELEPORT_CONQUER default target is tile */
    assert_int_equal(actres_target_kind_default(ACTRES_TELEPORT_CONQUER),
                     ATK_TILE);

    /* Test ACTRES_HOMELESS default target is self */
    assert_int_equal(actres_target_kind_default(ACTRES_HOMELESS),
                     ATK_SELF);

    /* Test ACTRES_ENABLER_CHECK default target is self */
    assert_int_equal(actres_target_kind_default(ACTRES_ENABLER_CHECK),
                     ATK_SELF);
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

    /* Test ACTRES_ROAD returns extra_not_there sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_ROAD),
                     ASTK_EXTRA_NOT_THERE);

    /* Test ACTRES_BASE returns extra_not_there sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_BASE),
                     ASTK_EXTRA_NOT_THERE);

    /* Test ACTRES_MINE returns extra_not_there sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_MINE),
                     ASTK_EXTRA_NOT_THERE);

    /* Test ACTRES_IRRIGATE returns extra_not_there sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_IRRIGATE),
                     ASTK_EXTRA_NOT_THERE);

    /* Test ACTRES_PILLAGE returns extra sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_PILLAGE),
                     ASTK_EXTRA);

    /* Test ACTRES_CLEAN returns extra sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_CLEAN),
                     ASTK_EXTRA);

    /* Test ACTRES_STRIKE_BUILDING returns building sub target */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_STRIKE_BUILDING),
                     ASTK_BUILDING);

    /* Test additional no sub target actions */
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_INVESTIGATE_CITY), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_POISON), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_STEAL_GOLD), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_SABOTAGE_CITY), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_SABOTAGE_CITY_PRODUCTION), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_STEAL_TECH), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_INCITE_CITY), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_TRADE_ROUTE), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_MARKETPLACE), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_HELP_WONDER), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_BRIBE_UNIT), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_BRIBE_STACK), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_SABOTAGE_UNIT), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_CAPTURE_UNITS), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_FOUND_CITY), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_JOIN_CITY), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_STEAL_MAPS), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_BOMBARD), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_NUKE), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_NUKE), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_NUKE_UNITS), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_DESTROY_CITY), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_EXPEL_UNIT), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_DISBAND_UNIT_RECOVER), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_DISBAND_UNIT), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_HOME_CITY), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_HOMELESS), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_UPGRADE_UNIT), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_PARADROP), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_PARADROP_CONQUER), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_AIRLIFT), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_ATTACK), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_WIPE_UNITS), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_ATTACK), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_COLLECT_RANSOM), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_CONQUER_CITY), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_HEAL_UNIT), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_TRANSFORM_TERRAIN), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_CULTIVATE), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_PLANT), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_FORTIFY), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_CONVERT), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_TRANSPORT_DEBOARD), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_TRANSPORT_UNLOAD), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_TRANSPORT_LOAD), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_TRANSPORT_DISEMBARK), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_TRANSPORT_BOARD), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_TRANSPORT_EMBARK), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_SPREAD_PLAGUE), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_CONQUER_EXTRAS), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_HUT_ENTER), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_HUT_FRIGHTEN), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_UNIT_MOVE), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_TELEPORT), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_TELEPORT_CONQUER), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_ENABLER_CHECK), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_SPY_ESCAPE), ASTK_NONE);
    assert_int_equal(actres_sub_target_kind_default(ACTRES_STRIKE_PRODUCTION), ASTK_NONE);
}

/* Test actres_get_act_time for instantaneous actions */
static void test_actres_get_act_time_instantaneous(void **state)
{
    (void) state;

    /* Test ACTRES_NONE - instantaneous */
    assert_int_equal(actres_get_act_time(ACTRES_NONE, nullptr, nullptr, nullptr),
                     ACT_TIME_INSTANTANEOUS);

    /* Test actions with ACTIVITY_LAST (instantaneous) */
    assert_int_equal(actres_get_act_time(ACTRES_ESTABLISH_EMBASSY, nullptr, nullptr, nullptr),
                     ACT_TIME_INSTANTANEOUS);
    assert_int_equal(actres_get_act_time(ACTRES_SPY_INVESTIGATE_CITY, nullptr, nullptr, nullptr),
                     ACT_TIME_INSTANTANEOUS);
    assert_int_equal(actres_get_act_time(ACTRES_SPY_POISON, nullptr, nullptr, nullptr),
                     ACT_TIME_INSTANTANEOUS);
    assert_int_equal(actres_get_act_time(ACTRES_TRADE_ROUTE, nullptr, nullptr, nullptr),
                     ACT_TIME_INSTANTANEOUS);
    assert_int_equal(actres_get_act_time(ACTRES_ATTACK, nullptr, nullptr, nullptr),
                     ACT_TIME_INSTANTANEOUS);
    assert_int_equal(actres_get_act_time(ACTRES_DISBAND_UNIT, nullptr, nullptr, nullptr),
                     ACT_TIME_INSTANTANEOUS);
    assert_int_equal(actres_get_act_time(ACTRES_NUKE, nullptr, nullptr, nullptr),
                     ACT_TIME_INSTANTANEOUS);
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
        cmocka_unit_test_setup_teardown(test_actres_get_act_time_instantaneous,
                                        setup_actres, teardown_actres),
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}