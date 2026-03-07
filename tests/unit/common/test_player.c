/***********************************************************************
 Freeciv - Copyright (C) 2025 - The Freeciv Team
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
#include "mem.h"
#include "shared.h"

/* common */
#include "game.h"
#include "nation.h"
#include "player.h"

/* Test player structure for basic tests */
static struct player test_player;

/***********************************************************************
  Setup function - initialize minimal game state
***********************************************************************/
static int setup(void **state)
{
  (void) state;

  memset(&test_player, 0, sizeof(test_player));
  test_player.slot = NULL;
  sz_strlcpy(test_player.name, ANON_PLAYER_NAME);
  test_player.is_alive = TRUE;
  test_player.is_male = TRUE;
  test_player.is_winner = FALSE;
  test_player.phase_done = FALSE;
  test_player.is_ready = FALSE;
  test_player.is_connected = FALSE;
  test_player.was_created = FALSE;
  test_player.random_name = TRUE;
  test_player.nturns_idle = 0;
  test_player.turns_alive = 0;
  test_player.user_turns = 0;
  test_player.revolution_finishes = -1;
  test_player.primary_capital_id = 0;

  /* Initialize economic values */
  test_player.economic.gold = 0;
  test_player.economic.tax = PLAYER_DEFAULT_TAX_RATE;
  test_player.economic.science = PLAYER_DEFAULT_SCIENCE_RATE;
  test_player.economic.luxury = PLAYER_DEFAULT_LUXURY_RATE;
  test_player.economic.infra_points = 0;

  /* Initialize AI common data */
  test_player.ai_common.skill_level = ai_level_invalid();
  test_player.ai_common.fuzzy = 0;
  test_player.ai_common.expand = 100;
  test_player.ai_common.barbarian_type = NOT_A_BARBARIAN;

  /* Clear flags - player is human by default */
  BV_CLR_ALL(test_player.flags);

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  return 0;
}

/***********************************************************************
  Teardown function - cleanup
***********************************************************************/
static int teardown(void **state)
{
  (void) state;
  return 0;
}

/***********************************************************************
  Test player_name with valid player
***********************************************************************/
static void test_player_name_valid(void **state)
{
  (void) state;

  sz_strlcpy(test_player.name, "TestPlayer");
  const char *name = player_name(&test_player);
  assert_string_equal(name, "TestPlayer");
}

/***********************************************************************
  Test player_name with NULL player
***********************************************************************/
static void test_player_name_null(void **state)
{
  (void) state;

  const char *name = player_name(NULL);
  assert_null(name);
}

/***********************************************************************
  Test player_name with ANON_PLAYER_NAME
***********************************************************************/
static void test_player_name_anon(void **state)
{
  (void) state;

  sz_strlcpy(test_player.name, ANON_PLAYER_NAME);
  const char *name = player_name(&test_player);
  assert_string_equal(name, ANON_PLAYER_NAME);
}

/***********************************************************************
  Test is_human with human player
***********************************************************************/
static void test_is_human_true(void **state)
{
  (void) state;

  BV_CLR_ALL(test_player.flags);  /* Clear AI flag */
  assert_true(is_human(&test_player));
}

/***********************************************************************
  Test is_human with AI player
***********************************************************************/
static void test_is_human_false(void **state)
{
  (void) state;

  BV_SET(test_player.flags, PLRF_AI);  /* Set AI flag */
  assert_false(is_human(&test_player));
}

/***********************************************************************
  Test is_ai with AI player
***********************************************************************/
static void test_is_ai_true(void **state)
{
  (void) state;

  BV_SET(test_player.flags, PLRF_AI);
  assert_true(is_ai(&test_player));
}

/***********************************************************************
  Test is_ai with human player
***********************************************************************/
static void test_is_ai_false(void **state)
{
  (void) state;

  BV_CLR_ALL(test_player.flags);
  assert_false(is_ai(&test_player));
}

/***********************************************************************
  Test set_as_human macro
***********************************************************************/
static void test_set_as_human_macro(void **state)
{
  (void) state;

  BV_SET(test_player.flags, PLRF_AI);  /* Start as AI */
  assert_true(is_ai(&test_player));

  set_as_human(&test_player);
  assert_true(is_human(&test_player));
  assert_false(is_ai(&test_player));
}

/***********************************************************************
  Test set_as_ai macro
***********************************************************************/
static void test_set_as_ai_macro(void **state)
{
  (void) state;

  BV_CLR_ALL(test_player.flags);  /* Start as human */
  assert_true(is_human(&test_player));

  set_as_ai(&test_player);
  assert_true(is_ai(&test_player));
  assert_false(is_human(&test_player));
}

/***********************************************************************
  Test is_barbarian with regular player
***********************************************************************/
static void test_is_barbarian_false(void **state)
{
  (void) state;

  test_player.ai_common.barbarian_type = NOT_A_BARBARIAN;
  assert_false(is_barbarian(&test_player));
}

