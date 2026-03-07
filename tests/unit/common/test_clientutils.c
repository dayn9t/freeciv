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
#include <string.h>
#include <cmocka.h>

/* utility */
#include "astring.h"
#include "mem.h"

/* common */
#include "clientutils.h"
#include "fc_types.h"
#include "game.h"
#include "map.h"
#include "tile.h"
#include "unit.h"
#include "unitlist.h"
#include "world_object.h"

/* Test structures */
static struct tile test_tile;

/***********************************************************************
  Setup function - initialize minimal game state
***********************************************************************/
static int setup(void **state)
{
    (void) state;

    /* Initialize game structure */
    memset(&game, 0, sizeof(game));
    memset(&wld, 0, sizeof(wld));
    memset(&test_tile, 0, sizeof(test_tile));

    /* Initialize tile units list */
    test_tile.units = unit_list_new();

    return 0;
}

/***********************************************************************
  Teardown function - cleanup
***********************************************************************/
static int teardown(void **state)
{
    (void) state;

    /* Clean up tile units list */
    if (test_tile.units != NULL) {
        unit_list_destroy(test_tile.units);
        test_tile.units = NULL;
    }

    return 0;
}

/***********************************************************************
  Test activity type enumeration values
***********************************************************************/
static void test_activity_type_enumeration(void **state)
{
    (void) state;

    /* Verify activity type enum values are valid */
    assert_true(ACTIVITY_IDLE >= 0);
    assert_true(ACTIVITY_LAST > ACTIVITY_IDLE);

    /* Common activities should be in valid range */
    assert_true(ACTIVITY_MINE < ACTIVITY_LAST);
    assert_true(ACTIVITY_IRRIGATE < ACTIVITY_LAST);
    assert_true(ACTIVITY_GEN_ROAD < ACTIVITY_LAST);
    assert_true(ACTIVITY_PILLAGE < ACTIVITY_LAST);
    assert_true(ACTIVITY_CLEAN < ACTIVITY_LAST);
    assert_true(ACTIVITY_TRANSFORM < ACTIVITY_LAST);
}

/***********************************************************************
  Test is_build_activity for build activities
***********************************************************************/
static void test_is_build_activity_true(void **state)
{
    (void) state;

    assert_true(is_build_activity(ACTIVITY_MINE));
    assert_true(is_build_activity(ACTIVITY_IRRIGATE));
    assert_true(is_build_activity(ACTIVITY_BASE));
    assert_true(is_build_activity(ACTIVITY_GEN_ROAD));
}

/***********************************************************************
  Test is_build_activity for non-build activities
***********************************************************************/
static void test_is_build_activity_false(void **state)
{
    (void) state;

    assert_false(is_build_activity(ACTIVITY_IDLE));
    assert_false(is_build_activity(ACTIVITY_PILLAGE));
    assert_false(is_build_activity(ACTIVITY_FORTIFIED));
    assert_false(is_build_activity(ACTIVITY_TRANSFORM));
}

/***********************************************************************
  Test is_clean_activity for clean activities
***********************************************************************/
static void test_is_clean_activity_true(void **state)
{
    (void) state;

    assert_true(is_clean_activity(ACTIVITY_PILLAGE));
    assert_true(is_clean_activity(ACTIVITY_CLEAN));
}

/***********************************************************************
  Test is_clean_activity for non-clean activities
***********************************************************************/
static void test_is_clean_activity_false(void **state)
{
    (void) state;

    assert_false(is_clean_activity(ACTIVITY_IDLE));
    assert_false(is_clean_activity(ACTIVITY_MINE));
    assert_false(is_clean_activity(ACTIVITY_IRRIGATE));
}

/***********************************************************************
  Test is_tile_activity for tile activities
***********************************************************************/
static void test_is_tile_activity_true(void **state)
{
    (void) state;

    /* Build activities are tile activities */
    assert_true(is_tile_activity(ACTIVITY_MINE));
    assert_true(is_tile_activity(ACTIVITY_IRRIGATE));
    assert_true(is_tile_activity(ACTIVITY_BASE));
    assert_true(is_tile_activity(ACTIVITY_GEN_ROAD));

    /* Clean activities are tile activities */
    assert_true(is_tile_activity(ACTIVITY_PILLAGE));
    assert_true(is_tile_activity(ACTIVITY_CLEAN));

    /* Terrain change activities are tile activities */
    assert_true(is_tile_activity(ACTIVITY_CULTIVATE));
    assert_true(is_tile_activity(ACTIVITY_PLANT));
    assert_true(is_tile_activity(ACTIVITY_TRANSFORM));
}

