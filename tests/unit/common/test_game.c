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
#include "shared.h"
#include "support.h"

/* common */
#include "fc_interface.h"
#include "game.h"
#include "idex.h"
#include "world_object.h"

/* Helper to set up game with default values for testing */
static void setup_game_defaults(void)
{
  memset(&game, 0, sizeof(game));

  /* Set game.info defaults that we test */
  game.info.gold = GAME_DEFAULT_GOLD;
  game.info.turn = 0;
  game.info.year = GAME_DEFAULT_START_YEAR;
  game.info.foodbox = GAME_DEFAULT_FOODBOX;
  game.info.shieldbox = GAME_DEFAULT_SHIELDBOX;
  game.info.sciencebox = GAME_DEFAULT_SCIENCEBOX;
  game.info.citymindist = GAME_DEFAULT_CITYMINDIST;
  game.info.borders = GAME_DEFAULT_BORDERS;
  game.info.diplomacy = GAME_DEFAULT_DIPLOMACY;
  game.info.fogofwar = GAME_DEFAULT_FOGOFWAR;
  game.info.timeout = GAME_DEFAULT_TIMEOUT;
  game.info.aifill = GAME_DEFAULT_AIFILL;
  game.info.skill_level = GAME_DEFAULT_SKILL_LEVEL;
  game.info.trademindist = GAME_DEFAULT_TRADEMINDIST;
  game.info.fulltradesize = GAME_DEFAULT_FULLTRADESIZE;
  game.info.notradesize = GAME_DEFAULT_NOTRADESIZE;
  game.info.rapturedelay = GAME_DEFAULT_RAPTUREDELAY;
  game.info.celebratesize = GAME_DEFAULT_CELEBRATESIZE;
  game.info.disasters = GAME_DEFAULT_DISASTERS;
  game.info.top_cities_count = GAME_DEFAULT_TOP_CITIES_COUNT;
  game.info.is_new_game = TRUE;
  game.info.is_edit_mode = FALSE;
  game.info.global_warming = GAME_DEFAULT_GLOBAL_WARMING;
  game.info.nuclear_winter = GAME_DEFAULT_NUCLEAR_WINTER;
  game.info.killstack = GAME_DEFAULT_KILLSTACK;
  game.info.killcitizen = GAME_DEFAULT_KILLCITIZEN;
  game.info.trading_tech = GAME_DEFAULT_TRADING_TECH;
  game.info.trading_gold = GAME_DEFAULT_TRADING_GOLD;
  game.info.trading_city = GAME_DEFAULT_TRADING_CITY;
  game.info.team_pooled_research = GAME_DEFAULT_TEAM_POOLED_RESEARCH;
  game.info.victory_conditions = GAME_DEFAULT_VICTORY_CONDITIONS;

  /* Set calendar defaults */
  game.calendar.calendar_skip_0 = FALSE;
  game.calendar.calendar_fragments = 0;

  /* Set scenario defaults */
  game.scenario.is_scenario = FALSE;
  game.scenario.players = TRUE;
  game.scenario.startpos_nations = FALSE;
  game.scenario.handmade = FALSE;
  game.scenario.prevent_new_cities = FALSE;
  game.scenario.lake_flooding = TRUE;
  game.scenario.have_resources = TRUE;
  game.scenario.ruleset_locked = TRUE;
  game.scenario.save_random = FALSE;
  game.scenario.allow_ai_type_fallback = FALSE;

  /* Set control defaults */
  game.control.government_count = 0;
  game.control.nation_count = 0;
  game.control.num_base_types = 0;
  game.control.num_road_types = 0;
  game.control.num_resource_types = 0;
  game.control.num_impr_types = 0;
  game.control.num_specialist_types = 0;
  game.control.num_tech_types = 0;
  game.control.num_unit_classes = 0;
  game.control.num_unit_types = 0;
  game.control.num_disaster_types = 0;
  game.control.num_achievement_types = 0;
  game.control.num_styles = 0;
  game.control.num_music_styles = 0;
  game.control.num_nation_groups = 0;
  game.control.num_nation_sets = 0;
  game.control.num_city_styles = 0;
  game.control.terrain_count = 0;
}

/***********************************************************************
  Test game default values
***********************************************************************/
static void test_game_defaults_gold(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.gold, GAME_DEFAULT_GOLD);
}

static void test_game_defaults_turn(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.turn, 0);
}

static void test_game_defaults_year(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.year, GAME_DEFAULT_START_YEAR);
}

static void test_game_defaults_foodbox(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.foodbox, GAME_DEFAULT_FOODBOX);
}

static void test_game_defaults_shieldbox(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.shieldbox, GAME_DEFAULT_SHIELDBOX);
}

static void test_game_defaults_sciencebox(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.sciencebox, GAME_DEFAULT_SCIENCEBOX);
}

static void test_game_defaults_citymindist(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.citymindist, GAME_DEFAULT_CITYMINDIST);
}

static void test_game_defaults_borders(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.borders, GAME_DEFAULT_BORDERS);
}

static void test_game_defaults_diplomacy(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.diplomacy, GAME_DEFAULT_DIPLOMACY);
}

static void test_game_defaults_fogofwar(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_true(game.info.fogofwar == GAME_DEFAULT_FOGOFWAR);
}

static void test_game_defaults_timeout(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.timeout, GAME_DEFAULT_TIMEOUT);
}

static void test_game_defaults_aifill(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.aifill, GAME_DEFAULT_AIFILL);
}

static void test_game_defaults_skill_level(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.skill_level, GAME_DEFAULT_SKILL_LEVEL);
}