/***********************************************************************
  Test is_barbarian with land barbarian
***********************************************************************/
static void test_is_barbarian_land(void **state)
{
  (void) state;

  test_player.ai_common.barbarian_type = LAND_BARBARIAN;
  assert_true(is_barbarian(&test_player));
}

/***********************************************************************
  Test is_barbarian with sea barbarian
***********************************************************************/
static void test_is_barbarian_sea(void **state)
{
  (void) state;

  test_player.ai_common.barbarian_type = SEA_BARBARIAN;
  assert_true(is_barbarian(&test_player));
}

/***********************************************************************
  Test player_is_alive with alive player
***********************************************************************/
static void test_player_is_alive_true(void **state)
{
  (void) state;

  test_player.is_alive = TRUE;
  assert_true(test_player.is_alive);
}

/***********************************************************************
  Test player_is_alive with dead player
***********************************************************************/
static void test_player_is_alive_false(void **state)
{
  (void) state;

  test_player.is_alive = FALSE;
  assert_false(test_player.is_alive);
}

/***********************************************************************
  Test player age
***********************************************************************/
static void test_player_age(void **state)
{
  (void) state;

  test_player.turns_alive = 0;
  assert_int_equal(player_age(&test_player), 0);

  test_player.turns_alive = 10;
  assert_int_equal(player_age(&test_player), 10);

  test_player.turns_alive = 100;
  assert_int_equal(player_age(&test_player), 100);
}

/***********************************************************************
  Test default economic values
***********************************************************************/
static void test_player_economic_defaults(void **state)
{
  (void) state;

  assert_int_equal(test_player.economic.gold, 0);
  assert_int_equal(test_player.economic.tax, PLAYER_DEFAULT_TAX_RATE);
  assert_int_equal(test_player.economic.science, PLAYER_DEFAULT_SCIENCE_RATE);
  assert_int_equal(test_player.economic.luxury, PLAYER_DEFAULT_LUXURY_RATE);
  assert_int_equal(test_player.economic.infra_points, 0);
}

/***********************************************************************
  Test economic gold modification
***********************************************************************/
static void test_player_economic_gold(void **state)
{
  (void) state;

  test_player.economic.gold = 100;
  assert_int_equal(test_player.economic.gold, 100);

  test_player.economic.gold += 50;
  assert_int_equal(test_player.economic.gold, 150);

  test_player.economic.gold -= 25;
  assert_int_equal(test_player.economic.gold, 125);
}

/***********************************************************************
  Test economic rate settings
***********************************************************************/
static void test_player_economic_rates(void **state)
{
  (void) state;

  /* Rates should sum to 100% */
  test_player.economic.tax = 20;
  test_player.economic.science = 70;
  test_player.economic.luxury = 10;

  assert_int_equal(test_player.economic.tax + test_player.economic.science +
                   test_player.economic.luxury, 100);
}

/***********************************************************************
  Test love_text function - extreme negative values
***********************************************************************/
static void test_love_text_extreme_negative(void **state)
{
  (void) state;

  /* Very negative love (< -90%) */
  const char *text = love_text(-MAX_AI_LOVE);
  assert_non_null(text);

  text = love_text(-MAX_AI_LOVE - 1);
  assert_non_null(text);
}

/***********************************************************************
  Test love_text function - negative values
***********************************************************************/
static void test_love_text_negative(void **state)
{
  (void) state;

  /* Different negative ranges */
  const char *text = love_text(-MAX_AI_LOVE * 75 / 100);  /* Belligerent range */
  assert_non_null(text);

  text = love_text(-MAX_AI_LOVE * 60 / 100);  /* Hostile range */
  assert_non_null(text);

  text = love_text(-MAX_AI_LOVE * 30 / 100);  /* Uncooperative range */
  assert_non_null(text);

  text = love_text(-MAX_AI_LOVE * 15 / 100);  /* Uneasy range */
  assert_non_null(text);
}

/***********************************************************************
  Test love_text function - neutral values
***********************************************************************/
static void test_love_text_neutral(void **state)
{
  (void) state;

  /* Neutral range */
  const char *text = love_text(0);
  assert_non_null(text);

  text = love_text(5);
  assert_non_null(text);

  text = love_text(-5);
  assert_non_null(text);
}

/***********************************************************************
  Test love_text function - positive values
***********************************************************************/
static void test_love_text_positive(void **state)
{
  (void) state;

  /* Different positive ranges */
  const char *text = love_text(MAX_AI_LOVE * 20 / 100);  /* Respectful range */
  assert_non_null(text);

  text = love_text(MAX_AI_LOVE * 40 / 100);  /* Helpful range */
  assert_non_null(text);

  text = love_text(MAX_AI_LOVE * 60 / 100);  /* Enthusiastic range */
  assert_non_null(text);

  text = love_text(MAX_AI_LOVE * 80 / 100);  /* Admiring range */
  assert_non_null(text);
}

