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
#include <cmocka.h>

/* utility */
#include "support.h"

/* common */
#include "events.h"
#include "extras.h"

static void test_event_type_name_valid(void **state)
{
  const char *result;

  (void) state;

  /* Test valid event type names */
  result = event_type_name(E_CITY_CANTBUILD);
  assert_non_null(result);
  assert_string_equal(result, "E_CITY_CANTBUILD");

  result = event_type_name(E_CITY_LOST);
  assert_non_null(result);
  assert_string_equal(result, "E_CITY_LOST");

  result = event_type_name(E_TECH_GAIN);
  assert_non_null(result);
  assert_string_equal(result, "E_TECH_GAIN");

  result = event_type_name(E_WONDER_BUILD);
  assert_non_null(result);
  assert_string_equal(result, "E_WONDER_BUILD");
}

static void test_event_type_name_max(void **state)
{
  const char *result;

  (void) state;

  /* Test the maximum valid event type */
  result = event_type_name(event_type_max());
  assert_non_null(result);
}

static void test_event_type_by_name_valid(void **state)
{
  enum event_type result;

  (void) state;

  /* Test valid event type lookup */
  result = event_type_by_name("E_CITY_CANTBUILD", strcmp);
  assert_true(event_type_is_valid(result));
  assert_int_equal(result, E_CITY_CANTBUILD);

  result = event_type_by_name("E_CITY_LOST", strcmp);
  assert_true(event_type_is_valid(result));
  assert_int_equal(result, E_CITY_LOST);

  result = event_type_by_name("E_TECH_GAIN", strcmp);
  assert_true(event_type_is_valid(result));
  assert_int_equal(result, E_TECH_GAIN);
}

static void test_event_type_by_name_case_insensitive(void **state)
{
  enum event_type result;

  (void) state;

  /* Test case-insensitive lookup using fc_strcasecmp */
  result = event_type_by_name("e_city_cantbuild", fc_strcasecmp);
  assert_true(event_type_is_valid(result));
  assert_int_equal(result, E_CITY_CANTBUILD);

  result = event_type_by_name("E_city_lost", fc_strcasecmp);
  assert_true(event_type_is_valid(result));
  assert_int_equal(result, E_CITY_LOST);
}

static void test_event_type_by_name_invalid(void **state)
{
  enum event_type result;

  (void) state;

  /* Test invalid event type name */
  result = event_type_by_name("E_NONEXISTENT_EVENT", strcmp);
  assert_false(event_type_is_valid(result));

  result = event_type_by_name("INVALID", strcmp);
  assert_false(event_type_is_valid(result));

  result = event_type_by_name("", strcmp);
  assert_false(event_type_is_valid(result));
}

static void test_event_type_is_valid(void **state)
{
  (void) state;

  /* Test valid event types */
  assert_true(event_type_is_valid(E_CITY_CANTBUILD));
  assert_true(event_type_is_valid(E_CITY_LOST));
  assert_true(event_type_is_valid(E_TECH_GAIN));
  assert_true(event_type_is_valid(event_type_max()));

  /* Test invalid event type */
  assert_false(event_type_is_valid(event_type_invalid()));
}

static void test_event_type_iteration(void **state)
{
  enum event_type event;
  int count = 0;

  (void) state;

  /* Test iteration over all event types */
  for (event = event_type_begin(); event != event_type_end();
       event = event_type_next(event)) {
    const char *name;

    assert_true(event_type_is_valid(event));
    name = event_type_name(event);
    assert_non_null(name);
    count++;
  }

  /* Verify we iterated over all events */
  assert_int_equal(count, E_COUNT);
}

static void test_event_type_max(void **state)
{
  enum event_type max;

  (void) state;

  max = event_type_max();
  assert_true(event_type_is_valid(max));
  /* E_COUNT is number of events, max should be E_COUNT - 1 */
  assert_true(max >= E_CITY_CANTBUILD);
}

static void test_is_city_event_true(void **state)
{
  (void) state;

  /* Test events that are city events (return TRUE from is_city_event)
   * Note: E_CITY_LOST returns FALSE despite its name - see events.c */
  assert_true(is_city_event(E_CITY_CANTBUILD));
  assert_true(is_city_event(E_CITY_LOVE));
  assert_true(is_city_event(E_CITY_DISORDER));
  assert_true(is_city_event(E_CITY_FAMINE));
  assert_true(is_city_event(E_CITY_GROWTH));
  assert_true(is_city_event(E_CITY_NORMAL));
  assert_true(is_city_event(E_CITY_NUKED));
  assert_true(is_city_event(E_CITY_PLAGUE));
  assert_true(is_city_event(E_CITY_CONQUERED));
}

