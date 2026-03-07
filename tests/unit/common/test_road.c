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

/* common */
#include "extras.h"
#include "fc_types.h"
#include "road.h"

static void test_road_by_number_negative(void **state)
{
  struct road_type *result;

  (void) state;

  result = road_by_number(-1);
  assert_null(result);
}

static void test_road_by_number_road_none(void **state)
{
  struct road_type *result;

  (void) state;

  result = road_by_number(ROAD_NONE);
  assert_null(result);
}

static void test_road_by_number_invalid(void **state)
{
  struct road_type *result;

  (void) state;

  /* Large invalid index */
  result = road_by_number(9999);
  assert_null(result);
}

static void test_road_number_null(void **state)
{
  Road_type_id result;

  (void) state;

  result = road_number(NULL);
  assert_int_equal(result, -1);
}

static void test_road_count(void **state)
{
  Road_type_id count;

  (void) state;

  count = road_count();
  /* Should return the current count, which is 0 before initialization */
  assert_int_equal(count, 0);
}

static void test_road_has_flag_null(void **state)
{
  (void) state;

  /* road_has_flag with NULL would crash due to BV_ISSET on NULL pointer.
   * Skip this test as the function doesn't handle NULL gracefully. */
}

static void test_road_extra_get_null(void **state)
{
  struct extra_type *result;

  (void) state;

  /* road_extra_get with NULL would crash due to dereferencing NULL pointer.
   * Skip this test as the function doesn't handle NULL gracefully. */
  (void) result;
}

static void test_road_by_compat_special_none(void **state)
{
  struct road_type *result;

  (void) state;

  result = road_by_compat_special(ROCO_NONE);
  assert_null(result);
}

static void test_road_by_gui_type_other(void **state)
{
  struct road_type *result;

  (void) state;

  /* ROAD_GUI_OTHER should return NULL according to implementation */
  result = road_by_gui_type(ROAD_GUI_OTHER);
  assert_null(result);
}

static void test_road_provides_move_bonus_null(void **state)
{
  (void) state;

  /* road_provides_move_bonus with NULL would crash due to dereferencing.
   * Skip this test as the function doesn't handle NULL gracefully. */
}

static void test_road_compat_special_null(void **state)
{
  (void) state;

  /* road_compat_special with NULL would crash due to dereferencing.
   * Skip this test as the function doesn't handle NULL gracefully. */
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_road_by_number_negative),
    cmocka_unit_test(test_road_by_number_road_none),
    cmocka_unit_test(test_road_by_number_invalid),
    cmocka_unit_test(test_road_number_null),
    cmocka_unit_test(test_road_count),
    cmocka_unit_test(test_road_has_flag_null),
    cmocka_unit_test(test_road_extra_get_null),
    cmocka_unit_test(test_road_by_compat_special_none),
    cmocka_unit_test(test_road_by_gui_type_other),
    cmocka_unit_test(test_road_provides_move_bonus_null),
    cmocka_unit_test(test_road_compat_special_null),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}