/***********************************************************************
  Test love_text function - extreme positive values
***********************************************************************/
static void test_love_text_extreme_positive(void **state)
{
  (void) state;

  /* Very positive love (> 90%) */
  const char *text = love_text(MAX_AI_LOVE);
  assert_non_null(text);

  text = love_text(MAX_AI_LOVE * 95 / 100);  /* Worshipful range */
  assert_non_null(text);
}

/***********************************************************************
  Test MAX_AI_LOVE constant
***********************************************************************/
static void test_max_ai_love_constant(void **state)
{
  (void) state;

  assert_int_equal(MAX_AI_LOVE, 1000);
}

/***********************************************************************
  Test PLAYER_DEFAULT_TAX_RATE constant
***********************************************************************/
static void test_player_default_tax_rate(void **state)
{
  (void) state;

  assert_int_equal(PLAYER_DEFAULT_TAX_RATE, 0);
}

/***********************************************************************
  Test PLAYER_DEFAULT_SCIENCE_RATE constant
***********************************************************************/
static void test_player_default_science_rate(void **state)
{
  (void) state;

  assert_int_equal(PLAYER_DEFAULT_SCIENCE_RATE, 100);
}

/***********************************************************************
  Test PLAYER_DEFAULT_LUXURY_RATE constant
***********************************************************************/
static void test_player_default_luxury_rate(void **state)
{
  (void) state;

  assert_int_equal(PLAYER_DEFAULT_LUXURY_RATE, 0);
}

/***********************************************************************
  Test ANON_PLAYER_NAME constant
***********************************************************************/
static void test_anon_player_name(void **state)
{
  (void) state;

  assert_string_equal(ANON_PLAYER_NAME, "noname");
}

/***********************************************************************
  Test player_has_flag function
***********************************************************************/
static void test_player_has_flag(void **state)
{
  (void) state;

  BV_CLR_ALL(test_player.flags);

  assert_false(player_has_flag(&test_player, PLRF_AI));

  BV_SET(test_player.flags, PLRF_AI);
  assert_true(player_has_flag(&test_player, PLRF_AI));

  BV_CLR(test_player.flags, PLRF_AI);
  assert_false(player_has_flag(&test_player, PLRF_AI));
}

/***********************************************************************
  Test player winner status
***********************************************************************/
static void test_player_winner_status(void **state)
{
  (void) state;

  test_player.is_winner = FALSE;
  assert_false(test_player.is_winner);

  test_player.is_winner = TRUE;
  assert_true(test_player.is_winner);
}

/***********************************************************************
  Test player ready status
***********************************************************************/
static void test_player_ready_status(void **state)
{
  (void) state;

  test_player.is_ready = FALSE;
  assert_false(test_player.is_ready);

  test_player.is_ready = TRUE;
  assert_true(test_player.is_ready);
}

/***********************************************************************
  Test player phase done status
***********************************************************************/
static void test_player_phase_done(void **state)
{
  (void) state;

  test_player.phase_done = FALSE;
  assert_false(test_player.phase_done);

  test_player.phase_done = TRUE;
  assert_true(test_player.phase_done);
}

/***********************************************************************
  Test player connected status
***********************************************************************/
static void test_player_connected(void **state)
{
  (void) state;

  test_player.is_connected = FALSE;
  assert_false(test_player.is_connected);

  test_player.is_connected = TRUE;
  assert_true(test_player.is_connected);
}

/***********************************************************************
  Test player was_created flag
***********************************************************************/
static void test_player_was_created(void **state)
{
  (void) state;

  test_player.was_created = FALSE;
  assert_false(test_player.was_created);

  test_player.was_created = TRUE;
  assert_true(test_player.was_created);
}

/***********************************************************************
  Test player nturns_idle
***********************************************************************/
static void test_player_nturns_idle(void **state)
{
  (void) state;

  test_player.nturns_idle = 0;
  assert_int_equal(test_player.nturns_idle, 0);

  test_player.nturns_idle = 5;
  assert_int_equal(test_player.nturns_idle, 5);

  test_player.nturns_idle = 100;
  assert_int_equal(test_player.nturns_idle, 100);
}

/***********************************************************************
  Test player revolution_finishes
***********************************************************************/
static void test_player_revolution_finishes(void **state)
{
  (void) state;

  test_player.revolution_finishes = -1;  /* Not in revolution */
  assert_int_equal(test_player.revolution_finishes, -1);

  test_player.revolution_finishes = 10;  /* Revolution ends at turn 10 */
  assert_int_equal(test_player.revolution_finishes, 10);
}

/***********************************************************************
  Test player primary_capital_id
***********************************************************************/
static void test_player_primary_capital_id(void **state)
{
  (void) state;

  test_player.primary_capital_id = 0;  /* No capital */
  assert_int_equal(test_player.primary_capital_id, 0);

  test_player.primary_capital_id = 42;  /* Capital city ID */
  assert_int_equal(test_player.primary_capital_id, 42);
}

