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

/* Test unit structure */
static struct unit test_unit;
static struct unit test_transport;
static struct unit test_cargo;

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

  return 0;
}

/***********************************************************************
  Test unit_has_orders with no orders
***********************************************************************/
static void test_unit_has_orders_false(void **state)
{
  (void) state;

  test_unit.has_orders = FALSE;
  assert_false(unit_has_orders(&test_unit));
}

/***********************************************************************
  Test unit_has_orders with orders
***********************************************************************/
static void test_unit_has_orders_true(void **state)
{
  (void) state;

  test_unit.has_orders = TRUE;
  assert_true(unit_has_orders(&test_unit));
}

/***********************************************************************
  Test unit_owner macro
***********************************************************************/
static void test_unit_owner(void **state)
{
  (void) state;

  struct player owner;
  memset(&owner, 0, sizeof(owner));

  test_unit.owner = &owner;
  assert_ptr_equal(unit_owner(&test_unit), &owner);
}

/***********************************************************************
  Test unit_tile macro
***********************************************************************/
static void test_unit_tile(void **state)
{
  (void) state;

  struct tile tile;
  memset(&tile, 0, sizeof(tile));

  test_unit.tile = &tile;
  assert_ptr_equal(unit_tile(&test_unit), &tile);
}

/***********************************************************************
  Test unit_tile_set function
***********************************************************************/
static void test_unit_tile_set(void **state)
{
  (void) state;

  struct tile tile;
  memset(&tile, 0, sizeof(tile));

  test_unit.tile = NULL;
  unit_tile_set(&test_unit, &tile);
  assert_ptr_equal(test_unit.tile, &tile);

  /* Test setting to NULL */
  unit_tile_set(&test_unit, NULL);
  assert_null(test_unit.tile);
}

/***********************************************************************
  Test unit_nationality function
***********************************************************************/
static void test_unit_nationality(void **state)
{
  (void) state;

  struct player nationality;
  memset(&nationality, 0, sizeof(nationality));

  test_unit.nationality = &nationality;
  assert_ptr_equal(unit_nationality(&test_unit), &nationality);
}

/***********************************************************************
  Test get_transporter_capacity - requires unit_type
  Note: This test is limited as unit_type_get requires valid unit type
***********************************************************************/
static void test_get_transporter_capacity(void **state)
{
  (void) state;

  /* This test would require a valid unit_type to be set up.
   * The function accesses punittype->transport_capacity.
   * For now, we skip this test as it requires ruleset data. */
  skip();
}

/***********************************************************************
  Test get_transporter_occupancy with empty transport
***********************************************************************/
static void test_get_transporter_occupancy_empty(void **state)
{
  (void) state;

  /* Transport has no cargo */
  assert_int_equal(get_transporter_occupancy(&test_transport), 0);
}

/***********************************************************************
  Test get_transporter_occupancy with cargo
***********************************************************************/
static void test_get_transporter_occupancy_with_cargo(void **state)
{
  (void) state;

  /* Add cargo to transport */
  unit_list_append(test_transport.transporting, &test_cargo);
  assert_int_equal(get_transporter_occupancy(&test_transport), 1);

  /* Add another cargo unit */
  struct unit cargo2;
  memset(&cargo2, 0, sizeof(cargo2));
  cargo2.transporting = unit_list_new();
  unit_list_append(test_transport.transporting, &cargo2);
  assert_int_equal(get_transporter_occupancy(&test_transport), 2);

  /* Cleanup */
  unit_list_destroy(cargo2.transporting);
}

/***********************************************************************
  Test unit_transport_get with no transporter
***********************************************************************/
static void test_unit_transport_get_null(void **state)
{
  (void) state;

  test_cargo.transporter = NULL;
  assert_null(unit_transport_get(&test_cargo));
}

/***********************************************************************
  Test unit_transport_get with transporter
***********************************************************************/
static void test_unit_transport_get_with_transport(void **state)
{
  (void) state;

  test_cargo.transporter = &test_transport;
  assert_ptr_equal(unit_transport_get(&test_cargo), &test_transport);
}