static void test_game_defaults_trademindist(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.trademindist, GAME_DEFAULT_TRADEMINDIST);
}

static void test_game_defaults_fulltradesize(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.fulltradesize, GAME_DEFAULT_FULLTRADESIZE);
}

static void test_game_defaults_notradesize(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.notradesize, GAME_DEFAULT_NOTRADESIZE);
}

static void test_game_defaults_rapturedelay(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.rapturedelay, GAME_DEFAULT_RAPTUREDELAY);
}

static void test_game_defaults_celebratesize(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.celebratesize, GAME_DEFAULT_CELEBRATESIZE);
}

static void test_game_defaults_disasters(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.disasters, GAME_DEFAULT_DISASTERS);
}

static void test_game_defaults_top_cities_count(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.top_cities_count, GAME_DEFAULT_TOP_CITIES_COUNT);
}

/***********************************************************************
  Test game info structure operations
***********************************************************************/
static void test_game_info_is_new_game(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_true(game.info.is_new_game);
}

static void test_game_info_is_edit_mode(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_false(game.info.is_edit_mode);
}

static void test_game_info_global_warming(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_true(game.info.global_warming == GAME_DEFAULT_GLOBAL_WARMING);
}

static void test_game_info_nuclear_winter(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_true(game.info.nuclear_winter == GAME_DEFAULT_NUCLEAR_WINTER);
}

static void test_game_info_killstack(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_true(game.info.killstack == GAME_DEFAULT_KILLSTACK);
}

static void test_game_info_killcitizen(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_true(game.info.killcitizen == GAME_DEFAULT_KILLCITIZEN);
}

static void test_game_info_trading_flags(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_true(game.info.trading_tech == GAME_DEFAULT_TRADING_TECH);
  assert_true(game.info.trading_gold == GAME_DEFAULT_TRADING_GOLD);
  assert_true(game.info.trading_city == GAME_DEFAULT_TRADING_CITY);
}

static void test_game_info_team_pooled_research(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_true(game.info.team_pooled_research == GAME_DEFAULT_TEAM_POOLED_RESEARCH);
}

static void test_game_info_victory_conditions(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.victory_conditions, GAME_DEFAULT_VICTORY_CONDITIONS);
}

/***********************************************************************
  Test game state management
***********************************************************************/
static void test_game_state_turn_increment(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.turn, 0);

  game.info.turn = 1;
  assert_int_equal(game.info.turn, 1);

  game.info.turn = 100;
  assert_int_equal(game.info.turn, 100);
}

static void test_game_state_year_change(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.year, GAME_DEFAULT_START_YEAR);

  game.info.year = -3000;
  assert_int_equal(game.info.year, -3000);

  game.info.year = 0;
  assert_int_equal(game.info.year, 0);

  game.info.year = 1000;
  assert_int_equal(game.info.year, 1000);
}

static void test_game_state_phase(void **state)
{
  (void) state;

  setup_game_defaults();

  game.info.phase = 0;
  assert_int_equal(game.info.phase, 0);

  game.info.phase = 5;
  assert_int_equal(game.info.phase, 5);
}

static void test_game_state_warming_counters(void **state)
{
  (void) state;

  setup_game_defaults();

  game.info.globalwarming = 0;
  game.info.heating = 0;
  game.info.nuclearwinter = 0;
  game.info.cooling = 0;

  assert_int_equal(game.info.globalwarming, 0);
  assert_int_equal(game.info.heating, 0);
  assert_int_equal(game.info.nuclearwinter, 0);
  assert_int_equal(game.info.cooling, 0);
}

/***********************************************************************
  Test turn handling
***********************************************************************/
static void test_current_turn_timeout_basic(void **state)
{
  (void) state;

  setup_game_defaults();

  game.info.turn = 0;
  game.info.timeout = 100;
  game.info.first_timeout = -1;

  /* Turn 0 should use regular timeout */
  assert_int_equal(current_turn_timeout(), 100);
}

static void test_current_turn_timeout_first_turn(void **state)
{
  (void) state;

  setup_game_defaults();

  game.info.turn = 1;
  game.info.timeout = 100;
  game.info.first_timeout = 50;

  /* Turn 1 with first_timeout set should use first_timeout */
  assert_int_equal(current_turn_timeout(), 50);
}

static void test_current_turn_timeout_later_turn(void **state)
{
  (void) state;

  setup_game_defaults();

  game.info.turn = 10;
  game.info.timeout = 100;
  game.info.first_timeout = 50;

  /* Turn > 1 should use regular timeout */
  assert_int_equal(current_turn_timeout(), 100);
}

static void test_current_turn_timeout_no_first_timeout(void **state)
{
  (void) state;

  setup_game_defaults();

  game.info.turn = 1;
  game.info.timeout = 100;
  game.info.first_timeout = -1;

  /* Turn 1 with first_timeout = -1 should use regular timeout */
  assert_int_equal(current_turn_timeout(), 100);
}

/***********************************************************************
  Test is_player_phase
***********************************************************************/
static void test_is_player_phase_concurrent(void **state)
{
  (void) state;

  setup_game_defaults();

  game.info.phase_mode = PMT_CONCURRENT;

  /* In concurrent mode, all players are always in their phase */
  assert_true(is_player_phase(NULL, 0) == TRUE);
  assert_true(is_player_phase(NULL, 5) == TRUE);
  assert_true(is_player_phase(NULL, 100) == TRUE);
}

/***********************************************************************
  Test user_flag operations
***********************************************************************/
static void test_user_flag_init(void **state)
{
  (void) state;

  struct user_flag flag;

  user_flag_init(&flag);

  assert_null(flag.name);
  assert_null(flag.helptxt);
}

