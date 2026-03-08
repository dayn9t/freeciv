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

#include "common/aicore/aisupport.h"
#include "common/game.h"

/* Test setup and teardown */
static int setup_aisupport(void **state)
{
    (void) state;
    game_init(false);
    return 0;
}

static int teardown_aisupport(void **state)
{
    (void) state;
    game_free();
    return 0;
}

/* Test player_leading_spacerace when no one is building spaceship */
static void test_player_leading_spacerace(void **state)
{
    (void) state;

    /* With no players building spaceship, should return NULL */
    struct player *leader = player_leading_spacerace();
    /* Result depends on game configuration - just verify no crash */
    assert_null(leader);
}

/* Test player_distance_to_player with same player */
static void test_player_distance_to_player_same_player(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    /* Distance to self should return 1 */
    int dist = player_distance_to_player(p1, p1);
    assert_int_equal(dist, 1);
}

/* Test player_distance_to_player with dead player */
static void test_player_distance_to_player_dead_player(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    struct player *p2 = player_by_number(1);

    if (p1 == NULL || p2 == NULL) {
        skip();
        return;
    }

    /* Set target player as not alive */
    p2->is_alive = FALSE;

    /* Distance to dead player should return 1 */
    int dist = player_distance_to_player(p1, p2);
    assert_int_equal(dist, 1);
}

/* Test player_distance_to_player with source player dead */
static void test_player_distance_to_player_source_dead(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    struct player *p2 = player_by_number(1);

    if (p1 == NULL || p2 == NULL) {
        skip();
        return;
    }

    /* Set source player as not alive */
    p1->is_alive = FALSE;

    /* Distance from dead player should return 1 */
    int dist = player_distance_to_player(p1, p2);
    assert_int_equal(dist, 1);
}

/* Test city_gold_worth basic functionality */
static void test_city_gold_worth_basic(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    /* Try to get a city - if no cities exist, test will skip */
    if (city_list_size(p1->cities) == 0) {
        skip();
        return;
    }

    struct city *pcity = city_list_get(p1->cities, 0);
    if (pcity == NULL) {
        skip();
        return;
    }

    /* Just verify the function doesn't crash and returns a value */
    int worth = city_gold_worth(&wld.map, pcity);
    assert_true(worth >= 0);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_player_leading_spacerace,
                                        setup_aisupport, teardown_aisupport),
        cmocka_unit_test_setup_teardown(test_player_distance_to_player_same_player,
                                        setup_aisupport, teardown_aisupport),
        cmocka_unit_test_setup_teardown(test_player_distance_to_player_dead_player,
                                        setup_aisupport, teardown_aisupport),
        cmocka_unit_test_setup_teardown(test_player_distance_to_player_source_dead,
                                        setup_aisupport, teardown_aisupport),
        cmocka_unit_test_setup_teardown(test_city_gold_worth_basic,
                                        setup_aisupport, teardown_aisupport),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
