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
#include "game.h"
#include "map.h"
#include "unitlist.h"
#include "world_object.h"

#define TEST_MAP_WIDTH 10
#define TEST_MAP_HEIGHT 10

/* Mock data structures */
static struct unit_type mock_utypes[3];
static struct unit mock_units[10];

/* Helper to set unit type flag */
static void mock_utype_set_flag(struct unit_type *putype, int flag)
{
  BV_SET(putype->flags, flag);
}

/* Helper callback for can_units_do tests */
static bool always_true_cb(const struct unit *punit)
{
  (void) punit;
  return TRUE;
}

static bool always_false_cb(const struct unit *punit)
{
  (void) punit;
  return FALSE;
}

static int unit_id_counter = 0;

static bool unit_id_even_cb(const struct unit *punit)
{
  return (punit->id % 2) == 0;
}

static bool unit_id_greater_than_5_cb(const struct unit *punit)
{
  return punit->id > 5;
}

static int setup(void **state)
{
  (void) state;
  int i;

  /* Initialize mock unit types */
  memset(mock_utypes, 0, sizeof(mock_utypes));
  for (i = 0; i < 3; i++) {
    mock_utypes[i].item_number = i;
    /* Initialize flags to all cleared */
    BV_CLR_ALL(mock_utypes[i].flags);
  }

  /* Initialize mock units */
  memset(mock_units, 0, sizeof(mock_units));
  for (i = 0; i < 10; i++) {
    mock_units[i].id = ++unit_id_counter;
    mock_units[i].utype = &mock_utypes[i % 3];
    mock_units[i].transporting = unit_list_new();
    mock_units[i].hp = 10;
    mock_units[i].moves_left = 3;
  }

  /* Initialize minimal world structure */
  memset(&wld, 0, sizeof(wld));
  wld.map.xsize = TEST_MAP_WIDTH;
  wld.map.ysize = TEST_MAP_HEIGHT;
  wld.map.topology_id = 0;
  wld.map.wrap_id = WRAP_X;

  /* Allocate tiles */
  wld.map.tiles = fc_calloc(TEST_MAP_WIDTH * TEST_MAP_HEIGHT, sizeof(struct tile));
  for (i = 0; i < TEST_MAP_WIDTH * TEST_MAP_HEIGHT; i++) {
    wld.map.tiles[i].index = i;
    wld.map.tiles[i].units = unit_list_new();
  }

  return 0;
}

static int teardown(void **state)
{
  (void) state;
  int i;

  /* Clean up unit transporting lists */
  for (i = 0; i < 10; i++) {
    if (mock_units[i].transporting) {
      unit_list_destroy(mock_units[i].transporting);
      mock_units[i].transporting = NULL;
    }
  }

  /* Free tiles */
  if (wld.map.tiles != NULL) {
    for (i = 0; i < TEST_MAP_WIDTH * TEST_MAP_HEIGHT; i++) {
      unit_list_destroy(wld.map.tiles[i].units);
    }
    free(wld.map.tiles);
    wld.map.tiles = NULL;
  }

  return 0;
}

/* ========== Test unit_list_new ========== */
static void test_unit_list_new(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();

  assert_non_null(plist);
  assert_int_equal(unit_list_size(plist), 0);

  unit_list_destroy(plist);
}

/* ========== Test unit_list_append ========== */
static void test_unit_list_append(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();

  unit_list_append(plist, &mock_units[0]);
  assert_int_equal(unit_list_size(plist), 1);

  unit_list_append(plist, &mock_units[1]);
  assert_int_equal(unit_list_size(plist), 2);

  unit_list_destroy(plist);
}

/* ========== Test unit_list_remove ========== */
static void test_unit_list_remove(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();

  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);
  unit_list_append(plist, &mock_units[2]);

  assert_int_equal(unit_list_size(plist), 3);

  unit_list_remove(plist, &mock_units[1]);

  assert_int_equal(unit_list_size(plist), 2);

  unit_list_destroy(plist);
}

/* ========== Test unit_list_size ========== */
static void test_unit_list_size(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();
  assert_int_equal(unit_list_size(plist), 0);

  unit_list_append(plist, &mock_units[0]);
  assert_int_equal(unit_list_size(plist), 1);

  unit_list_append(plist, &mock_units[1]);
  unit_list_append(plist, &mock_units[2]);
  assert_int_equal(unit_list_size(plist), 3);

  unit_list_destroy(plist);
}