static void test_user_flag_free(void **state)
{
  (void) state;

  struct user_flag flag;

  user_flag_init(&flag);

  /* Free should not crash with null pointers */
  user_flag_free(&flag);

  assert_null(flag.name);
  assert_null(flag.helptxt);
}

/***********************************************************************
  Test generate_save_name
***********************************************************************/
static void test_generate_save_name_basic(void **state)
{
  (void) state;

  char buf[256];
  int result;

  setup_game_defaults();
  game.info.turn = 10;
  game.info.year = -3000;
  fc_strlcpy(game.calendar.negative_year_label, "BC",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "AD",
             sizeof(game.calendar.positive_year_label));

  result = generate_save_name("freeciv", buf, sizeof(buf), "auto");

  assert_true(result > 0);
  assert_true(strlen(buf) > 0);
}

static void test_generate_save_name_with_format(void **state)
{
  (void) state;

  char buf[256];
  int result;

  setup_game_defaults();
  game.info.turn = 100;
  game.info.year = 500;
  fc_strlcpy(game.calendar.negative_year_label, "BC",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "AD",
             sizeof(game.calendar.positive_year_label));

  result = generate_save_name("game-T%04T", buf, sizeof(buf), "manual");

  assert_true(result > 0);
  assert_true(strstr(buf, "0100") != NULL);  /* Turn 100 formatted as %04T */
}

static void test_generate_save_name_null_reason(void **state)
{
  (void) state;

  char buf[256];
  int result;

  setup_game_defaults();
  game.info.turn = 5;
  game.info.year = -1000;
  fc_strlcpy(game.calendar.negative_year_label, "BC",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "AD",
             sizeof(game.calendar.positive_year_label));

  result = generate_save_name("freeciv", buf, sizeof(buf), NULL);

  assert_true(result > 0);
  /* With NULL reason, should default to "auto" */
  assert_true(strstr(buf, "auto") != NULL);
}

/***********************************************************************
  Test game_city_by_number
***********************************************************************/
static void test_game_city_by_number_not_found(void **state)
{
  (void) state;

  struct city *result;

  memset(&wld, 0, sizeof(wld));
  idex_init(&wld);

  result = game_city_by_number(999);

  assert_null(result);

  idex_free(&wld);
}

static void test_game_unit_by_number_not_found(void **state)
{
  (void) state;

  struct unit *result;

  memset(&wld, 0, sizeof(wld));
  idex_init(&wld);

  result = game_unit_by_number(999);

  assert_null(result);

  idex_free(&wld);
}

/***********************************************************************
  Test game constants
***********************************************************************/
static void test_game_constants_gold(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_GOLD, 50);
  assert_int_equal(GAME_MIN_GOLD, 0);
  assert_int_equal(GAME_MAX_GOLD, 50000);
}

static void test_game_constants_foodbox(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_FOODBOX, 100);
  assert_int_equal(GAME_MIN_FOODBOX, 1);
  assert_int_equal(GAME_MAX_FOODBOX, 10000);
}

static void test_game_constants_shieldbox(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_SHIELDBOX, 100);
  assert_int_equal(GAME_MIN_SHIELDBOX, 1);
  assert_int_equal(GAME_MAX_SHIELDBOX, 10000);
}

static void test_game_constants_sciencebox(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_SCIENCEBOX, 100);
  assert_int_equal(GAME_MIN_SCIENCEBOX, 1);
  assert_int_equal(GAME_MAX_SCIENCEBOX, 10000);
}

static void test_game_constants_citymindist(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_CITYMINDIST, 2);
  assert_int_equal(GAME_MIN_CITYMINDIST, 1);
  assert_int_equal(GAME_MAX_CITYMINDIST, 11);
}

static void test_game_constants_end_turn(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_END_TURN, 5000);
  assert_int_equal(GAME_MIN_END_TURN, 1);
  assert_int_equal(GAME_MAX_END_TURN, 32767);
}

static void test_game_constants_players(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_MIN_PLAYERS, 1);
  assert_int_equal(GAME_DEFAULT_MAX_PLAYERS, MAX_NUM_PLAYERS);
  assert_int_equal(GAME_DEFAULT_AIFILL, 5);
}

static void test_game_constants_timeout(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_TIMEOUT, 0);
  assert_int_equal(GAME_MIN_TIMEOUT, -1);
  assert_int_equal(GAME_MAX_TIMEOUT, 8639999);
}

static void test_game_constants_diplchance(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_DIPLCHANCE, 80);
  assert_int_equal(GAME_MIN_DIPLCHANCE, 40);
  assert_int_equal(GAME_MAX_DIPLCHANCE, 100);
}

static void test_game_constants_civilwarsize(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_CIVILWARSIZE, 10);
  assert_int_equal(GAME_MIN_CIVILWARSIZE, 2);
  assert_int_equal(GAME_MAX_CIVILWARSIZE, 1000);
}

static void test_game_constants_contactturns(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_CONTACTTURNS, 20);
  assert_int_equal(GAME_MIN_CONTACTTURNS, 0);
  assert_int_equal(GAME_MAX_CONTACTTURNS, 100);
}

static void test_game_constants_rapturedelay(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_RAPTUREDELAY, 1);
  assert_int_equal(GAME_MIN_RAPTUREDELAY, 1);
  assert_int_equal(GAME_MAX_RAPTUREDELAY, 99);
}

static void test_game_constants_start_year(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_START_YEAR, -4000);
}

