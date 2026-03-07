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

#include "common/culture.h"
#include "common/city.h"
#include "common/player.h"
#include "common/game.h"
#include "common/effects.h"

/* Test setup and teardown */
static int setup_culture(void **state)
{
    (void) state;
    game_init(false);
    ruleset_cache_init();
    return 0;
}

static int teardown_culture(void **state)
{
    (void) state;
    ruleset_cache_free();
    game_free();
    return 0;
}

/* Test city_culture with minimal city */
static void test_city_culture_minimal(void **state)
{
    (void) state;

    /* Create a minimal city for testing */
    struct city test_city;
    memset(&test_city, 0, sizeof(test_city));
    test_city.history = 0;

    /* Test city culture calculation */
    int culture = city_culture(&test_city);

    /* With no history and no effects, culture should be 0 */
    assert_int_equal(culture, 0);
}

/* Test city_culture with history */
static void test_city_culture_with_history(void **state)
{
    (void) state;

    /* Create a city with some history */
    struct city test_city;
    memset(&test_city, 0, sizeof(test_city));
    test_city.history = 100;

    /* Test city culture calculation */
    int culture = city_culture(&test_city);

    /* Culture should include the history value */
    assert_int_equal(culture, 100);
}

/* Test city_history_gain with minimal city */
static void test_city_history_gain_minimal(void **state)
{
    (void) state;

    /* Create a minimal city */
    struct city test_city;
    memset(&test_city, 0, sizeof(test_city));
    test_city.history = 0;

    /* Test history gain calculation */
    int gain = city_history_gain(&test_city);

    /* With no effects, gain should be based on game settings */
    assert_int_equal(gain, 0);
}

/* Test player_culture with minimal player */
static void test_player_culture_minimal(void **state)
{
    (void) state;

    /* Create a minimal player */
    struct player test_player;
    memset(&test_player, 0, sizeof(test_player));
    test_player.history = 0;
    test_player.cities = city_list_new();

    /* Test player culture calculation */
    int culture = player_culture(&test_player);

    /* With no history and no cities, culture should be 0 */
    assert_int_equal(culture, 0);

    city_list_destroy(test_player.cities);
}

/* Test player_culture with history */
static void test_player_culture_with_history(void **state)
{
    (void) state;

    /* Create a player with some history */
    struct player test_player;
    memset(&test_player, 0, sizeof(test_player));
    test_player.history = 50;
    test_player.cities = city_list_new();

    /* Test player culture calculation */
    int culture = player_culture(&test_player);

    /* Culture should include the history value */
    assert_int_equal(culture, 50);

    city_list_destroy(test_player.cities);
}

/* Test nation_history_gain with minimal player */
static void test_nation_history_gain_minimal(void **state)
{
    (void) state;

    /* Create a minimal player */
    struct player test_player;
    memset(&test_player, 0, sizeof(test_player));
    test_player.history = 0;

    /* Test nation history gain calculation */
    int gain = nation_history_gain(&test_player);

    /* With no effects, gain should be 0 */
    assert_int_equal(gain, 0);
}

/* Test nation_history_gain with history */
static void test_nation_history_gain_with_history(void **state)
{
    (void) state;

    /* Create a player with some history */
    struct player test_player;
    memset(&test_player, 0, sizeof(test_player));
    test_player.history = 100;

    /* Test nation history gain calculation with interest */
    int gain = nation_history_gain(&test_player);

    /* Gain should include interest from existing history */
    /* game.info.history_interest_pml is typically 0 by default */
    assert_int_equal(gain, 0);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_city_culture_minimal,
                                        setup_culture, teardown_culture),
        cmocka_unit_test_setup_teardown(test_city_culture_with_history,
                                        setup_culture, teardown_culture),
        cmocka_unit_test_setup_teardown(test_city_history_gain_minimal,
                                        setup_culture, teardown_culture),
        cmocka_unit_test_setup_teardown(test_player_culture_minimal,
                                        setup_culture, teardown_culture),
        cmocka_unit_test_setup_teardown(test_player_culture_with_history,
                                        setup_culture, teardown_culture),
        cmocka_unit_test_setup_teardown(test_nation_history_gain_minimal,
                                        setup_culture, teardown_culture),
        cmocka_unit_test_setup_teardown(test_nation_history_gain_with_history,
                                        setup_culture, teardown_culture),
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}