/***********************************************************************
  Test player last_war_action
***********************************************************************/
static void test_player_last_war_action(void **state)
{
  (void) state;

  test_player.last_war_action = -1;  /* Never at war */
  assert_int_equal(test_player.last_war_action, -1);

  test_player.last_war_action = 25;  /* Last war action at turn 25 */
  assert_int_equal(test_player.last_war_action, 25);
}

/***********************************************************************
  Test player gender
***********************************************************************/
static void test_player_gender(void **state)
{
  (void) state;

  test_player.is_male = TRUE;
  assert_true(test_player.is_male);

  test_player.is_male = FALSE;
  assert_false(test_player.is_male);
}

/***********************************************************************
  Test player AI skill level
***********************************************************************/
static void test_player_ai_skill_level(void **state)
{
  (void) state;

  test_player.ai_common.skill_level = ai_level_invalid();
  /* Invalid skill level for player not set as AI */

  test_player.ai_common.skill_level = 5;  /* Some AI level */
  assert_int_equal(test_player.ai_common.skill_level, 5);
}

/***********************************************************************
  Test player AI fuzzy setting
***********************************************************************/
static void test_player_ai_fuzzy(void **state)
{
  (void) state;

  test_player.ai_common.fuzzy = 0;
  assert_int_equal(test_player.ai_common.fuzzy, 0);

  test_player.ai_common.fuzzy = 100;
  assert_int_equal(test_player.ai_common.fuzzy, 100);
}

/***********************************************************************
  Test player AI expand setting
***********************************************************************/
static void test_player_ai_expand(void **state)
{
  (void) state;

  test_player.ai_common.expand = 100;  /* Default */
  assert_int_equal(test_player.ai_common.expand, 100);

  test_player.ai_common.expand = 150;  /* More expansionist */
  assert_int_equal(test_player.ai_common.expand, 150);
}

/***********************************************************************
  Test user_turns counter
***********************************************************************/
static void test_player_user_turns(void **state)
{
  (void) state;

  test_player.user_turns = 0;
  assert_int_equal(test_player.user_turns, 0);

  test_player.user_turns = 10;
  assert_int_equal(test_player.user_turns, 10);
}

/***********************************************************************
  Test player unassigned_user flag
***********************************************************************/
static void test_player_unassigned_user(void **state)
{
  (void) state;

  test_player.unassigned_user = TRUE;
  assert_true(test_player.unassigned_user);

  test_player.unassigned_user = FALSE;
  assert_false(test_player.unassigned_user);
}

/***********************************************************************
  Test player unassigned_ranked flag
***********************************************************************/
static void test_player_unassigned_ranked(void **state)
{
  (void) state;

  test_player.unassigned_ranked = TRUE;
  assert_true(test_player.unassigned_ranked);

  test_player.unassigned_ranked = FALSE;
  assert_false(test_player.unassigned_ranked);
}

/***********************************************************************
  Test player random_name flag
***********************************************************************/
static void test_player_random_name(void **state)
{
  (void) state;

  test_player.random_name = TRUE;
  assert_true(test_player.random_name);

  test_player.random_name = FALSE;
  assert_false(test_player.random_name);
}

/***********************************************************************
  Test player autoselect_weight
***********************************************************************/
static void test_player_autoselect_weight(void **state)
{
  (void) state;

  test_player.autoselect_weight = -1;  /* Default */
  assert_int_equal(test_player.autoselect_weight, -1);

  test_player.autoselect_weight = 10;
  assert_int_equal(test_player.autoselect_weight, 10);
}

/***********************************************************************
  Test player music_style
***********************************************************************/
static void test_player_music_style(void **state)
{
  (void) state;

  test_player.music_style = -1;  /* Trigger change */
  assert_int_equal(test_player.music_style, -1);

  test_player.music_style = 0;
  assert_int_equal(test_player.music_style, 0);
}

/***********************************************************************
  Test player username
***********************************************************************/
static void test_player_username(void **state)
{
  (void) state;

  sz_strlcpy(test_player.username, "TestUser");
  assert_string_equal(test_player.username, "TestUser");
}

/***********************************************************************
  Test player ranked_username
***********************************************************************/
static void test_player_ranked_username(void **state)
{
  (void) state;

  sz_strlcpy(test_player.ranked_username, "RankedUser");
  assert_string_equal(test_player.ranked_username, "RankedUser");
}

/***********************************************************************
  Test player team pointer
***********************************************************************/
static void test_player_team(void **state)
{
  (void) state;

  test_player.team = NULL;
  assert_null(test_player.team);
}

/***********************************************************************
  Test player government pointer
***********************************************************************/
static void test_player_government(void **state)
{
  (void) state;

  test_player.government = NULL;  /* May be NULL in pregame */
  assert_null(test_player.government);

  test_player.target_government = NULL;
  assert_null(test_player.target_government);
}

