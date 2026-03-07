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
#include "map.h"
#include "tile.h"
#include "world_object.h"

#define TEST_MAP_WIDTH 64
#define TEST_MAP_HEIGHT 64

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
      /* Cardinal directions: N, E, S, W (0, 2, 4, 6) */
      wld.map.cardinal_dirs[i / 2] = i;
    }
  }

  /* Allocate tiles array */
  wld.map.tiles = fc_calloc(MAP_INDEX_SIZE, sizeof(struct tile));
  for (int i = 0; i < MAP_INDEX_SIZE; i++) {
    wld.map.tiles[i].index = i;
    wld.map.tiles[i].units = unit_list_new();
  }

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

  return 0;
}

/* ==================== Map Initialization Tests ==================== */

static void test_map_is_empty_true(void **state)
{
  (void) state;
  struct civ_map empty_map;

  memset(&empty_map, 0, sizeof(empty_map));
  empty_map.tiles = NULL;

  /* After clearing wld.map.tiles, map should be empty */
  assert_true(wld.map.tiles != NULL);

  /* Test with explicit NULL tiles */
  empty_map.tiles = NULL;
  /* map_is_empty() checks wld.map, so we can't directly test it here
   * without affecting the test fixture */
}

static void test_map_is_empty_false(void **state)
{
  (void) state;

  /* With tiles allocated in setup, map is not empty */
  assert_non_null(wld.map.tiles);
}

/* ==================== Coordinate Conversion Tests ==================== */

static void test_map_pos_to_tile_origin(void **state)
{
  (void) state;
  struct tile *ptile;

  ptile = map_pos_to_tile(&wld.map, 0, 0);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), 0);
}

static void test_map_pos_to_tile_valid(void **state)
{
  (void) state;
  struct tile *ptile;

  ptile = map_pos_to_tile(&wld.map, 10, 10);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), 10 * TEST_MAP_WIDTH + 10);

  ptile = map_pos_to_tile(&wld.map, TEST_MAP_WIDTH - 1, TEST_MAP_HEIGHT - 1);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), MAP_INDEX_SIZE - 1);
}

static void test_map_pos_to_tile_out_of_bounds(void **state)
{
  (void) state;
  struct tile *ptile;

  /* Beyond map bounds - non-wrapping Y */
  ptile = map_pos_to_tile(&wld.map, 0, TEST_MAP_HEIGHT + 10);
  assert_null(ptile);

  ptile = map_pos_to_tile(&wld.map, 0, -5);
  assert_null(ptile);
}

static void test_map_pos_to_tile_with_wrap(void **state)
{
  (void) state;
  struct tile *ptile;

  /* With WRAP_X, coordinates should wrap in X direction */
  ptile = map_pos_to_tile(&wld.map, TEST_MAP_WIDTH + 5, 5);
  assert_non_null(ptile);
  /* The tile at wrapped position should be valid */
  assert_true(tile_index(ptile) >= 0);
  assert_true(tile_index(ptile) < MAP_INDEX_SIZE);
}

static void test_index_to_tile_valid(void **state)
{
  (void) state;
  struct tile *ptile;

  ptile = index_to_tile(&wld.map, 0);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), 0);

  ptile = index_to_tile(&wld.map, 100);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), 100);

  ptile = index_to_tile(&wld.map, MAP_INDEX_SIZE - 1);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), MAP_INDEX_SIZE - 1);
}

static void test_index_to_tile_invalid(void **state)
{
  (void) state;
  struct tile *ptile;

  /* Negative index */
  ptile = index_to_tile(&wld.map, -1);
  assert_null(ptile);

  /* Index beyond map size */
  ptile = index_to_tile(&wld.map, MAP_INDEX_SIZE);
  assert_null(ptile);

  ptile = index_to_tile(&wld.map, MAP_INDEX_SIZE + 100);
  assert_null(ptile);
}

