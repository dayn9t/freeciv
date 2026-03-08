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
#include "mem.h"

/* common */
#include "base.h"
#include "extras.h"
#include "fc_types.h"
#include "game.h"
#include "map.h"
#include "tile.h"

/* mock */
#include "../mock/mock_game.h"
#include "../mock/mock_map.h"
#include "../mock/mock_player.h"
#include "../mock/mock_unit.h"

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

static void test_get_base_by_gui_type_null(void **state)
{
  struct base_type *result;

  (void) state;

  /* Pass NULL for unit and tile - should handle gracefully or return NULL */
  result = get_base_by_gui_type(BASE_GUI_FORTRESS, NULL, NULL);
  /* Result depends on game state, just verify it doesn't crash */
  (void) result;
}

/***********************************************************************
  Test base_extra_get with valid base
***********************************************************************/
static void test_base_extra_get_valid(void **state)
{
  (void) state;

  /* Create a mock extra_type and base_type */
  struct extra_type mock_extra;
  struct base_type mock_base;

  memset(&mock_extra, 0, sizeof(mock_extra));
  memset(&mock_base, 0, sizeof(mock_base));

  mock_base.self = &mock_extra;

  /* Test that base_extra_get returns the correct extra */
  struct extra_type *result = base_extra_get(&mock_base);
  assert_non_null(result);
  assert_ptr_equal(result, &mock_extra);
}

/***********************************************************************
  Test base_type_init
***********************************************************************/
static void test_base_type_init(void **state)
{
  (void) state;

  struct extra_type mock_extra;
  memset(&mock_extra, 0, sizeof(mock_extra));

  /* Initialize a base type */
  base_type_init(&mock_extra, 5);

  /* Verify the base was created */
  struct base_type *pbase = mock_extra.data.base;
  assert_non_null(pbase);
  assert_int_equal(pbase->item_number, 5);
  assert_ptr_equal(pbase->self, &mock_extra);

  /* Clean up */
  free(pbase);
}

/***********************************************************************
  Test territory_claiming_base
***********************************************************************/
static void test_territory_claiming_base_positive(void **state)
{
  (void) state;

  struct base_type mock_base;
  memset(&mock_base, 0, sizeof(mock_base));
  mock_base.border_sq = 9; /* Positive value means claims territory */

  assert_true(territory_claiming_base(&mock_base));
}

static void test_territory_claiming_base_negative(void **state)
{
  (void) state;

  struct base_type mock_base;
  memset(&mock_base, 0, sizeof(mock_base));
  mock_base.border_sq = -1; /* Negative value means no territory claim */

  assert_false(territory_claiming_base(&mock_base));
}

/***********************************************************************
  Test base_by_number with valid index
***********************************************************************/
static void test_base_by_number_valid(void **state)
{
  (void) state;

  /* After game initialization, this should return a valid base */
  /* For now, test with index 0 which should be valid if bases exist */
  struct base_type *result = base_by_number(0);
  /* Result depends on game state, just verify it doesn't crash */
  (void) result;
}

/***********************************************************************
  Test base_number with valid base
***********************************************************************/
static void test_base_number_valid(void **state)
{
  (void) state;

  struct base_type mock_base;
  memset(&mock_base, 0, sizeof(mock_base));
  mock_base.item_number = 42;

  Base_type_id result = base_number(&mock_base);
  assert_int_equal(result, 42);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_base_by_number_negative),
    cmocka_unit_test(test_base_by_number_base_none),
    cmocka_unit_test(test_base_by_number_invalid),
    cmocka_unit_test(test_base_by_number_valid),
    cmocka_unit_test(test_base_number_null),
    cmocka_unit_test(test_base_number_valid),
    cmocka_unit_test(test_base_extra_get_valid),
    cmocka_unit_test(test_base_count),
    cmocka_unit_test(test_base_type_init),
    cmocka_unit_test(test_territory_claiming_base_positive),
    cmocka_unit_test(test_territory_claiming_base_negative),
    cmocka_unit_test(test_get_base_by_gui_type_null),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