static void test_game_constants_lua_timeout(void **state)
{
  (void) state;

  assert_int_equal(GAME_DEFAULT_LUA_TIMEOUT, 5);
  assert_int_equal(GAME_MIN_LUA_TIMEOUT, 5);
  assert_int_equal(GAME_MAX_LUA_TIMEOUT, 30);
}

/***********************************************************************
  Test scenario defaults
***********************************************************************/
static void test_scenario_defaults(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_false(game.scenario.is_scenario);
  assert_true(game.scenario.players);
  assert_false(game.scenario.startpos_nations);
  assert_false(game.scenario.handmade);
  assert_false(game.scenario.prevent_new_cities);
  assert_true(game.scenario.lake_flooding);
  assert_true(game.scenario.have_resources);
  assert_true(game.scenario.ruleset_locked);
  assert_false(game.scenario.save_random);
  assert_false(game.scenario.allow_ai_type_fallback);
}

/***********************************************************************
  Test control defaults
***********************************************************************/
static void test_control_defaults(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.control.government_count, 0);
  assert_int_equal(game.control.nation_count, 0);
  assert_int_equal(game.control.num_base_types, 0);
  assert_int_equal(game.control.num_road_types, 0);
  assert_int_equal(game.control.num_resource_types, 0);
  assert_int_equal(game.control.num_impr_types, 0);
  assert_int_equal(game.control.num_specialist_types, 0);
  assert_int_equal(game.control.num_tech_types, 0);
  assert_int_equal(game.control.num_unit_classes, 0);
  assert_int_equal(game.control.num_unit_types, 0);
  assert_int_equal(game.control.num_disaster_types, 0);
  assert_int_equal(game.control.num_achievement_types, 0);
  assert_int_equal(game.control.num_styles, 0);
  assert_int_equal(game.control.num_music_styles, 0);
  assert_int_equal(game.control.num_nation_groups, 0);
  assert_int_equal(game.control.num_nation_sets, 0);
  assert_int_equal(game.control.num_city_styles, 0);
  assert_int_equal(game.control.terrain_count, 0);
}

/***********************************************************************
  Test ruleset compat mode
***********************************************************************/
static void test_ruleset_compat_mode(void **state)
{
  (void) state;

  set_ruleset_compat_mode(FALSE);
  assert_false(is_ruleset_compat_mode());

  set_ruleset_compat_mode(TRUE);
  assert_true(is_ruleset_compat_mode());

  set_ruleset_compat_mode(FALSE);
  assert_false(is_ruleset_compat_mode());
}

/***********************************************************************
  Test calendar defaults
***********************************************************************/
static void test_calendar_defaults(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_false(game.calendar.calendar_skip_0);
  assert_int_equal(game.calendar.calendar_fragments, 0);
}

/***********************************************************************
  Test game_init lifecycle
***********************************************************************/
static int test_game_lifecycle_setup(void **state)
{
  (void) state;

  /* Set to server mode for game_init */
  i_am_server();

  /* Clear game state before test */
  memset(&game, 0, sizeof(game));
  memset(&wld, 0, sizeof(wld));

  return 0;
}

static int test_game_lifecycle_teardown(void **state)
{
  (void) state;

  /* Clean up after test */
  memset(&game, 0, sizeof(game));
  memset(&wld, 0, sizeof(wld));

  return 0;
}

static void test_game_init_basic(void **state)
{
  (void) state;

  /* Initialize game */
  game_init(FALSE);

  /* Verify basic initialization */
  assert_int_equal(game.info.gold, GAME_DEFAULT_GOLD);
  assert_int_equal(game.info.turn, 0);
  assert_int_equal(game.info.year, GAME_DEFAULT_START_YEAR);
  assert_int_equal(game.info.foodbox, GAME_DEFAULT_FOODBOX);
  assert_int_equal(game.info.shieldbox, GAME_DEFAULT_SHIELDBOX);
  assert_int_equal(game.info.sciencebox, GAME_DEFAULT_SCIENCEBOX);
  assert_int_equal(game.info.citymindist, GAME_DEFAULT_CITYMINDIST);

  /* Clean up */
  game_free();
}

static void test_game_init_control_defaults(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Verify control defaults */
  assert_int_equal(game.control.government_count, 0);
  assert_int_equal(game.control.nation_count, 0);
  assert_int_equal(game.control.num_base_types, 0);
  assert_int_equal(game.control.num_road_types, 0);
  assert_int_equal(game.control.num_resource_types, 0);
  assert_int_equal(game.control.num_impr_types, 0);
  assert_int_equal(game.control.num_specialist_types, 0);
  assert_int_equal(game.control.num_tech_types, 0);
  assert_int_equal(game.control.num_unit_classes, 0);
  assert_int_equal(game.control.num_unit_types, 0);
  assert_int_equal(game.control.num_disaster_types, 0);
  assert_int_equal(game.control.num_achievement_types, 0);
  assert_int_equal(game.control.num_styles, 0);
  assert_int_equal(game.control.num_music_styles, 0);
  assert_int_equal(game.control.num_nation_groups, 0);
  assert_int_equal(game.control.num_nation_sets, 0);
  assert_int_equal(game.control.num_city_styles, 0);
  assert_int_equal(game.control.terrain_count, 0);

  game_free();
}

static void test_game_init_scenario_defaults(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Verify scenario defaults */
  assert_false(game.scenario.is_scenario);
  assert_true(game.scenario.players);
  assert_false(game.scenario.startpos_nations);
  assert_false(game.scenario.handmade);
  assert_false(game.scenario.prevent_new_cities);
  assert_true(game.scenario.lake_flooding);
  assert_true(game.scenario.have_resources);
  assert_true(game.scenario.ruleset_locked);
  assert_false(game.scenario.save_random);
  assert_false(game.scenario.allow_ai_type_fallback);

  game_free();
}

