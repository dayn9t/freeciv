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
#include "borders.h"
#include "city.h"
#include "game.h"
#include "map.h"
#include "tile.h"
#include "world_object.h"

#define TEST_MAP_WIDTH 10
#define TEST_MAP_HEIGHT 10

/***********************************************************************
  Setup function - initialize minimal map and game state
***********************************************************************/
static int setup(void **state)
{
  int i;

  (void) state;

  /* Clear game and world state */
  memset(&game, 0, sizeof(game));
  memset(&wld, 0, sizeof(wld));

  /* Initialize map */
  wld.map.xsize = TEST_MAP_WIDTH;
  wld.map.ysize = TEST_MAP_HEIGHT;
  wld.map.topology_id = 0;
  wld.map.wrap_id = 0;  /* No wrapping */

  /* Allocate tiles */
  wld.map.tiles = fc_calloc(TEST_MAP_WIDTH * TEST_MAP_HEIGHT,
                            sizeof(struct tile));
  for (i = 0; i < TEST_MAP_WIDTH * TEST_MAP_HEIGHT; i++) {
    wld.map.tiles[i].index = i;
    wld.map.tiles[i].units = unit_list_new();
  }

  return 0;
}

/***********************************************************************
  Teardown function - clean up map and game state
***********************************************************************/
static int teardown(void **state)
{
  int i;

  (void) state;

  /* Free tiles */
  if (wld.map.tiles != NULL) {
    for (i = 0; i < TEST_MAP_WIDTH * TEST_MAP_HEIGHT; i++) {
      if (wld.map.tiles[i].units != NULL) {
        unit_list_destroy(wld.map.tiles[i].units);
      }
    }
    free(wld.map.tiles);
    wld.map.tiles = NULL;
  }

  /* Clear game and world state */
  memset(&game, 0, sizeof(game));
  memset(&wld, 0, sizeof(wld));

  return 0;
}

/***********************************************************************
  Helper: Create a mock city on a tile
***********************************************************************/
static struct city *create_mock_city(struct tile *ptile, int size)
{
  struct city *pcity;

  pcity = fc_calloc(1, sizeof(struct city));
  pcity->id = 1;
  pcity->name = fc_strdup("TestCity");
  pcity->size = size;
  pcity->tile = ptile;
  pcity->owner = NULL;

  /* Link city to tile - tile->worked points to city for city center */
  ptile->worked = pcity;

  return pcity;
}

/***********************************************************************
  Helper: Destroy mock city
***********************************************************************/
static void destroy_mock_city(struct city *pcity)
{
  if (pcity == NULL) {
    return;
  }
  if (pcity->tile != NULL) {
    pcity->tile->worked = NULL;
  }
  free(pcity->name);
  free(pcity);
}

/***********************************************************************
  Test: tile_border_source_radius_sq with borders disabled
***********************************************************************/
static void test_tile_border_source_radius_sq_disabled(void **state)
{
  int result;

  (void) state;

  game.info.borders = BORDERS_DISABLED;

  result = tile_border_source_radius_sq(NULL);
  assert_int_equal(result, 0);
}

/***********************************************************************
  Test: tile_border_source_radius_sq with borders enabled, no city
***********************************************************************/
static void test_tile_border_source_radius_sq_no_city(void **state)
{
  struct tile *ptile;
  int result;

  game.info.borders = BORDERS_ENABLED;
  game.info.border_city_radius_sq = 9;  /* 3^2 = 9 */

  ptile = &wld.map.tiles[0];
  ptile->worked = NULL;  /* No city on this tile */

  result = tile_border_source_radius_sq(ptile);
  /* No city, no terr_claimer extra -> radius_sq = 0 */
  assert_int_equal(result, 0);
}

/***********************************************************************
  Test: tile_border_source_radius_sq with city, size 1
***********************************************************************/
static void test_tile_border_source_radius_sq_city_size_one(void **state)
{
  struct tile *ptile;
  struct city *pcity;
  int result;

  game.info.borders = BORDERS_ENABLED;
  game.info.border_city_radius_sq = 9;  /* Base radius squared = 9 */
  game.info.border_size_effect = 1;     /* Each size adds 1 to radius_sq */

  ptile = &wld.map.tiles[0];
  pcity = create_mock_city(ptile, 1);  /* City size 1 */

  result = tile_border_source_radius_sq(ptile);
  /* radius_sq = 9 + MIN(1, CITY_MAP_MAX_RADIUS_SQ) * 1 = 9 + 1 = 10 */
  assert_int_equal(result, 10);

  destroy_mock_city(pcity);
}

