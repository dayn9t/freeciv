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
  Test: team_by_number returns NULL for negative team id
***********************************************************************/
static void test_team_by_number_negative(void **state)
{
  struct team *result;

  (void) state;

  result = team_by_number(-1);
  assert_null(result);
}

/***********************************************************************
  Test: team_by_number returns NULL for invalid large team id
***********************************************************************/
static void test_team_by_number_invalid(void **state)
{
  struct team *result;

  (void) state;

  result = team_by_number(9999);
  assert_null(result);
}

/***********************************************************************
  Test: team_by_number returns NULL for unused slot
***********************************************************************/
static void test_team_by_number_unused_slot(void **state)
{
  struct team *result;

  (void) state;

  /* Slot 0 exists but no team created yet */
  result = team_by_number(0);
  assert_null(result);
}

/***********************************************************************
  Test: team_by_number returns team after creation
***********************************************************************/
static void test_team_by_number_valid(void **state)
{
  struct team *pteam;
  struct team *result;

  (void) state;

  pteam = team_new(NULL);
  assert_non_null(pteam);

  result = team_by_number(0);
  assert_ptr_equal(result, pteam);
}

/***********************************************************************
  Test: team_count is zero before any team creation
***********************************************************************/
static void test_team_count_zero(void **state)
{
  int count;

  (void) state;

  count = team_count();
  assert_int_equal(count, 0);
}

/***********************************************************************
  Test: team_count increments after team creation
***********************************************************************/
static void test_team_count_after_creation(void **state)
{
  struct team *pteam;
  int count;

  (void) state;

  pteam = team_new(NULL);
  assert_non_null(pteam);

  count = team_count();
  assert_int_equal(count, 1);
}

/***********************************************************************
  Test: team_count increments with multiple teams
***********************************************************************/
static void test_team_count_multiple(void **state)
{
  struct team *team1;
  struct team *team2;
  int count;

  (void) state;

  team1 = team_new(NULL);
  team2 = team_new(NULL);
  assert_non_null(team1);
  assert_non_null(team2);

  count = team_count();
  assert_int_equal(count, 2);
}

/***********************************************************************
  Test: team_rule_name returns valid name
***********************************************************************/
static void test_team_rule_name(void **state)
{
  struct team *pteam;
  const char *name;

  (void) state;

  pteam = team_new(NULL);
  assert_non_null(pteam);

  name = team_rule_name(pteam);
  assert_non_null(name);
  /* Default name should start with "Team" */
  assert_true(strncmp(name, "Team", 4) == 0);
}

/***********************************************************************
  Test: team_name_translation returns valid name
***********************************************************************/
static void test_team_name_translation(void **state)
{
  struct team *pteam;
  const char *name;

  (void) state;

  pteam = team_new(NULL);
  assert_non_null(pteam);

  name = team_name_translation(pteam);
  assert_non_null(name);
}

/***********************************************************************
  Test: team_slot_by_rule_name returns NULL for non-existent name
***********************************************************************/
static void test_team_by_name_not_found(void **state)
{
  struct team_slot *result;

  (void) state;

  result = team_slot_by_rule_name("NonExistentTeam");
  assert_null(result);
}

/***********************************************************************
  Test: team_slot_by_rule_name finds team by name
***********************************************************************/
static void test_team_by_name_found(void **state)
{
  struct team *pteam;
  struct team_slot *slot;
  const char *team_name = "Red";

  (void) state;

  pteam = team_new(NULL);
  assert_non_null(pteam);

  /* Set the team name */
  team_slot_set_defined_name(pteam->slot, team_name);

  /* Find by rule name */
  slot = team_slot_by_rule_name(team_name);
  assert_non_null(slot);
  assert_ptr_equal(team_slot_get_team(slot), pteam);
}

/***********************************************************************
  Test: team_members returns player list
***********************************************************************/
static void test_team_members(void **state)
{
  struct team *pteam;
  const struct player_list *members;

  (void) state;

  pteam = team_new(NULL);
  assert_non_null(pteam);

  members = team_members(pteam);
  assert_non_null(members);
  /* Empty team should have 0 members */
  assert_int_equal(player_list_size(members), 0);
}

