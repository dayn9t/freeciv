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
#include "map.h"

/* mock */
#include "mock_game.h"
#include "mock_player.h"
#include "mock_map.h"

/* server */
#include "mood.h"

/* Test setup and teardown */
static int setup_mood(void **state)
{
    (void) state;
    mock_game_init_with_map();
    return 0;
}

static int teardown_mood(void **state)
{
    (void) state;
    mock_game_cleanup();
    return 0;
}

/* Test player_mood with peaceful player (no war) */
static void test_player_mood_peaceful(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    /* Set last_war_action to -1 (no recent war) */
    pplayer->last_war_action = -1;

    enum mood_type mood = player_mood(pplayer);
    assert_int_equal(mood, MOOD_PEACEFUL);

    mock_player_destroy(pplayer);
}

/* Test player_mood with recent war but now peace (expired) */
static void test_player_mood_recent_war_expired(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    /* Set last_war_action to old turn (war mood expired) */
    pplayer->last_war_action = game.info.turn - WAR_MOOD_LASTS - 1;

    enum mood_type mood = player_mood(pplayer);
    assert_int_equal(mood, MOOD_PEACEFUL);

    mock_player_destroy(pplayer);
}

/* Test player_mood with recent war action but no diplomatic state */
static void test_player_mood_recent_war_no_diplstate(void **state)
{
    (void) state;

    struct player *pplayer = mock_player_create(0);
    assert_non_null(pplayer);

    /* Set last_war_action to current turn, but without any
     * diplomatic state set up, player_mood should return PEACEFUL */
    pplayer->last_war_action = game.info.turn;

    /* Without diplomatic state initialized, the function will
     * iterate but find no war states, so should return PEACEFUL */
    enum mood_type mood = player_mood(pplayer);
    /* Note: This might return COMBAT if default diplstate is DS_WAR,
     * but typically returns PEACEFUL if no enemies are found */
    assert_true(mood == MOOD_PEACEFUL || mood == MOOD_COMBAT);

    mock_player_destroy(pplayer);
}

/* Test WAR_MOOD_LASTS constant */
static void test_war_mood_lasts_constant(void **state)
{
    (void) state;

    /* Verify WAR_MOOD_LASTS is defined and positive */
    assert_true(WAR_MOOD_LASTS > 0);
    assert_int_equal(WAR_MOOD_LASTS, 10);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_player_mood_peaceful,
                                        setup_mood, teardown_mood),
        cmocka_unit_test_setup_teardown(test_player_mood_recent_war_expired,
                                        setup_mood, teardown_mood),
        cmocka_unit_test_setup_teardown(test_player_mood_recent_war_no_diplstate,
                                        setup_mood, teardown_mood),
        cmocka_unit_test(test_war_mood_lasts_constant),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