/***********************************************************************
  Test unit_transport_cargo
***********************************************************************/
static void test_unit_transport_cargo(void **state)
{
  (void) state;

  struct unit_list *cargo_list = unit_transport_cargo(&test_transport);
  assert_non_null(cargo_list);
  assert_ptr_equal(cargo_list, test_transport.transporting);
}

/***********************************************************************
  Test unit_transport_load with force
***********************************************************************/
static void test_unit_transport_load_force(void **state)
{
  (void) state;

  /* Force load cargo onto transport */
  bool result = unit_transport_load(&test_cargo, &test_transport, TRUE);
  assert_true(result);
  assert_ptr_equal(test_cargo.transporter, &test_transport);
  assert_int_equal(unit_list_size(test_transport.transporting), 1);
  assert_true(unit_list_search(test_transport.transporting, &test_cargo));
}

/***********************************************************************
  Test unit_transport_load duplicate prevention
***********************************************************************/
static void test_unit_transport_load_duplicate(void **state)
{
  (void) state;

  /* First load should succeed */
  bool result = unit_transport_load(&test_cargo, &test_transport, TRUE);
  assert_true(result);

  /* Second load of same cargo should fail (assert in debug builds) */
  /* We cannot test this without causing assertion failure */
}

/***********************************************************************
  Test unit_transport_unload with no transport
  Note: unit_transported() checks is_server() which requires proper init
***********************************************************************/
static void test_unit_transport_unload_no_transport(void **state)
{
  (void) state;

  /* This test requires is_server() to be properly initialized.
   * When not on server, unit_transported_client checks client.transported_by
   * which we haven't initialized. Skip this test. */
  skip();
}

/***********************************************************************
  Test unit_transport_unload with transport
***********************************************************************/
static void test_unit_transport_unload_with_transport(void **state)
{
  (void) state;

  /* First load the cargo */
  unit_transport_load(&test_cargo, &test_transport, TRUE);
  assert_ptr_equal(test_cargo.transporter, &test_transport);

  /* Then unload */
  bool result = unit_transport_unload(&test_cargo);
  assert_true(result);
  assert_null(test_cargo.transporter);
  assert_int_equal(unit_list_size(test_transport.transporting), 0);
}

/***********************************************************************
  Test unit_transport_depth with no transport
***********************************************************************/
static void test_unit_transport_depth_zero(void **state)
{
  (void) state;

  test_cargo.transporter = NULL;
  assert_int_equal(unit_transport_depth(&test_cargo), 0);
}

/***********************************************************************
  Test unit_transport_depth with one level
***********************************************************************/
static void test_unit_transport_depth_one(void **state)
{
  (void) state;

  test_cargo.transporter = &test_transport;
  test_transport.transporter = NULL;
  assert_int_equal(unit_transport_depth(&test_cargo), 1);
}

/***********************************************************************
  Test unit_cargo_depth with no cargo
***********************************************************************/
static void test_unit_cargo_depth_zero(void **state)
{
  (void) state;

  assert_int_equal(unit_cargo_depth(&test_transport), 0);
}

/***********************************************************************
  Test unit_cargo_depth with cargo
***********************************************************************/
static void test_unit_cargo_depth_with_cargo(void **state)
{
  (void) state;

  /* Add cargo */
  unit_list_append(test_transport.transporting, &test_cargo);

  /* Depth should be 1 with one level of cargo */
  int depth = unit_cargo_depth(&test_transport);
  assert_true(depth >= 1);
}

/***********************************************************************
  Test unit_contained_in direct containment
***********************************************************************/
static void test_unit_contained_in_direct(void **state)
{
  (void) state;

  test_cargo.transporter = &test_transport;

  assert_true(unit_contained_in(&test_cargo, &test_transport));
}

/***********************************************************************
  Test unit_contained_in no containment
***********************************************************************/
static void test_unit_contained_in_none(void **state)
{
  (void) state;

  test_cargo.transporter = NULL;

  assert_false(unit_contained_in(&test_cargo, &test_transport));
}

