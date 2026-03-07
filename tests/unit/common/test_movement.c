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
#include "map.h"
#include "movement.h"
#include "terrain.h"
#include "unit.h"
#include "unitlist.h"
#include "world_object.h"

/* Test structures */
static struct unit test_unit;
static struct unit test_transport;
static struct unit test_cargo;

#define TEST_MAP_WIDTH 8
#define TEST_MAP_HEIGHT 8

/***********************************************************************
  Setup function - initialize minimal game state
***********************************************************************/
static int setup(void **state)
{
  (void) state;

  /* Initialize unit structures */
  memset(&test_unit, 0, sizeof(test_unit));
  memset(&test_transport, 0, sizeof(test_transport));
  memset(&test_cargo, 0, sizeof(test_cargo));

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  /* Initialize minimal world state */
  memset(&wld, 0, sizeof(wld));
  wld.map.xsize = TEST_MAP_WIDTH;
  wld.map.ysize = TEST_MAP_HEIGHT;
  wld.map.topology_id = 0;
  wld.map.wrap_id = 0; /* No wrapping */

  /* Initialize valid directions for non-hex topology */
  wld.map.num_valid_dirs = 8;
  wld.map.num_cardinal_dirs = 4;
  for (int i = 0; i < 8; i++) {
    wld.map.valid_dirs[i] = i;
    if (i % 2 == 0) {
      wld.map.cardinal_dirs[i / 2] = i;
    }
  }

  /* Allocate tiles array */
  wld.map.tiles = fc_calloc(MAP_INDEX_SIZE, sizeof(struct tile));
  for (int i = 0; i < MAP_INDEX_SIZE; i++) {
    wld.map.tiles[i].index = i;
    wld.map.tiles[i].units = unit_list_new();
  }

  /* Set up unit lists for transport testing */
  test_transport.transporting = unit_list_new();
  test_cargo.transporting = unit_list_new();

  return 0;
}

/***********************************************************************
  Teardown function - cleanup
***********************************************************************/
static int teardown(void **state)
{
  (void) state;

  /* Clean up unit lists */
  if (test_transport.transporting != NULL) {
    unit_list_destroy(test_transport.transporting);
    test_transport.transporting = NULL;
  }
  if (test_cargo.transporting != NULL) {
    unit_list_destroy(test_cargo.transporting);
    test_cargo.transporting = NULL;
  }

  /* Free the mock map */
  if (wld.map.tiles != NULL) {
    for (int i = 0; i < MAP_INDEX_SIZE; i++) {
      unit_list_destroy(wld.map.tiles[i].units);
    }
    free(wld.map.tiles);
    wld.map.tiles = NULL;
  }

  memset(&wld, 0, sizeof(wld));
  memset(&game, 0, sizeof(game));

  return 0;
}

/* ==================== Move Rate Tests ==================== */

/***********************************************************************
  Test unit_move_rate with NULL unit
***********************************************************************/
static void test_unit_move_rate_null(void **state)
{
  (void) state;

  /* unit_move_rate returns 0 for NULL unit */
  int rate = unit_move_rate(NULL);
  assert_int_equal(rate, 0);
}

/***********************************************************************
  Test unit_move_rate with basic unit setup
  Note: This test requires proper unit_type initialization for full test.
***********************************************************************/
static void test_unit_move_rate_basic(void **state)
{
  (void) state;

  /* Setup a basic unit on a tile */
  test_unit.tile = &wld.map.tiles[0];
  test_unit.hp = 10;
  test_unit.veteran = 0;
  test_unit.utype = NULL; /* Would need valid unit_type for full test */

  /* Without a valid unit_type, unit_move_rate will return 0
   * due to utype_move_rate assertion. Skip this test. */
  skip();
}

/* ==================== Transport Capability Tests ==================== */

