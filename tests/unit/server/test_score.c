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

/* common */
#include "game.h"
#include "player.h"
#include "city.h"
#include "map.h"
#include "support.h"

/* mock */
#include "mock_game.h"
#include "mock_player.h"
#include "mock_map.h"
#include "mock_city.h"

/* server */
#include "score.h"

/* Test setup and teardown */
static int setup_score(void **state)
{
    (void) state;
    mock_game_init_with_map();
    return 0;
}

static int teardown_score(void **state)
{
    (void) state;
    mock_game_cleanup();
    return 0;
}

/* Test total_player_citizens with empty player */
static void test_total_player_citizens_empty(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    /* Empty player should have 0 citizens */
    int total_citizens = total_player_citizens(pplayer);
    assert_int_equal(total_citizens, 0);

    mock_player_destroy(pplayer);
}

/* Test total_player_citizens with happy citizens */
static void test_total_player_citizens_with_citizens(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    /* Set up some happy citizens */
    pplayer->score.happy = 10;
    pplayer->score.content = 5;
    pplayer->score.unhappy = 3;
    pplayer->score.angry = 2;

    int total_citizens = total_player_citizens(pplayer);
    assert_int_equal(total_citizens, 20);  /* 10 + 5 + 3 + 2 */

    mock_player_destroy(pplayer);
}

/* Test total_player_citizens with specialists */
static void test_total_player_citizens_with_specialists(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    /* Set up citizens and specialists */
    pplayer->score.happy = 10;
    pplayer->score.specialists[0] = 2;
    pplayer->score.specialists[1] = 3;

    int total_citizens = total_player_citizens(pplayer);
    assert_true(total_citizens >= 10);

    mock_player_destroy(pplayer);
}

/* Test get_civ_score with minimal player */
static void test_get_civ_score_minimal(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    int score = get_civ_score(pplayer);
    assert_true(score >= 0);

    mock_player_destroy(pplayer);
}

/* Test get_civ_score with techs */
static void test_get_civ_score_with_techs(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    pplayer->score.techs = 10;

    int score = get_civ_score(pplayer);
    assert_true(score >= 20);

    mock_player_destroy(pplayer);
}

/* Test get_civ_score with wonders */
static void test_get_civ_score_with_wonders(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    pplayer->score.wonders = 3;

    int score = get_civ_score(pplayer);
    assert_true(score >= 15);

    mock_player_destroy(pplayer);
}

/* Test get_civ_score with culture */
static void test_get_civ_score_with_culture(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    pplayer->score.culture = 500;

    int score = get_civ_score(pplayer);
    assert_true(score >= 10);

    mock_player_destroy(pplayer);
}

/* Test get_civ_score with units */
static void test_get_civ_score_with_units(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    pplayer->score.units_built = 100;
    pplayer->score.units_killed = 30;

    int score = get_civ_score(pplayer);
    assert_true(score >= 0);

    mock_player_destroy(pplayer);
}

/* Test calc_civ_score initializes all score fields */
static void test_calc_civ_score_initializes_fields(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    pplayer->score.happy = 999;
    pplayer->score.wonders = 999;
    pplayer->score.techs = 999;

    calc_civ_score(pplayer);

    assert_true(pplayer->score.happy >= 0);
    assert_true(pplayer->score.wonders >= 0);
    assert_true(pplayer->score.techs >= 0);

    mock_player_destroy(pplayer);
}

/* Test calc_civ_score with barbarian player */
static void test_calc_civ_score_barbarian(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    pplayer->ai_common.barbarian_type = 1;
    pplayer->score.happy = 100;

    calc_civ_score(pplayer);

    assert_int_equal(pplayer->score.happy, 0);
    assert_int_equal(pplayer->score.wonders, 0);
    assert_int_equal(pplayer->score.techs, 0);

    mock_player_destroy(pplayer);
}

/* Test calc_civ_score with city */
static void test_calc_civ_score_with_city(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    struct city *pcity = mock_city_create(pplayer, 10, 10);
    assert_non_null(pcity);

    calc_civ_score(pplayer);

    assert_true(pplayer->score.cities >= 1);
    assert_true(pplayer->score.population >= 0);

    mock_city_destroy(pcity);
    mock_player_destroy(pplayer);
}

/* Test rank_users does not crash */
static void test_rank_users_basic(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    rank_users(false);
    rank_users(true);

    mock_player_destroy(pplayer);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_total_player_citizens_empty,
                                        setup_score, teardown_score),
        cmocka_unit_test_setup_teardown(test_total_player_citizens_with_citizens,
                                        setup_score, teardown_score),
        cmocka_unit_test_setup_teardown(test_total_player_citizens_with_specialists,
                                        setup_score, teardown_score),
        cmocka_unit_test_setup_teardown(test_get_civ_score_minimal,
                                        setup_score, teardown_score),
        cmocka_unit_test_setup_teardown(test_get_civ_score_with_techs,
                                        setup_score, teardown_score),
        cmocka_unit_test_setup_teardown(test_get_civ_score_with_wonders,
                                        setup_score, teardown_score),
        cmocka_unit_test_setup_teardown(test_get_civ_score_with_culture,
                                        setup_score, teardown_score),
        cmocka_unit_test_setup_teardown(test_get_civ_score_with_units,
                                        setup_score, teardown_score),
        cmocka_unit_test_setup_teardown(test_calc_civ_score_initializes_fields,
                                        setup_score, teardown_score),
        cmocka_unit_test_setup_teardown(test_calc_civ_score_barbarian,
                                        setup_score, teardown_score),
        cmocka_unit_test_setup_teardown(test_calc_civ_score_with_city,
                                        setup_score, teardown_score),
        cmocka_unit_test_setup_teardown(test_rank_users_basic,
                                        setup_score, teardown_score),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
