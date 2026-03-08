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
#include "accessarea.h"
#include "unittype.h"

/***********************************************************************
  Setup function - initialize minimal game state
***********************************************************************/
static int setup(void **state)
{
  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  return 0;
}

/***********************************************************************
  Teardown function - cleanup
***********************************************************************/
static int teardown(void **state)
{
  (void) state;

  /* Ensure access info is closed */
  access_info_close();

  return 0;
}

/***********************************************************************
  Test access_info_init with null unit type
***********************************************************************/
static void test_access_info_init_null(void **state)
{
  (void) state;

  /* Initialize with null - should not crash */
  access_info_init(NULL);

  /* Access unit should be null since init with null does nothing */
  assert_null(access_info_access_unit());
}

/***********************************************************************
  Test access_info_close without init
***********************************************************************/
static void test_access_info_close_no_init(void **state)
{
  (void) state;

  /* Close without init should not crash */
  access_info_close();

  /* Access unit should still be null */
  assert_null(access_info_access_unit());
}

/***********************************************************************
  Test access_info_access_unit without init
***********************************************************************/
static void test_access_info_access_unit_no_init(void **state)
{
  (void) state;

  /* Without init, access_unit should be null */
  assert_null(access_info_access_unit());
}

/***********************************************************************
  Test access_info_init and close cycle with mock unit type
***********************************************************************/
static void test_access_info_init_close_cycle(void **state)
{
  (void) state;

  /* Create a mock unit type */
  struct unit_type mock_utype;
  memset(&mock_utype, 0, sizeof(mock_utype));

  /* Initialize with mock unit type */
  access_info_init(&mock_utype);

  /* Access unit should return our mock unit type */
  const struct unit_type *result = access_info_access_unit();
  assert_non_null(result);
  assert_ptr_equal(result, &mock_utype);

  /* Close should clean up */
  access_info_close();

  /* Access unit should be null after close */
  assert_null(access_info_access_unit());
}

/***********************************************************************
  Test access_info_init multiple times
***********************************************************************/
static void test_access_info_init_multiple(void **state)
{
  (void) state;

  struct unit_type mock_utype1;
  struct unit_type mock_utype2;
  memset(&mock_utype1, 0, sizeof(mock_utype1));
  memset(&mock_utype2, 0, sizeof(mock_utype2));

  /* First init */
  access_info_init(&mock_utype1);
  assert_ptr_equal(access_info_access_unit(), &mock_utype1);

  /* Second init without close - note: this may leak memory in the
   * current implementation, but should not crash */
  access_info_init(&mock_utype2);
  assert_ptr_equal(access_info_access_unit(), &mock_utype2);

  /* Clean up */
  access_info_close();
  assert_null(access_info_access_unit());
}

/***********************************************************************
  Test access_info_close multiple times
***********************************************************************/
static void test_access_info_close_multiple(void **state)
{
  (void) state;

  struct unit_type mock_utype;
  memset(&mock_utype, 0, sizeof(mock_utype));

  /* Initialize */
  access_info_init(&mock_utype);
  assert_non_null(access_info_access_unit());

  /* First close */
  access_info_close();
  assert_null(access_info_access_unit());

  /* Second close should not crash */
  access_info_close();
  assert_null(access_info_access_unit());
}

/***********************************************************************
  Test access_area structure constants
***********************************************************************/
static void test_access_area_constants(void **state)
{
  (void) state;

  /* Test that MAX_NUM_PLAYERS is defined and reasonable */
  assert_true(MAX_NUM_PLAYERS > 0);
  /* MAX_NUM_PLAYERS is typically 128 or 256 depending on configuration */
  assert_true(MAX_NUM_PLAYERS >= 32);
}

/***********************************************************************
  Test aarea_list creation and destruction
***********************************************************************/
static void test_aarea_list_basic(void **state)
{
  (void) state;

  /* Create a new access area list */
  struct aarea_list *list = aarea_list_new();
  assert_non_null(list);
  assert_int_equal(aarea_list_size(list), 0);

  /* Destroy the empty list */
  aarea_list_destroy(list);
}

