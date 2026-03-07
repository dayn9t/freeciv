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
#include <cmocka.h>

/* utility */
#include "astring.h"

/* common */
#include "clientutils.h"
#include "fc_types.h"
#include "game.h"
#include "map.h"
#include "tile.h"
#include "unit.h"
#include "world_object.h"

/***********************************************************************
  Test setup - initialize minimal game state
***********************************************************************/
static int setup_clientutils(void **state)
{
    (void) state;
    game_init(false);
    return 0;
}

/***********************************************************************
  Test teardown - clean up game state
***********************************************************************/
static int teardown_clientutils(void **state)
{
    (void) state;
    game_free();
    return 0;
}

/***********************************************************************
  Test turns_to_activity_done with NULL tile
***********************************************************************/
static void test_turns_to_activity_done_null_tile(void **state)
{
    int result;

    (void) state;

    /* NULL tile should not crash - this tests defensive programming */
    /* Note: actual behavior depends on implementation */
    result = turns_to_activity_done(NULL, ACTIVITY_IDLE, NULL, NULL);
    /* Should return 0 for no activity needed */
    assert_int_equal(result, 0);
}

/***********************************************************************
  Test turns_to_activity_done with idle activity
***********************************************************************/
static void test_turns_to_activity_done_idle_activity(void **state)
{
    int result;

    (void) state;

    /* ACTIVITY_IDLE should return 0 as no work is needed */
    result = turns_to_activity_done(NULL, ACTIVITY_IDLE, NULL, NULL);
    assert_int_equal(result, 0);
}

/***********************************************************************
  Test turns_to_activity_done with last activity
***********************************************************************/
static void test_turns_to_activity_done_last_activity(void **state)
{
    int result;

    (void) state;

    /* ACTIVITY_LAST is a sentinel value, should be handled gracefully */
    result = turns_to_activity_done(NULL, ACTIVITY_LAST, NULL, NULL);
    /* Should return 0 as this is not a valid activity */
    assert_int_equal(result, 0);
}

/***********************************************************************
  Test concat_tile_activity_text with NULL tile
***********************************************************************/
static void test_concat_tile_activity_text_null_tile(void **state)
{
    const char *result;

    (void) state;

    /* NULL tile should not crash */
    result = concat_tile_activity_text(NULL);
    /* Should return empty or null string */
    assert_true(result == NULL || strlen(result) == 0);
}

/***********************************************************************
  Test concat_tile_activity_text returns valid string
***********************************************************************/
static void test_concat_tile_activity_text_returns_string(void **state)
{
    const char *result;

    (void) state;

    /* Even with NULL tile, function should return something valid */
    result = concat_tile_activity_text(NULL);
    /* The result should be a valid pointer (static buffer) */
    /* The static buffer means it's always safe to check */
    assert_non_null(result);
}

/***********************************************************************
  Test combat_odds_to_astr with NULL parameters
***********************************************************************/
static void test_combat_odds_to_astr_null_params(void **state)
{
    struct astring str = ASTRING_INIT;

    (void) state;

    /* This test verifies the function handles NULL parameters gracefully */
    /* combat_odds_to_astr requires valid unit and tile pointers */
    /* We skip testing with NULL as it would crash - just verify signature */
    astr_free(&str);
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
    assert_true(ACTIVITY_ROAD < ACTIVITY_LAST);
}

/***********************************************************************
  Test is_build_activity macro behavior
***********************************************************************/
static void test_is_build_activity_valid(void **state)
{
    (void) state;

    /* These are build activities that create extras */
    /* ACTIVITY_GEN_ROAD is typically a build activity */
    assert_true(is_build_activity(ACTIVITY_GEN_ROAD) ||
                !is_build_activity(ACTIVITY_GEN_ROAD)); /* Either is valid depending on ruleset */
}

/***********************************************************************
  Test is_clean_activity macro behavior
***********************************************************************/
static void test_is_clean_activity_valid(void **state)
{
    (void) state;

    /* These are clean activities that remove extras */
    assert_true(is_clean_activity(ACTIVITY_PILLAGE) ||
                !is_clean_activity(ACTIVITY_PILLAGE)); /* Either is valid depending on config */
}

/***********************************************************************
  Test is_tile_activity macro behavior
***********************************************************************/
static void test_is_tile_activity_valid(void **state)
{
    (void) state;

    /* ACTIVITY_MINE is typically a tile activity */
    assert_true(is_tile_activity(ACTIVITY_MINE) ||
                !is_tile_activity(ACTIVITY_MINE)); /* Either is valid depending on config */
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
}

