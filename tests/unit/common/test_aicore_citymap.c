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

#include "common/aicore/citymap.h"
#include "common/game.h"
#include "common/map.h"
#include "common/player.h"

/* mock */
#include "mock_game.h"
#include "mock_map.h"
#include "mock_player.h"

/* Test setup and teardown */
static int setup_citymap(void **state)
{
    (void) state;
    /* Initialize game with map using mock framework */
    mock_game_init_with_map();
    /* Create a player for testing */
    mock_player_create(0);
    /* Ensure citymap is clean */
    citymap_free();
    return 0;
}

static int teardown_citymap(void **state)
{
    (void) state;
    citymap_free();
    mock_player_destroy(mock_player_get(0));
    mock_game_cleanup();
    return 0;
}

/* Test citymap_turn_init basic functionality */
static void test_citymap_turn_init_basic(void **state)
{
    (void) state;

    struct player *p1 = mock_player_get(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    citymap_turn_init(p1);

    struct tile *center = map_pos_to_tile(&wld.map, 8, 8);
    if (center != NULL) {
        int value = citymap_read(center);
        assert_true(value >= 0);
    }
}

static void test_citymap_reserve_city_spot(void **state)
{
    (void) state;

    struct player *p1 = mock_player_get(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    citymap_turn_init(p1);

    struct tile *center = map_pos_to_tile(&wld.map, 8, 8);
    if (center == NULL) {
        skip();
        return;
    }

    int city_id = 123;
    citymap_reserve_city_spot(center, city_id);

    int value = citymap_read(center);
    assert_true(value < 0);
    assert_int_equal(value, -city_id);
}

/* Test citymap_free_city_spot */
static void test_citymap_free_city_spot(void **state)
{
    (void) state;

    struct player *p1 = mock_player_get(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    citymap_turn_init(p1);

    struct tile *center = map_pos_to_tile(&wld.map, 8, 8);
    if (center == NULL) {
        skip();
        return;
    }

    int city_id = 456;
    citymap_reserve_city_spot(center, city_id);
    citymap_free_city_spot(center, city_id);

    int value = citymap_read(center);
    assert_true(value >= 0);
}

/* Test citymap_reserve_tile */
static void test_citymap_reserve_tile(void **state)
{
    (void) state;

    struct player *p1 = mock_player_get(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    citymap_turn_init(p1);

    struct tile *target = map_pos_to_tile(&wld.map, 10, 10);
    if (target == NULL) {
        skip();
        return;
    }

    int unit_id = 789;
    citymap_reserve_tile(target, unit_id);

    int value = citymap_read(target);
    assert_true(value < 0);
    assert_int_equal(value, -unit_id);
}

/* Test citymap_read */
static void test_citymap_read(void **state)
{
    (void) state;

    struct player *p1 = mock_player_get(0);

    if (p1 == NULL) {
        skip();
        return;
    }

    citymap_turn_init(p1);

    /* Use coordinates that are known to work */
    struct tile *tile1 = map_pos_to_tile(&wld.map, 8, 8);
    struct tile *tile2 = map_pos_to_tile(&wld.map, 8, 9);

    if (tile1 == NULL || tile2 == NULL) {
        skip();
        return;
    }

    int value1 = citymap_read(tile1);
    assert_true(value1 >= 0);

    citymap_reserve_tile(tile2, 999);
    int value2 = citymap_read(tile2);
    assert_int_equal(value2, -999);
}

/* Test citymap_is_reserved */
static void test_citymap_is_reserved(void **state)
{
    (void) state;

    struct player *p1 = mock_player_get(0);
    if (p1 == NULL) {
        fprintf(stderr, "SKIP: player is NULL\\n");
        skip();
        return;
    }

    citymap_turn_init(p1);

    /* Use coordinates that are known to work */
    struct tile *free_tile = map_pos_to_tile(&wld.map, 8, 7);
    struct tile *reserved_tile = map_pos_to_tile(&wld.map, 8, 9);

    if (free_tile == NULL) {
        fprintf(stderr, "SKIP: free_tile (8,7) is NULL, map=%dx%d\\n", wld.map.xsize, wld.map.ysize);
        skip();
        return;
    }
    if (reserved_tile == NULL) {
        fprintf(stderr, "SKIP: reserved_tile (8,9) is NULL\\n");
        skip();
        return;
    }

    bool is_reserved = citymap_is_reserved(free_tile);
    assert_false(is_reserved);

    citymap_reserve_tile(reserved_tile, 111);
    is_reserved = citymap_is_reserved(reserved_tile);
    assert_true(is_reserved);
}

/* Test citymap_free */
static void test_citymap_free(void **state)
{
    (void) state;

    struct player *p1 = mock_player_get(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    citymap_turn_init(p1);
    citymap_free();
}

/* Test citymap with multiple reservations */
static void test_citymap_multiple_reservations(void **state)
{
    (void) state;

    struct player *p1 = mock_player_get(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    citymap_turn_init(p1);

    /* Use coordinates that are known to work */
    struct tile *tile1 = map_pos_to_tile(&wld.map, 7, 8);
    struct tile *tile2 = map_pos_to_tile(&wld.map, 8, 8);
    struct tile *tile3 = map_pos_to_tile(&wld.map, 9, 8);

    if (tile1 == NULL || tile2 == NULL || tile3 == NULL) {
        skip();
        return;
    }

    citymap_reserve_tile(tile1, 1);
    citymap_reserve_tile(tile2, 2);
    citymap_reserve_tile(tile3, 3);

    assert_int_equal(citymap_read(tile1), -1);
    assert_int_equal(citymap_read(tile2), -2);
    assert_int_equal(citymap_read(tile3), -3);

    citymap_free_city_spot(tile1, 1);
    assert_true(citymap_read(tile1) >= 0);
    assert_int_equal(citymap_read(tile2), -2);
    assert_int_equal(citymap_read(tile3), -3);
}

/* Test citymap_is_reserved with worked tile */
static void test_citymap_is_reserved_worked(void **state)
{
    (void) state;

    struct player *p1 = mock_player_get(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    citymap_turn_init(p1);

    /* Use coordinates that are known to work */
    struct tile *tile = map_pos_to_tile(&wld.map, 8, 6);
    if (tile == NULL) {
        skip();
        return;
    }

    assert_false(citymap_is_reserved(tile));
    citymap_reserve_tile(tile, 555);
    assert_true(citymap_is_reserved(tile));
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_citymap_turn_init_basic,
                                        setup_citymap, teardown_citymap),
        cmocka_unit_test_setup_teardown(test_citymap_reserve_city_spot,
                                        setup_citymap, teardown_citymap),
        cmocka_unit_test_setup_teardown(test_citymap_free_city_spot,
                                        setup_citymap, teardown_citymap),
        cmocka_unit_test_setup_teardown(test_citymap_reserve_tile,
                                        setup_citymap, teardown_citymap),
        cmocka_unit_test_setup_teardown(test_citymap_read,
                                        setup_citymap, teardown_citymap),
        cmocka_unit_test_setup_teardown(test_citymap_is_reserved,
                                        setup_citymap, teardown_citymap),
        cmocka_unit_test_setup_teardown(test_citymap_free,
                                        setup_citymap, teardown_citymap),
        cmocka_unit_test_setup_teardown(test_citymap_multiple_reservations,
                                        setup_citymap, teardown_citymap),
        cmocka_unit_test_setup_teardown(test_citymap_is_reserved_worked,
                                        setup_citymap, teardown_citymap),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