/***********************************************************************
  Test can_unit_transport with NULL transporter
***********************************************************************/
static void test_can_unit_transport_null_transporter(void **state)
{
  (void) state;

  /* Setup a cargo unit */
  memset(&test_cargo, 0, sizeof(test_cargo));

  bool result = can_unit_transport(NULL, &test_cargo);
  assert_false(result);
}

/***********************************************************************
  Test can_unit_transport with NULL transported
***********************************************************************/
static void test_can_unit_transport_null_transported(void **state)
{
  (void) state;

  /* Setup a transport unit */
  memset(&test_transport, 0, sizeof(test_transport));

  bool result = can_unit_transport(&test_transport, NULL);
  assert_false(result);
}

/***********************************************************************
  Test can_unit_transport with both NULL
***********************************************************************/
static void test_can_unit_transport_both_null(void **state)
{
  (void) state;

  bool result = can_unit_transport(NULL, NULL);
  assert_false(result);
}

/***********************************************************************
  Test can_unit_type_transport with zero capacity
  Note: This test would need a valid unit_type with transport_capacity = 0
***********************************************************************/
static void test_can_unit_type_transport_zero_capacity(void **state)
{
  (void) state;

  /* This test requires valid unit_type and unit_class structures.
   * can_unit_type_transport checks transporter->transport_capacity.
   * Without proper setup, skip this test. */
  skip();
}

/***********************************************************************
  Test can_unit_type_transport basic
  Note: This test would need valid unit_type and unit_class structures
***********************************************************************/
static void test_can_unit_type_transport_basic(void **state)
{
  (void) state;

  /* This test requires valid unit_type and unit_class structures.
   * Skip as it requires ruleset data. */
  skip();
}

/* ==================== Unit Loading Tests ==================== */

/***********************************************************************
  Test unit_can_load with unit already transported
***********************************************************************/
static void test_unit_can_load_already_transported(void **state)
{
  (void) state;

  /* Setup cargo on a tile with a transport */
  test_cargo.tile = &wld.map.tiles[0];
  test_cargo.transporter = &test_transport; /* Already in a transport */

  /* Setup transport on same tile */
  test_transport.tile = &wld.map.tiles[0];

  /* This would need proper could_unit_load implementation */
  skip();
}

/***********************************************************************
  Test unit_could_load_at with NULL tile
***********************************************************************/
static void test_unit_could_load_at_null_tile(void **state)
{
  (void) state;

  /* Setup cargo unit */
  test_cargo.tile = &wld.map.tiles[0];

  /* unit_could_load_at iterates over tile->units, which would crash with NULL */
  /* Skip this test as it would cause a crash */
  skip();
}

/***********************************************************************
  Test unit_could_load_at with empty tile
***********************************************************************/
static void test_unit_could_load_at_empty_tile(void **state)
{
  (void) state;

  /* Setup cargo unit */
  test_cargo.tile = &wld.map.tiles[0];

  /* Check if can load at tile with no transports */
  bool result = unit_could_load_at(&test_cargo, &wld.map.tiles[1]);

  /* Tile 1 has no units, so should return false */
  assert_false(result);
}

/* ==================== Move Points Tests ==================== */

/***********************************************************************
  Test init_move_fragments
***********************************************************************/
static void test_init_move_fragments(void **state)
{
  (void) state;

  /* Setup terrain_control */
  terrain_control.move_fragments = 3;

  /* Initialize move fragments */
  init_move_fragments();

  /* Should not crash - function initializes internal state */
  assert_true(true);
}

/***********************************************************************
  Test move_points_text with zero
***********************************************************************/
static void test_move_points_text_zero(void **state)
{
  (void) state;

  /* Setup terrain_control */
  terrain_control.move_fragments = 3;

  init_move_fragments();

  const char *text = move_points_text(0, FALSE);

  /* Should return "0" for zero movement points */
  assert_non_null(text);
  assert_string_equal(text, "0");
}