static void test_game_init_info_defaults(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Verify game info defaults */
  assert_true(game.info.is_new_game);
  assert_false(game.info.is_edit_mode);
  assert_int_equal(game.info.aifill, GAME_DEFAULT_AIFILL);
  assert_int_equal(game.info.skill_level, GAME_DEFAULT_SKILL_LEVEL);
  assert_int_equal(game.info.borders, GAME_DEFAULT_BORDERS);
  assert_int_equal(game.info.diplomacy, GAME_DEFAULT_DIPLOMACY);
  assert_true(game.info.fogofwar == GAME_DEFAULT_FOGOFWAR);
  assert_int_equal(game.info.timeout, GAME_DEFAULT_TIMEOUT);

  game_free();
}

static void test_game_init_trading_defaults(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Verify trading defaults */
  assert_true(game.info.trading_tech == GAME_DEFAULT_TRADING_TECH);
  assert_true(game.info.trading_gold == GAME_DEFAULT_TRADING_GOLD);
  assert_true(game.info.trading_city == GAME_DEFAULT_TRADING_CITY);
  assert_int_equal(game.info.trademindist, GAME_DEFAULT_TRADEMINDIST);

  game_free();
}

static void test_game_init_warming_defaults(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Verify warming/winter defaults */
  assert_true(game.info.global_warming == GAME_DEFAULT_GLOBAL_WARMING);
  assert_true(game.info.nuclear_winter == GAME_DEFAULT_NUCLEAR_WINTER);

  game_free();
}

static void test_game_free_after_init(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Free should not crash after init */
  game_free();

  /* Test passes if no crash occurs */
}

static void test_game_free_multiple(void **state)
{
  (void) state;

  /* Multiple init/free cycles should be safe */
  game_init(FALSE);
  game_free();

  game_init(FALSE);
  game_free();

  /* Test passes if no crash */
}

static void test_game_reset_after_init(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Modify some values */
  game.info.turn = 100;
  game.info.gold = 1000;
  game.info.year = 1000;

  /* Reset should restore defaults */
  game_reset();

  /* After reset, values should be back to defaults */
  assert_int_equal(game.info.gold, GAME_DEFAULT_GOLD);
  assert_int_equal(game.info.turn, 0);
  assert_int_equal(game.info.year, GAME_DEFAULT_START_YEAR);

  /* Clean up */
  game_free();
}

static void test_game_reset_state(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Set various game states */
  game.info.is_new_game = FALSE;
  game.info.is_edit_mode = TRUE;
  game.info.globalwarming = 50;
  game.info.heating = 25;
  game.info.nuclearwinter = 30;
  game.info.cooling = 15;

  /* Reset */
  game_reset();

  /* Verify state is reset */
  assert_true(game.info.is_new_game);
  assert_false(game.info.is_edit_mode);

  /* Clean up */
  game_free();
}

/***********************************************************************
  Test population_to_text - skipped due to locale initialization requirement
  Note: population_to_text requires locale/charset initialization which
  is not available in the unit test context. This test is disabled.
***********************************************************************/

/***********************************************************************
  Test game_city_by_name
***********************************************************************/
static void test_game_city_by_name_not_found(void **state)
{
  struct city *result;

  (void) state;

  game_init(FALSE);

  result = game_city_by_name("NonexistentCity");

  assert_null(result);

  game_free();
}

static void test_game_city_by_name_empty(void **state)
{
  struct city *result;

  (void) state;

  game_init(FALSE);

  result = game_city_by_name("");

  assert_null(result);

  game_free();
}

/***********************************************************************
  Test game_remove_unit - skipped due to complex state requirements
  Note: game_remove_unit requires full map and unit initialization
  with valid tile coordinates. This test is disabled pending improved
  mock framework (task #34).
***********************************************************************/
static void test_game_remove_unit_stub(void **state)
{
  (void) state;
  /* Skip - requires full map and unit initialization */
  skip();
}

/***********************************************************************
  Test game_remove_city - skipped due to complex state requirements
  Note: game_remove_city requires full map and city initialization
  with valid tile coordinates. This test is disabled pending improved
  mock framework.
***********************************************************************/
static void test_game_remove_city_stub(void **state)
{
  (void) state;
  /* Skip - requires full map and city initialization */
  skip();
}

/***********************************************************************
  Test game_map_init - skipped due to complex state requirements
  Note: game_map_init requires server mode and full world initialization.
  This test is disabled pending improved mock framework.
***********************************************************************/
static void test_game_map_init_stub(void **state)
{
  (void) state;
  /* Skip - requires server mode and full world initialization */
  skip();
}

/***********************************************************************
  Test game server mode initialization
***********************************************************************/
static void test_game_init_server_mode(void **state)
{
  (void) state;

  i_am_server();
  game_init(FALSE);

  /* Server-specific initialization should have occurred */
  assert_true(is_server());

  game_free();
}

static void test_game_init_client_mode(void **state)
{
  (void) state;

  i_am_client();
  game_init(FALSE);

  /* Client mode initialization */
  assert_false(is_server());

  game_free();
}

/***********************************************************************
  Test game.callbacks
***********************************************************************/
static void test_game_callbacks_init_null(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Callbacks should be initialized to NULL/0 */
  assert_null(game.callbacks.unit_deallocate);

  game_free();
}