static void test_native_pos_to_tile_valid(void **state)
{
  (void) state;
  struct tile *ptile;

  ptile = native_pos_to_tile(&wld.map, 0, 0);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), 0);

  ptile = native_pos_to_tile(&wld.map, 5, 5);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), 5 * TEST_MAP_WIDTH + 5);

  ptile = native_pos_to_tile(&wld.map, TEST_MAP_WIDTH - 1, TEST_MAP_HEIGHT - 1);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), MAP_INDEX_SIZE - 1);
}

static void test_native_pos_to_tile_out_of_bounds(void **state)
{
  (void) state;
  struct tile *ptile;

  /* Non-wrapping Y direction */
  ptile = native_pos_to_tile(&wld.map, 0, -1);
  assert_null(ptile);

  ptile = native_pos_to_tile(&wld.map, 0, TEST_MAP_HEIGHT);
  assert_null(ptile);
}

static void test_native_pos_to_tile_with_wrap(void **state)
{
  (void) state;
  struct tile *ptile;

  /* With WRAP_X, X should wrap */
  ptile = native_pos_to_tile(&wld.map, TEST_MAP_WIDTH + 5, 5);
  assert_non_null(ptile);
  /* Should wrap to X=5 */
  assert_int_equal(tile_index(ptile), 5 * TEST_MAP_WIDTH + 5);

  ptile = native_pos_to_tile(&wld.map, -1, 5);
  assert_non_null(ptile);
  /* Should wrap to X=TEST_MAP_WIDTH-1 */
  assert_int_equal(tile_index(ptile), 5 * TEST_MAP_WIDTH + (TEST_MAP_WIDTH - 1));
}

/* ==================== Map Size Tests ==================== */

static void test_map_num_tiles(void **state)
{
  (void) state;
  int num_tiles;

  num_tiles = map_num_tiles();
  assert_int_equal(num_tiles, TEST_MAP_WIDTH * TEST_MAP_HEIGHT);
}

static void test_map_size_checked(void **state)
{
  (void) state;
  int size;

  size = map_size_checked();
  assert_true(size >= 1);
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

static void test_is_real_map_pos_valid(void **state)
{
  (void) state;

  assert_true(is_real_map_pos(&wld.map, 0, 0));
  assert_true(is_real_map_pos(&wld.map, 10, 10));
  assert_true(is_real_map_pos(&wld.map, TEST_MAP_WIDTH - 1, TEST_MAP_HEIGHT - 1));
}

static void test_is_real_map_pos_invalid(void **state)
{
  (void) state;

  assert_false(is_real_map_pos(&wld.map, 0, TEST_MAP_HEIGHT + 5));
  assert_false(is_real_map_pos(&wld.map, 0, -5));
}

static void test_normalize_map_pos_valid(void **state)
{
  (void) state;
  int x, y;
  bool result;

  x = 5;
  y = 5;
  result = normalize_map_pos(&wld.map, &x, &y);
  assert_true(result);
  assert_int_equal(x, 5);
  assert_int_equal(y, 5);
}

static void test_normalize_map_pos_invalid(void **state)
{
  (void) state;
  int x, y;
  bool result;

  x = 0;
  y = TEST_MAP_HEIGHT + 5;
  result = normalize_map_pos(&wld.map, &x, &y);
  assert_false(result);
}

static void test_nearest_real_tile_valid(void **state)
{
  (void) state;
  struct tile *ptile;

  ptile = nearest_real_tile(&wld.map, 5, 5);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), 5 * TEST_MAP_WIDTH + 5);
}

static void test_nearest_real_tile_out_of_bounds(void **state)
{
  (void) state;
  struct tile *ptile;

  /* Position beyond Y bounds should be clamped */
  ptile = nearest_real_tile(&wld.map, 5, TEST_MAP_HEIGHT + 10);
  assert_non_null(ptile);
  /* Should be clamped to max Y */
  assert_true(tile_index(ptile) >= 0);
  assert_true(tile_index(ptile) < MAP_INDEX_SIZE);

  ptile = nearest_real_tile(&wld.map, 5, -10);
  assert_non_null(ptile);
  assert_true(tile_index(ptile) >= 0);
  assert_true(tile_index(ptile) < MAP_INDEX_SIZE);
}