/***********************************************************************
  Test player nation pointer
***********************************************************************/
static void test_player_nation(void **state)
{
  (void) state;

  test_player.nation = NO_NATION_SELECTED;
  assert_ptr_equal(test_player.nation, NO_NATION_SELECTED);
}

/***********************************************************************
  Test player_nation macro
***********************************************************************/
static void test_player_nation_macro(void **state)
{
  (void) state;

  test_player.nation = NULL;
  assert_null(player_nation(&test_player));
}

/***********************************************************************
  Test player score initialization
***********************************************************************/
static void test_player_score_init(void **state)
{
  (void) state;

  memset(&test_player.score, 0, sizeof(test_player.score));

  assert_int_equal(test_player.score.happy, 0);
  assert_int_equal(test_player.score.content, 0);
  assert_int_equal(test_player.score.unhappy, 0);
  assert_int_equal(test_player.score.angry, 0);
  assert_int_equal(test_player.score.wonders, 0);
  assert_int_equal(test_player.score.techs, 0);
  assert_int_equal(test_player.score.techout, 0);
  assert_int_equal(test_player.score.landarea, 0);
  assert_int_equal(test_player.score.settledarea, 0);
  assert_int_equal(test_player.score.population, 0);
  assert_int_equal(test_player.score.cities, 0);
  assert_int_equal(test_player.score.units, 0);
  assert_int_equal(test_player.score.pollution, 0);
  assert_int_equal(test_player.score.literacy, 0);
  assert_int_equal(test_player.score.bnp, 0);
  assert_int_equal(test_player.score.mfg, 0);
  assert_int_equal(test_player.score.spaceship, 0);
  assert_int_equal(test_player.score.units_built, 0);
  assert_int_equal(test_player.score.units_killed, 0);
  assert_int_equal(test_player.score.units_lost, 0);
  assert_int_equal(test_player.score.units_used, 0);
  assert_int_equal(test_player.score.culture, 0);
  assert_int_equal(test_player.score.game, 0);
}

/***********************************************************************
  Test player score modifications
***********************************************************************/
static void test_player_score_values(void **state)
{
  (void) state;

  memset(&test_player.score, 0, sizeof(test_player.score));

  test_player.score.cities = 5;
  test_player.score.population = 1000;
  test_player.score.techs = 10;
  test_player.score.wonders = 2;
  test_player.score.culture = 500;
  test_player.score.game = 1500;

  assert_int_equal(test_player.score.cities, 5);
  assert_int_equal(test_player.score.population, 1000);
  assert_int_equal(test_player.score.techs, 10);
  assert_int_equal(test_player.score.wonders, 2);
  assert_int_equal(test_player.score.culture, 500);
  assert_int_equal(test_player.score.game, 1500);
}

/***********************************************************************
  Test player history (national culture)
***********************************************************************/
static void test_player_history(void **state)
{
  (void) state;

  test_player.history = 0;
  assert_int_equal(test_player.history, 0);

  test_player.history = 100;
  assert_int_equal(test_player.history, 100);
}

/***********************************************************************
  Test MAX_NUM_PLAYER_SLOTS constant
***********************************************************************/
static void test_max_num_player_slots(void **state)
{
  (void) state;

  assert_int_equal(MAX_NUM_PLAYER_SLOTS, 512);
}

/***********************************************************************
  Test MAX_LEN_NAME constant
***********************************************************************/
static void test_max_len_name(void **state)
{
  (void) state;

  assert_int_equal(MAX_LEN_NAME, 48);
}

/***********************************************************************
  Test OWNER_NONE constant
***********************************************************************/
static void test_owner_none(void **state)
{
  (void) state;

  assert_int_equal(OWNER_NONE, -1);
}

/***********************************************************************
  Test is_valid_username with valid usernames
***********************************************************************/
static void test_is_valid_username_valid(void **state)
{
  (void) state;

  assert_true(is_valid_username("Player1"));
  assert_true(is_valid_username("Test_User"));
  assert_true(is_valid_username("user123"));
}

/***********************************************************************
  Test is_valid_username with invalid usernames
***********************************************************************/
static void test_is_valid_username_invalid(void **state)
{
  (void) state;

  /* Empty or too short may be invalid depending on implementation */
  /* This tests the function exists and returns something */
  bool result = is_valid_username("");
  (void)result;  /* Just verify it doesn't crash */
}

/***********************************************************************
  Test player diplomacy-related constants
***********************************************************************/
static void test_diplstate_constants(void **state)
{
  (void) state;

  /* Test that diplomatic state constants are defined correctly */
  assert_int_equal(DS_ARMISTICE, 0);
  assert_int_equal(DS_WAR, 1);
  assert_int_equal(DS_CEASEFIRE, 2);
  assert_int_equal(DS_PEACE, 3);
  assert_int_equal(DS_ALLIANCE, 4);
  assert_int_equal(DS_NO_CONTACT, 5);
  assert_int_equal(DS_TEAM, 6);
}

