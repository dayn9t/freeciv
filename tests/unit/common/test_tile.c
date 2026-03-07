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
  if (wld.map.tiles != nullptr) {
    for (int i = 0; i < MAP_INDEX_SIZE; i++) {
      unit_list_destroy(wld.map.tiles[i].units);
    }
    free(wld.map.tiles);
    wld.map.tiles = nullptr;
  }

  return 0;
}

static void test_tile_index_basic(void **state)
{
  (void) state;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];
  assert_int_equal(tile_index(ptile), 0);

  ptile = &wld.map.tiles[5];
  assert_int_equal(tile_index(ptile), 5);
}

static void test_index_to_tile_valid(void **state)
{
  (void) state;
  struct tile *ptile;

  ptile = index_to_tile(&wld.map, 0);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), 0);

  ptile = index_to_tile(&wld.map, 5);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), 5);

  ptile = index_to_tile(&wld.map, TEST_MAP_WIDTH * TEST_MAP_HEIGHT - 1);
  assert_non_null(ptile);
}

static void test_index_to_tile_invalid(void **state)
{
  (void) state;
  struct tile *ptile;

  /* Negative index should return nullptr */
  ptile = index_to_tile(&wld.map, -1);
  assert_null(ptile);

  /* Index beyond map size should return nullptr */
  ptile = index_to_tile(&wld.map, TEST_MAP_WIDTH * TEST_MAP_HEIGHT);
  assert_null(ptile);
}

static void test_map_pos_to_tile(void **state)
{
  (void) state;
  struct tile *ptile;

  ptile = map_pos_to_tile(&wld.map, 0, 0);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), 0);

  ptile = map_pos_to_tile(&wld.map, 5, 5);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), 5 * TEST_MAP_WIDTH + 5);
}

static void test_native_pos_to_tile(void **state)
{
  (void) state;
  struct tile *ptile;

  ptile = native_pos_to_tile(&wld.map, 0, 0);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), 0);

  ptile = native_pos_to_tile(&wld.map, 10, 10);
  assert_non_null(ptile);
  assert_int_equal(tile_index(ptile), 10 * TEST_MAP_WIDTH + 10);
}

static void test_tile_continent(void **state)
{
  (void) state;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];
  ptile->continent = 1;

  assert_int_equal(tile_continent(ptile), 1);
}

static void test_tile_set_continent(void **state)
{
  (void) state;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];
  tile_set_continent(ptile, 5);

  assert_int_equal(tile_continent(ptile), 5);
}

static void test_same_pos(void **state)
{
  (void) state;
  struct tile *ptile1, *ptile2;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[0];

  assert_true(same_pos(ptile1, ptile2));

  ptile2 = &wld.map.tiles[1];
  assert_false(same_pos(ptile1, ptile2));
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_tile_index_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_index_to_tile_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_index_to_tile_invalid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_map_pos_to_tile, setup, teardown),
    cmocka_unit_test_setup_teardown(test_native_pos_to_tile, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_continent, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_set_continent, setup, teardown),
    cmocka_unit_test_setup_teardown(test_same_pos, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