/***********************************************************************
  Test aarea_list with entries
***********************************************************************/
static void test_aarea_list_with_entries(void **state)
{
  (void) state;

  /* Create a new access area list */
  struct aarea_list *list = aarea_list_new();
  assert_non_null(list);

  /* Create a mock access area */
  struct access_area *aarea = fc_malloc(sizeof(struct access_area));
  memset(aarea, 0, sizeof(struct access_area));
  aarea->cities = city_list_new();
  aarea->capital = false;

  /* Add to list */
  aarea_list_append(list, aarea);
  assert_int_equal(aarea_list_size(list), 1);

  /* Verify the entry */
  struct access_area *first = aarea_list_get(list, 0);
  assert_ptr_equal(first, aarea);
  assert_false(first->capital);

  /* Clean up */
  city_list_destroy(aarea->cities);
  free(aarea);
  aarea_list_destroy(list);
}

/***********************************************************************
  Test aarea_list iteration macro
***********************************************************************/
static void test_aarea_list_iteration(void **state)
{
  (void) state;

  /* Create a new access area list */
  struct aarea_list *list = aarea_list_new();
  assert_non_null(list);

  /* Create mock access areas */
  struct access_area *aarea1 = fc_malloc(sizeof(struct access_area));
  struct access_area *aarea2 = fc_malloc(sizeof(struct access_area));
  memset(aarea1, 0, sizeof(struct access_area));
  memset(aarea2, 0, sizeof(struct access_area));
  aarea1->cities = city_list_new();
  aarea1->capital = true;
  aarea2->cities = city_list_new();
  aarea2->capital = false;

  /* Add to list */
  aarea_list_append(list, aarea1);
  aarea_list_append(list, aarea2);
  assert_int_equal(aarea_list_size(list), 2);

  /* Iterate and count */
  int count = 0;
  int capital_count = 0;
  aarea_list_iterate(list, parea) {
    count++;
    if (parea->capital) {
      capital_count++;
    }
  } aarea_list_iterate_end;

  assert_int_equal(count, 2);
  assert_int_equal(capital_count, 1);

  /* Clean up */
  city_list_destroy(aarea1->cities);
  city_list_destroy(aarea2->cities);
  free(aarea1);
  free(aarea2);
  aarea_list_destroy(list);
}

/***********************************************************************
  Test access_area structure fields
***********************************************************************/
static void test_access_area_structure(void **state)
{
  (void) state;

  /* Create a mock access area and test its fields */
  struct access_area aarea;
  memset(&aarea, 0, sizeof(aarea));

  /* Test default values after memset */
  assert_null(aarea.plr);
  assert_null(aarea.cities);
  assert_false(aarea.capital);

  /* Set values */
  aarea.cities = city_list_new();
  aarea.capital = true;

  /* Verify */
  assert_non_null(aarea.cities);
  assert_true(aarea.capital);

  /* Clean up */
  city_list_destroy(aarea.cities);
}

/***********************************************************************
  Main test runner
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    /* access_info init/close tests */
    cmocka_unit_test_setup_teardown(test_access_info_init_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_access_info_close_no_init, setup, teardown),
    cmocka_unit_test_setup_teardown(test_access_info_access_unit_no_init, setup, teardown),
    cmocka_unit_test_setup_teardown(test_access_info_init_close_cycle, setup, teardown),
    cmocka_unit_test_setup_teardown(test_access_info_init_multiple, setup, teardown),
    cmocka_unit_test_setup_teardown(test_access_info_close_multiple, setup, teardown),

    /* Constants tests */
    cmocka_unit_test_setup_teardown(test_access_area_constants, setup, teardown),

    /* aarea_list tests */
    cmocka_unit_test_setup_teardown(test_aarea_list_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_aarea_list_with_entries, setup, teardown),
    cmocka_unit_test_setup_teardown(test_aarea_list_iteration, setup, teardown),

    /* access_area structure tests */
    cmocka_unit_test_setup_teardown(test_access_area_structure, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}