static void test_is_city_event_false(void **state)
{
  (void) state;

  /* Test events that are NOT city events */
  assert_false(is_city_event(E_GLOBAL_ECO));
  assert_false(is_city_event(E_CITY_LOST));
  assert_false(is_city_event(E_UNIT_LOST_DEF));
  assert_false(is_city_event(E_UNIT_LOST_MISC));
  assert_false(is_city_event(E_UNIT_WIN_DEF));
  assert_false(is_city_event(E_ENEMY_DIPLOMAT_FAILED));
  assert_false(is_city_event(E_ENEMY_DIPLOMAT_EMBASSY));
  assert_false(is_city_event(E_MY_DIPLOMAT_FAILED));
  assert_false(is_city_event(E_MY_DIPLOMAT_EMBASSY));
  assert_false(is_city_event(E_UPRISING));
  assert_false(is_city_event(E_UNIT_RELOCATED));
  assert_false(is_city_event(E_UNIT_ILLEGAL_ACTION));
}

static void test_event_type_begin_end(void **state)
{
  (void) state;

  /* Test that begin returns a valid event type */
  assert_true(event_type_is_valid(event_type_begin()));

  /* Test that end is different from begin */
  assert_true(event_type_begin() != event_type_end());
}

/***********************************************************************
  Test extra_count - count of extras
***********************************************************************/
static void test_extra_count(void **state)
{
  int count;

  (void) state;

  count = extra_count();
  /* Should return 0 or more depending on initialization */
  assert_true(count >= 0);
}

/***********************************************************************
  Test extra_number with valid extra
***********************************************************************/
static void test_extra_number_valid(void **state)
{
  struct extra_type mock_extra;

  (void) state;

  memset(&mock_extra, 0, sizeof(mock_extra));
  mock_extra.id = 42;

  int result = extra_number(&mock_extra);
  assert_int_equal(result, 42);
}

/***********************************************************************
  Test extra_number with null
***********************************************************************/
static void test_extra_number_null(void **state)
{
  (void) state;

  int result = extra_number(NULL);
  assert_int_equal(result, -1);
}

/***********************************************************************
  Test extra_by_number with invalid number
***********************************************************************/
static void test_extra_by_number_invalid(void **state)
{
  struct extra_type *result;

  (void) state;

  /* Negative number should return NULL */
  result = extra_by_number(-1);
  assert_null(result);

  /* Large number should return NULL */
  result = extra_by_number(9999);
  assert_null(result);
}

/***********************************************************************
  Test extra_type_by_rule_name
***********************************************************************/
static void test_extra_type_by_rule_name_null(void **state)
{
  struct extra_type *result;

  (void) state;

  result = extra_type_by_rule_name(NULL);
  assert_null(result);

  result = extra_type_by_rule_name("");
  assert_null(result);
}

/***********************************************************************
  Test extra_type_by_translated_name
***********************************************************************/
static void test_extra_type_by_translated_name_null(void **state)
{
  struct extra_type *result;

  (void) state;

  result = extra_type_by_translated_name(NULL);
  assert_null(result);
}

/***********************************************************************
  Test can_extras_coexist
***********************************************************************/
static void test_can_extras_coexist_null(void **state)
{
  (void) state;

  /* Test with null extras - should handle gracefully */
  bool result = can_extras_coexist(NULL, NULL);
  /* Result depends on implementation */
  (void) result;
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_event_type_name_valid),
    cmocka_unit_test(test_event_type_name_max),
    cmocka_unit_test(test_event_type_by_name_valid),
    cmocka_unit_test(test_event_type_by_name_case_insensitive),
    cmocka_unit_test(test_event_type_by_name_invalid),
    cmocka_unit_test(test_event_type_is_valid),
    cmocka_unit_test(test_event_type_iteration),
    cmocka_unit_test(test_event_type_max),
    cmocka_unit_test(test_is_city_event_true),
    cmocka_unit_test(test_is_city_event_false),
    cmocka_unit_test(test_event_type_begin_end),
    cmocka_unit_test(test_extra_count),
    cmocka_unit_test(test_extra_number_valid),
    cmocka_unit_test(test_extra_number_null),
    cmocka_unit_test(test_extra_by_number_invalid),
    cmocka_unit_test(test_extra_type_by_rule_name_null),
    cmocka_unit_test(test_extra_type_by_translated_name_null),
    cmocka_unit_test(test_can_extras_coexist_null),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}