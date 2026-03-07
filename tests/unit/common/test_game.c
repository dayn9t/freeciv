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
    cmocka_unit_test(test_generate_save_name_basic),
    cmocka_unit_test(test_generate_save_name_with_format),
    cmocka_unit_test(test_generate_save_name_null_reason),

    /* City/unit lookup tests */
    cmocka_unit_test(test_game_city_by_number_not_found),
    cmocka_unit_test(test_game_unit_by_number_not_found),

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
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}