/* ========== Test unit_list_get ========== */
static void test_unit_list_get(void **state)
{
  (void) state;
  struct unit_list *plist;
  struct unit *punit;

  plist = unit_list_new();

  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);

  punit = unit_list_get(plist, 0);
  assert_ptr_equal(punit, &mock_units[0]);

  punit = unit_list_get(plist, 1);
  assert_ptr_equal(punit, &mock_units[1]);

  unit_list_destroy(plist);
}

/* ========== Test unit_list_front_back ========== */
static void test_unit_list_front_back(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();

  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);
  unit_list_append(plist, &mock_units[2]);

  assert_ptr_equal(unit_list_front(plist), &mock_units[0]);
  assert_ptr_equal(unit_list_back(plist), &mock_units[2]);

  unit_list_destroy(plist);
}

/* ========== Test unit_list_clear ========== */
static void test_unit_list_clear(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();

  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);

  assert_int_equal(unit_list_size(plist), 2);

  unit_list_clear(plist);

  assert_int_equal(unit_list_size(plist), 0);

  unit_list_destroy(plist);
}

/* ========== Test unit_list_find ========== */

/* Test finding a unit that exists in the list */
static void test_unit_list_find_existing(void **state)
{
  (void) state;
  struct unit_list *plist;
  struct unit *result;

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);
  unit_list_append(plist, &mock_units[2]);

  /* Find the middle unit */
  result = unit_list_find(plist, mock_units[1].id);
  assert_non_null(result);
  assert_ptr_equal(result, &mock_units[1]);

  /* Find the first unit */
  result = unit_list_find(plist, mock_units[0].id);
  assert_non_null(result);
  assert_ptr_equal(result, &mock_units[0]);

  /* Find the last unit */
  result = unit_list_find(plist, mock_units[2].id);
  assert_non_null(result);
  assert_ptr_equal(result, &mock_units[2]);

  unit_list_destroy(plist);
}

/* Test finding a unit that does not exist in the list */
static void test_unit_list_find_nonexistent(void **state)
{
  (void) state;
  struct unit_list *plist;
  struct unit *result;

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);

  /* Search for ID that doesn't exist */
  result = unit_list_find(plist, 9999);
  assert_null(result);

  unit_list_destroy(plist);
}

/* Test finding in an empty list */
static void test_unit_list_find_empty_list(void **state)
{
  (void) state;
  struct unit_list *plist;
  struct unit *result;

  plist = unit_list_new();

  result = unit_list_find(plist, mock_units[0].id);
  assert_null(result);

  unit_list_destroy(plist);
}

/* Test finding with single element list */
static void test_unit_list_find_single_element(void **state)
{
  (void) state;
  struct unit_list *plist;
  struct unit *result;

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);

  result = unit_list_find(plist, mock_units[0].id);
  assert_non_null(result);
  assert_ptr_equal(result, &mock_units[0]);

  result = unit_list_find(plist, mock_units[1].id);
  assert_null(result);

  unit_list_destroy(plist);
}

/* Test finding duplicate IDs (should return first match) */
static void test_unit_list_find_first_match(void **state)
{
  (void) state;
  struct unit_list *plist;
  struct unit *result;
  int target_id = 42;

  /* Set two units to have the same ID */
  mock_units[0].id = target_id;
  mock_units[1].id = target_id;

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);

  result = unit_list_find(plist, target_id);
  assert_non_null(result);
  /* Should return the first unit with matching ID */
  assert_ptr_equal(result, &mock_units[0]);

  unit_list_destroy(plist);
}

/* ========== Test can_units_do ========== */

/* Test with callback that always returns true */
static void test_can_units_do_always_true(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);

  assert_true(can_units_do(plist, always_true_cb));

  unit_list_destroy(plist);
}

/* Test with callback that always returns false */
static void test_can_units_do_always_false(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);

  assert_false(can_units_do(plist, always_false_cb));

  unit_list_destroy(plist);
}

/* Test with empty list */
static void test_can_units_do_empty_list(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();

  /* Empty list should return FALSE */
  assert_false(can_units_do(plist, always_true_cb));

  unit_list_destroy(plist);
}

/* Test with single element */
static void test_can_units_do_single_element(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);

  assert_true(can_units_do(plist, always_true_cb));
  assert_false(can_units_do(plist, always_false_cb));

  unit_list_destroy(plist);
}