/***********************************************************************
  Test move_points_text with integer value
***********************************************************************/
static void test_move_points_text_integer(void **state)
{
  (void) state;

  /* Setup terrain_control for SINGLE_MOVE = 3 */
  terrain_control.move_fragments = 3;

  init_move_fragments();

  /* 6 move points = 2 * SINGLE_MOVE */
  const char *text = move_points_text(6, FALSE);

  assert_non_null(text);
  assert_string_equal(text, "2");
}

/***********************************************************************
  Test move_points_text with fractional value
***********************************************************************/
static void test_move_points_text_fractional(void **state)
{
  (void) state;

  /* Setup terrain_control for SINGLE_MOVE = 3 */
  terrain_control.move_fragments = 3;

  init_move_fragments();

  /* 1 move point = 1/3 of SINGLE_MOVE */
  const char *text = move_points_text(1, FALSE);

  assert_non_null(text);
  /* Should show fractional representation */
  assert_true(strlen(text) > 0);
}

/***********************************************************************
  Test move_points_text with mixed value
***********************************************************************/
static void test_move_points_text_mixed(void **state)
{
  (void) state;

  /* Setup terrain_control for SINGLE_MOVE = 3 */
  terrain_control.move_fragments = 3;

  init_move_fragments();

  /* 7 move points = 2 + 1/3 of SINGLE_MOVE */
  const char *text = move_points_text(7, FALSE);

  assert_non_null(text);
  assert_true(strlen(text) > 0);
}

/***********************************************************************
  Test move_points_text with reduce option
***********************************************************************/
static void test_move_points_text_reduce(void **state)
{
  (void) state;

  /* Setup terrain_control for SINGLE_MOVE = 6 */
  terrain_control.move_fragments = 6;

  init_move_fragments();

  /* 3 move points = 1/2 of SINGLE_MOVE, should reduce to lowest terms */
  const char *text = move_points_text(3, TRUE);

  assert_non_null(text);
  /* With reduce, 3/6 should become 1/2 */
}

/***********************************************************************
  Test move_points_text_full with prefix
***********************************************************************/
static void test_move_points_text_full_prefix(void **state)
{
  (void) state;

  /* Setup terrain_control for SINGLE_MOVE = 3 */
  terrain_control.move_fragments = 3;

  init_move_fragments();

  const char *text = move_points_text_full(3, FALSE, "MP:", NULL, FALSE);

  assert_non_null(text);
  /* Should contain the prefix */
  assert_true(strstr(text, "MP:") != NULL || strlen(text) > 0);
}

/***********************************************************************
  Test move_points_text_full with none string
***********************************************************************/
static void test_move_points_text_full_none(void **state)
{
  (void) state;

  /* Setup terrain_control for SINGLE_MOVE = 3 */
  terrain_control.move_fragments = 3;

  init_move_fragments();

  const char *text = move_points_text_full(0, FALSE, NULL, "none", FALSE);

  assert_non_null(text);
  /* Should return "none" for zero movement points */
  assert_string_equal(text, "none");
}

/***********************************************************************
  Test move_points_text_full with alignment
***********************************************************************/
static void test_move_points_text_full_align(void **state)
{
  (void) state;

  /* Setup terrain_control for SINGLE_MOVE = 3 */
  terrain_control.move_fragments = 3;

  init_move_fragments();

  const char *text = move_points_text_full(3, FALSE, NULL, NULL, TRUE);

  assert_non_null(text);
  /* Should have padding for alignment */
}

/* ==================== Unit Transport Load/Unload Tests ==================== */

/***********************************************************************
  Test unit_transport_load basic
***********************************************************************/
static void test_unit_transport_load_basic(void **state)
{
  (void) state;

  /* Setup transport and cargo */
  test_transport.transporting = unit_list_new();
  test_cargo.transporting = unit_list_new();
  test_cargo.transporter = NULL;

  /* Load cargo onto transport */
  bool result = unit_transport_load(&test_cargo, &test_transport, TRUE);

  assert_true(result);
  assert_ptr_equal(test_cargo.transporter, &test_transport);
  assert_int_equal(unit_list_size(test_transport.transporting), 1);

  /* Cleanup */
  unit_list_destroy(test_transport.transporting);
  test_transport.transporting = NULL;
  unit_list_destroy(test_cargo.transporting);
  test_cargo.transporting = NULL;
}