/***********************************************************************
  Test: tile_border_source_radius_sq with city, larger size
***********************************************************************/
static void test_tile_border_source_radius_sq_city_size_large(void **state)
{
  struct tile *ptile;
  struct city *pcity;
  int result;

  game.info.borders = BORDERS_ENABLED;
  game.info.border_city_radius_sq = 9;  /* Base radius squared = 9 */
  game.info.border_size_effect = 2;     /* Each size adds 2 to radius_sq */

  ptile = &wld.map.tiles[0];
  pcity = create_mock_city(ptile, 5);  /* City size 5 */

  result = tile_border_source_radius_sq(ptile);
  /* radius_sq = 9 + MIN(5, CITY_MAP_MAX_RADIUS_SQ) * 2 = 9 + 10 = 19 */
  assert_int_equal(result, 19);

  destroy_mock_city(pcity);
}

/***********************************************************************
  Test: tile_border_source_radius_sq with city, zero size effect
***********************************************************************/
static void test_tile_border_source_radius_sq_city_zero_effect(void **state)
{
  struct tile *ptile;
  struct city *pcity;
  int result;

  game.info.borders = BORDERS_ENABLED;
  game.info.border_city_radius_sq = 16;  /* Base radius squared = 16 */
  game.info.border_size_effect = 0;      /* Size has no effect */

  ptile = &wld.map.tiles[0];
  pcity = create_mock_city(ptile, 10);  /* City size 10 */

  result = tile_border_source_radius_sq(ptile);
  /* radius_sq = 16 + MIN(10, CITY_MAP_MAX_RADIUS_SQ) * 0 = 16 */
  assert_int_equal(result, 16);

  destroy_mock_city(pcity);
}

/***********************************************************************
  Test: tile_border_source_strength with borders disabled
***********************************************************************/
static void test_tile_border_source_strength_disabled(void **state)
{
  int result;

  (void) state;

  game.info.borders = BORDERS_DISABLED;

  result = tile_border_source_strength(NULL);
  assert_int_equal(result, 0);
}

/***********************************************************************
  Test: tile_border_source_strength with borders enabled, no city
***********************************************************************/
static void test_tile_border_source_strength_no_city(void **state)
{
  struct tile *ptile;
  int result;

  game.info.borders = BORDERS_ENABLED;

  ptile = &wld.map.tiles[0];
  ptile->worked = NULL;  /* No city on this tile */

  result = tile_border_source_strength(ptile);
  /* No city, no terr_claimer extra -> strength = 0 */
  assert_int_equal(result, 0);
}

/***********************************************************************
  Test: tile_border_source_strength with city, size 1
***********************************************************************/
static void test_tile_border_source_strength_city_size_one(void **state)
{
  struct tile *ptile;
  struct city *pcity;
  int result;

  game.info.borders = BORDERS_ENABLED;

  ptile = &wld.map.tiles[0];
  pcity = create_mock_city(ptile, 1);  /* City size 1 */

  result = tile_border_source_strength(ptile);
  /* strength = (1 + 2) * (100 + 0) / 100 = 3 * 100 / 100 = 3 */
  assert_int_equal(result, 3);

  destroy_mock_city(pcity);
}

/***********************************************************************
  Test: tile_border_source_strength with city, larger size
***********************************************************************/
static void test_tile_border_source_strength_city_size_large(void **state)
{
  struct tile *ptile;
  struct city *pcity;
  int result;

  game.info.borders = BORDERS_ENABLED;

  ptile = &wld.map.tiles[0];
  pcity = create_mock_city(ptile, 10);  /* City size 10 */

  result = tile_border_source_strength(ptile);
  /* strength = (10 + 2) * (100 + 0) / 100 = 12 * 100 / 100 = 12 */
  assert_int_equal(result, 12);

  destroy_mock_city(pcity);
}

/***********************************************************************
  Test: tile_border_source_strength with city, size 0
***********************************************************************/
static void test_tile_border_source_strength_city_size_zero(void **state)
{
  struct tile *ptile;
  struct city *pcity;
  int result;

  game.info.borders = BORDERS_ENABLED;

  ptile = &wld.map.tiles[0];
  pcity = create_mock_city(ptile, 0);  /* City size 0 */

  result = tile_border_source_strength(ptile);
  /* strength = (0 + 2) * (100 + 0) / 100 = 2 * 100 / 100 = 2 */
  assert_int_equal(result, 2);

  destroy_mock_city(pcity);
}