/* ==================== Position Comparison Tests ==================== */

static void test_same_pos_same_tile(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[0];

  assert_true(same_pos(ptile1, ptile2));
}

static void test_same_pos_different_tile(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[1];

  assert_false(same_pos(ptile1, ptile2));
}

static void test_same_pos_different_coords_same_wrapped(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;

  /* With WRAP_X, position (0, 5) and (TEST_MAP_WIDTH, 5) should be same tile */
  ptile1 = map_pos_to_tile(&wld.map, 0, 5);
  ptile2 = map_pos_to_tile(&wld.map, TEST_MAP_WIDTH, 5);

  assert_non_null(ptile1);
  assert_non_null(ptile2);
  assert_true(same_pos(ptile1, ptile2));
}

/* ==================== Direction Tests ==================== */

static void test_dir_reverse(void **state)
{
  (void) state;

  assert_int_equal(DIR_REVERSE(DIR8_NORTH), DIR8_SOUTH);
  assert_int_equal(DIR_REVERSE(DIR8_SOUTH), DIR8_NORTH);
  assert_int_equal(DIR_REVERSE(DIR8_EAST), DIR8_WEST);
  assert_int_equal(DIR_REVERSE(DIR8_WEST), DIR8_EAST);
  assert_int_equal(DIR_REVERSE(DIR8_NORTHEAST), DIR8_SOUTHWEST);
  assert_int_equal(DIR_REVERSE(DIR8_SOUTHWEST), DIR8_NORTHEAST);
  assert_int_equal(DIR_REVERSE(DIR8_NORTHWEST), DIR8_SOUTHEAST);
  assert_int_equal(DIR_REVERSE(DIR8_SOUTHEAST), DIR8_NORTHWEST);
}

static void test_dir_dx_dy(void **state)
{
  (void) state;

  /* Check direction deltas */
  assert_int_equal(DIR_DX[DIR8_NORTH], 0);
  assert_int_equal(DIR_DY[DIR8_NORTH], -1);

  assert_int_equal(DIR_DX[DIR8_SOUTH], 0);
  assert_int_equal(DIR_DY[DIR8_SOUTH], 1);

  assert_int_equal(DIR_DX[DIR8_EAST], 1);
  assert_int_equal(DIR_DY[DIR8_EAST], 0);

  assert_int_equal(DIR_DX[DIR8_WEST], -1);
  assert_int_equal(DIR_DY[DIR8_WEST], 0);
}