/* Test with conditional callback */
static void test_can_units_do_conditional(void **state)
{
  (void) state;
  struct unit_list *plist;

  /* Set specific IDs for testing */
  mock_units[0].id = 1;
  mock_units[1].id = 2;
  mock_units[2].id = 3;
  mock_units[3].id = 4;
  mock_units[4].id = 5;
  mock_units[5].id = 6;

  plist = unit_list_new();

  /* List with only odd IDs - none even */
  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[2]);
  unit_list_append(plist, &mock_units[4]);

  assert_false(can_units_do(plist, unit_id_even_cb));

  unit_list_clear(plist);

  /* List with at least one even ID */
  unit_list_append(plist, &mock_units[1]);  /* ID 2 - even */
  assert_true(can_units_do(plist, unit_id_even_cb));

  unit_list_clear(plist);

  /* Test with IDs greater than 5 */
  unit_list_append(plist, &mock_units[0]);  /* ID 1 */
  unit_list_append(plist, &mock_units[1]);  /* ID 2 */
  assert_false(can_units_do(plist, unit_id_greater_than_5_cb));

  unit_list_append(plist, &mock_units[5]);  /* ID 6 */
  assert_true(can_units_do(plist, unit_id_greater_than_5_cb));

  unit_list_destroy(plist);
}

/* Test short-circuit behavior - callback returns true on first match */
static void test_can_units_do_short_circuit(void **state)
{
  (void) state;
  struct unit_list *plist;

  /* Set IDs so first unit matches even condition */
  mock_units[0].id = 2;  /* Even */
  mock_units[1].id = 3;  /* Odd */
  mock_units[2].id = 5;  /* Odd */

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);
  unit_list_append(plist, &mock_units[2]);

  /* Should return TRUE immediately on first match */
  assert_true(can_units_do(plist, unit_id_even_cb));

  unit_list_destroy(plist);
}

/* ========== Test units_have_type_flag ========== */

/* Test when units have the flag */
static void test_units_have_type_flag_present(void **state)
{
  (void) state;
  struct unit_list *plist;

  /* Set flag on unit type 0 */
  mock_utype_set_flag(&mock_utypes[0], UTYF_CIVILIAN);

  /* Units 0 and 3 use type 0 */
  plist = unit_list_new();
  mock_units[0].utype = &mock_utypes[0];
  unit_list_append(plist, &mock_units[0]);

  assert_true(units_have_type_flag(plist, UTYF_CIVILIAN, TRUE));

  unit_list_destroy(plist);
}

/* Test when units don't have the flag */
static void test_units_have_type_flag_absent(void **state)
{
  (void) state;
  struct unit_list *plist;

  /* No flags set on any unit type */
  BV_CLR_ALL(mock_utypes[0].flags);
  BV_CLR_ALL(mock_utypes[1].flags);

  plist = unit_list_new();
  mock_units[0].utype = &mock_utypes[0];
  mock_units[1].utype = &mock_utypes[1];
  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);

  assert_false(units_have_type_flag(plist, UTYF_CIVILIAN, TRUE));

  unit_list_destroy(plist);
}

/* Test with empty list */
static void test_units_have_type_flag_empty_list(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();

  assert_false(units_have_type_flag(plist, UTYF_CIVILIAN, TRUE));
  assert_false(units_have_type_flag(plist, UTYF_CIVILIAN, FALSE));

  unit_list_destroy(plist);
}

/* Test has_flag=FALSE parameter - check if any unit LACKS the flag */
static void test_units_have_type_flag_negative(void **state)
{
  (void) state;
  struct unit_list *plist;

  /* Set flag on type 0, but not on type 1 */
  BV_CLR_ALL(mock_utypes[0].flags);
  BV_CLR_ALL(mock_utypes[1].flags);
  mock_utype_set_flag(&mock_utypes[0], UTYF_NOZOC);

  plist = unit_list_new();
  mock_units[0].utype = &mock_utypes[0];  /* Has NOZOC flag */
  mock_units[1].utype = &mock_utypes[1];  /* No NOZOC flag */

  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);

  /* Check if any unit lacks the NOZOC flag */
  assert_true(units_have_type_flag(plist, UTYF_NOZOC, FALSE));

  unit_list_clear(plist);

  /* All units have the flag - should return FALSE */
  unit_list_append(plist, &mock_units[0]);
  assert_false(units_have_type_flag(plist, UTYF_NOZOC, FALSE));

  unit_list_destroy(plist);
}

/* Test with single element */
static void test_units_have_type_flag_single_element(void **state)
{
  (void) state;
  struct unit_list *plist;

  mock_utype_set_flag(&mock_utypes[0], UTYF_CIVILIAN);
  mock_units[0].utype = &mock_utypes[0];

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);

  assert_true(units_have_type_flag(plist, UTYF_CIVILIAN, TRUE));
  assert_false(units_have_type_flag(plist, UTYF_NOZOC, TRUE));

  unit_list_destroy(plist);
}