/***********************************************************************
  Test: tile_border_strength at source tile (distance 0)
***********************************************************************/
static void test_tile_border_strength_same_tile(void **state)
{
  struct tile *ptile;
  int result;

  game.info.borders = BORDERS_ENABLED;

  ptile = &wld.map.tiles[0];

  /* When source and target are the same tile, sq_dist = 0 */
  result = tile_border_strength(ptile, ptile);
  assert_int_equal(result, FC_INFINITY);
}

/***********************************************************************
  Test: tile_border_strength with distance
***********************************************************************/
static void test_tile_border_strength_with_distance(void **state)
{
  struct tile *source_tile;
  struct tile *target_tile;
  struct city *pcity;
  int result;

  game.info.borders = BORDERS_ENABLED;

  /* Source tile at (0,0) has a city */
  source_tile = &wld.map.tiles[0];
  pcity = create_mock_city(source_tile, 8);  /* City size 8 */

  /* Target tile at (2,0) - index 2 in a 10x10 map */
  target_tile = &wld.map.tiles[2];

  /* Strength at source: (8 + 2) * 100 / 100 = 10 */
  /* Distance squared between (0,0) and (2,0) is 4 */
  /* Border strength = 10 * 10 / 4 = 25 */
  result = tile_border_strength(target_tile, source_tile);
  assert_int_equal(result, 25);

  destroy_mock_city(pcity);
}

/***********************************************************************
  Test: tile_border_strength with larger distance
***********************************************************************/
static void test_tile_border_strength_with_larger_distance(void **state)
{
  struct tile *source_tile;
  struct tile *target_tile;
  struct city *pcity;
  int result;

  game.info.borders = BORDERS_ENABLED;

  /* Source tile at (0,0) has a city */
  source_tile = &wld.map.tiles[0];
  pcity = create_mock_city(source_tile, 4);  /* City size 4 */

  /* Target tile at (5,0) - index 5 in a 10x10 map */
  target_tile = &wld.map.tiles[5];

  /* Strength at source: (4 + 2) * 100 / 100 = 6 */
  /* Distance squared between (0,0) and (5,0) is 25 */
  /* Border strength = 6 * 6 / 25 = 36 / 25 = 1 */
  result = tile_border_strength(target_tile, source_tile);
  assert_int_equal(result, 1);

  destroy_mock_city(pcity);
}

/***********************************************************************
  Test: is_border_source with no city, no extras
***********************************************************************/
static void test_is_border_source_no_source(void **state)
{
  struct tile *ptile;
  bool result;

  game.info.borders = BORDERS_ENABLED;

  ptile = &wld.map.tiles[0];
  ptile->worked = NULL;
  ptile->extras_owner = NULL;

  result = is_border_source(ptile);
  assert_false(result);
}

/***********************************************************************
  Test: is_border_source with city
***********************************************************************/
static void test_is_border_source_with_city(void **state)
{
  struct tile *ptile;
  struct city *pcity;
  bool result;

  game.info.borders = BORDERS_ENABLED;

  ptile = &wld.map.tiles[0];
  pcity = create_mock_city(ptile, 1);

  result = is_border_source(ptile);
  assert_true(result);

  destroy_mock_city(pcity);
}

/***********************************************************************
  Test: FC_INFINITY constant value
***********************************************************************/
static void test_fc_infinity_constant(void **state)
{
  (void) state;

  /* FC_INFINITY should be a large positive value */
  assert_true(FC_INFINITY > 0);
  assert_true(FC_INFINITY > 1000000);
  /* FC_INFINITY is defined as 1000 * 1000 * 1000 = 1000000000 */
  assert_int_equal(FC_INFINITY, 1000000000);
}

/***********************************************************************
  Test: BORDERS_DISABLED constant value
***********************************************************************/
static void test_borders_disabled_constant(void **state)
{
  (void) state;

  /* BORDERS_DISABLED should be 0 */
  assert_int_equal(BORDERS_DISABLED, 0);
}

/***********************************************************************
  Test: BORDERS_ENABLED constant value
***********************************************************************/
static void test_borders_enabled_constant(void **state)
{
  (void) state;

  /* BORDERS_ENABLED should be greater than BORDERS_DISABLED */
  assert_true(BORDERS_ENABLED > BORDERS_DISABLED);
}