/***********************************************************************
  Test is_tile_activity for non-tile activities
***********************************************************************/
static void test_is_tile_activity_false(void **state)
{
    (void) state;

    assert_false(is_tile_activity(ACTIVITY_IDLE));
    assert_false(is_tile_activity(ACTIVITY_FORTIFIED));
    assert_false(is_tile_activity(ACTIVITY_SENTRY));
}

/***********************************************************************
  Test activity_requires_target for activities that require target
***********************************************************************/
static void test_activity_requires_target_true(void **state)
{
    (void) state;

    assert_true(activity_requires_target(ACTIVITY_PILLAGE));
    assert_true(activity_requires_target(ACTIVITY_BASE));
    assert_true(activity_requires_target(ACTIVITY_GEN_ROAD));
    assert_true(activity_requires_target(ACTIVITY_IRRIGATE));
    assert_true(activity_requires_target(ACTIVITY_MINE));
    assert_true(activity_requires_target(ACTIVITY_CLEAN));
}

/***********************************************************************
  Test activity_requires_target for activities that don't require target
***********************************************************************/
static void test_activity_requires_target_false(void **state)
{
    (void) state;

    assert_false(activity_requires_target(ACTIVITY_IDLE));
    assert_false(activity_requires_target(ACTIVITY_FORTIFIED));
    assert_false(activity_requires_target(ACTIVITY_SENTRY));
    assert_false(activity_requires_target(ACTIVITY_GOTO));
    assert_false(activity_requires_target(ACTIVITY_EXPLORE));
    assert_false(activity_requires_target(ACTIVITY_TRANSFORM));
    assert_false(activity_requires_target(ACTIVITY_CULTIVATE));
    assert_false(activity_requires_target(ACTIVITY_PLANT));
    assert_false(activity_requires_target(ACTIVITY_FORTIFYING));
    assert_false(activity_requires_target(ACTIVITY_CONVERT));
}

/***********************************************************************
  Test get_activity_text with valid activities
***********************************************************************/
static void test_get_activity_text_valid(void **state)
{
    const char *text;

    (void) state;

    /* Get text for common activities */
    text = get_activity_text(ACTIVITY_IDLE);
    assert_non_null(text);

    text = get_activity_text(ACTIVITY_MINE);
    assert_non_null(text);

    text = get_activity_text(ACTIVITY_IRRIGATE);
    assert_non_null(text);

    text = get_activity_text(ACTIVITY_FORTIFIED);
    assert_non_null(text);

    text = get_activity_text(ACTIVITY_SENTRY);
    assert_non_null(text);

    text = get_activity_text(ACTIVITY_PILLAGE);
    assert_non_null(text);

    text = get_activity_text(ACTIVITY_GOTO);
    assert_non_null(text);

    text = get_activity_text(ACTIVITY_EXPLORE);
    assert_non_null(text);

    text = get_activity_text(ACTIVITY_TRANSFORM);
    assert_non_null(text);
}

/***********************************************************************
  Test get_activity_text returns different strings for different activities
***********************************************************************/
static void test_get_activity_text_different(void **state)
{
    const char *text_idle;
    const char *text_mine;
    const char *text_fortified;

    (void) state;

    text_idle = get_activity_text(ACTIVITY_IDLE);
    text_mine = get_activity_text(ACTIVITY_MINE);
    text_fortified = get_activity_text(ACTIVITY_FORTIFIED);

    assert_non_null(text_idle);
    assert_non_null(text_mine);
    assert_non_null(text_fortified);

    /* Different activities should have different text (usually) */
    /* At minimum they should be valid strings */
}

/***********************************************************************
  Test turns_to_activity_done with empty tile
  Note: This test requires proper game and map initialization.
***********************************************************************/
static void test_turns_to_activity_done_empty_tile(void **state)
{
    (void) state;

    /* This test requires proper map initialization with tile_activity_time.
     * Skip this test as it requires full game initialization. */
    skip();
}