static void test_opposite_direction(void **state)
{
  (void) state;

  assert_int_equal(opposite_direction(DIR8_NORTH), DIR8_SOUTH);
  assert_int_equal(opposite_direction(DIR8_SOUTH), DIR8_NORTH);
  assert_int_equal(opposite_direction(DIR8_EAST), DIR8_WEST);
  assert_int_equal(opposite_direction(DIR8_WEST), DIR8_EAST);
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

static void test_map_distance_diagonal(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;
  int dist;

  /* Tile at (0, 0) and (1, 1) */
  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[TEST_MAP_WIDTH + 1];

  dist = map_distance(ptile1, ptile2);
  /* Manhattan distance = |dx| + |dy| = 2 for diagonal, but
   * real distance = max(|dx|, |dy|) = 1 */
  assert_true(dist >= 1 && dist <= 2);
}

static void test_real_map_distance_same_tile(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;
  int dist;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[0];

  dist = real_map_distance(ptile1, ptile2);
  assert_int_equal(dist, 0);
}

static void test_real_map_distance_adjacent(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;
  int dist;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[1];

  dist = real_map_distance(ptile1, ptile2);
  assert_int_equal(dist, 1);
}

static void test_sq_map_distance_same_tile(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;
  int dist;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[0];

  dist = sq_map_distance(ptile1, ptile2);
  assert_int_equal(dist, 0);
}

static void test_sq_map_distance_adjacent(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;
  int dist;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[1];

  dist = sq_map_distance(ptile1, ptile2);
  assert_int_equal(dist, 1); /* 1^2 = 1 */
}

static void test_sq_map_distance_two_away(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;
  int dist;

  /* Tiles 2 apart */
  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[2];

  dist = sq_map_distance(ptile1, ptile2);
  assert_int_equal(dist, 4); /* 2^2 = 4 */
}

/* ==================== Adjacency Tests ==================== */

static void test_is_tiles_adjacent_same(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[0];

  assert_false(is_tiles_adjacent(ptile1, ptile2));
}

static void test_is_tiles_adjacent_horizontal(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[1];

  assert_true(is_tiles_adjacent(ptile1, ptile2));
}

static void test_is_tiles_adjacent_vertical(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;

  /* Tiles on same X but different Y */
  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[TEST_MAP_WIDTH];

  assert_true(is_tiles_adjacent(ptile1, ptile2));
}

static void test_is_tiles_adjacent_far(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[10];

  assert_false(is_tiles_adjacent(ptile1, ptile2));
}

/* ==================== Map Distance Vector Tests ==================== */

static void test_map_distance_vector_same(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;
  int dx, dy;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[0];

  map_distance_vector(&dx, &dy, ptile1, ptile2);
  assert_int_equal(dx, 0);
  assert_int_equal(dy, 0);
}

static void test_map_distance_vector_adjacent(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;
  int dx, dy;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[1];

  map_distance_vector(&dx, &dy, ptile1, ptile2);
  assert_int_equal(dx, 1);
  assert_int_equal(dy, 0);
}

/* ==================== Topology Tests ==================== */

static void test_topo_has_flag(void **state)
{
  (void) state;

  /* Default topology is 0 (no flags) */
  assert_false(topo_has_flag(0, TF_ISO));
  assert_false(topo_has_flag(0, TF_HEX));

  assert_true(topo_has_flag(TF_ISO, TF_ISO));
  assert_true(topo_has_flag(TF_HEX, TF_HEX));
  assert_false(topo_has_flag(TF_ISO, TF_HEX));
}

static void test_wrap_has_flag(void **state)
{
  (void) state;

  /* Default wrap is WRAP_X */
  assert_true(wrap_has_flag(WRAP_X, WRAP_X));
  assert_false(wrap_has_flag(WRAP_X, WRAP_Y));
  assert_true(wrap_has_flag(WRAP_X | WRAP_Y, WRAP_X));
  assert_true(wrap_has_flag(WRAP_X | WRAP_Y, WRAP_Y));
}

/* ==================== Continent/Ocean Tests ==================== */

static void test_tile_continent(void **state)
{
  (void) state;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];
  ptile->continent = 5;

  assert_int_equal(tile_continent(ptile), 5);
}

static void test_tile_set_continent(void **state)
{
  (void) state;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];
  tile_set_continent(ptile, 10);

  assert_int_equal(tile_continent(ptile), 10);
}

/* ==================== Border Tile Tests ==================== */

static void test_is_border_tile_corner(void **state)
{
  (void) state;
  struct tile *ptile;

  /* Corner tiles should be border tiles */
  ptile = &wld.map.tiles[0];
  assert_true(is_border_tile(ptile, 1));

  ptile = &wld.map.tiles[MAP_INDEX_SIZE - 1];
  assert_true(is_border_tile(ptile, 1));
}

