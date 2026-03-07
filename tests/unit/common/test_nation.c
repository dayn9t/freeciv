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
#include "game.h"
#include "nation.h"

/***********************************************************************
  Test nation_by_number with negative value
***********************************************************************/
static void test_nation_by_number_negative(void **state)
{
  struct nation_type *result;

  (void) state;

  result = nation_by_number(-1);
  assert_null(result);
}

/***********************************************************************
  Test nation_by_number with NATION_NONE
***********************************************************************/
static void test_nation_by_number_none(void **state)
{
  struct nation_type *result;

  (void) state;

  result = nation_by_number(NATION_NONE);
  assert_null(result);
}

/***********************************************************************
  Test nation_by_number with large invalid value
***********************************************************************/
static void test_nation_by_number_invalid(void **state)
{
  struct nation_type *result;

  (void) state;

  result = nation_by_number(9999);
  assert_null(result);
}

/***********************************************************************
  Test nation_count returns 0 before initialization
***********************************************************************/
static void test_nation_count_uninitialized(void **state)
{
  Nation_type_id count;

  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  count = nation_count();
  assert_int_equal(count, 0);
}

/***********************************************************************
  Test nation_by_rule_name with non-existent name
***********************************************************************/
static void test_nation_by_rule_name_nonexistent(void **state)
{
  struct nation_type *result;

  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  result = nation_by_rule_name("NonExistentNation");
  assert_null(result);
}

/***********************************************************************
  Test nation_by_translated_plural with NULL name
***********************************************************************/
static void test_nation_by_translated_plural_null(void **state)
{
  struct nation_type *result;

  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  result = nation_by_translated_plural(NULL);
  assert_null(result);
}

/***********************************************************************
  Test nation_rule_name with NULL nation
***********************************************************************/
static void test_nation_rule_name_null(void **state)
{
  const char *result;

  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  result = nation_rule_name(NULL);
  assert_string_equal(result, "");
}

/***********************************************************************
  Test nation_adjective_translation with NULL nation
***********************************************************************/
static void test_nation_adjective_translation_null(void **state)
{
  const char *result;

  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  result = nation_adjective_translation(NULL);
  assert_string_equal(result, "");
}

/***********************************************************************
  Test nation_plural_translation with NULL nation
***********************************************************************/
static void test_nation_plural_translation_null(void **state)
{
  const char *result;

  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  result = nation_plural_translation(NULL);
  assert_string_equal(result, "");
}

/***********************************************************************
  Test is_nation_playable with NULL nation
***********************************************************************/
static void test_is_nation_playable_null(void **state)
{
  bool result;

  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  result = is_nation_playable(NULL);
  assert_false(result);
}

/***********************************************************************
  Test nation_barbarian_type with NULL nation
***********************************************************************/
static void test_nation_barbarian_type_null(void **state)
{
  enum barbarian_type result;

  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  result = nation_barbarian_type(NULL);
  assert_int_equal(result, NOT_A_BARBARIAN);
}

/***********************************************************************
  Test nation_leaders with NULL nation
***********************************************************************/
static void test_nation_leaders_null(void **state)
{
  const struct nation_leader_list *result;

  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  result = nation_leaders(NULL);
  assert_null(result);
}

/***********************************************************************
  Test nation_cities with NULL nation
***********************************************************************/
static void test_nation_cities_null(void **state)
{
  const struct nation_city_list *result;

  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  result = nation_cities(NULL);
  assert_null(result);
}

/***********************************************************************
  Test style_of_nation with NULL nation
***********************************************************************/
static void test_style_of_nation_null(void **state)
{
  struct nation_style *result;

  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  result = style_of_nation(NULL);
  assert_null(result);
}

/***********************************************************************
  Test nation_color with NULL nation
***********************************************************************/
static void test_nation_color_null(void **state)
{
  const struct rgbcolor *result;

  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  result = nation_color(NULL);
  assert_null(result);
}

/***********************************************************************
  Test nation_set_count
***********************************************************************/
static void test_nation_set_count(void **state)
{
  int count;

  (void) state;

  count = nation_set_count();
  assert_int_equal(count, 0);
}

/***********************************************************************
  Test nation_group_count
***********************************************************************/
static void test_nation_group_count(void **state)
{
  int count;

  (void) state;

  count = nation_group_count();
  assert_int_equal(count, 0);
}

/***********************************************************************
  Test nation_set_by_number with invalid value
***********************************************************************/
static void test_nation_set_by_number_invalid(void **state)
{
  struct nation_set *result;

  (void) state;

  result = nation_set_by_number(-1);
  assert_null(result);
}

/***********************************************************************
  Test nation_group_by_number with invalid value
***********************************************************************/
static void test_nation_group_by_number_invalid(void **state)
{
  struct nation_group *result;

  (void) state;

  result = nation_group_by_number(-1);
  assert_null(result);
}

/***********************************************************************
  Test nation_set_by_rule_name with empty string
***********************************************************************/
static void test_nation_set_by_rule_name_empty(void **state)
{
  struct nation_set *result;

  (void) state;

  result = nation_set_by_rule_name("");
  assert_null(result);
}

/***********************************************************************
  Test nation_group_by_rule_name with empty string
***********************************************************************/
static void test_nation_group_by_rule_name_empty(void **state)
{
  struct nation_group *result;

  (void) state;

  result = nation_group_by_rule_name("");
  assert_null(result);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_nation_by_number_negative),
    cmocka_unit_test(test_nation_by_number_none),
    cmocka_unit_test(test_nation_by_number_invalid),
    cmocka_unit_test(test_nation_count_uninitialized),
    cmocka_unit_test(test_nation_by_rule_name_nonexistent),
    cmocka_unit_test(test_nation_by_translated_plural_null),
    cmocka_unit_test(test_nation_rule_name_null),
    cmocka_unit_test(test_nation_adjective_translation_null),
    cmocka_unit_test(test_nation_plural_translation_null),
    cmocka_unit_test(test_is_nation_playable_null),
    cmocka_unit_test(test_nation_barbarian_type_null),
    cmocka_unit_test(test_nation_leaders_null),
    cmocka_unit_test(test_nation_cities_null),
    cmocka_unit_test(test_style_of_nation_null),
    cmocka_unit_test(test_nation_color_null),
    cmocka_unit_test(test_nation_set_count),
    cmocka_unit_test(test_nation_group_count),
    cmocka_unit_test(test_nation_set_by_number_invalid),
    cmocka_unit_test(test_nation_group_by_number_invalid),
    cmocka_unit_test(test_nation_set_by_rule_name_empty),
    cmocka_unit_test(test_nation_group_by_rule_name_empty),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}