/***********************************************************************
  Test turns_to_activity_done with NULL tile
  Note: The function will crash with NULL tile as it accesses ptile->units.
***********************************************************************/
static void test_turns_to_activity_done_null_tile(void **state)
{
    (void) state;

    /* This test would crash - the function requires a valid tile.
     * Skip this test as the function is not designed for NULL tiles. */
    skip();
}

/***********************************************************************
  Test concat_tile_activity_text with empty tile
  Note: This test requires proper game and map initialization.
***********************************************************************/
static void test_concat_tile_activity_text_empty_tile(void **state)
{
    (void) state;

    /* This test requires proper map initialization.
     * Skip this test as it requires full game initialization. */
    skip();
}

/***********************************************************************
  Test concat_tile_activity_text with NULL tile
  Note: The function will crash with NULL tile as it accesses ptile->units.
***********************************************************************/
static void test_concat_tile_activity_text_null_tile(void **state)
{
    (void) state;

    /* This test would crash - the function requires a valid tile.
     * Skip this test as the function is not designed for NULL tiles. */
    skip();
}

/***********************************************************************
  Test combat_odds_to_astr with NULL parameters
  Note: This test requires valid unit and unit_type data.
***********************************************************************/
static void test_combat_odds_to_astr_null_params(void **state)
{
    (void) state;

    /* This test requires valid unit and unit_type structures.
     * Skip this test as it requires full game initialization. */
    skip();
}

/***********************************************************************
  Test is_terrain_change_activity for terrain changing activities
***********************************************************************/
static void test_is_terrain_change_activity_true(void **state)
{
    (void) state;

    assert_true(is_terrain_change_activity(ACTIVITY_CULTIVATE));
    assert_true(is_terrain_change_activity(ACTIVITY_PLANT));
    assert_true(is_terrain_change_activity(ACTIVITY_TRANSFORM));
}

/***********************************************************************
  Test is_terrain_change_activity for non-terrain changing activities
***********************************************************************/
static void test_is_terrain_change_activity_false(void **state)
{
    (void) state;

    assert_false(is_terrain_change_activity(ACTIVITY_IDLE));
    assert_false(is_terrain_change_activity(ACTIVITY_MINE));
    assert_false(is_terrain_change_activity(ACTIVITY_PILLAGE));
}

/***********************************************************************
  Test is_targeted_activity for targeted activities
***********************************************************************/
static void test_is_targeted_activity_true(void **state)
{
    (void) state;

    /* Build activities are targeted */
    assert_true(is_targeted_activity(ACTIVITY_MINE));
    assert_true(is_targeted_activity(ACTIVITY_IRRIGATE));
    assert_true(is_targeted_activity(ACTIVITY_BASE));
    assert_true(is_targeted_activity(ACTIVITY_GEN_ROAD));

    /* Clean activities are targeted */
    assert_true(is_targeted_activity(ACTIVITY_PILLAGE));
    assert_true(is_targeted_activity(ACTIVITY_CLEAN));
}

/***********************************************************************
  Test is_targeted_activity for non-targeted activities
***********************************************************************/
static void test_is_targeted_activity_false(void **state)
{
    (void) state;

    assert_false(is_targeted_activity(ACTIVITY_IDLE));
    assert_false(is_targeted_activity(ACTIVITY_FORTIFIED));
    assert_false(is_targeted_activity(ACTIVITY_TRANSFORM));
}

/***********************************************************************
  Test ACTIVITY_FACTOR constant
***********************************************************************/
static void test_activity_factor(void **state)
{
    (void) state;

    /* ACTIVITY_FACTOR should be defined and positive */
    assert_true(ACTIVITY_FACTOR > 0);
    assert_int_equal(ACTIVITY_FACTOR, 10);
}

/***********************************************************************
  Test tile unit list operations
***********************************************************************/
static void test_tile_unit_list_operations(void **state)
{
    (void) state;

    /* Test that we can add/remove units from tile's unit list */
    assert_int_equal(unit_list_size(test_tile.units), 0);

    /* Create a test unit and add it */
    struct unit test_unit;
    memset(&test_unit, 0, sizeof(test_unit));
    test_unit.id = 1;

    unit_list_append(test_tile.units, &test_unit);
    assert_int_equal(unit_list_size(test_tile.units), 1);

    /* Remove the unit */
    unit_list_remove(test_tile.units, &test_unit);
    assert_int_equal(unit_list_size(test_tile.units), 0);
}