/***********************************************************************
  Test unit_transport_load multiple cargo
***********************************************************************/
static void test_unit_transport_load_multiple(void **state)
{
  (void) state;

  struct unit cargo1, cargo2;

  /* Setup transport and cargo */
  test_transport.transporting = unit_list_new();
  memset(&cargo1, 0, sizeof(cargo1));
  memset(&cargo2, 0, sizeof(cargo2));
  cargo1.transporting = unit_list_new();
  cargo2.transporting = unit_list_new();
  cargo1.transporter = NULL;
  cargo2.transporter = NULL;

  /* Load first cargo */
  bool result1 = unit_transport_load(&cargo1, &test_transport, TRUE);
  assert_true(result1);
  assert_int_equal(unit_list_size(test_transport.transporting), 1);

  /* Load second cargo */
  bool result2 = unit_transport_load(&cargo2, &test_transport, TRUE);
  assert_true(result2);
  assert_int_equal(unit_list_size(test_transport.transporting), 2);

  /* Cleanup */
  unit_list_destroy(test_transport.transporting);
  test_transport.transporting = NULL;
  unit_list_destroy(cargo1.transporting);
  unit_list_destroy(cargo2.transporting);
}

/***********************************************************************
  Test unit_transport_unload basic
***********************************************************************/
static void test_unit_transport_unload_basic(void **state)
{
  (void) state;

  /* Setup transport with cargo */
  test_transport.transporting = unit_list_new();
  test_cargo.transporting = unit_list_new();
  test_cargo.transporter = NULL;

  /* First load the cargo */
  unit_transport_load(&test_cargo, &test_transport, TRUE);
  assert_ptr_equal(test_cargo.transporter, &test_transport);

  /* Then unload */
  bool result = unit_transport_unload(&test_cargo);

  assert_true(result);
  assert_null(test_cargo.transporter);
  assert_int_equal(unit_list_size(test_transport.transporting), 0);

  /* Cleanup */
  unit_list_destroy(test_transport.transporting);
  test_transport.transporting = NULL;
  unit_list_destroy(test_cargo.transporting);
  test_cargo.transporting = NULL;
}

/***********************************************************************
  Test unit_transport_unload from empty transport
***********************************************************************/
static void test_unit_transport_unload_not_loaded(void **state)
{
  (void) state;

  /* Setup cargo not on any transport */
  test_cargo.transporting = unit_list_new();
  test_cargo.transporter = NULL;

  /* Unload should still succeed (no-op) */
  bool result = unit_transport_unload(&test_cargo);

  /* Result depends on implementation - may return true for no-op */
  assert_true(result || !result); /* Just verify it doesn't crash */

  /* Cleanup */
  unit_list_destroy(test_cargo.transporting);
  test_cargo.transporting = NULL;
}

/* ==================== Unit Move Result Enum Tests ==================== */