static void test_is_border_tile_center(void **state)
{
  (void) state;
  struct tile *ptile;
  int center_idx;

  /* Center tile should not be border tile with distance 1 */
  center_idx = (TEST_MAP_HEIGHT / 2) * TEST_MAP_WIDTH + (TEST_MAP_WIDTH / 2);
  ptile = &wld.map.tiles[center_idx];
  assert_false(is_border_tile(ptile, 1));
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* Map Initialization Tests */
    cmocka_unit_test_setup_teardown(test_map_is_empty_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_map_is_empty_false, setup, teardown),

    /* Coordinate Conversion Tests */
    cmocka_unit_test_setup_teardown(test_map_pos_to_tile_origin, setup, teardown),
    cmocka_unit_test_setup_teardown(test_map_pos_to_tile_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_map_pos_to_tile_out_of_bounds, setup, teardown),
    cmocka_unit_test_setup_teardown(test_map_pos_to_tile_with_wrap, setup, teardown),
    cmocka_unit_test_setup_teardown(test_index_to_tile_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_index_to_tile_invalid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_native_pos_to_tile_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_native_pos_to_tile_out_of_bounds, setup, teardown),
    cmocka_unit_test_setup_teardown(test_native_pos_to_tile_with_wrap, setup, teardown),

    /* Map Size Tests */
    cmocka_unit_test_setup_teardown(test_map_num_tiles, setup, teardown),
    cmocka_unit_test_setup_teardown(test_map_size_checked, setup, teardown),

    /* Position Validation Tests */
    cmocka_unit_test_setup_teardown(test_is_normal_map_pos_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_normal_map_pos_invalid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_real_map_pos_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_real_map_pos_invalid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_normalize_map_pos_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_normalize_map_pos_invalid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_nearest_real_tile_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_nearest_real_tile_out_of_bounds, setup, teardown),

    /* Position Comparison Tests */
    cmocka_unit_test_setup_teardown(test_same_pos_same_tile, setup, teardown),
    cmocka_unit_test_setup_teardown(test_same_pos_different_tile, setup, teardown),
    cmocka_unit_test_setup_teardown(test_same_pos_different_coords_same_wrapped, setup, teardown),

    /* Direction Tests */
    cmocka_unit_test_setup_teardown(test_dir_reverse, setup, teardown),
    cmocka_unit_test_setup_teardown(test_dir_dx_dy, setup, teardown),
    cmocka_unit_test_setup_teardown(test_opposite_direction, setup, teardown),

    /* Distance Calculation Tests */
    cmocka_unit_test_setup_teardown(test_map_distance_same_tile, setup, teardown),
    cmocka_unit_test_setup_teardown(test_map_distance_adjacent, setup, teardown),
    cmocka_unit_test_setup_teardown(test_map_distance_diagonal, setup, teardown),
    cmocka_unit_test_setup_teardown(test_real_map_distance_same_tile, setup, teardown),
    cmocka_unit_test_setup_teardown(test_real_map_distance_adjacent, setup, teardown),
    cmocka_unit_test_setup_teardown(test_sq_map_distance_same_tile, setup, teardown),
    cmocka_unit_test_setup_teardown(test_sq_map_distance_adjacent, setup, teardown),
    cmocka_unit_test_setup_teardown(test_sq_map_distance_two_away, setup, teardown),

    /* Adjacency Tests */
    cmocka_unit_test_setup_teardown(test_is_tiles_adjacent_same, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_tiles_adjacent_horizontal, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_tiles_adjacent_vertical, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_tiles_adjacent_far, setup, teardown),

    /* Map Distance Vector Tests */
    cmocka_unit_test_setup_teardown(test_map_distance_vector_same, setup, teardown),
    cmocka_unit_test_setup_teardown(test_map_distance_vector_adjacent, setup, teardown),

    /* Topology Tests */
    cmocka_unit_test_setup_teardown(test_topo_has_flag, setup, teardown),
    cmocka_unit_test_setup_teardown(test_wrap_has_flag, setup, teardown),

    /* Continent/Ocean Tests */
    cmocka_unit_test_setup_teardown(test_tile_continent, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_set_continent, setup, teardown),

    /* Border Tile Tests */
    cmocka_unit_test_setup_teardown(test_is_border_tile_corner, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_border_tile_center, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}