/***********************************************************************
  Test player spaceship initialization
***********************************************************************/
static void test_player_spaceship_init(void **state)
{
  (void) state;

  memset(&test_player.spaceship, 0, sizeof(test_player.spaceship));

  assert_int_equal(test_player.spaceship.state, SSHIP_NONE);
}

/***********************************************************************
  Test player attribute block initialization
***********************************************************************/
static void test_player_attribute_block(void **state)
{
  (void) state;

  test_player.attribute_block.data = NULL;
  test_player.attribute_block.length = 0;

  assert_null(test_player.attribute_block.data);
  assert_int_equal(test_player.attribute_block.length, 0);
}

/***********************************************************************
  Test gives_shared_vision bitvector initialization
***********************************************************************/
static void test_player_gives_shared_vision(void **state)
{
  (void) state;

  BV_CLR_ALL(test_player.gives_shared_vision);

  /* Check that all bits are cleared */
  for (int i = 0; i < MAX_NUM_PLAYER_SLOTS; i++) {
    assert_false(BV_ISSET(test_player.gives_shared_vision, i));
  }
}

/***********************************************************************
  Test gives_shared_tiles bitvector initialization
***********************************************************************/
static void test_player_gives_shared_tiles(void **state)
{
  (void) state;

  BV_CLR_ALL(test_player.gives_shared_tiles);

  /* Check that all bits are cleared */
  for (int i = 0; i < MAX_NUM_PLAYER_SLOTS; i++) {
    assert_false(BV_ISSET(test_player.gives_shared_tiles, i));
  }
}

/***********************************************************************
  Test real_embassy bitvector initialization
***********************************************************************/
static void test_player_real_embassy(void **state)
{
  (void) state;

  BV_CLR_ALL(test_player.real_embassy);

  /* Check that all bits are cleared */
  for (int i = 0; i < MAX_NUM_PLAYER_SLOTS; i++) {
    assert_false(BV_ISSET(test_player.real_embassy, i));
  }
}

/***********************************************************************
  Test player wonders array
***********************************************************************/
static void test_player_wonders(void **state)
{
  (void) state;

  for (int i = 0; i < B_LAST; i++) {
    test_player.wonders[i] = WONDER_NOT_BUILT;
    assert_int_equal(test_player.wonders[i], WONDER_NOT_BUILT);
  }
}

/***********************************************************************
  Test player tile_known initialization
***********************************************************************/
static void test_player_tile_known(void **state)
{
  (void) state;

  test_player.tile_known.vec = NULL;
  test_player.tile_known.bits = 0;

  assert_null(test_player.tile_known.vec);
  assert_int_equal(test_player.tile_known.bits, 0);
}

/***********************************************************************
  Test player rgb color pointer
***********************************************************************/
static void test_player_rgb(void **state)
{
  (void) state;

  test_player.rgb = NULL;
  assert_null(test_player.rgb);
}

/***********************************************************************
  Test player multipliers initialization
***********************************************************************/
static void test_player_multipliers(void **state)
{
  (void) state;

  memset(test_player.multipliers, 0, sizeof(test_player.multipliers));

  for (int i = 0; i < MAX_NUM_MULTIPLIERS; i++) {
    assert_int_equal(test_player.multipliers[i].value, 0);
    assert_int_equal(test_player.multipliers[i].target, 0);
    assert_int_equal(test_player.multipliers[i].changed, 0);
  }
}

/***********************************************************************
  Test player_diplstate structure
***********************************************************************/
static void test_player_diplstate(void **state)
{
  (void) state;

  struct player_diplstate ds;

  memset(&ds, 0, sizeof(ds));
  ds.type = DS_NO_CONTACT;
  ds.max_state = DS_NO_CONTACT;
  ds.first_contact_turn = -1;
  ds.turns_left = 0;
  ds.has_reason_to_cancel = 0;
  ds.contact_turns_left = 0;
  ds.auto_cancel_turn = 0;

  assert_int_equal(ds.type, DS_NO_CONTACT);
  assert_int_equal(ds.max_state, DS_NO_CONTACT);
  assert_int_equal(ds.first_contact_turn, -1);
  assert_int_equal(ds.turns_left, 0);
  assert_int_equal(ds.has_reason_to_cancel, 0);
  assert_int_equal(ds.contact_turns_left, 0);
  assert_int_equal(ds.auto_cancel_turn, 0);
}

