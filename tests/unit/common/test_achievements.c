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

/* common */
#include "fc_types.h"
#include "game.h"
#include "player.h"
#include "achievements.h"

extern struct civ_game game;

static int setup(void **state)
{
  (void) state;
  memset(&game, 0, sizeof(game));
  achievements_init();
  return 0;
}

static int teardown(void **state)
{
  (void) state;
  achievements_free();
  memset(&game, 0, sizeof(game));
  return 0;
}

/* Test achievement_by_number with no achievement types configured */
static void test_achievement_by_number_empty(void **state)
{
  (void) state;

  game.control.num_achievement_types = 0;

  /* No achievements should be available */
  assert_null(achievement_by_number(0));
  assert_null(achievement_by_number(-1));
  assert_null(achievement_by_number(5));
}

/* Test achievement_by_number with valid and invalid IDs */
static void test_achievement_by_number_valid(void **state)
{
  (void) state;

  game.control.num_achievement_types = 3;

  /* Valid IDs should return achievement pointers */
  struct achievement *ach0 = achievement_by_number(0);
  struct achievement *ach1 = achievement_by_number(1);
  struct achievement *ach2 = achievement_by_number(2);

  assert_non_null(ach0);
  assert_non_null(ach1);
  assert_non_null(ach2);

  /* Verify each achievement is distinct */
  assert_ptr_not_equal(ach0, ach1);
  assert_ptr_not_equal(ach1, ach2);
  assert_ptr_not_equal(ach0, ach2);
}

/* Test achievement_by_number with out of bounds IDs */
static void test_achievement_by_number_out_of_bounds(void **state)
{
  (void) state;

  game.control.num_achievement_types = 2;

  /* IDs at and beyond the boundary should return NULL */
  assert_null(achievement_by_number(2));
  assert_null(achievement_by_number(3));
  assert_null(achievement_by_number(100));
  assert_null(achievement_by_number(-1));
}

/* Test achievement_number returns correct ID */
static void test_achievement_number(void **state)
{
  (void) state;

  game.control.num_achievement_types = 5;

  struct achievement *ach0 = achievement_by_number(0);
  struct achievement *ach3 = achievement_by_number(3);
  struct achievement *ach4 = achievement_by_number(4);

  assert_non_null(ach0);
  assert_non_null(ach3);
  assert_non_null(ach4);

  assert_int_equal(achievement_number(ach0), 0);
  assert_int_equal(achievement_number(ach3), 3);
  assert_int_equal(achievement_number(ach4), 4);
}

/* Test achievement_number with NULL input */
static void test_achievement_number_null(void **state)
{
  (void) state;

  /* NULL achievement should return -1 */
  assert_int_equal(achievement_number(NULL), -1);
}

/* Test achievement_index returns correct index */
static void test_achievement_index(void **state)
{
  (void) state;

  game.control.num_achievement_types = 3;

  struct achievement *ach0 = achievement_by_number(0);
  struct achievement *ach1 = achievement_by_number(1);
  struct achievement *ach2 = achievement_by_number(2);

  assert_non_null(ach0);
  assert_non_null(ach1);
  assert_non_null(ach2);

  assert_int_equal(achievement_index(ach0), 0);
  assert_int_equal(achievement_index(ach1), 1);
  assert_int_equal(achievement_index(ach2), 2);
}

/* Test achievement_index with NULL input */
static void test_achievement_index_null(void **state)
{
  (void) state;

  /* NULL achievement should return -1 */
  assert_int_equal(achievement_index(NULL), -1);
}

/* Test achievement_by_rule_name with no match */
static void test_achievement_by_rule_name_no_match(void **state)
{
  (void) state;

  game.control.num_achievement_types = 2;

  /* No achievement with this name exists */
  assert_null(achievement_by_rule_name("nonexistent_achievement"));
}

/* Test achievement_by_rule_name with empty game */
static void test_achievement_by_rule_name_empty(void **state)
{
  (void) state;

  game.control.num_achievement_types = 0;

  /* No achievements configured, should return NULL */
  assert_null(achievement_by_rule_name("any_achievement"));
}

/* Test achievement_player_has with NULL player */
static void test_achievement_player_has_null_player(void **state)
{
  (void) state;

  game.control.num_achievement_types = 1;
  struct achievement *ach = achievement_by_number(0);
  assert_non_null(ach);

  assert_false(achievement_player_has(ach, NULL));
}

/* Test achievement_claimed when not claimed */
static void test_achievement_claimed_not_claimed(void **state)
{
  (void) state;

  game.control.num_achievement_types = 1;
  struct achievement *ach = achievement_by_number(0);
  assert_non_null(ach);

  /* Achievement is initialized with first = NULL, so not claimed */
  assert_false(achievement_claimed(ach));
}

/* Test get_literacy with zero population */
static void test_get_literacy_zero_population(void **state)
{
  (void) state;

  struct player pplayer;
  memset(&pplayer, 0, sizeof(pplayer));

  pplayer.score.population = 0;
  pplayer.score.literacy = 100;

  /* With zero population, literacy should be 0 */
  assert_int_equal(get_literacy(&pplayer), 0);
}

/* Test get_literacy with normal population */
static void test_get_literacy_normal(void **state)
{
  (void) state;

  struct player pplayer;
  memset(&pplayer, 0, sizeof(pplayer));

  /* Population 100, literacy 50 -> (50 * 100) / 100 = 50 */
  pplayer.score.population = 100;
  pplayer.score.literacy = 50;

  assert_int_equal(get_literacy(&pplayer), 50);
}

/* Test get_literacy with large population */
static void test_get_literacy_large_population(void **state)
{
  (void) state;

  struct player pplayer;
  memset(&pplayer, 0, sizeof(pplayer));

  /* Population 20000, literacy 15000 -> 15000 / 200 = 75 */
  pplayer.score.population = 20000;
  pplayer.score.literacy = 15000;

  assert_int_equal(get_literacy(&pplayer), 75);
}

/* Test get_literacy boundary case at 10000 population */
static void test_get_literacy_boundary(void **state)
{
  (void) state;

  struct player pplayer;
  memset(&pplayer, 0, sizeof(pplayer));

  /* Population exactly 10000, literacy 10000 -> 10000 / 100 = 100 */
  pplayer.score.population = 10000;
  pplayer.score.literacy = 10000;

  assert_int_equal(get_literacy(&pplayer), 100);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_achievement_by_number_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_achievement_by_number_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_achievement_by_number_out_of_bounds, setup, teardown),
    cmocka_unit_test_setup_teardown(test_achievement_number, setup, teardown),
    cmocka_unit_test_setup_teardown(test_achievement_number_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_achievement_index, setup, teardown),
    cmocka_unit_test_setup_teardown(test_achievement_index_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_achievement_by_rule_name_no_match, setup, teardown),
    cmocka_unit_test_setup_teardown(test_achievement_by_rule_name_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_achievement_player_has_null_player, setup, teardown),
    cmocka_unit_test_setup_teardown(test_achievement_claimed_not_claimed, setup, teardown),
    cmocka_unit_test_setup_teardown(test_get_literacy_zero_population, setup, teardown),
    cmocka_unit_test_setup_teardown(test_get_literacy_normal, setup, teardown),
    cmocka_unit_test_setup_teardown(test_get_literacy_large_population, setup, teardown),
    cmocka_unit_test_setup_teardown(test_get_literacy_boundary, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}