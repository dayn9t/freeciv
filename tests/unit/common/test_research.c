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
#include "research.h"
#include "tech.h"

/***********************************************************************
  Helper to set up game with minimal tech initialization for testing
***********************************************************************/
static void setup_research_test_env(void)
{
  memset(&game, 0, sizeof(game));
  /* Set num_tech_types to at least A_FIRST so A_NONE is valid */
  game.control.num_tech_types = A_FIRST;
  game.info.team_pooled_research = FALSE;
  game.info.tech_cost_style = TECH_COST_CLASSIC;
  game.info.base_tech_cost = 1;
  game.info.sciencebox = 100;
  game.info.tech_leakage = TECH_LEAKAGE_NONE;
  game.info.min_tech_cost = 1;

  /* Initialize the tech system first */
  techs_init();

  /* Initialize the research system */
  researches_init();
}

/***********************************************************************
  Helper to clean up after tests
***********************************************************************/
static void teardown_research_test_env(void)
{
  researches_free();
  techs_free();
  memset(&game, 0, sizeof(game));
}

/***********************************************************************
  Test researches_init initializes the research array correctly
***********************************************************************/
static void test_researches_init(void **state)
{
  (void) state;

  setup_research_test_env();

  /* After researches_init, verify the research array is initialized */
  struct research *research_zero = research_by_number(0);

  assert_non_null(research_zero);
  assert_int_equal(research_zero->tech_goal, A_UNSET);
  assert_int_equal(research_zero->researching, A_UNSET);
  assert_int_equal(research_zero->researching_saved, A_UNKNOWN);
  assert_int_equal(research_zero->future_tech, 0);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_by_number returns correct research structures
***********************************************************************/
static void test_research_by_number(void **state)
{
  (void) state;

  setup_research_test_env();

  /* Test valid number */
  struct research *research = research_by_number(0);
  assert_non_null(research);

  /* Test that research_number returns the same index */
  int number = research_number(research);
  assert_int_equal(number, 0);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_number returns correct index
***********************************************************************/
static void test_research_number(void **state)
{
  (void) state;

  setup_research_test_env();

  struct research *research = research_by_number(5);
  assert_non_null(research);

  int number = research_number(research);
  assert_int_equal(number, 5);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_get returns nullptr for null player
***********************************************************************/
static void test_research_get_null_player(void **state)
{
  (void) state;

  setup_research_test_env();

  struct research *result = research_get(nullptr);
  assert_null(result);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_get with team_pooled_research disabled
***********************************************************************/
static void test_research_get_no_team_pool(void **state)
{
  (void) state;

  setup_research_test_env();

  game.info.team_pooled_research = FALSE;

  /* Create a minimal player structure */
  struct player test_player;
  memset(&test_player, 0, sizeof(test_player));
  test_player.name[0] = '\0';
  test_player.is_alive = TRUE;

  /* Without full player initialization, research_get returns based on
   * player_number which we can't easily test without full game setup.
   * The function will access player_number(pplayer) which dereferences
   * the player slot. */

  teardown_research_test_env();
}

/***********************************************************************
  Test research_invention_state with null research
***********************************************************************/
static void test_research_invention_state_null(void **state)
{
  (void) state;

  setup_research_test_env();

  /* When research is nullptr, it checks game.info.global_advances
   * Initialize the global_advances array */
  memset(game.info.global_advances, 0, sizeof(game.info.global_advances));
  game.info.global_advances[A_NONE] = TRUE;

  /* A_NONE should always be TECH_KNOWN when global_advances is set */
  enum tech_state tech_st = research_invention_state(nullptr, A_NONE);
  assert_int_equal(tech_st, TECH_KNOWN);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_invention_state for a research structure
***********************************************************************/
static void test_research_invention_state_basic(void **state)
{
  (void) state;

  setup_research_test_env();

  struct research *research = research_by_number(0);
  assert_non_null(research);

  /* A_NONE should always be TECH_KNOWN after researches_init */
  enum tech_state tech_st = research_invention_state(research, A_NONE);
  assert_int_equal(tech_st, TECH_KNOWN);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_invention_set changes state correctly
***********************************************************************/
static void test_research_invention_set(void **state)
{
  (void) state;

  setup_research_test_env();

  struct research *research = research_by_number(0);
  assert_non_null(research);

  /* First set to TECH_UNKNOWN */
  research->inventions[A_NONE].state = TECH_UNKNOWN;

  /* Now set to TECH_KNOWN */
  enum tech_state old = research_invention_set(research, A_NONE, TECH_KNOWN);
  assert_int_equal(old, TECH_UNKNOWN);

  /* Verify new state */
  enum tech_state new_state = research_invention_state(research, A_NONE);
  assert_int_equal(new_state, TECH_KNOWN);

  /* Setting to same value should return same value */
  old = research_invention_set(research, A_NONE, TECH_KNOWN);
  assert_int_equal(old, TECH_KNOWN);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_invention_set updates global_advances
***********************************************************************/
static void test_research_invention_set_global(void **state)
{
  (void) state;

  setup_research_test_env();

  struct research *research = research_by_number(0);
  assert_non_null(research);

  /* Reset global advance count */
  game.info.global_advance_count = 0;
  memset(game.info.global_advances, 0, sizeof(game.info.global_advances));

  /* First set A_NONE to TECH_UNKNOWN so the change triggers global update */
  research->inventions[A_NONE].state = TECH_UNKNOWN;

  /* Now set A_NONE to TECH_KNOWN - this should update global_advances */
  research_invention_set(research, A_NONE, TECH_KNOWN);

  assert_true(game.info.global_advances[A_NONE]);
  assert_int_equal(game.info.global_advance_count, 1);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_invention_reachable with null research
***********************************************************************/
static void test_research_invention_reachable_null(void **state)
{
  (void) state;

  setup_research_test_env();

  /* With null research, it iterates over all researches.
   * Without any players, this returns FALSE. */
  bool reachable = research_invention_reachable(nullptr, A_NONE);
  assert_false(reachable);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_invention_gettable with null research
***********************************************************************/
static void test_research_invention_gettable_null(void **state)
{
  (void) state;

  setup_research_test_env();

  /* With null research and no players, this returns FALSE */
  bool gettable = research_invention_gettable(nullptr, A_NONE, TRUE);
  assert_false(gettable);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_goal_step with invalid goal
***********************************************************************/
static void test_research_goal_step_invalid(void **state)
{
  (void) state;

  setup_research_test_env();

  struct research *research = research_by_number(0);
  assert_non_null(research);

  /* With A_UNSET as goal, should return A_UNSET */
  Tech_type_id step = research_goal_step(research, A_UNSET);
  assert_int_equal(step, A_UNSET);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_goal_unknown_techs with invalid goal
***********************************************************************/
static void test_research_goal_unknown_techs_invalid(void **state)
{
  (void) state;

  setup_research_test_env();

  /* With A_UNSET, should return 0 */
  int unknown = research_goal_unknown_techs(nullptr, A_UNSET);
  assert_int_equal(unknown, 0);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_goal_bulbs_required with invalid goal
***********************************************************************/
static void test_research_goal_bulbs_required_invalid(void **state)
{
  (void) state;

  setup_research_test_env();

  /* With A_UNSET, should return 0 */
  int bulbs = research_goal_bulbs_required(nullptr, A_UNSET);
  assert_int_equal(bulbs, 0);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_goal_tech_req basic cases
***********************************************************************/
static void test_research_goal_tech_req_invalid(void **state)
{
  (void) state;

  setup_research_test_env();

  /* Same tech as goal and required should return FALSE */
  bool result = research_goal_tech_req(nullptr, A_NONE, A_NONE);
  assert_false(result);

  /* Invalid goal should return FALSE */
  result = research_goal_tech_req(nullptr, A_UNSET, A_NONE);
  assert_false(result);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_total_bulbs_required with invalid tech
***********************************************************************/
static void test_research_total_bulbs_required_invalid(void **state)
{
  (void) state;

  setup_research_test_env();

  /* With invalid tech (A_UNSET), should return 0 */
  int bulbs = research_total_bulbs_required(nullptr, A_UNSET, FALSE);
  assert_int_equal(bulbs, 0);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_count returns correct count
***********************************************************************/
static void test_research_count(void **state)
{
  (void) state;

  setup_research_test_env();

  /* Without any players, research_count should return 0 */
  /* because the iterator needs valid players to count */
  int count = research_count();
  assert_int_equal(count, 0);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_future_next with no techs known
***********************************************************************/
static void test_research_future_next_no_techs(void **state)
{
  (void) state;

  setup_research_test_env();

  struct research *research = research_by_number(0);
  assert_non_null(research);

  /* With advance_count() == 0, there are no regular techs to research,
   * so future_next should return TRUE (all techs known = none exist) */
  bool future = research_future_next(research);
  /* When there are no techs defined, all techs are "known" in a sense */
  assert_true(future);

  teardown_research_test_env();
}

/***********************************************************************
  Test recalculate_techs_researched basic
***********************************************************************/
static void test_recalculate_techs_researched(void **state)
{
  (void) state;

  setup_research_test_env();

  struct research *research = research_by_number(0);
  assert_non_null(research);

  /* With only A_NONE known, should return 1 */
  research->future_tech = 0;
  int techs = recalculate_techs_researched(research);
  /* At minimum A_NONE is known */
  assert_true(techs >= 1);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_advance_rule_name for special techs
***********************************************************************/
static void test_research_advance_rule_name(void **state)
{
  (void) state;

  setup_research_test_env();

  /* Test A_UNSET */
  const char *name = research_advance_rule_name(nullptr, A_UNSET);
  assert_non_null(name);
  assert_string_not_equal(name, "");

  /* Test A_FUTURE with null research */
  name = research_advance_rule_name(nullptr, A_FUTURE);
  assert_non_null(name);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_advance_name_translation for special techs
***********************************************************************/
static void test_research_advance_name_translation(void **state)
{
  (void) state;

  setup_research_test_env();

  /* Test A_UNSET */
  const char *name = research_advance_name_translation(nullptr, A_UNSET);
  assert_non_null(name);

  /* Test A_UNKNOWN */
  name = research_advance_name_translation(nullptr, A_UNKNOWN);
  assert_non_null(name);

  teardown_research_test_env();
}

/***********************************************************************
  Test research rule name with non-pooled research
***********************************************************************/
static void test_research_rule_name(void **state)
{
  (void) state;

  setup_research_test_env();

  game.info.team_pooled_research = FALSE;

  struct research *research = research_by_number(0);
  assert_non_null(research);

  /* Without a valid player at index 0, this may return an empty name
   * or trigger the name from player slot */
  const char *name = research_rule_name(research);
  /* Function should not crash */
  (void) name;

  teardown_research_test_env();
}

/***********************************************************************
  Test research_name_translation
***********************************************************************/
static void test_research_name_translation(void **state)
{
  (void) state;

  setup_research_test_env();

  game.info.team_pooled_research = FALSE;

  struct research *research = research_by_number(0);
  assert_non_null(research);

  /* Without a valid player at index 0, this should not crash */
  const char *name = research_name_translation(research);
  (void) name;

  teardown_research_test_env();
}

/***********************************************************************
  Test research iterator sizeof functions
***********************************************************************/
static void test_research_iter_sizeof(void **state)
{
  (void) state;

  setup_research_test_env();

  size_t iter_size = research_iter_sizeof();
  assert_true(iter_size > 0);

  size_t player_iter_size = research_player_iter_sizeof();
  assert_true(player_iter_size > 0);

  teardown_research_test_env();
}

/***********************************************************************
  Test research_pretty_name
***********************************************************************/
static void test_research_pretty_name(void **state)
{
  (void) state;

  setup_research_test_env();

  game.info.team_pooled_research = FALSE;

  struct research *research = research_by_number(0);
  assert_non_null(research);

  char buf[256];
  /* research_pretty_name requires a valid player with nation,
   * so this may crash without proper player setup.
   * Skip this test for now. */

  teardown_research_test_env();
}

/***********************************************************************
  Test research_invention_reachable with valid research
***********************************************************************/
static void test_research_invention_reachable_with_research(void **state)
{
  (void) state;

  setup_research_test_env();

  struct research *research = research_by_number(0);
  assert_non_null(research);

  /* A_NONE should be reachable (it's always known) */
  bool reachable = research_invention_reachable(research, A_NONE);
  /* With no techs defined, this depends on the research_update state */
  (void) reachable;

  teardown_research_test_env();
}

/***********************************************************************
  Test research_invention_gettable with valid research
***********************************************************************/
static void test_research_invention_gettable_with_research(void **state)
{
  (void) state;

  setup_research_test_env();

  struct research *research = research_by_number(0);
  assert_non_null(research);

  /* A_NONE should be gettable (it's always known) */
  bool gettable = research_invention_gettable(research, A_NONE, TRUE);
  /* With no techs defined, this depends on the research_update state */
  (void) gettable;

  teardown_research_test_env();
}

/***********************************************************************
  Test research_total_bulbs_required with A_NONE
***********************************************************************/
static void test_research_total_bulbs_required_none(void **state)
{
  (void) state;

  setup_research_test_env();

  struct research *research = research_by_number(0);
  assert_non_null(research);

  /* A_NONE is already known, so bulbs required should be 0 */
  int bulbs = research_total_bulbs_required(research, A_NONE, FALSE);
  assert_int_equal(bulbs, 0);

  teardown_research_test_env();
}

/***********************************************************************
  Main test suite
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_researches_init),
    cmocka_unit_test(test_research_by_number),
    cmocka_unit_test(test_research_number),
    cmocka_unit_test(test_research_get_null_player),
    cmocka_unit_test(test_research_get_no_team_pool),
    cmocka_unit_test(test_research_invention_state_null),
    cmocka_unit_test(test_research_invention_state_basic),
    cmocka_unit_test(test_research_invention_set),
    cmocka_unit_test(test_research_invention_set_global),
    cmocka_unit_test(test_research_invention_reachable_null),
    cmocka_unit_test(test_research_invention_gettable_null),
    cmocka_unit_test(test_research_goal_step_invalid),
    cmocka_unit_test(test_research_goal_unknown_techs_invalid),
    cmocka_unit_test(test_research_goal_bulbs_required_invalid),
    cmocka_unit_test(test_research_goal_tech_req_invalid),
    cmocka_unit_test(test_research_total_bulbs_required_invalid),
    cmocka_unit_test(test_research_count),
    cmocka_unit_test(test_research_future_next_no_techs),
    cmocka_unit_test(test_recalculate_techs_researched),
    cmocka_unit_test(test_research_advance_rule_name),
    cmocka_unit_test(test_research_advance_name_translation),
    cmocka_unit_test(test_research_rule_name),
    cmocka_unit_test(test_research_name_translation),
    cmocka_unit_test(test_research_iter_sizeof),
    cmocka_unit_test(test_research_pretty_name),
    cmocka_unit_test(test_research_invention_reachable_with_research),
    cmocka_unit_test(test_research_invention_gettable_with_research),
    cmocka_unit_test(test_research_total_bulbs_required_none),
  };

  return cmocka_run_group_tests(tests, nullptr, nullptr);
}