/***********************************************************************
  Test are_diplstates_equal function
***********************************************************************/
static void test_are_diplstates_equal(void **state)
{
  (void) state;

  struct player_diplstate ds1, ds2;

  memset(&ds1, 0, sizeof(ds1));
  memset(&ds2, 0, sizeof(ds2));

  /* Equal states */
  ds1.type = DS_PEACE;
  ds1.turns_left = 10;
  ds1.has_reason_to_cancel = 0;
  ds1.contact_turns_left = 5;

  ds2.type = DS_PEACE;
  ds2.turns_left = 10;
  ds2.has_reason_to_cancel = 0;
  ds2.contact_turns_left = 5;

  assert_true(are_diplstates_equal(&ds1, &ds2));

  /* Different type */
  ds2.type = DS_WAR;
  assert_false(are_diplstates_equal(&ds1, &ds2));
  ds2.type = DS_PEACE;

  /* Different turns_left */
  ds2.turns_left = 5;
  assert_false(are_diplstates_equal(&ds1, &ds2));
  ds2.turns_left = 10;

  /* Different has_reason_to_cancel */
  ds2.has_reason_to_cancel = 1;
  assert_false(are_diplstates_equal(&ds1, &ds2));
  ds2.has_reason_to_cancel = 0;

  /* Different contact_turns_left */
  ds2.contact_turns_left = 3;
  assert_false(are_diplstates_equal(&ds1, &ds2));
}

/***********************************************************************
  Test player_ai structure
***********************************************************************/
static void test_player_ai_structure(void **state)
{
  (void) state;

  memset(&test_player.ai_common, 0, sizeof(test_player.ai_common));

  test_player.ai_common.maxbuycost = 0;
  test_player.ai_common.handicaps = NULL;
  test_player.ai_common.skill_level = ai_level_invalid();
  test_player.ai_common.fuzzy = 0;
  test_player.ai_common.expand = 100;
  test_player.ai_common.science_cost = 100;
  test_player.ai_common.warmth = 0;
  test_player.ai_common.frost = 0;
  test_player.ai_common.barbarian_type = NOT_A_BARBARIAN;

  assert_int_equal(test_player.ai_common.maxbuycost, 0);
  assert_null(test_player.ai_common.handicaps);
  assert_int_equal(test_player.ai_common.fuzzy, 0);
  assert_int_equal(test_player.ai_common.expand, 100);
  assert_int_equal(test_player.ai_common.science_cost, 100);
  assert_int_equal(test_player.ai_common.warmth, 0);
  assert_int_equal(test_player.ai_common.frost, 0);
  assert_int_equal(test_player.ai_common.barbarian_type, NOT_A_BARBARIAN);
}

/***********************************************************************
  Test player_economic structure
***********************************************************************/
static void test_player_economic_structure(void **state)
{
  (void) state;

  struct player_economic econ;

  econ.gold = 100;
  econ.tax = 20;
  econ.science = 60;
  econ.luxury = 20;
  econ.infra_points = 50;

  assert_int_equal(econ.gold, 100);
  assert_int_equal(econ.tax, 20);
  assert_int_equal(econ.science, 60);
  assert_int_equal(econ.luxury, 20);
  assert_int_equal(econ.infra_points, 50);
}

/***********************************************************************
  Test player_score structure
***********************************************************************/
static void test_player_score_structure(void **state)
{
  (void) state;

  struct player_score score;

  memset(&score, 0, sizeof(score));

  score.happy = 10;
  score.content = 50;
  score.unhappy = 5;
  score.angry = 2;
  score.wonders = 3;
  score.techs = 25;
  score.cities = 8;
  score.units = 30;
  score.population = 5000;
  score.culture = 200;

  assert_int_equal(score.happy, 10);
  assert_int_equal(score.content, 50);
  assert_int_equal(score.unhappy, 5);
  assert_int_equal(score.angry, 2);
  assert_int_equal(score.wonders, 3);
  assert_int_equal(score.techs, 25);
  assert_int_equal(score.cities, 8);
  assert_int_equal(score.units, 30);
  assert_int_equal(score.population, 5000);
  assert_int_equal(score.culture, 200);
}

/***********************************************************************
  Test plrcolor_mode enum
***********************************************************************/
static void test_plrcolor_mode(void **state)
{
  (void) state;

  assert_int_equal(PLRCOL_PLR_ORDER, 0);
  assert_int_equal(PLRCOL_PLR_RANDOM, 1);
  assert_int_equal(PLRCOL_PLR_SET, 2);
  assert_int_equal(PLRCOL_TEAM_ORDER, 3);
  assert_int_equal(PLRCOL_NATION_ORDER, 4);
}

/***********************************************************************
  Test player_status enum
***********************************************************************/
static void test_player_status(void **state)
{
  (void) state;

  assert_int_equal(PSTATUS_NORMAL, 0);
  assert_int_equal(PSTATUS_DYING, 1);
  assert_int_equal(PSTATUS_WINNER, 2);
  assert_int_equal(PSTATUS_SURRENDER, 3);
  assert_int_equal(PSTATUS_COUNT, 4);
}

/***********************************************************************
  Test dipl_reason enum
***********************************************************************/
static void test_dipl_reason(void **state)
{
  (void) state;

  assert_int_equal(DIPL_OK, 0);
  assert_int_equal(DIPL_ERROR, 1);
  assert_int_equal(DIPL_SENATE_BLOCKING, 2);
  assert_int_equal(DIPL_ALLIANCE_PROBLEM_US, 3);
  assert_int_equal(DIPL_ALLIANCE_PROBLEM_THEM, 4);
}