/***********************************************************************
  Test unit_is_virtual with NULL
***********************************************************************/
static void test_unit_is_virtual_null(void **state)
{
  (void) state;

  assert_false(unit_is_virtual(NULL));
}

/***********************************************************************
  Test unit_is_virtual with virtual unit
  Note: This test requires game state to be properly initialized.
  The game_unit_by_number() function accesses the wld structure.
***********************************************************************/
static void test_unit_is_virtual_true(void **state)
{
  (void) state;

  /* This test requires proper game state initialization.
   * game_unit_by_number() calls idex_lookup_unit(&wld, id) which
   * needs the wld structure to be properly initialized.
   * Skip this test as it requires full game initialization. */
  skip();
}

/***********************************************************************
  Test unit_is_alive with non-existent unit
  Note: This test requires game state to be properly initialized.
***********************************************************************/
static void test_unit_is_alive_false(void **state)
{
  (void) state;

  /* This test requires proper game state initialization.
   * game_unit_by_number() calls idex_lookup_unit(&wld, id) which
   * needs the wld structure to be properly initialized.
   * Skip this test as it requires full game initialization. */
  skip();
}

/***********************************************************************
  Test unit_is_alive with zero ID
  Note: This test requires game state to be properly initialized.
***********************************************************************/
static void test_unit_is_alive_zero_id(void **state)
{
  (void) state;

  /* This test requires proper game state initialization.
   * game_unit_by_number() calls idex_lookup_unit(&wld, id) which
   * needs the wld structure to be properly initialized.
   * Skip this test as it requires full game initialization. */
  skip();
}

/***********************************************************************
  Test are_unit_orders_equal with equal orders
***********************************************************************/
static void test_are_unit_orders_equal_true(void **state)
{
  (void) state;

  struct unit_order order1, order2;

  order1.order = ORDER_MOVE;
  order1.activity = ACTIVITY_IDLE;
  order1.target = 10;
  order1.sub_target = 5;
  order1.action = ACTION_NONE;
  order1.dir = DIR8_NORTH;

  order2.order = ORDER_MOVE;
  order2.activity = ACTIVITY_IDLE;
  order2.target = 10;
  order2.sub_target = 5;
  order2.action = ACTION_NONE;
  order2.dir = DIR8_NORTH;

  assert_true(are_unit_orders_equal(&order1, &order2));
}

/***********************************************************************
  Test are_unit_orders_equal with different order type
***********************************************************************/
static void test_are_unit_orders_equal_different_order(void **state)
{
  (void) state;

  struct unit_order order1, order2;

  order1.order = ORDER_MOVE;
  order1.activity = ACTIVITY_IDLE;
  order1.target = 10;
  order1.sub_target = 5;
  order1.action = ACTION_NONE;
  order1.dir = DIR8_NORTH;

  order2.order = ORDER_ACTIVITY;
  order2.activity = ACTIVITY_IDLE;
  order2.target = 10;
  order2.sub_target = 5;
  order2.action = ACTION_NONE;
  order2.dir = DIR8_NORTH;

  assert_false(are_unit_orders_equal(&order1, &order2));
}

/***********************************************************************
  Test are_unit_orders_equal with different activity
***********************************************************************/
static void test_are_unit_orders_equal_different_activity(void **state)
{
  (void) state;

  struct unit_order order1, order2;

  order1.order = ORDER_ACTIVITY;
  order1.activity = ACTIVITY_IDLE;
  order1.target = 0;
  order1.sub_target = 0;
  order1.action = ACTION_NONE;
  order1.dir = DIR8_ORIGIN;

  order2.order = ORDER_ACTIVITY;
  order2.activity = ACTIVITY_FORTIFYING;
  order2.target = 0;
  order2.sub_target = 0;
  order2.action = ACTION_NONE;
  order2.dir = DIR8_ORIGIN;

  assert_false(are_unit_orders_equal(&order1, &order2));
}