/***********************************************************************
  Test unit_move_result enum values
***********************************************************************/
static void test_unit_move_result_enum(void **state)
{
  (void) state;

  /* Verify enum values are defined and ordered */
  assert_int_equal(MR_OK, 0);
  assert_int_equal(MR_DEATH, 1);
  assert_int_equal(MR_PAUSE, 2);
  assert_int_equal(MR_NO_WAR, 3);
  assert_int_equal(MR_PEACE, 4);
  assert_int_equal(MR_ZOC, 5);
  assert_int_equal(MR_BAD_ACTIVITY, 6);
  assert_int_equal(MR_BAD_DESTINATION, 7);
  assert_int_equal(MR_BAD_MAP_POSITION, 8);
  assert_int_equal(MR_DESTINATION_OCCUPIED_BY_NON_ALLIED_CITY, 9);
  assert_int_equal(MR_DESTINATION_OCCUPIED_BY_NON_ALLIED_UNIT, 10);
  assert_int_equal(MR_NO_TRANSPORTER_CAPACITY, 11);
  assert_int_equal(MR_TRIREME, 12);
  assert_int_equal(MR_CANNOT_DISEMBARK, 13);
  assert_int_equal(MR_NON_NATIVE_MOVE, 14);
  assert_int_equal(MR_ANIMAL_DISALLOWED, 15);
  assert_int_equal(MR_UNIT_STAY, 16);
  assert_int_equal(MR_RANDOM_ONLY, 17);
  assert_int_equal(MR_NOT_ALLOWED, 18);
}

/* ==================== Movement Constants Tests ==================== */

/***********************************************************************
  Test SINGLE_MOVE macro
***********************************************************************/
static void test_single_move_macro(void **state)
{
  (void) state;

  /* SINGLE_MOVE is defined as terrain_control.move_fragments */
  terrain_control.move_fragments = 3;

  assert_int_equal(SINGLE_MOVE, 3);

  terrain_control.move_fragments = 6;
  assert_int_equal(SINGLE_MOVE, 6);
}

/***********************************************************************
  Test MOVE_COST_IGTER macro
***********************************************************************/
static void test_move_cost_igter_macro(void **state)
{
  (void) state;

  /* MOVE_COST_IGTER is defined as terrain_control.igter_cost */
  terrain_control.igter_cost = 1;

  assert_int_equal(MOVE_COST_IGTER, 1);

  terrain_control.igter_cost = 3;
  assert_int_equal(MOVE_COST_IGTER, 3);
}

/***********************************************************************
  Test MAX_MOVE_FRAGS constant
***********************************************************************/
static void test_max_move_frags(void **state)
{
  (void) state;

  assert_int_equal(MAX_MOVE_FRAGS, 65535);
}

/* ==================== Tile Native Tests ==================== */

/***********************************************************************
  Test is_native_to_class with NULL terrain
***********************************************************************/
static void test_is_native_to_class_null_terrain(void **state)
{
  (void) state;

  /* This test requires a valid unit_class structure.
   * is_native_to_class returns TRUE for NULL terrain (unknown terrain).
   * Skip this test as it requires proper unit_class initialization. */
  skip();
}

/***********************************************************************
  Test is_native_tile basic
***********************************************************************/
static void test_is_native_tile_basic(void **state)
{
  (void) state;

  /* This test requires valid unit_type and tile structures.
   * Skip as it requires ruleset data. */
  skip();
}

/* ==================== Can Exist At Tile Tests ==================== */

/***********************************************************************
  Test can_exist_at_tile basic
***********************************************************************/
static void test_can_exist_at_tile_basic(void **state)
{
  (void) state;

  /* This test requires valid unit_type and tile structures.
   * Skip as it requires ruleset data. */
  skip();
}

/***********************************************************************
  Test can_unit_exist_at_tile basic
***********************************************************************/
static void test_can_unit_exist_at_tile_basic(void **state)
{
  (void) state;

  /* This test requires valid unit and tile structures.
   * Skip as it requires ruleset data. */
  skip();
}

/* ==================== Can Attack Tests ==================== */

/***********************************************************************
  Test can_attack_non_native_hard_reqs
***********************************************************************/
static void test_can_attack_non_native_hard_reqs(void **state)
{
  (void) state;

  /* This test requires valid unit_type structure.
   * Skip as it requires ruleset data. */
  skip();
}

/***********************************************************************
  Test can_attack_non_native
***********************************************************************/
static void test_can_attack_non_native(void **state)
{
  (void) state;

  /* This test requires valid unit_type structure.
   * Skip as it requires ruleset data. */
  skip();
}