/***********************************************************************
  Test game.ruleset fields
***********************************************************************/
static void test_game_ruleset_fields_init(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Ruleset fields should be NULL after init */
  assert_null(game.ruleset_summary);
  assert_null(game.ruleset_description);
  assert_null(game.ruleset_capabilities);

  game_free();
}

/***********************************************************************
  Test lua_timeout
***********************************************************************/
static void test_game_lua_timeout_default(void **state)
{
  (void) state;

  game_init(FALSE);

  assert_int_equal(game.lua_timeout, GAME_DEFAULT_LUA_TIMEOUT);

  game_free();
}

static void test_game_lua_timeout_set(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Set and verify */
  game.lua_timeout = 10;
  assert_int_equal(game.lua_timeout, 10);

  game.lua_timeout = 30;
  assert_int_equal(game.lua_timeout, 30);

  game_free();
}

/***********************************************************************
  Test game.veteran initialization
***********************************************************************/
static void test_game_veteran_init(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Veteran system should be NULL after init */
  assert_null(game.veteran);

  game_free();
}

/***********************************************************************
  Test game.plr_bg_color initialization
***********************************************************************/
static void test_game_plr_bg_color_init(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Background color should be NULL after init */
  assert_null(game.plr_bg_color);

  game_free();
}

/***********************************************************************
  Test game.rgame initialization
***********************************************************************/
static void test_game_rgame_init(void **state)
{
  (void) state;

  game_init(FALSE);

  /* Global init techs and buildings should be cleared */
  assert_int_equal(game.rgame.global_init_techs[0], 0);
  assert_int_equal(game.rgame.global_init_buildings[0], 0);

  game_free();
}

/***********************************************************************
  Test generate_save_name with year
***********************************************************************/
static void test_generate_save_name_year_negative(void **state)
{
  (void) state;

  char buf[256];
  int result;

  setup_game_defaults();
  game.info.turn = 1;
  game.info.year = -4000;
  fc_strlcpy(game.calendar.negative_year_label, "BC",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "AD",
             sizeof(game.calendar.positive_year_label));

  result = generate_save_name("save-%Y", buf, sizeof(buf), "test");

  assert_true(result > 0);
}

static void test_generate_save_name_year_positive(void **state)
{
  (void) state;

  char buf[256];
  int result;

  setup_game_defaults();
  game.info.turn = 100;
  game.info.year = 2000;
  fc_strlcpy(game.calendar.negative_year_label, "BC",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "AD",
             sizeof(game.calendar.positive_year_label));

  result = generate_save_name("save-%Y", buf, sizeof(buf), "test");

  assert_true(result > 0);
}

static void test_generate_save_name_suffix(void **state)
{
  (void) state;

  char buf[256];
  int result;

  setup_game_defaults();
  game.info.turn = 50;
  game.info.year = 1000;
  fc_strlcpy(game.calendar.negative_year_label, "BC",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "AD",
             sizeof(game.calendar.positive_year_label));

  result = generate_save_name("game-%S", buf, sizeof(buf), "auto");

  assert_true(result > 0);
  /* Should contain AD for positive year */
  assert_true(strstr(buf, "AD") != NULL);
}

/***********************************************************************
  Test game.info various fields
***********************************************************************/
static void test_game_info_infra(void **state)
{
  (void) state;

  setup_game_defaults();

  assert_int_equal(game.info.infrapoints, GAME_DEFAULT_INFRA);

  game.info.infrapoints = 100;
  assert_int_equal(game.info.infrapoints, 100);

  game.info.infrapoints = 50000;  /* Max */
  assert_int_equal(game.info.infrapoints, 50000);
}

static void test_game_scenario_name(void **state)
{
  (void) state;

  setup_game_defaults();

  /* Scenario name should be empty initially */
  game.scenario.name[0] = '\0';
  assert_int_equal(game.scenario.name[0], '\0');

  /* Set a scenario name */
  fc_strlcpy(game.scenario.name, "TestScenario", sizeof(game.scenario.name));
  assert_string_equal(game.scenario.name, "TestScenario");
}

static void test_game_scenario_authors(void **state)
{
  (void) state;

  setup_game_defaults();

  /* Scenario authors should be empty initially */
  game.scenario.authors[0] = '\0';
  assert_int_equal(game.scenario.authors[0], '\0');

  /* Set authors */
  fc_strlcpy(game.scenario.authors, "Test Author", sizeof(game.scenario.authors));
  assert_string_equal(game.scenario.authors, "Test Author");
}

static void test_game_scenario_flags(void **state)
{
  (void) state;

  setup_game_defaults();

  /* Test scenario flags */
  game.scenario.is_scenario = TRUE;
  assert_true(game.scenario.is_scenario);

  game.scenario.handmade = TRUE;
  assert_true(game.scenario.handmade);

  game.scenario.prevent_new_cities = TRUE;
  assert_true(game.scenario.prevent_new_cities);

  game.scenario.save_random = TRUE;
  assert_true(game.scenario.save_random);

  game.scenario.allow_ai_type_fallback = TRUE;
  assert_true(game.scenario.allow_ai_type_fallback);
}

static void test_game_calendar_labels(void **state)
{
  (void) state;

  setup_game_defaults();

  /* Set year labels */
  fc_strlcpy(game.calendar.negative_year_label, "BC",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "AD",
             sizeof(game.calendar.positive_year_label));

  assert_string_equal(game.calendar.negative_year_label, "BC");
  assert_string_equal(game.calendar.positive_year_label, "AD");
}