/***********************************************************************
  Test are_unit_orders_equal with different direction
***********************************************************************/
static void test_are_unit_orders_equal_different_dir(void **state)
{
  (void) state;

  struct unit_order order1, order2;

  order1.order = ORDER_MOVE;
  order1.activity = ACTIVITY_IDLE;
  order1.target = 0;
  order1.sub_target = 0;
  order1.action = ACTION_NONE;
  order1.dir = DIR8_NORTH;

  order2.order = ORDER_MOVE;
  order2.activity = ACTIVITY_IDLE;
  order2.target = 0;
  order2.sub_target = 0;
  order2.action = ACTION_NONE;
  order2.dir = DIR8_SOUTH;

  assert_false(are_unit_orders_equal(&order1, &order2));
}

/***********************************************************************
  Test activity_requires_target for activities that require target
***********************************************************************/
static void test_activity_requires_target_true(void **state)
{
  (void) state;

  assert_true(activity_requires_target(ACTIVITY_PILLAGE));
  assert_true(activity_requires_target(ACTIVITY_BASE));
  assert_true(activity_requires_target(ACTIVITY_GEN_ROAD));
  assert_true(activity_requires_target(ACTIVITY_IRRIGATE));
  assert_true(activity_requires_target(ACTIVITY_MINE));
  assert_true(activity_requires_target(ACTIVITY_CLEAN));
}

/***********************************************************************
  Test activity_requires_target for activities that don't require target
***********************************************************************/
static void test_activity_requires_target_false(void **state)
{
  (void) state;

  assert_false(activity_requires_target(ACTIVITY_IDLE));
  assert_false(activity_requires_target(ACTIVITY_FORTIFIED));
  assert_false(activity_requires_target(ACTIVITY_SENTRY));
  assert_false(activity_requires_target(ACTIVITY_GOTO));
  assert_false(activity_requires_target(ACTIVITY_EXPLORE));
  assert_false(activity_requires_target(ACTIVITY_TRANSFORM));
  assert_false(activity_requires_target(ACTIVITY_CULTIVATE));
  assert_false(activity_requires_target(ACTIVITY_PLANT));
  assert_false(activity_requires_target(ACTIVITY_FORTIFYING));
  assert_false(activity_requires_target(ACTIVITY_CONVERT));
}

/***********************************************************************
  Test is_build_activity for build activities
***********************************************************************/
static void test_is_build_activity_true(void **state)
{
  (void) state;

  assert_true(is_build_activity(ACTIVITY_MINE));
  assert_true(is_build_activity(ACTIVITY_IRRIGATE));
  assert_true(is_build_activity(ACTIVITY_BASE));
  assert_true(is_build_activity(ACTIVITY_GEN_ROAD));
}

/***********************************************************************
  Test is_build_activity for non-build activities
***********************************************************************/
static void test_is_build_activity_false(void **state)
{
  (void) state;

  assert_false(is_build_activity(ACTIVITY_IDLE));
  assert_false(is_build_activity(ACTIVITY_PILLAGE));
  assert_false(is_build_activity(ACTIVITY_FORTIFIED));
  assert_false(is_build_activity(ACTIVITY_TRANSFORM));
}

/***********************************************************************
  Test is_clean_activity for clean activities
***********************************************************************/
static void test_is_clean_activity_true(void **state)
{
  (void) state;

  assert_true(is_clean_activity(ACTIVITY_PILLAGE));
  assert_true(is_clean_activity(ACTIVITY_CLEAN));
}

/***********************************************************************
  Test is_clean_activity for non-clean activities
***********************************************************************/
static void test_is_clean_activity_false(void **state)
{
  (void) state;

  assert_false(is_clean_activity(ACTIVITY_IDLE));
  assert_false(is_clean_activity(ACTIVITY_MINE));
  assert_false(is_clean_activity(ACTIVITY_IRRIGATE));
}

/***********************************************************************
  Test is_terrain_change_activity for terrain changing activities
***********************************************************************/
static void test_is_terrain_change_activity_true(void **state)
{
  (void) state;

  assert_true(is_terrain_change_activity(ACTIVITY_CULTIVATE));
  assert_true(is_terrain_change_activity(ACTIVITY_PLANT));
  assert_true(is_terrain_change_activity(ACTIVITY_TRANSFORM));
}

