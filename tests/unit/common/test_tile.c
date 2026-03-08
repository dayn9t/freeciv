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

/* common */
#include "map.h"
#include "tile.h"
#include "world_object.h"
#include "game.h"

#define TEST_MAP_WIDTH 64
#define TEST_MAP_HEIGHT 64

/* Test player structure */
static struct player test_player;

static int setup(void **state)
{
  (void) state;

  /* Initialize a minimal mock map for testing */
  memset(&wld, 0, sizeof(wld));
  wld.map.xsize = TEST_MAP_WIDTH;
  wld.map.ysize = TEST_MAP_HEIGHT;
  wld.map.topology_id = 0; /* Non-isometric */
  wld.map.wrap_id = WRAP_X;

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
    wld.map.tiles[i].continent = 1; /* Default continent */
  }

  /* Initialize test player */
  memset(&test_player, 0, sizeof(test_player));

  /* Initialize game borders setting */
  game.info.borders = 1; /* BORDERS_ENABLED */

  return 0;
}

static int teardown(void **state)
{
  (void) state;

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

/* ==================== Tile Owner Tests ==================== */

static void test_tile_set_owner_basic(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[0];

  /* Initially owner should be NULL */
  assert_null(tile_owner(ptile));

  /* Set owner */
  tile_set_owner(ptile, &test_player, NULL);
  assert_ptr_equal(tile_owner(ptile), &test_player);

  /* Set owner to NULL */
  tile_set_owner(ptile, NULL, NULL);
  assert_null(tile_owner(ptile));
}

static void test_tile_set_owner_borders_disabled(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[1];

  /* Disable borders */
  game.info.borders = BORDERS_DISABLED;

  /* Set owner when borders disabled - owner should not change from NULL
   * because tile is not a city center and has no existing owner */
  tile_set_owner(ptile, &test_player, NULL);
  /* When borders are disabled and tile has no owner, it remains NULL */
  assert_null(tile_owner(ptile));
}

static void test_tile_set_owner_with_claimer(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[2];
  struct tile *claimer = &wld.map.tiles[3];

  tile_set_owner(ptile, &test_player, claimer);
  assert_ptr_equal(tile_owner(ptile), &test_player);
  assert_ptr_equal(tile_claimer(ptile), claimer);
}

/* ==================== Tile Worked Tests ==================== */

static void test_tile_set_worked(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[4];
  struct city test_city;

  memset(&test_city, 0, sizeof(test_city));
  test_city.id = 1;

  /* Initially worked should be NULL */
  assert_null(tile_worked(ptile));

  /* Set worked city */
  tile_set_worked(ptile, &test_city);
  assert_ptr_equal(tile_worked(ptile), &test_city);

  /* Clear worked city */
  tile_set_worked(ptile, NULL);
  assert_null(tile_worked(ptile));
}

static void test_tile_city(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[5];
  struct city test_city;

  memset(&test_city, 0, sizeof(test_city));
  test_city.id = 1;

  /* No city worked - should return NULL */
  assert_null(tile_city(ptile));

  /* Set worked city but not city center - returns NULL */
  tile_set_worked(ptile, &test_city);
  /* tile_city checks is_city_center which requires proper setup */
  /* For now just test that tile_worked returns what we set */
  assert_ptr_equal(tile_worked(ptile), &test_city);
}

/* ==================== Tile Terrain Tests ==================== */

static void test_tile_set_terrain_basic(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[6];
  struct terrain test_terrain;

  memset(&test_terrain, 0, sizeof(test_terrain));

  /* Initially terrain should be NULL */
  assert_null(tile_terrain(ptile));

  /* Set terrain */
  tile_set_terrain(ptile, &test_terrain);
  assert_ptr_equal(tile_terrain(ptile), &test_terrain);

  /* Change terrain */
  struct terrain test_terrain2;
  memset(&test_terrain2, 0, sizeof(test_terrain2));
  tile_set_terrain(ptile, &test_terrain2);
  assert_ptr_equal(tile_terrain(ptile), &test_terrain2);
}

static void test_tile_set_terrain_null(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[7];
  struct terrain test_terrain;

  memset(&test_terrain, 0, sizeof(test_terrain));

  /* Set terrain then clear it */
  tile_set_terrain(ptile, &test_terrain);
  assert_ptr_equal(tile_terrain(ptile), &test_terrain);

  tile_set_terrain(ptile, NULL);
  assert_null(tile_terrain(ptile));
}

/* ==================== Tile Extras Tests ==================== */

static void test_tile_extras_null(void **state)
{
  (void) state;

  const bv_extras *extras = tile_extras_null();
  assert_non_null(extras);
}

static void test_tile_extras_safe_null_tile(void **state)
{
  (void) state;

  const bv_extras *extras = tile_extras_safe(NULL);
  assert_non_null(extras);
  /* Should return the same as tile_extras_null */
  assert_ptr_equal(extras, tile_extras_null());
}

static void test_tile_extras_safe_valid_tile(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[8];

  const bv_extras *extras = tile_extras_safe(ptile);
  assert_non_null(extras);
  assert_ptr_equal(extras, &(ptile->extras));
}

/* ==================== Tile Resource Tests ==================== */

static void test_tile_set_resource_basic(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[9];
  struct extra_type test_resource;

  memset(&test_resource, 0, sizeof(test_resource));

  /* Initially resource should be NULL */
  assert_null(tile_resource(ptile));

  /* Set resource */
  tile_set_resource(ptile, &test_resource);
  assert_ptr_equal(tile_resource(ptile), &test_resource);
}

static void test_tile_set_resource_same(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[10];
  struct extra_type test_resource;

  memset(&test_resource, 0, sizeof(test_resource));

  /* Set resource */
  tile_set_resource(ptile, &test_resource);

  /* Set same resource again - should be no-op */
  tile_set_resource(ptile, &test_resource);
  assert_ptr_equal(tile_resource(ptile), &test_resource);
}

static void test_tile_set_resource_null(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[11];
  struct extra_type test_resource;

  memset(&test_resource, 0, sizeof(test_resource));

  /* Set then clear resource */
  tile_set_resource(ptile, &test_resource);
  assert_ptr_equal(tile_resource(ptile), &test_resource);

  tile_set_resource(ptile, NULL);
  assert_null(tile_resource(ptile));
}

/* ==================== Tile Continent Tests ==================== */

static void test_tile_continent_default(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[12];

  /* Continent should be set in setup */
  assert_int_equal(tile_continent(ptile), 1);
}

static void test_tile_set_continent_value(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[13];

  tile_set_continent(ptile, 5);
  assert_int_equal(tile_continent(ptile), 5);

  tile_set_continent(ptile, -1); /* Ocean */
  assert_int_equal(tile_continent(ptile), -1);
}

/* ==================== Tile Label Tests ==================== */

static void test_tile_set_label_null(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[14];

  /* Initially label should be NULL */
  assert_null(ptile->label);

  /* Setting NULL label should return false (no change) */
  bool changed = tile_set_label(ptile, NULL);
  assert_false(changed);
  assert_null(ptile->label);
}

static void test_tile_set_label_basic(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[15];

  /* Set label */
  bool changed = tile_set_label(ptile, "TestLabel");
  assert_true(changed);
  assert_string_equal(ptile->label, "TestLabel");

  /* Change label */
  changed = tile_set_label(ptile, "NewLabel");
  assert_true(changed);
  assert_string_equal(ptile->label, "NewLabel");

  /* Clear label */
  changed = tile_set_label(ptile, NULL);
  assert_true(changed);
  assert_null(ptile->label);
}

static void test_tile_set_label_empty_string(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[16];

  /* Empty string should be treated as NULL */
  bool changed = tile_set_label(ptile, "");
  assert_false(changed);
  assert_null(ptile->label);
}

static void test_tile_set_label_same(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[17];

  tile_set_label(ptile, "SameLabel");
  assert_string_equal(ptile->label, "SameLabel");

  /* Setting same label should return false */
  bool changed = tile_set_label(ptile, "SameLabel");
  assert_false(changed);
  assert_string_equal(ptile->label, "SameLabel");
}

/* ==================== Tile Virtual Tests ==================== */

static void test_tile_virtual_new_blank(void **state)
{
  (void) state;

  struct tile *vtile = tile_virtual_new(NULL);
  assert_non_null(vtile);

  /* Check initial values */
  assert_int_equal(vtile->index, TILE_INDEX_NONE);
  assert_int_equal(vtile->continent, -1);
  assert_null(vtile->terrain);
  assert_null(vtile->resource);
  assert_null(vtile->worked);
  assert_null(vtile->owner);
  assert_null(vtile->claimer);
  assert_int_equal(vtile->altitude, 0);
  assert_non_null(vtile->units);

  tile_virtual_destroy(vtile);
}

static void test_tile_virtual_new_copy(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[18];
  struct terrain test_terrain;

  memset(&test_terrain, 0, sizeof(test_terrain));
  ptile->terrain = &test_terrain;
  ptile->altitude = 100;

  struct tile *vtile = tile_virtual_new(ptile);
  assert_non_null(vtile);

  /* Should copy some values from source tile */
  assert_int_equal(vtile->index, tile_index(ptile));
  /* Note: continent is NOT copied, it stays -1 for virtual tiles */
  assert_int_equal(vtile->continent, -1);
  assert_int_equal(vtile->altitude, 100);
  assert_ptr_equal(vtile->terrain, &test_terrain);

  tile_virtual_destroy(vtile);
}

static void test_tile_virtual_destroy_null(void **state)
{
  (void) state;

  /* Should not crash */
  tile_virtual_destroy(NULL);
}

static void test_tile_virtual_destroy_blank(void **state)
{
  (void) state;

  struct tile *vtile = tile_virtual_new(NULL);
  assert_non_null(vtile);

  tile_virtual_destroy(vtile);
  /* vtile should be freed */
}

/* ==================== Tile Map Check Tests ==================== */

static void test_tile_map_check_null(void **state)
{
  (void) state;

  bool result = tile_map_check(&wld.map, NULL);
  assert_false(result);
}

static void test_tile_map_check_valid(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[19];

  bool result = tile_map_check(&wld.map, ptile);
  assert_true(result);
}

static void test_tile_map_check_invalid_index(void **state)
{
  (void) state;

  /* Create a fake tile with invalid index */
  struct tile fake_tile;
  memset(&fake_tile, 0, sizeof(fake_tile));
  fake_tile.index = -1;

  bool result = tile_map_check(&wld.map, &fake_tile);
  assert_false(result);
}

/* ==================== Tile Hash Key Tests ==================== */

static void test_tile_hash_key(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[20];

  void *key = tile_hash_key(ptile);
  assert_non_null(key);

  /* Key should be based on tile index */
  int index = FC_PTR_TO_INT(key);
  assert_int_equal(index, tile_index(ptile));
}

/* ==================== Tile Is Placing Tests ==================== */

static void test_tile_is_placing_null(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[21];

  /* Initially placing should be NULL */
  assert_false(tile_is_placing(ptile));
}

static void test_tile_is_placing_set(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[22];
  struct extra_type test_extra;

  memset(&test_extra, 0, sizeof(test_extra));
  ptile->placing = &test_extra;

  assert_true(tile_is_placing(ptile));

  ptile->placing = NULL;
  assert_false(tile_is_placing(ptile));
}

/* ==================== Tile Add/Remove Extra Tests ==================== */

static void test_tile_add_extra_null(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[23];

  /* Adding NULL extra should not crash */
  tile_add_extra(ptile, NULL);
}

static void test_tile_remove_extra_null(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[24];

  /* Removing NULL extra should not crash */
  tile_remove_extra(ptile, NULL);
}

static void test_tile_remove_extra_clears_resource(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[25];
  struct extra_type test_extra;

  memset(&test_extra, 0, sizeof(test_extra));

  /* Set resource */
  ptile->resource = &test_extra;
  assert_ptr_equal(ptile->resource, &test_extra);

  /* Remove same extra - should clear resource */
  tile_remove_extra(ptile, &test_extra);
  assert_null(ptile->resource);
}

/* ==================== Tile Helper Functions Tests ==================== */

static void test_tile_has_base_no_base(void **state)
{
  (void) state;
  /* This test requires ruleset data to be loaded for base_extra_get.
   * Skip this test as it requires full game initialization. */
  skip();
}

static void test_tile_has_road_no_road(void **state)
{
  (void) state;
  /* This test requires ruleset data to be loaded for road_extra_get.
   * Skip this test as it requires full game initialization. */
  skip();
}

static void test_tile_has_river_no_river(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[28];

  /* Tile without river should return false */
  bool result = tile_has_river(ptile);
  assert_false(result);
}

/* ==================== Tile Visibility Tests ==================== */

static void test_tile_get_known_not_known(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[29];

  /* Without setting known status, should return TILE_UNKNOWN */
  enum known_type known = tile_get_known(ptile, &test_player);
  assert_int_equal(known, TILE_UNKNOWN);
}

static void test_tile_is_seen_not_seen(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[30];

  /* Without setting seen status, should return false */
  bool seen = tile_is_seen(ptile, &test_player);
  assert_false(seen);
}

/* ==================== Tile Defense Bonus Tests ==================== */

static void test_tile_extras_class_defense_bonus_no_extras(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[31];
  struct unit_class test_class;

  memset(&test_class, 0, sizeof(test_class));

  /* No extras - should return 0 */
  int bonus = tile_extras_class_defense_bonus(ptile, &test_class);
  assert_int_equal(bonus, 0);
}

static void test_tile_extras_defense_bonus_wrapper(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[32];
  struct unit_type test_utype;

  memset(&test_utype, 0, sizeof(test_utype));

  /* Wrapper function - requires unit type class setup */
  /* This is a placeholder test */
  int bonus = tile_extras_defense_bonus(ptile, &test_utype);
  /* Should return 0 without extras */
  assert_int_equal(bonus, 0);
}

/* ==================== Tile Output Tests ==================== */

static void test_tile_roads_output_incr_no_roads(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[33];

  /* This test requires terrain to be set up properly */
  /* Skip this test as it requires ruleset initialization */
  skip();
}

static void test_tile_roads_output_bonus(void **state)
{
  (void) state;
  struct tile *ptile = &wld.map.tiles[34];

  /* This function requires ruleset data */
  /* Placeholder test - just verify it doesn't crash */
  skip();
}

/* ==================== Tile Refuel Extra Tests ==================== */

static void test_tile_has_refuel_extra_no_extra(void **state)
{
  (void) state;

  /* This test requires unit class cache to be initialized */
  /* Skip this test as it requires full game initialization */
  skip();
}

/* ==================== Tile Native Base Tests ==================== */

static void test_tile_has_native_base_no_base(void **state)
{
  (void) state;

  /* This test requires unit type cache to be initialized */
  /* Skip this test as it requires full game initialization */
  skip();
}

/* ==================== Position Validation Tests ==================== */

static void test_is_normal_map_pos_valid(void **state)
{
  (void) state;

  assert_true(is_normal_map_pos(0, 0));
  assert_true(is_normal_map_pos(10, 10));
  assert_true(is_normal_map_pos(TEST_MAP_WIDTH - 1, TEST_MAP_HEIGHT - 1));
}

static void test_is_normal_map_pos_invalid(void **state)
{
  (void) state;

  assert_false(is_normal_map_pos(-1, 0));
  assert_false(is_normal_map_pos(0, -1));
  assert_false(is_normal_map_pos(TEST_MAP_WIDTH, 0));
  assert_false(is_normal_map_pos(0, TEST_MAP_HEIGHT));
}

/* ==================== Distance Calculation Tests ==================== */

static void test_map_distance_same_tile(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;
  int dist;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[0];

  dist = map_distance(ptile1, ptile2);
  assert_int_equal(dist, 0);
}

static void test_map_distance_adjacent(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;
  int dist;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[1];

  dist = map_distance(ptile1, ptile2);
  assert_int_equal(dist, 1);
}

/* ==================== Direction Tests ==================== */

static void test_dir_reverse(void **state)
{
  (void) state;

  assert_int_equal(DIR_REVERSE(DIR8_NORTH), DIR8_SOUTH);
  assert_int_equal(DIR_REVERSE(DIR8_SOUTH), DIR8_NORTH);
  assert_int_equal(DIR_REVERSE(DIR8_EAST), DIR8_WEST);
  assert_int_equal(DIR_REVERSE(DIR8_WEST), DIR8_EAST);
}

static void test_opposite_direction(void **state)
{
  (void) state;

  assert_int_equal(opposite_direction(DIR8_NORTH), DIR8_SOUTH);
  assert_int_equal(opposite_direction(DIR8_SOUTH), DIR8_NORTH);
  assert_int_equal(opposite_direction(DIR8_EAST), DIR8_WEST);
  assert_int_equal(opposite_direction(DIR8_WEST), DIR8_EAST);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* Tile Owner Tests */
    cmocka_unit_test_setup_teardown(test_tile_set_owner_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_set_owner_borders_disabled, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_set_owner_with_claimer, setup, teardown),

    /* Tile Worked Tests */
    cmocka_unit_test_setup_teardown(test_tile_set_worked, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_city, setup, teardown),

    /* Tile Terrain Tests */
    cmocka_unit_test_setup_teardown(test_tile_set_terrain_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_set_terrain_null, setup, teardown),

    /* Tile Extras Tests */
    cmocka_unit_test_setup_teardown(test_tile_extras_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_extras_safe_null_tile, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_extras_safe_valid_tile, setup, teardown),

    /* Tile Resource Tests */
    cmocka_unit_test_setup_teardown(test_tile_set_resource_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_set_resource_same, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_set_resource_null, setup, teardown),

    /* Tile Continent Tests */
    cmocka_unit_test_setup_teardown(test_tile_continent_default, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_set_continent_value, setup, teardown),

    /* Tile Label Tests */
    cmocka_unit_test_setup_teardown(test_tile_set_label_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_set_label_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_set_label_empty_string, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_set_label_same, setup, teardown),

    /* Tile Virtual Tests */
    cmocka_unit_test_setup_teardown(test_tile_virtual_new_blank, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_virtual_new_copy, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_virtual_destroy_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_virtual_destroy_blank, setup, teardown),

    /* Tile Map Check Tests */
    cmocka_unit_test_setup_teardown(test_tile_map_check_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_map_check_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_map_check_invalid_index, setup, teardown),

    /* Tile Hash Key Tests */
    cmocka_unit_test_setup_teardown(test_tile_hash_key, setup, teardown),

    /* Tile Is Placing Tests */
    cmocka_unit_test_setup_teardown(test_tile_is_placing_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_is_placing_set, setup, teardown),

    /* Tile Add/Remove Extra Tests */
    cmocka_unit_test_setup_teardown(test_tile_add_extra_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_remove_extra_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_remove_extra_clears_resource, setup, teardown),

    /* Tile Helper Functions Tests */
    cmocka_unit_test_setup_teardown(test_tile_has_base_no_base, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_has_road_no_road, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_has_river_no_river, setup, teardown),

    /* Tile Visibility Tests */
    cmocka_unit_test_setup_teardown(test_tile_get_known_not_known, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_is_seen_not_seen, setup, teardown),

    /* Tile Defense Bonus Tests */
    cmocka_unit_test_setup_teardown(test_tile_extras_class_defense_bonus_no_extras, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_extras_defense_bonus_wrapper, setup, teardown),

    /* Tile Output Tests */
    cmocka_unit_test_setup_teardown(test_tile_roads_output_incr_no_roads, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_roads_output_bonus, setup, teardown),

    /* Tile Refuel Extra Tests */
    cmocka_unit_test_setup_teardown(test_tile_has_refuel_extra_no_extra, setup, teardown),

    /* Tile Native Base Tests */
    cmocka_unit_test_setup_teardown(test_tile_has_native_base_no_base, setup, teardown),

    /* Position Validation Tests */
    cmocka_unit_test_setup_teardown(test_is_normal_map_pos_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_normal_map_pos_invalid, setup, teardown),

    /* Distance Calculation Tests */
    cmocka_unit_test_setup_teardown(test_map_distance_same_tile, setup, teardown),
    cmocka_unit_test_setup_teardown(test_map_distance_adjacent, setup, teardown),

    /* Direction Tests */
    cmocka_unit_test_setup_teardown(test_dir_reverse, setup, teardown),
    cmocka_unit_test_setup_teardown(test_opposite_direction, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