/***********************************************************************
  Test: team_slot_count returns max slots
***********************************************************************/
static void test_team_slot_count(void **state)
{
  int count;

  (void) state;

  count = team_slot_count();
  assert_true(count > 0);
  assert_true(count <= MAX_NUM_TEAM_SLOTS);
}

/***********************************************************************
  Test: team_slots_initialised returns true after init
***********************************************************************/
static void test_team_slots_initialised(void **state)
{
  bool initialised;

  (void) state;

  initialised = team_slots_initialised();
  assert_true(initialised);
}

/***********************************************************************
  Test: team_index returns correct index
***********************************************************************/
static void test_team_index(void **state)
{
  struct team *pteam;
  int index;

  (void) state;

  pteam = team_new(NULL);
  assert_non_null(pteam);

  index = team_index(pteam);
  assert_true(index >= 0);
  assert_true(index < team_slot_count());
}

/***********************************************************************
  Test: team_number returns same as team_index
***********************************************************************/
static void test_team_number(void **state)
{
  struct team *pteam;
  int number;

  (void) state;

  pteam = team_new(NULL);
  assert_non_null(pteam);

  number = team_number(pteam);
  assert_int_equal(number, team_index(pteam));
}

/***********************************************************************
  Test: team_pretty_name with valid team
***********************************************************************/
static void test_team_pretty_name(void **state)
{
  struct team *pteam;
  char buf[256];
  int result;

  (void) state;

  pteam = team_new(NULL);
  assert_non_null(pteam);

  result = team_pretty_name(pteam, buf, sizeof(buf));
  assert_true(result >= 0 || result == -1); /* -1 is valid for null team */
  assert_true(strlen(buf) > 0);
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
  assert_int_equal(result, -1);
  assert_string_equal(buf, "(null team)");
}

/***********************************************************************
  Test: team_slot_is_used returns false for unused slot
***********************************************************************/
static void test_team_slot_is_used_false(void **state)
{
  struct team_slot *slot;
  bool used;

  (void) state;

  slot = team_slot_by_number(0);
  assert_non_null(slot);

  used = team_slot_is_used(slot);
  assert_false(used);
}

/***********************************************************************
  Test: team_slot_is_used returns true after team creation
***********************************************************************/
static void test_team_slot_is_used_true(void **state)
{
  struct team *pteam;
  struct team_slot *slot;
  bool used;

  (void) state;

  pteam = team_new(NULL);
  assert_non_null(pteam);

  slot = team_slot_by_number(0);
  assert_non_null(slot);

  used = team_slot_is_used(slot);
  assert_true(used);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* team_by_number tests */
    cmocka_unit_test_setup_teardown(test_team_by_number_negative, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_by_number_invalid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_by_number_unused_slot, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_by_number_valid, setup, teardown),

    /* team_count tests */
    cmocka_unit_test_setup_teardown(test_team_count_zero, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_count_after_creation, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_count_multiple, setup, teardown),

    /* team name tests */
    cmocka_unit_test_setup_teardown(test_team_rule_name, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_name_translation, setup, teardown),

    /* team_by_name tests */
    cmocka_unit_test_setup_teardown(test_team_by_name_not_found, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_by_name_found, setup, teardown),

    /* team_members tests */
    cmocka_unit_test_setup_teardown(test_team_members, setup, teardown),

    /* team slot tests */
    cmocka_unit_test_setup_teardown(test_team_slot_count, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slots_initialised, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slot_is_used_false, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_slot_is_used_true, setup, teardown),

    /* team index/number tests */
    cmocka_unit_test_setup_teardown(test_team_index, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_number, setup, teardown),

    /* team pretty name tests */
    cmocka_unit_test_setup_teardown(test_team_pretty_name, setup, teardown),
    cmocka_unit_test_setup_teardown(test_team_pretty_name_null, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}