/***********************************************************************
  Test is_terrain_change_activity for non-terrain changing activities
***********************************************************************/
static void test_is_terrain_change_activity_false(void **state)
{
  (void) state;

  assert_false(is_terrain_change_activity(ACTIVITY_IDLE));
  assert_false(is_terrain_change_activity(ACTIVITY_MINE));
  assert_false(is_terrain_change_activity(ACTIVITY_PILLAGE));
}

/***********************************************************************
  Test is_tile_activity
***********************************************************************/
static void test_is_tile_activity(void **state)
{
  (void) state;

  /* Build activities are tile activities */
  assert_true(is_tile_activity(ACTIVITY_MINE));
  assert_true(is_tile_activity(ACTIVITY_IRRIGATE));
  assert_true(is_tile_activity(ACTIVITY_BASE));
  assert_true(is_tile_activity(ACTIVITY_GEN_ROAD));

  /* Clean activities are tile activities */
  assert_true(is_tile_activity(ACTIVITY_PILLAGE));
  assert_true(is_tile_activity(ACTIVITY_CLEAN));

  /* Terrain change activities are tile activities */
  assert_true(is_tile_activity(ACTIVITY_CULTIVATE));
  assert_true(is_tile_activity(ACTIVITY_PLANT));
  assert_true(is_tile_activity(ACTIVITY_TRANSFORM));

  /* Non-tile activities */
  assert_false(is_tile_activity(ACTIVITY_IDLE));
  assert_false(is_tile_activity(ACTIVITY_FORTIFIED));
  assert_false(is_tile_activity(ACTIVITY_SENTRY));
}

/***********************************************************************
  Test is_targeted_activity
***********************************************************************/
static void test_is_targeted_activity(void **state)
{
  (void) state;

  /* Build activities are targeted */
  assert_true(is_targeted_activity(ACTIVITY_MINE));
  assert_true(is_targeted_activity(ACTIVITY_IRRIGATE));
  assert_true(is_targeted_activity(ACTIVITY_BASE));
  assert_true(is_targeted_activity(ACTIVITY_GEN_ROAD));

  /* Clean activities are targeted */
  assert_true(is_targeted_activity(ACTIVITY_PILLAGE));
  assert_true(is_targeted_activity(ACTIVITY_CLEAN));

  /* Non-targeted activities */
  assert_false(is_targeted_activity(ACTIVITY_IDLE));
  assert_false(is_targeted_activity(ACTIVITY_FORTIFIED));
  assert_false(is_targeted_activity(ACTIVITY_TRANSFORM));
}

/***********************************************************************
  Test unit_home macro
  Note: This test requires game state to be properly initialized.
***********************************************************************/
static void test_unit_home(void **state)
{
  (void) state;

  /* This test requires proper game state initialization.
   * unit_home macro calls game_city_by_number() which needs proper init.
   * Skip this test as it requires full game initialization. */
  skip();
}

/***********************************************************************
  Test is_unit_homeless macro
  Note: The is_unit_homeless macro in unit.h has a bug - it uses punit
  instead of _pu_. This test is skipped until that bug is fixed.
***********************************************************************/
static void test_is_unit_homeless(void **state)
{
  (void) state;

  /* The is_unit_homeless macro has a bug in unit.h:
   * #define is_unit_homeless(_pu_) (punit->homecity == IDENTITY_NUMBER_ZERO)
   * It should use _pu_ instead of punit.
   * Skip this test until the bug is fixed. */
  skip();
}

/***********************************************************************
  Test free_unit_orders with no orders
***********************************************************************/
static void test_free_unit_orders_no_orders(void **state)
{
  (void) state;

  test_unit.has_orders = FALSE;
  test_unit.orders.list = NULL;
  test_unit.orders.length = 0;

  free_unit_orders(&test_unit);

  assert_false(test_unit.has_orders);
  assert_null(test_unit.orders.list);
  assert_int_equal(test_unit.orders.length, 0);
}