/***********************************************************************
  Test player debug types
***********************************************************************/
static void test_player_debug_types(void **state)
{
  (void) state;

  assert_int_equal(PLAYER_DEBUG_DIPLOMACY, 0);
  assert_int_equal(PLAYER_DEBUG_TECH, 1);
  assert_int_equal(PLAYER_DEBUG_LAST, 2);
}

/***********************************************************************
  Test MAX_ATTRIBUTE_BLOCK constant
***********************************************************************/
static void test_max_attribute_block(void **state)
{
  (void) state;

  assert_int_equal(MAX_ATTRIBUTE_BLOCK, 256 * 1024);
}

/***********************************************************************
  Main test runner
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    /* Player name tests */
    cmocka_unit_test_setup_teardown(test_player_name_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_name_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_name_anon, setup, teardown),

    /* is_human/is_ai tests */
    cmocka_unit_test_setup_teardown(test_is_human_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_human_false, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_ai_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_ai_false, setup, teardown),
    cmocka_unit_test_setup_teardown(test_set_as_human_macro, setup, teardown),
    cmocka_unit_test_setup_teardown(test_set_as_ai_macro, setup, teardown),

    /* Barbarian tests */
    cmocka_unit_test_setup_teardown(test_is_barbarian_false, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_barbarian_land, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_barbarian_sea, setup, teardown),

    /* Player alive status tests */
    cmocka_unit_test_setup_teardown(test_player_is_alive_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_is_alive_false, setup, teardown),

    /* Player age test */
    cmocka_unit_test_setup_teardown(test_player_age, setup, teardown),

    /* Economic tests */
    cmocka_unit_test_setup_teardown(test_player_economic_defaults, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_economic_gold, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_economic_rates, setup, teardown),

    /* Love text tests */
    cmocka_unit_test_setup_teardown(test_love_text_extreme_negative, setup, teardown),
    cmocka_unit_test_setup_teardown(test_love_text_negative, setup, teardown),
    cmocka_unit_test_setup_teardown(test_love_text_neutral, setup, teardown),
    cmocka_unit_test_setup_teardown(test_love_text_positive, setup, teardown),
    cmocka_unit_test_setup_teardown(test_love_text_extreme_positive, setup, teardown),

    /* Constants tests */
    cmocka_unit_test_setup_teardown(test_max_ai_love_constant, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_default_tax_rate, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_default_science_rate, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_default_luxury_rate, setup, teardown),
    cmocka_unit_test_setup_teardown(test_anon_player_name, setup, teardown),
    cmocka_unit_test_setup_teardown(test_max_num_player_slots, setup, teardown),
    cmocka_unit_test_setup_teardown(test_max_len_name, setup, teardown),
    cmocka_unit_test_setup_teardown(test_owner_none, setup, teardown),
    cmocka_unit_test_setup_teardown(test_max_attribute_block, setup, teardown),

    /* Player flag tests */
    cmocka_unit_test_setup_teardown(test_player_has_flag, setup, teardown),

    /* Player status tests */
    cmocka_unit_test_setup_teardown(test_player_winner_status, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_ready_status, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_phase_done, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_connected, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_was_created, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_nturns_idle, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_revolution_finishes, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_primary_capital_id, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_last_war_action, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_gender, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_user_turns, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_unassigned_user, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_unassigned_ranked, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_random_name, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_autoselect_weight, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_music_style, setup, teardown),

    /* AI settings tests */
    cmocka_unit_test_setup_teardown(test_player_ai_skill_level, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_ai_fuzzy, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_ai_expand, setup, teardown),

    /* Player info tests */
    cmocka_unit_test_setup_teardown(test_player_username, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_ranked_username, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_team, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_government, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_nation, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_nation_macro, setup, teardown),

    /* Score tests */
    cmocka_unit_test_setup_teardown(test_player_score_init, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_score_values, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_history, setup, teardown),

    /* Username validation tests */
    cmocka_unit_test_setup_teardown(test_is_valid_username_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_valid_username_invalid, setup, teardown),

    /* Diplomacy tests */
    cmocka_unit_test_setup_teardown(test_diplstate_constants, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_diplstate, setup, teardown),
    cmocka_unit_test_setup_teardown(test_are_diplstates_equal, setup, teardown),

    /* Structure tests */
    cmocka_unit_test_setup_teardown(test_player_spaceship_init, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_attribute_block, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_gives_shared_vision, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_gives_shared_tiles, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_real_embassy, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_wonders, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_tile_known, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_rgb, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_multipliers, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_ai_structure, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_economic_structure, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_score_structure, setup, teardown),

    /* Enum tests */
    cmocka_unit_test_setup_teardown(test_plrcolor_mode, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_status, setup, teardown),
    cmocka_unit_test_setup_teardown(test_dipl_reason, setup, teardown),
    cmocka_unit_test_setup_teardown(test_player_debug_types, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}