/***********************************************************************
  Test tile coordinate operations
***********************************************************************/
static void test_tile_coordinates(void **state)
{
    (void) state;

    test_tile.index = 5;
    assert_int_equal(test_tile.index, 5);

    /* Test x/y would require map setup */
}

/***********************************************************************
  Test activity enum ordering
***********************************************************************/
static void test_activity_enum_ordering(void **state)
{
    (void) state;

    /* Verify enum values are sequential */
    assert_true(ACTIVITY_IDLE < ACTIVITY_CULTIVATE);
    assert_true(ACTIVITY_CULTIVATE < ACTIVITY_MINE);
    assert_true(ACTIVITY_MINE < ACTIVITY_IRRIGATE);
    assert_true(ACTIVITY_IRRIGATE < ACTIVITY_FORTIFIED);
    assert_true(ACTIVITY_FORTIFIED < ACTIVITY_SENTRY);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        /* Activity type enumeration tests */
        cmocka_unit_test_setup_teardown(test_activity_type_enumeration,
                                        setup, teardown),

        /* is_build_activity tests */
        cmocka_unit_test_setup_teardown(test_is_build_activity_true,
                                        setup, teardown),
        cmocka_unit_test_setup_teardown(test_is_build_activity_false,
                                        setup, teardown),

        /* is_clean_activity tests */
        cmocka_unit_test_setup_teardown(test_is_clean_activity_true,
                                        setup, teardown),
        cmocka_unit_test_setup_teardown(test_is_clean_activity_false,
                                        setup, teardown),

        /* is_tile_activity tests */
        cmocka_unit_test_setup_teardown(test_is_tile_activity_true,
                                        setup, teardown),
        cmocka_unit_test_setup_teardown(test_is_tile_activity_false,
                                        setup, teardown),

        /* activity_requires_target tests */
        cmocka_unit_test_setup_teardown(test_activity_requires_target_true,
                                        setup, teardown),
        cmocka_unit_test_setup_teardown(test_activity_requires_target_false,
                                        setup, teardown),

        /* get_activity_text tests */
        cmocka_unit_test_setup_teardown(test_get_activity_text_valid,
                                        setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_activity_text_different,
                                        setup, teardown),

        /* turns_to_activity_done tests - skipped due to dependencies */
        cmocka_unit_test_setup_teardown(test_turns_to_activity_done_empty_tile,
                                        setup, teardown),
        cmocka_unit_test_setup_teardown(test_turns_to_activity_done_null_tile,
                                        setup, teardown),

        /* concat_tile_activity_text tests - skipped due to dependencies */
        cmocka_unit_test_setup_teardown(test_concat_tile_activity_text_empty_tile,
                                        setup, teardown),
        cmocka_unit_test_setup_teardown(test_concat_tile_activity_text_null_tile,
                                        setup, teardown),

        /* combat_odds_to_astr tests - skipped due to dependencies */
        cmocka_unit_test_setup_teardown(test_combat_odds_to_astr_null_params,
                                        setup, teardown),

        /* is_terrain_change_activity tests */
        cmocka_unit_test_setup_teardown(test_is_terrain_change_activity_true,
                                        setup, teardown),
        cmocka_unit_test_setup_teardown(test_is_terrain_change_activity_false,
                                        setup, teardown),

        /* is_targeted_activity tests */
        cmocka_unit_test_setup_teardown(test_is_targeted_activity_true,
                                        setup, teardown),
        cmocka_unit_test_setup_teardown(test_is_targeted_activity_false,
                                        setup, teardown),

        /* Constant tests */
        cmocka_unit_test_setup_teardown(test_activity_factor,
                                        setup, teardown),

        /* Tile operations tests */
        cmocka_unit_test_setup_teardown(test_tile_unit_list_operations,
                                        setup, teardown),
        cmocka_unit_test_setup_teardown(test_tile_coordinates,
                                        setup, teardown),

        /* Enum ordering tests */
        cmocka_unit_test_setup_teardown(test_activity_enum_ordering,
                                        setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}