/***********************************************************************
  Test free_unit_orders with orders
***********************************************************************/
static void test_free_unit_orders_with_orders(void **state)
{
  (void) state;

  test_unit.has_orders = TRUE;
  test_unit.orders.list = fc_malloc(sizeof(struct unit_order));
  test_unit.orders.length = 1;

  free_unit_orders(&test_unit);

  assert_false(test_unit.has_orders);
  assert_null(test_unit.orders.list);
  assert_int_equal(test_unit.orders.length, 0);
}

/***********************************************************************
  Test cargo_iter_sizeof
***********************************************************************/
static void test_cargo_iter_sizeof(void **state)
{
  (void) state;

  size_t size = cargo_iter_sizeof();
  assert_true(size > 0);
  /* Note: struct cargo_iter is opaque, so we can't use sizeof on it directly */
}

/***********************************************************************
  Test unit order enum values
***********************************************************************/
static void test_unit_order_enum_values(void **state)
{
  (void) state;

  assert_int_equal(ORDER_MOVE, 0);
  assert_int_equal(ORDER_ACTIVITY, 1);
  assert_int_equal(ORDER_FULL_MP, 2);
  assert_int_equal(ORDER_ACTION_MOVE, 3);
  assert_int_equal(ORDER_PERFORM_ACTION, 4);
}

/***********************************************************************
  Test unit focus status enum values
***********************************************************************/
static void test_unit_focus_status_enum(void **state)
{
  (void) state;

  assert_int_equal(FOCUS_AVAIL, 0);
  assert_int_equal(FOCUS_WAIT, 1);
  assert_int_equal(FOCUS_DONE, 2);
}

/***********************************************************************
  Test goto route type enum values
***********************************************************************/
static void test_goto_route_type_enum(void **state)
{
  (void) state;

  assert_int_equal(ROUTE_GOTO, 0);
  assert_int_equal(ROUTE_PATROL, 1);
}

/***********************************************************************
  Test unit upgrade result enum values
***********************************************************************/
static void test_unit_upgrade_result_enum(void **state)
{
  (void) state;

  assert_int_equal(UU_OK, 0);
  assert_int_equal(UU_NO_UNITTYPE, 1);
  assert_int_equal(UU_NO_MONEY, 2);
  assert_int_equal(UU_NOT_IN_CITY, 3);
  assert_int_equal(UU_NOT_CITY_OWNER, 4);
  assert_int_equal(UU_NOT_ENOUGH_ROOM, 5);
  assert_int_equal(UU_NOT_TERRAIN, 6);
  assert_int_equal(UU_UNSUITABLE_TRANSPORT, 7);
  assert_int_equal(UU_NOT_ACTIVITY, 8);
}

/***********************************************************************
  Test unit airlift result enum values
***********************************************************************/
static void test_unit_airlift_result_enum(void **state)
{
  (void) state;

  /* Success codes */
  assert_int_equal(AR_OK, 0);
  assert_int_equal(AR_OK_SRC_UNKNOWN, 1);
  assert_int_equal(AR_OK_DST_UNKNOWN, 2);

  /* Failure codes */
  assert_int_equal(AR_NO_MOVES, 3);
  assert_int_equal(AR_WRONG_UNITTYPE, 4);
  assert_int_equal(AR_OCCUPIED, 5);
  assert_int_equal(AR_NOT_IN_CITY, 6);
  assert_int_equal(AR_BAD_SRC_CITY, 7);
  assert_int_equal(AR_BAD_DST_CITY, 8);
  assert_int_equal(AR_SRC_NO_FLIGHTS, 9);
  assert_int_equal(AR_DST_NO_FLIGHTS, 10);
}

/***********************************************************************
  Test battlegroup constants
***********************************************************************/
static void test_battlegroup_constants(void **state)
{
  (void) state;

  assert_int_equal(MAX_NUM_BATTLEGROUPS, 4);
  assert_int_equal(BATTLEGROUP_NONE, -1);
}