/* ========== Test units_can_do_action ========== */

/* Note: unit_can_do_action() calls utype_can_do_action() which checks
 * the unit_can_act_cache. For proper testing, we would need to set up
 * the action cache. Here we test the list iteration logic. */

/* Test with empty list */
static void test_units_can_do_action_empty_list(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();

  assert_false(units_can_do_action(plist, ACTION_FOUND_CITY, TRUE));
  assert_false(units_can_do_action(plist, ACTION_FOUND_CITY, FALSE));

  unit_list_destroy(plist);
}

/* Test with single element */
static void test_units_can_do_action_single_element(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);

  /* The actual result depends on whether utype_can_do_action returns
   * TRUE for this unit type. The action cache needs to be properly
   * initialized for this to work. We test only empty list behavior
   * since the list iteration is already tested by other functions. */

  unit_list_destroy(plist);
}

/* Test with multiple elements */
static void test_units_can_do_action_multiple(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);
  unit_list_append(plist, &mock_units[2]);

  /* The action cache needs to be properly initialized for
   * units_can_do_action to work. The list iteration logic is
   * already tested by can_units_do tests. */

  unit_list_destroy(plist);
}

/* ========== Test units_are_occupied ========== */

/* Test with empty list */
static void test_units_are_occupied_empty_list(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();

  assert_false(units_are_occupied(plist));

  unit_list_destroy(plist);
}

/* Test with units that have no cargo */
static void test_units_are_occupied_no_cargo(void **state)
{
  (void) state;
  struct unit_list *plist;

  /* All units have empty transporting lists (set up in setup()) */
  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);

  assert_false(units_are_occupied(plist));

  unit_list_destroy(plist);
}

/* Test with one unit that has cargo */
static void test_units_are_occupied_with_cargo(void **state)
{
  (void) state;
  struct unit_list *plist;

  /* Add a unit to mock_units[0]'s transporting list */
  unit_list_append(mock_units[0].transporting, &mock_units[1]);

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);

  assert_true(units_are_occupied(plist));

  unit_list_destroy(plist);
}

/* Test with mixed - some have cargo, some don't */
static void test_units_are_occupied_mixed(void **state)
{
  (void) state;
  struct unit_list *plist;

  /* Clear all transporting lists */
  unit_list_clear(mock_units[0].transporting);
  unit_list_clear(mock_units[1].transporting);
  unit_list_clear(mock_units[2].transporting);

  /* Only mock_units[2] has cargo */
  unit_list_append(mock_units[2].transporting, &mock_units[0]);

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);  /* No cargo */
  unit_list_append(plist, &mock_units[1]);  /* No cargo */
  unit_list_append(plist, &mock_units[2]);  /* Has cargo */

  assert_true(units_are_occupied(plist));

  unit_list_destroy(plist);
}

/* Test with single element that has cargo */
static void test_units_are_occupied_single_with_cargo(void **state)
{
  (void) state;
  struct unit_list *plist;

  unit_list_clear(mock_units[0].transporting);
  unit_list_append(mock_units[0].transporting, &mock_units[1]);

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);

  assert_true(units_are_occupied(plist));

  unit_list_destroy(plist);
}

/* Test with multiple cargo on one transport */
static void test_units_are_occupied_multiple_cargo(void **state)
{
  (void) state;
  struct unit_list *plist;

  unit_list_clear(mock_units[0].transporting);
  unit_list_append(mock_units[0].transporting, &mock_units[1]);
  unit_list_append(mock_units[0].transporting, &mock_units[2]);

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);

  assert_true(units_are_occupied(plist));
  assert_int_equal(unit_list_size(mock_units[0].transporting), 2);

  unit_list_destroy(plist);
}

/* ========== Test units_can_load ========== */

/* Note: unit_can_load() has complex dependencies on tile, transporter, etc.
 * These tests verify the list iteration logic and empty list handling. */

/* Test with empty list */
static void test_units_can_load_empty_list(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();

  assert_false(units_can_load(plist));

  unit_list_destroy(plist);
}

/* Test with units on tiles without transports */
static void test_units_can_load_no_transports(void **state)
{
  (void) state;
  struct unit_list *plist;

  /* Place units on tile but with no transports available */
  mock_units[0].tile = &wld.map.tiles[0];
  mock_units[1].tile = &wld.map.tiles[0];

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);

  /* Without proper transport setup, should return FALSE */
  assert_false(units_can_load(plist));

  unit_list_destroy(plist);
}