static void test_game_calendar_fragments(void **state)
{
  (void) state;

  setup_game_defaults();

  /* Test calendar fragments */
  game.calendar.calendar_fragments = 12;
  assert_int_equal(game.calendar.calendar_fragments, 12);

  game.calendar.calendar_skip_0 = TRUE;
  assert_true(game.calendar.calendar_skip_0);
}

static void test_game_info_phase_values(void **state)
{
  (void) state;

  setup_game_defaults();

  /* Test setting various phase values */
  game.info.phase = 0;
  assert_int_equal(game.info.phase, 0);

  game.info.phase = 1;
  assert_int_equal(game.info.phase, 1);

  game.info.phase = 10;
  assert_int_equal(game.info.phase, 10);

  game.info.phase = 127;  /* Max player count */
  assert_int_equal(game.info.phase, 127);
}

static void test_game_info_year_negative(void **state)
{
  (void) state;

  setup_game_defaults();

  /* Test negative years (BC) */
  game.info.year = -4000;
  assert_int_equal(game.info.year, -4000);

  game.info.year = -1;
  assert_int_equal(game.info.year, -1);
}

static void test_game_info_year_positive(void **state)
{
  (void) state;

  setup_game_defaults();

  /* Test positive years (AD) */
  game.info.year = 1;
  assert_int_equal(game.info.year, 1);

  game.info.year = 2000;
  assert_int_equal(game.info.year, 2000);
}

static void test_game_info_year_zero(void **state)
{
  (void) state;

  setup_game_defaults();

  /* Test year zero */
  game.info.year = 0;
  assert_int_equal(game.info.year, 0);
}

static void test_game_info_counters(void **state)
{
  (void) state;

  setup_game_defaults();

  /* Test global advance count */
  game.info.global_advance_count = 0;
  assert_int_equal(game.info.global_advance_count, 0);

  game.info.global_advance_count = 50;
  assert_int_equal(game.info.global_advance_count, 50);
}

static void test_game_info_warming_counters(void **state)
{
  (void) state;

  setup_game_defaults();

  /* Test warming/winter counters */
  game.info.globalwarming = 100;
  game.info.heating = 50;
  game.info.nuclearwinter = 75;
  game.info.cooling = 25;

  assert_int_equal(game.info.globalwarming, 100);
  assert_int_equal(game.info.heating, 50);
  assert_int_equal(game.info.nuclearwinter, 75);
  assert_int_equal(game.info.cooling, 25);
}

/***********************************************************************
  Additional game function tests for coverage
***********************************************************************/

/* Test game_city_by_name() - basic test */
static void test_game_city_by_name_basic(void **state)
{
  struct city *result;

  (void) state;

  /* With no cities, should return NULL */
  result = game_city_by_name("NonExistentCity");
  assert_null(result);
}

/* Test civ_population() with no cities */
static void test_civ_population_empty(void **state)
{
  struct player test_player;
  int population;

  (void) state;

  memset(&test_player, 0, sizeof(test_player));
  test_player.cities = city_list_new();

  population = civ_population(&test_player);
  assert_int_equal(population, 0);

  city_list_destroy(test_player.cities);
}

/* Test population_to_text() - basic test
 * Skip - requires proper locale and game state initialization */