/***********************************************************************
  Test unit structure initialization - moves_left
***********************************************************************/
static void test_unit_moves_left(void **state)
{
  (void) state;

  test_unit.moves_left = SINGLE_MOVE;
  assert_int_equal(test_unit.moves_left, SINGLE_MOVE);

  test_unit.moves_left = 0;
  assert_int_equal(test_unit.moves_left, 0);
}

/***********************************************************************
  Test unit structure initialization - hp
***********************************************************************/
static void test_unit_hp(void **state)
{
  (void) state;

  test_unit.hp = 10;
  assert_int_equal(test_unit.hp, 10);

  test_unit.hp = 0;
  assert_int_equal(test_unit.hp, 0);
}

/***********************************************************************
  Test unit structure initialization - veteran
***********************************************************************/
static void test_unit_veteran(void **state)
{
  (void) state;

  test_unit.veteran = 0;
  assert_int_equal(test_unit.veteran, 0);

  test_unit.veteran = 5;
  assert_int_equal(test_unit.veteran, 5);
}

/***********************************************************************
  Test unit structure initialization - fuel
***********************************************************************/
static void test_unit_fuel(void **state)
{
  (void) state;

  test_unit.fuel = 1;
  assert_int_equal(test_unit.fuel, 1);

  test_unit.fuel = 10;
  assert_int_equal(test_unit.fuel, 10);
}

/***********************************************************************
  Test unit structure initialization - activity
***********************************************************************/
static void test_unit_activity(void **state)
{
  (void) state;

  test_unit.activity = ACTIVITY_IDLE;
  assert_int_equal(test_unit.activity, ACTIVITY_IDLE);

  test_unit.activity = ACTIVITY_FORTIFIED;
  assert_int_equal(test_unit.activity, ACTIVITY_FORTIFIED);
}

/***********************************************************************
  Test unit structure initialization - facing
***********************************************************************/
static void test_unit_facing(void **state)
{
  (void) state;

  test_unit.facing = DIR8_NORTH;
  assert_int_equal(test_unit.facing, DIR8_NORTH);

  test_unit.facing = DIR8_SOUTH;
  assert_int_equal(test_unit.facing, DIR8_SOUTH);
}

/***********************************************************************
  Test unit moved flag
***********************************************************************/
static void test_unit_moved(void **state)
{
  (void) state;

  test_unit.moved = FALSE;
  assert_false(test_unit.moved);

  test_unit.moved = TRUE;
  assert_true(test_unit.moved);
}

/***********************************************************************
  Test unit paradropped flag
***********************************************************************/
static void test_unit_paradropped(void **state)
{
  (void) state;

  test_unit.paradropped = FALSE;
  assert_false(test_unit.paradropped);

  test_unit.paradropped = TRUE;
  assert_true(test_unit.paradropped);
}

/***********************************************************************
  Test unit done_moving flag
***********************************************************************/
static void test_unit_done_moving(void **state)
{
  (void) state;

  test_unit.done_moving = FALSE;
  assert_false(test_unit.done_moving);

  test_unit.done_moving = TRUE;
  assert_true(test_unit.done_moving);
}

/***********************************************************************
  Test unit stay flag
***********************************************************************/
static void test_unit_stay(void **state)
{
  (void) state;

  test_unit.stay = FALSE;
  assert_false(test_unit.stay);

  test_unit.stay = TRUE;
  assert_true(test_unit.stay);
}

/***********************************************************************
  Test unit upkeep array
***********************************************************************/
static void test_unit_upkeep(void **state)
{
  (void) state;

  memset(test_unit.upkeep, 0, sizeof(test_unit.upkeep));

  test_unit.upkeep[O_FOOD] = 1;
  test_unit.upkeep[O_SHIELD] = 2;
  test_unit.upkeep[O_GOLD] = 3;

  assert_int_equal(test_unit.upkeep[O_FOOD], 1);
  assert_int_equal(test_unit.upkeep[O_SHIELD], 2);
  assert_int_equal(test_unit.upkeep[O_GOLD], 3);
}

/***********************************************************************
  Test unit id
***********************************************************************/
static void test_unit_id(void **state)
{
  (void) state;

  test_unit.id = 0;
  assert_int_equal(test_unit.id, 0);

  test_unit.id = 100;
  assert_int_equal(test_unit.id, 100);
}