/***********************************************************************
  Test: tile_border_source_radius_sq with multiple cities
***********************************************************************/
static void test_tile_border_source_radius_sq_independent(void **state)
{
  struct tile *ptile1;
  struct tile *ptile2;
  struct city *pcity1;
  struct city *pcity2;
  int result1, result2;

  game.info.borders = BORDERS_ENABLED;
  game.info.border_city_radius_sq = 4;  /* Base radius squared = 4 */
  game.info.border_size_effect = 1;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[1];

  pcity1 = create_mock_city(ptile1, 2);
  pcity2 = create_mock_city(ptile2, 5);

  result1 = tile_border_source_radius_sq(ptile1);
  result2 = tile_border_source_radius_sq(ptile2);

  /* Each city should have independent radius based on its size */
  /* City 1: 4 + 2 = 6 */
  /* City 2: 4 + 5 = 9 */
  assert_int_equal(result1, 6);
  assert_int_equal(result2, 9);

  destroy_mock_city(pcity1);
  destroy_mock_city(pcity2);
}

/***********************************************************************
  Test: tile_border_source_strength with multiple cities
***********************************************************************/
static void test_tile_border_source_strength_independent(void **state)
{
  struct tile *ptile1;
  struct tile *ptile2;
  struct city *pcity1;
  struct city *pcity2;
  int result1, result2;

  game.info.borders = BORDERS_ENABLED;

  ptile1 = &wld.map.tiles[0];
  ptile2 = &wld.map.tiles[1];

  pcity1 = create_mock_city(ptile1, 3);
  pcity2 = create_mock_city(ptile2, 7);

  result1 = tile_border_source_strength(ptile1);
  result2 = tile_border_source_strength(ptile2);

  /* City 1 strength: (3 + 2) = 5 */
  /* City 2 strength: (7 + 2) = 9 */
  assert_int_equal(result1, 5);
  assert_int_equal(result2, 9);

  destroy_mock_city(pcity1);
  destroy_mock_city(pcity2);
}

/***********************************************************************
  Test: Border calculation consistency between radius and strength
***********************************************************************/
static void test_border_calculation_consistency(void **state)
{
  struct tile *ptile;
  struct city *pcity;
  int radius_sq, strength;

  game.info.borders = BORDERS_ENABLED;
  game.info.border_city_radius_sq = 9;
  game.info.border_size_effect = 1;

  ptile = &wld.map.tiles[0];
  pcity = create_mock_city(ptile, 5);

  radius_sq = tile_border_source_radius_sq(ptile);
  strength = tile_border_source_strength(ptile);

  /* With borders enabled, both should be non-zero for a city */
  assert_true(radius_sq > 0);
  assert_true(strength > 0);

  /* Radius should be consistent: 9 + 5 = 14 */
  assert_int_equal(radius_sq, 14);
  /* Strength should be: 5 + 2 = 7 */
  assert_int_equal(strength, 7);

  destroy_mock_city(pcity);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* Basic disabled/enabled tests */
    cmocka_unit_test_setup_teardown(test_tile_border_source_radius_sq_disabled,
                                     setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_border_source_strength_disabled,
                                     setup, teardown),

    /* Radius tests */
    cmocka_unit_test_setup_teardown(test_tile_border_source_radius_sq_no_city,
                                     setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_border_source_radius_sq_city_size_one,
                                     setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_border_source_radius_sq_city_size_large,
                                     setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_border_source_radius_sq_city_zero_effect,
                                     setup, teardown),

    /* Strength tests */
    cmocka_unit_test_setup_teardown(test_tile_border_source_strength_no_city,
                                     setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_border_source_strength_city_size_one,
                                     setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_border_source_strength_city_size_large,
                                     setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_border_source_strength_city_size_zero,
                                     setup, teardown),

    /* Border strength at distance tests */
    cmocka_unit_test_setup_teardown(test_tile_border_strength_same_tile,
                                     setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_border_strength_with_distance,
                                     setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_border_strength_with_larger_distance,
                                     setup, teardown),

    /* is_border_source tests */
    cmocka_unit_test_setup_teardown(test_is_border_source_no_source,
                                     setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_border_source_with_city,
                                     setup, teardown),

    /* Constant tests */
    cmocka_unit_test(test_fc_infinity_constant),
    cmocka_unit_test(test_borders_disabled_constant),
    cmocka_unit_test(test_borders_enabled_constant),

    /* Multiple cities tests */
    cmocka_unit_test_setup_teardown(test_tile_border_source_radius_sq_independent,
                                     setup, teardown),
    cmocka_unit_test_setup_teardown(test_tile_border_source_strength_independent,
                                     setup, teardown),

    /* Consistency test */
    cmocka_unit_test_setup_teardown(test_border_calculation_consistency,
                                     setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}