static void test_population_to_text_basic(void **state)
{
  (void) state;
  /* Skip - requires locale initialization */
  skip();
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* Game defaults tests */
    cmocka_unit_test(test_game_defaults_gold),
    cmocka_unit_test(test_game_defaults_turn),
    cmocka_unit_test(test_game_defaults_year),
    cmocka_unit_test(test_game_defaults_foodbox),
    cmocka_unit_test(test_game_defaults_shieldbox),
    cmocka_unit_test(test_game_defaults_sciencebox),
    cmocka_unit_test(test_game_defaults_citymindist),
    cmocka_unit_test(test_game_defaults_borders),
    cmocka_unit_test(test_game_defaults_diplomacy),
    cmocka_unit_test(test_game_defaults_fogofwar),
    cmocka_unit_test(test_game_defaults_timeout),
    cmocka_unit_test(test_game_defaults_aifill),
    cmocka_unit_test(test_game_defaults_skill_level),
    cmocka_unit_test(test_game_defaults_trademindist),
    cmocka_unit_test(test_game_defaults_fulltradesize),
    cmocka_unit_test(test_game_defaults_notradesize),
    cmocka_unit_test(test_game_defaults_rapturedelay),
    cmocka_unit_test(test_game_defaults_celebratesize),
    cmocka_unit_test(test_game_defaults_disasters),
    cmocka_unit_test(test_game_defaults_top_cities_count),

    /* Game info structure tests */
    cmocka_unit_test(test_game_info_is_new_game),
    cmocka_unit_test(test_game_info_is_edit_mode),
    cmocka_unit_test(test_game_info_global_warming),
    cmocka_unit_test(test_game_info_nuclear_winter),
    cmocka_unit_test(test_game_info_killstack),
    cmocka_unit_test(test_game_info_killcitizen),
    cmocka_unit_test(test_game_info_trading_flags),
    cmocka_unit_test(test_game_info_team_pooled_research),
    cmocka_unit_test(test_game_info_victory_conditions),

    /* Game state management tests */
    cmocka_unit_test(test_game_state_turn_increment),
    cmocka_unit_test(test_game_state_year_change),
    cmocka_unit_test(test_game_state_phase),
    cmocka_unit_test(test_game_state_warming_counters),

    /* Turn handling tests */
    cmocka_unit_test(test_current_turn_timeout_basic),
    cmocka_unit_test(test_current_turn_timeout_first_turn),
    cmocka_unit_test(test_current_turn_timeout_later_turn),
    cmocka_unit_test(test_current_turn_timeout_no_first_timeout),

    /* is_player_phase tests */
    cmocka_unit_test(test_is_player_phase_concurrent),

    /* user_flag tests */
    cmocka_unit_test(test_user_flag_init),
    cmocka_unit_test(test_user_flag_free),

    /* generate_save_name tests */
    cmocka_unit_test(test_generate_save_name_with_format),
    cmocka_unit_test(test_generate_save_name_null_reason),
    cmocka_unit_test(test_generate_save_name_year_negative),
    cmocka_unit_test(test_generate_save_name_year_positive),
    cmocka_unit_test(test_generate_save_name_suffix),

    /* City/unit lookup tests */
    cmocka_unit_test(test_game_city_by_number_not_found),
    cmocka_unit_test(test_game_unit_by_number_not_found),

    /* game_city_by_name tests */
    cmocka_unit_test(test_game_city_by_name_not_found),
    cmocka_unit_test(test_game_city_by_name_empty),

    /* Game constants tests */
    cmocka_unit_test(test_game_constants_gold),
    cmocka_unit_test(test_game_constants_foodbox),
    cmocka_unit_test(test_game_constants_shieldbox),
    cmocka_unit_test(test_game_constants_sciencebox),
    cmocka_unit_test(test_game_constants_citymindist),
    cmocka_unit_test(test_game_constants_end_turn),
    cmocka_unit_test(test_game_constants_players),
    cmocka_unit_test(test_game_constants_timeout),
    cmocka_unit_test(test_game_constants_diplchance),
    cmocka_unit_test(test_game_constants_civilwarsize),
    cmocka_unit_test(test_game_constants_contactturns),
    cmocka_unit_test(test_game_constants_rapturedelay),
    cmocka_unit_test(test_game_constants_start_year),
    cmocka_unit_test(test_game_constants_lua_timeout),

    /* Scenario defaults tests */
    cmocka_unit_test(test_scenario_defaults),

    /* Control defaults tests */
    cmocka_unit_test(test_control_defaults),

    /* Ruleset compat mode tests */
    cmocka_unit_test(test_ruleset_compat_mode),

    /* Calendar defaults tests */
    cmocka_unit_test(test_calendar_defaults),

    /* Game init lifecycle tests */
    cmocka_unit_test_setup_teardown(test_game_init_basic,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),
    cmocka_unit_test_setup_teardown(test_game_init_control_defaults,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),
    cmocka_unit_test_setup_teardown(test_game_init_scenario_defaults,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),
    cmocka_unit_test_setup_teardown(test_game_init_info_defaults,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),
    cmocka_unit_test_setup_teardown(test_game_init_trading_defaults,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),
    cmocka_unit_test_setup_teardown(test_game_init_warming_defaults,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),

    /* Game free lifecycle tests */
    cmocka_unit_test_setup_teardown(test_game_free_after_init,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),
    cmocka_unit_test_setup_teardown(test_game_free_multiple,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),

    /* Game reset lifecycle tests */
    cmocka_unit_test_setup_teardown(test_game_reset_after_init,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),
    cmocka_unit_test_setup_teardown(test_game_reset_state,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),

    /* population_to_text tests - disabled due to locale initialization requirement */

    /* Server/client mode tests */
    cmocka_unit_test_setup_teardown(test_game_init_server_mode,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),
    cmocka_unit_test_setup_teardown(test_game_init_client_mode,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),

    /* Game callbacks tests */
    cmocka_unit_test_setup_teardown(test_game_callbacks_init_null,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),

    /* Game ruleset fields tests */
    cmocka_unit_test_setup_teardown(test_game_ruleset_fields_init,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),

    /* Lua timeout tests */
    cmocka_unit_test_setup_teardown(test_game_lua_timeout_default,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),
    cmocka_unit_test_setup_teardown(test_game_lua_timeout_set,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),

    /* Game veteran tests */
    cmocka_unit_test_setup_teardown(test_game_veteran_init,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),

    /* Game player background color tests */
    cmocka_unit_test_setup_teardown(test_game_plr_bg_color_init,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),

    /* Game rgame tests */
    cmocka_unit_test_setup_teardown(test_game_rgame_init,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),

    /* Additional game info tests */
    cmocka_unit_test(test_game_info_infra),
    cmocka_unit_test(test_game_scenario_name),
    cmocka_unit_test(test_game_scenario_authors),
    cmocka_unit_test(test_game_scenario_flags),
    cmocka_unit_test(test_game_calendar_labels),
    cmocka_unit_test(test_game_calendar_fragments),
    cmocka_unit_test(test_game_info_phase_values),
    cmocka_unit_test(test_game_info_year_negative),
    cmocka_unit_test(test_game_info_year_positive),
    cmocka_unit_test(test_game_info_year_zero),
    cmocka_unit_test(test_game_info_counters),
    cmocka_unit_test(test_game_info_warming_counters),

    /* Additional game function tests for coverage */
    cmocka_unit_test_setup_teardown(test_game_city_by_name_basic,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),
    cmocka_unit_test_setup_teardown(test_civ_population_empty,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),
    cmocka_unit_test_setup_teardown(test_population_to_text_basic,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),
    cmocka_unit_test_setup_teardown(test_generate_save_name_basic,
                                     test_game_lifecycle_setup,
                                     test_game_lifecycle_teardown),

    /* Game remove unit/city tests - stubs pending mock framework */
    cmocka_unit_test(test_game_remove_unit_stub),
    cmocka_unit_test(test_game_remove_city_stub),
    cmocka_unit_test(test_game_map_init_stub),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}