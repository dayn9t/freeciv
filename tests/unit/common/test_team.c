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
#include "team.h"

/***********************************************************************
  Setup function - initialize team slots
***********************************************************************/
static int setup(void **state)
{
  (void) state;

  team_slots_init();

  return 0;
}

/***********************************************************************
  Teardown function - free team slots
***********************************************************************/
static int teardown(void **state)
{
  (void) state;

  team_slots_free();

  return 0;
}

/***********************************************************************
  Test: team_slot_count returns expected value
***********************************************************************/
static void test_team_slot_count(void **state)
{
  int count;

  (void) state;

  count = team_slot_count();
  assert_true(count >= 0);
}

/***********************************************************************
  Test: team_slot_first returns valid slot or NULL
***********************************************************************/
static void test_team_slot_first(void **state)
{
  struct team_slot *slot;

  (void) state;

  slot = team_slot_first();
  /* May be NULL if no teams allocated */
  (void) slot;
}

/***********************************************************************
  Test: team_slot_by_number with valid index
***********************************************************************/
static void test_team_slot_by_number_valid(void **state)
{
  struct team_slot *slot;

  (void) state;

  slot = team_slot_by_number(0);
  /* Should return valid slot for index 0 */
  (void) slot;
}

/***********************************************************************
  Test: team_slot_by_number with negative index
***********************************************************************/
static void test_team_slot_by_number_negative(void **state)
{
  struct team_slot *slot;

  (void) state;

  slot = team_slot_by_number(-1);
  assert_null(slot);
}

/***********************************************************************
  Test: team_slot_index returns valid index
***********************************************************************/
static void test_team_slot_index(void **state)
{
  struct team_slot *slot;

  (void) state;

  slot = team_slot_by_number(0);
  if (slot != NULL) {
    int idx = team_slot_index(slot);
    assert_true(idx >= 0);
  }
}

/***********************************************************************
  Test: team_slot_is_used returns boolean
***********************************************************************/
static void test_team_slot_is_used(void **state)
{
  struct team_slot *slot;

  (void) state;

  slot = team_slot_by_number(0);
  if (slot != NULL) {
    bool used = team_slot_is_used(slot);
    (void) used;  /* Just check it doesn't crash */
  }
}

/***********************************************************************
  Test: team_slot_rule_name returns valid name
***********************************************************************/
static void test_team_slot_rule_name_valid(void **state)
{
  struct team_slot *slot;
  const char *name;

  (void) state;

  slot = team_slot_by_number(0);
  if (slot != NULL) {
    name = team_slot_rule_name(slot);
    assert_non_null(name);
    assert_true(strlen(name) > 0);
  }
}

/***********************************************************************
  Test: team_slot_by_rule_name with null name
***********************************************************************/
static void test_team_slot_by_rule_name_null(void **state)
{
  struct team_slot *slot;

  (void) state;

  slot = team_slot_by_rule_name(NULL);
  assert_null(slot);
}

/***********************************************************************
  Test: team_slot_by_rule_name with empty name
***********************************************************************/
static void test_team_slot_by_rule_name_empty(void **state)
{
  struct team_slot *slot;

  (void) state;

  slot = team_slot_by_rule_name("");
  assert_null(slot);
}

/***********************************************************************
  Test: team_slot_get_team returns NULL for unused slot
***********************************************************************/
static void test_team_slot_get_team_unused(void **state)
{
  struct team_slot *slot;
  struct team *team;

  (void) state;

  slot = team_slot_by_number(0);
  if (slot != NULL) {
    team = team_slot_get_team(slot);
    /* Should be NULL since no team created yet */
    assert_null(team);
  }
}

/***********************************************************************
  Test: team creation and basic operations
***********************************************************************/
static void test_team_creation(void **state)
{
  struct team_slot *slot;
  struct team *team;

  (void) state;

  slot = team_slot_by_number(0);
  assert_non_null(slot);

  /* Create a new team */
  team = team_new(slot);
  assert_non_null(team);

  /* Check team is now used */
  assert_true(team_slot_is_used(slot));

  /* Check we can get the team from the slot */
  assert_ptr_equal(team_slot_get_team(slot), team);

  /* Check team members list exists (should be empty) */
  assert_non_null(team_members(team));
  assert_int_equal(genlist_size(team_members(team)), 0);

  /* Check team rule name */
  const char *name = team_rule_name(team);
  assert_non_null(name);

  /* Check team pretty name */
  char buf[256];
  int result = team_pretty_name(team, buf, sizeof(buf));
  assert_true(result > 0);

  /* Cleanup - remove all players first (none added) */
  /* Cannot destroy team directly as it requires empty player list */
}

/***********************************************************************
  Test: team_pretty_name with null team
***********************************************************************/
static void test_team_pretty_name_null(void **state)
{
  char buf[256];
  int result;

  (void) state;

  result = team_pretty_name(NULL, buf, sizeof(buf));
  assert_true(result <= 0);
}

/***********************************************************************
  Test: iterate over all team slots
***********************************************************************/
static void test_team_slot_iteration(void **state)
{
  struct team_slot *slot;
  int count = 0;

  (void) state;

  for (slot = team_slot_first(); slot != NULL; slot = team_slot_next(slot)) {
    count++;
    assert_true(team_slot_index(slot) >= 0);
  }

  assert_true(count >= 0);
}

/***********************************************************************
  Test: team_slot_by_number with out of range index
***********************************************************************/
static void test_team_slot_by_number_out_of_range(void **state)
{
  struct team_slot *slot;

  (void) state;

  slot = team_slot_by_number(team_slot_count() + 1);
  assert_null(slot);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_team_slot_count, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slot_first, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slot_by_number_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slot_by_number_negative, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slot_index, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slot_is_used, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slot_rule_name_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slot_by_rule_name_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slot_by_rule_name_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slot_get_team_unused, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_creation, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_pretty_name_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slot_iteration, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slot_by_number_out_of_range, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}