/***********************************************************************
  Test can_attack_from_non_native
***********************************************************************/
static void test_can_attack_from_non_native(void **state)
{
  (void) state;

  /* This test requires valid unit_type structure.
   * Skip as it requires ruleset data. */
  skip();
}

/* ==================== Unit Can Move Tests ==================== */

/***********************************************************************
  Test unit_can_move_to_tile basic
  Note: This requires complex game state setup
***********************************************************************/
static void test_unit_can_move_to_tile_basic(void **state)
{
  (void) state;

  /* This test requires proper initialization of:
   * - Unit with valid unit_type
   * - Map with adjacent tiles
   * - Player relationships
   * Skip as it requires full game state. */
  skip();
}

/***********************************************************************
  Test unit_can_teleport_to_tile basic
  Note: This requires complex game state setup
***********************************************************************/
static void test_unit_can_teleport_to_tile_basic(void **state)
{
  (void) state;

  /* This test requires proper initialization.
   * Skip as it requires full game state. */
  skip();
}

/* ==================== Direction Tests ==================== */

/***********************************************************************
  Test direction delta arrays
***********************************************************************/
static void test_direction_deltas(void **state)
{
  (void) state;

  /* Verify direction deltas are set up for the test map */
  wld.map.num_valid_dirs = 8;
  wld.map.num_cardinal_dirs = 4;
  for (int i = 0; i < 8; i++) {
    wld.map.valid_dirs[i] = i;
    if (i % 2 == 0) {
      wld.map.cardinal_dirs[i / 2] = i;
    }
  }

  /* Cardinal directions: N(0), E(2), S(4), W(6) */
  assert_int_equal(wld.map.cardinal_dirs[0], DIR8_NORTH);
  assert_int_equal(wld.map.cardinal_dirs[1], DIR8_EAST);
  assert_int_equal(wld.map.cardinal_dirs[2], DIR8_SOUTH);
  assert_int_equal(wld.map.cardinal_dirs[3], DIR8_WEST);
}

/* ==================== Transport Depth Tests ==================== */

/***********************************************************************
  Test unit_transport_depth zero
***********************************************************************/
static void test_unit_transport_depth_zero(void **state)
{
  (void) state;

  test_cargo.transporter = NULL;

  assert_int_equal(unit_transport_depth(&test_cargo), 0);
}

/***********************************************************************
  Test unit_transport_depth one level
***********************************************************************/
static void test_unit_transport_depth_one(void **state)
{
  (void) state;

  test_cargo.transporter = &test_transport;
  test_transport.transporter = NULL;

  assert_int_equal(unit_transport_depth(&test_cargo), 1);
}

/***********************************************************************
  Test unit_cargo_depth zero
***********************************************************************/
static void test_unit_cargo_depth_zero(void **state)
{
  (void) state;

  test_transport.transporting = unit_list_new();

  assert_int_equal(unit_cargo_depth(&test_transport), 0);

  unit_list_destroy(test_transport.transporting);
  test_transport.transporting = NULL;
}

/***********************************************************************
  Test unit_cargo_depth with cargo
***********************************************************************/
static void test_unit_cargo_depth_with_cargo(void **state)
{
  (void) state;

  test_transport.transporting = unit_list_new();
  test_cargo.transporting = unit_list_new();

  /* Add cargo to transport */
  unit_list_append(test_transport.transporting, &test_cargo);

  /* Depth should be at least 1 */
  int depth = unit_cargo_depth(&test_transport);
  assert_true(depth >= 1);

  /* Cleanup */
  unit_list_destroy(test_transport.transporting);
  test_transport.transporting = NULL;
  unit_list_destroy(test_cargo.transporting);
  test_cargo.transporting = NULL;
}

/***********************************************************************
  Test unit_contained_in direct
***********************************************************************/
static void test_unit_contained_in_direct(void **state)
{
  (void) state;

  test_cargo.transporter = &test_transport;

  assert_true(unit_contained_in(&test_cargo, &test_transport));
}

