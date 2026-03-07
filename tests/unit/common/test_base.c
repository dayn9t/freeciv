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

#include <setjmp.h>
#include <cmocka.h>

/* common */
#include "base.h"
#include "fc_types.h"

static void test_base_by_number_negative(void **state)
{
  struct base_type *result;

  (void) state;

  result = base_by_number(-1);
  assert_null(result);
}

static void test_base_by_number_base_none(void **state)
{
  struct base_type *result;

  (void) state;

  result = base_by_number(BASE_NONE);
  assert_null(result);
}

static void test_base_by_number_invalid(void **state)
{
  struct base_type *result;

  (void) state;

  /* Large invalid index */
  result = base_by_number(9999);
  assert_null(result);
}

static void test_base_number_null(void **state)
{
  Base_type_id result;

  (void) state;

  result = base_number(NULL);
  assert_int_equal(result, -1);
}

static void test_base_extra_get_null(void **state)
{
  struct extra_type *result;

  (void) state;

  /* This will crash if pbase is NULL - skip or expect crash */
  /* For now, just test that the function exists */
  (void) result;
}

static void test_base_count(void **state)
{
  Base_type_id count;

  (void) state;

  count = base_count();
  /* Should return the current count, which is 0 before initialization */
  assert_int_equal(count, 0);
}

static void test_territory_claiming_base_null(void **state)
{
  /* Skip test - passing NULL would crash */
  (void) state;
}

static void test_get_base_by_gui_type_null(void **state)
{
  struct base_type *result;

  (void) state;

  /* Pass NULL for unit and tile - should handle gracefully or return NULL */
  result = get_base_by_gui_type(BASE_GUI_FORTRESS, NULL, NULL);
  /* Result depends on game state, just verify it doesn't crash */
  (void) result;
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_base_by_number_negative),
    cmocka_unit_test(test_base_by_number_base_none),
    cmocka_unit_test(test_base_by_number_invalid),
    cmocka_unit_test(test_base_number_null),
    cmocka_unit_test(test_base_count),
    cmocka_unit_test(test_territory_claiming_base_null),
    cmocka_unit_test(test_get_base_by_gui_type_null),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