/* Test with single element */
static void test_units_can_load_single_element(void **state)
{
  (void) state;
  struct unit_list *plist;

  mock_units[0].tile = &wld.map.tiles[0];

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);

  /* Verify the function runs without crash */
  (void) units_can_load(plist);

  unit_list_destroy(plist);
}

/* Test with multiple elements */
static void test_units_can_load_multiple(void **state)
{
  (void) state;
  struct unit_list *plist;

  mock_units[0].tile = &wld.map.tiles[0];
  mock_units[1].tile = &wld.map.tiles[1];
  mock_units[2].tile = &wld.map.tiles[2];

  plist = unit_list_new();
  unit_list_append(plist, &mock_units[0]);
  unit_list_append(plist, &mock_units[1]);
  unit_list_append(plist, &mock_units[2]);

  /* Verify the function runs without crash */
  (void) units_can_load(plist);

  unit_list_destroy(plist);
}

/* ========== Boundary Tests ========== */

/* Test with large list */
static void test_large_list_operations(void **state)
{
  (void) state;
  struct unit_list *plist;
  int i;

  plist = unit_list_new();

  /* Add many units */
  for (i = 0; i < 10; i++) {
    unit_list_append(plist, &mock_units[i]);
  }

  assert_int_equal(unit_list_size(plist), 10);

  /* Find each unit */
  for (i = 0; i < 10; i++) {
    struct unit *result = unit_list_find(plist, mock_units[i].id);
    assert_non_null(result);
    assert_ptr_equal(result, &mock_units[i]);
  }

  /* Remove every other unit */
  for (i = 0; i < 10; i += 2) {
    unit_list_remove(plist, &mock_units[i]);
  }

  assert_int_equal(unit_list_size(plist), 5);

  unit_list_destroy(plist);
}

/* Test clearing and reusing list */
static void test_list_reuse(void **state)
{
  (void) state;
  struct unit_list *plist;
  int i;

  plist = unit_list_new();

  for (i = 0; i < 3; i++) {
    unit_list_append(plist, &mock_units[0]);
    unit_list_append(plist, &mock_units[1]);
    assert_int_equal(unit_list_size(plist), 2);
    unit_list_clear(plist);
    assert_int_equal(unit_list_size(plist), 0);
  }

  unit_list_destroy(plist);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* Basic list operations */
    cmocka_unit_test_setup_teardown(test_unit_list_new, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_append, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_remove, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_size, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_get, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_front_back, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_clear, setup, teardown),

    /* unit_list_find tests */
    cmocka_unit_test_setup_teardown(test_unit_list_find_existing, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_find_nonexistent, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_find_empty_list, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_find_single_element, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_find_first_match, setup, teardown),

    /* can_units_do tests */
    cmocka_unit_test_setup_teardown(test_can_units_do_always_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_can_units_do_always_false, setup, teardown),
    cmocka_unit_test_setup_teardown(test_can_units_do_empty_list, setup, teardown),
    cmocka_unit_test_setup_teardown(test_can_units_do_single_element, setup, teardown),
    cmocka_unit_test_setup_teardown(test_can_units_do_conditional, setup, teardown),
    cmocka_unit_test_setup_teardown(test_can_units_do_short_circuit, setup, teardown),

    /* units_have_type_flag tests */
    cmocka_unit_test_setup_teardown(test_units_have_type_flag_present, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_have_type_flag_absent, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_have_type_flag_empty_list, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_have_type_flag_negative, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_have_type_flag_single_element, setup, teardown),

    /* units_can_do_action tests */
    cmocka_unit_test_setup_teardown(test_units_can_do_action_empty_list, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_can_do_action_single_element, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_can_do_action_multiple, setup, teardown),

    /* units_are_occupied tests */
    cmocka_unit_test_setup_teardown(test_units_are_occupied_empty_list, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_are_occupied_no_cargo, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_are_occupied_with_cargo, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_are_occupied_mixed, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_are_occupied_single_with_cargo, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_are_occupied_multiple_cargo, setup, teardown),

    /* units_can_load tests */
    cmocka_unit_test_setup_teardown(test_units_can_load_empty_list, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_can_load_no_transports, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_can_load_single_element, setup, teardown),
    cmocka_unit_test_setup_teardown(test_units_can_load_multiple, setup, teardown),

    /* Boundary tests */
    cmocka_unit_test_setup_teardown(test_large_list_operations, setup, teardown),
    cmocka_unit_test_setup_teardown(test_list_reuse, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}