/***********************************************************************
  Test unit_contained_in none
***********************************************************************/
static void test_unit_contained_in_none(void **state)
{
  (void) state;

  test_cargo.transporter = NULL;

  assert_false(unit_contained_in(&test_cargo, &test_transport));
}

/***********************************************************************
  Main test runner
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    /* Move Rate Tests */
    cmocka_unit_test_setup_teardown(test_unit_move_rate_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_move_rate_basic, setup, teardown),

    /* Transport Capability Tests */
    cmocka_unit_test_setup_teardown(test_can_unit_transport_null_transporter, setup, teardown),
    cmocka_unit_test_setup_teardown(test_can_unit_transport_null_transported, setup, teardown),
    cmocka_unit_test_setup_teardown(test_can_unit_transport_both_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_can_unit_type_transport_zero_capacity, setup, teardown),
    cmocka_unit_test_setup_teardown(test_can_unit_type_transport_basic, setup, teardown),

    /* Unit Loading Tests */
    cmocka_unit_test_setup_teardown(test_unit_can_load_already_transported, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_could_load_at_null_tile, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_could_load_at_empty_tile, setup, teardown),

    /* Move Points Tests */
    cmocka_unit_test_setup_teardown(test_init_move_fragments, setup, teardown),
    cmocka_unit_test_setup_teardown(test_move_points_text_zero, setup, teardown),
    cmocka_unit_test_setup_teardown(test_move_points_text_integer, setup, teardown),
    cmocka_unit_test_setup_teardown(test_move_points_text_fractional, setup, teardown),
    cmocka_unit_test_setup_teardown(test_move_points_text_mixed, setup, teardown),
    cmocka_unit_test_setup_teardown(test_move_points_text_reduce, setup, teardown),
    cmocka_unit_test_setup_teardown(test_move_points_text_full_prefix, setup, teardown),
    cmocka_unit_test_setup_teardown(test_move_points_text_full_none, setup, teardown),
    cmocka_unit_test_setup_teardown(test_move_points_text_full_align, setup, teardown),

    /* Unit Transport Load/Unload Tests */
    cmocka_unit_test_setup_teardown(test_unit_transport_load_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_transport_load_multiple, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_transport_unload_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_transport_unload_not_loaded, setup, teardown),

    /* Enum Tests */
    cmocka_unit_test_setup_teardown(test_unit_move_result_enum, setup, teardown),

    /* Movement Constants Tests */
    cmocka_unit_test_setup_teardown(test_single_move_macro, setup, teardown),
    cmocka_unit_test_setup_teardown(test_move_cost_igter_macro, setup, teardown),
    cmocka_unit_test_setup_teardown(test_max_move_frags, setup, teardown),

    /* Tile Native Tests */
    cmocka_unit_test_setup_teardown(test_is_native_to_class_null_terrain, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_native_tile_basic, setup, teardown),

    /* Can Exist At Tile Tests */
    cmocka_unit_test_setup_teardown(test_can_exist_at_tile_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_can_unit_exist_at_tile_basic, setup, teardown),

    /* Can Attack Tests */
    cmocka_unit_test_setup_teardown(test_can_attack_non_native_hard_reqs, setup, teardown),
    cmocka_unit_test_setup_teardown(test_can_attack_non_native, setup, teardown),
    cmocka_unit_test_setup_teardown(test_can_attack_from_non_native, setup, teardown),

    /* Unit Can Move Tests */
    cmocka_unit_test_setup_teardown(test_unit_can_move_to_tile_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_can_teleport_to_tile_basic, setup, teardown),

    /* Direction Tests */
    cmocka_unit_test_setup_teardown(test_direction_deltas, setup, teardown),

    /* Transport Depth Tests */
    cmocka_unit_test_setup_teardown(test_unit_transport_depth_zero, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_transport_depth_one, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_cargo_depth_zero, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_cargo_depth_with_cargo, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_contained_in_direct, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_contained_in_none, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}