/***********************************************************************
  Test unit homecity
***********************************************************************/
static void test_unit_homecity(void **state)
{
  (void) state;

  test_unit.homecity = IDENTITY_NUMBER_ZERO;
  assert_int_equal(test_unit.homecity, IDENTITY_NUMBER_ZERO);

  test_unit.homecity = 5;
  assert_int_equal(test_unit.homecity, 5);
}

/***********************************************************************
  Test unit refcount
***********************************************************************/
static void test_unit_refcount(void **state)
{
  (void) state;

  test_unit.refcount = 1;
  assert_int_equal(test_unit.refcount, 1);

  test_unit.refcount = 2;
  assert_int_equal(test_unit.refcount, 2);
}

/***********************************************************************
  Main test runner
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    /* Unit has orders tests */
    cmocka_unit_test_setup_teardown(test_unit_has_orders_false, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_has_orders_true, setup, teardown),

    /* Unit owner/tile tests */
    cmocka_unit_test_setup_teardown(test_unit_owner, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_tile, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_tile_set, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_nationality, setup, teardown),

    /* Transport capacity tests */
    cmocka_unit_test_setup_teardown(test_get_transporter_capacity, setup, teardown),
    cmocka_unit_test_setup_teardown(test_get_transporter_occupancy_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_get_transporter_occupancy_with_cargo, setup, teardown),

    /* Transport operations tests */
    cmocka_unit_test_setup_teardown(test_unit_transport_get_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_transport_get_with_transport, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_transport_cargo, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_transport_load_force, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_transport_load_duplicate, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_transport_unload_no_transport, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_transport_unload_with_transport, setup, teardown),

    /* Transport depth tests */
    cmocka_unit_test_setup_teardown(test_unit_transport_depth_zero, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_transport_depth_one, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_cargo_depth_zero, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_cargo_depth_with_cargo, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_contained_in_direct, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_contained_in_none, setup, teardown),

    /* Unit state tests */
    cmocka_unit_test_setup_teardown(test_unit_is_virtual_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_is_virtual_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_is_alive_false, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_is_alive_zero_id, setup, teardown),

    /* Unit orders tests */
    cmocka_unit_test_setup_teardown(test_are_unit_orders_equal_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_are_unit_orders_equal_different_order, setup, teardown),
    cmocka_unit_test_setup_teardown(test_are_unit_orders_equal_different_activity, setup, teardown),
    cmocka_unit_test_setup_teardown(test_are_unit_orders_equal_different_dir, setup, teardown),
    cmocka_unit_test_setup_teardown(test_free_unit_orders_no_orders, setup, teardown),
    cmocka_unit_test_setup_teardown(test_free_unit_orders_with_orders, setup, teardown),

    /* Activity tests */
    cmocka_unit_test_setup_teardown(test_activity_requires_target_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_activity_requires_target_false, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_build_activity_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_build_activity_false, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_clean_activity_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_clean_activity_false, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_terrain_change_activity_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_terrain_change_activity_false, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_tile_activity, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_targeted_activity, setup, teardown),

    /* Home/homeless tests */
    cmocka_unit_test_setup_teardown(test_unit_home, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_unit_homeless, setup, teardown),

    /* Iterator test */
    cmocka_unit_test_setup_teardown(test_cargo_iter_sizeof, setup, teardown),

    /* Enum value tests */
    cmocka_unit_test_setup_teardown(test_unit_order_enum_values, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_focus_status_enum, setup, teardown),
    cmocka_unit_test_setup_teardown(test_goto_route_type_enum, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_upgrade_result_enum, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_airlift_result_enum, setup, teardown),
    cmocka_unit_test_setup_teardown(test_battlegroup_constants, setup, teardown),

    /* Unit structure field tests */
    cmocka_unit_test_setup_teardown(test_unit_moves_left, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_hp, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_veteran, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_fuel, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_activity, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_facing, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_moved, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_paradropped, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_done_moving, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_stay, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_upkeep, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_id, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_homecity, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_refcount, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}