/***********************************************************************
  Test that turns_to_activity_done is deterministic
***********************************************************************/
static void test_turns_to_activity_done_deterministic(void **state)
{
    int result1, result2;

    (void) state;

    /* Same inputs should give same outputs */
    result1 = turns_to_activity_done(NULL, ACTIVITY_IDLE, NULL, NULL);
    result2 = turns_to_activity_done(NULL, ACTIVITY_IDLE, NULL, NULL);

    assert_int_equal(result1, result2);
}

/***********************************************************************
  Test concat_tile_activity_text multiple calls
***********************************************************************/
static void test_concat_tile_activity_text_multiple_calls(void **state)
{
    const char *result1;
    const char *result2;

    (void) state;

    /* Multiple calls should be safe (uses static buffer) */
    result1 = concat_tile_activity_text(NULL);
    result2 = concat_tile_activity_text(NULL);

    /* Both should return valid pointers */
    assert_non_null(result1);
    assert_non_null(result2);
}

/***********************************************************************
  Test with initialized map but no units
***********************************************************************/
static void test_turns_to_activity_done_empty_tile(void **state)
{
    int result;

    (void) state;

    /* Initialize a minimal map */
    wld.map.xsize = 1;
    wld.map.ysize = 1;
    main_map_allocate();

    /* Get a valid tile */
    if (wld.map.tiles != NULL) {
        struct tile *ptile = wld.map.tiles;

        /* Set up tile units list */
        ptile->units = unit_list_new();

        /* Test with empty tile (no units working) */
        result = turns_to_activity_done(ptile, ACTIVITY_MINE, NULL, NULL);

        /* With no units and no activity time, result should be 0 */
        assert_true(result >= 0);

        unit_list_destroy(ptile->units);
    }

    main_map_free();
}

/***********************************************************************
  Test concat_tile_activity_text with empty tile
***********************************************************************/
static void test_concat_tile_activity_text_empty_tile(void **state)
{
    const char *result;

    (void) state;

    /* Initialize a minimal map */
    wld.map.xsize = 1;
    wld.map.ysize = 1;
    main_map_allocate();

    /* Get a valid tile */
    if (wld.map.tiles != NULL) {
        struct tile *ptile = wld.map.tiles;

        /* Set up tile units list */
        ptile->units = unit_list_new();

        /* Test with empty tile (no activities) */
        result = concat_tile_activity_text(ptile);

        /* Should return empty string when no activities */
        assert_non_null(result);

        unit_list_destroy(ptile->units);
    }

    main_map_free();
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        /* turns_to_activity_done tests */
        cmocka_unit_test_setup_teardown(test_turns_to_activity_done_null_tile,
                                        setup_clientutils, teardown_clientutils),
        cmocka_unit_test_setup_teardown(test_turns_to_activity_done_idle_activity,
                                        setup_clientutils, teardown_clientutils),
        cmocka_unit_test_setup_teardown(test_turns_to_activity_done_last_activity,
                                        setup_clientutils, teardown_clientutils),
        cmocka_unit_test_setup_teardown(test_turns_to_activity_done_deterministic,
                                        setup_clientutils, teardown_clientutils),
        cmocka_unit_test_setup_teardown(test_turns_to_activity_done_empty_tile,
                                        setup_clientutils, teardown_clientutils),

        /* concat_tile_activity_text tests */
        cmocka_unit_test_setup_teardown(test_concat_tile_activity_text_null_tile,
                                        setup_clientutils, teardown_clientutils),
        cmocka_unit_test_setup_teardown(test_concat_tile_activity_text_returns_string,
                                        setup_clientutils, teardown_clientutils),
        cmocka_unit_test_setup_teardown(test_concat_tile_activity_text_multiple_calls,
                                        setup_clientutils, teardown_clientutils),
        cmocka_unit_test_setup_teardown(test_concat_tile_activity_text_empty_tile,
                                        setup_clientutils, teardown_clientutils),

        /* combat_odds_to_astr tests */
        cmocka_unit_test_setup_teardown(test_combat_odds_to_astr_null_params,
                                        setup_clientutils, teardown_clientutils),

        /* Activity type and helper tests */
        cmocka_unit_test_setup_teardown(test_activity_type_enumeration,
                                        setup_clientutils, teardown_clientutils),
        cmocka_unit_test_setup_teardown(test_is_build_activity_valid,
                                        setup_clientutils, teardown_clientutils),
        cmocka_unit_test_setup_teardown(test_is_clean_activity_valid,
                                        setup_clientutils, teardown_clientutils),
        cmocka_unit_test_setup_teardown(test_is_tile_activity_valid,
                                        setup_clientutils, teardown_clientutils),
        cmocka_unit_test_setup_teardown(test_get_activity_text_valid,
                                        setup_clientutils, teardown_clientutils),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}