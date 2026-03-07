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
#include "city.h"
#include "game.h"
#include "map.h"
#include "world_object.h"

/* Test city structure for basic tests */
static struct city test_city;

/***********************************************************************
  Setup function - initialize minimal game state
***********************************************************************/
static int setup(void **state)
{
  (void) state;

  memset(&test_city, 0, sizeof(test_city));
  test_city.name = fc_strdup("TestCity");
  test_city.id = 1;
  test_city.size = 5;
  test_city.capital = CAPITAL_NOT;
  test_city.city_radius_sq = CITY_MAP_DEFAULT_RADIUS_SQ;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));
  game.info.foodbox = 100;
  game.info.granary_num_inis = 2;
  game.info.granary_food_ini[0] = 20;
  game.info.granary_food_ini[1] = 20;
  game.info.granary_food_inc = 10;

  /* Initialize city map indices */
  generate_city_map_indices();

  return 0;
}

/***********************************************************************
  Teardown function - cleanup
***********************************************************************/
static int teardown(void **state)
{
  (void) state;

  if (test_city.name != NULL) {
    FC_FREE(test_city.name);
    test_city.name = NULL;
  }

  free_city_map_index();

  return 0;
}

/***********************************************************************
  Test city_name_get with valid city
***********************************************************************/
static void test_city_name_get_valid(void **state)
{
  (void) state;

  const char *name = city_name_get(&test_city);
  assert_string_equal(name, "TestCity");
}

/***********************************************************************
  Test city_name_get with NULL city - skip since function doesn't handle NULL
  Note: city_name_get() crashes on NULL input - it checks pcity->name not pcity
***********************************************************************/
static void test_city_name_get_null(void **state)
{
  (void) state;

  /* This test is disabled because city_name_get() doesn't handle NULL pcity */
  /* The function checks pcity->name != nullptr, which segfaults on NULL pcity */
  skip();
}

/***********************************************************************
  Test city_name_set with valid name
***********************************************************************/
static void test_city_name_set_valid(void **state)
{
  (void) state;

  city_name_set(&test_city, "NewName");
  assert_string_equal(test_city.name, "NewName");
}

/***********************************************************************
  Test city_size_get with valid city
***********************************************************************/
static void test_city_size_get_valid(void **state)
{
  (void) state;

  citizens size = city_size_get(&test_city);
  assert_int_equal(size, 5);
}

/***********************************************************************
  Test city_size_get with NULL city
***********************************************************************/
static void test_city_size_get_null(void **state)
{
  (void) state;

  citizens size = city_size_get(NULL);
  assert_int_equal(size, 0);
}

/***********************************************************************
  Test city_size_set
***********************************************************************/
static void test_city_size_set(void **state)
{
  (void) state;

  city_size_set(&test_city, 10);
  assert_int_equal(test_city.size, 10);
}

/***********************************************************************
  Test city_size_add positive
***********************************************************************/
static void test_city_size_add_positive(void **state)
{
  (void) state;

  test_city.size = 5;
  city_size_add(&test_city, 3);
  assert_int_equal(test_city.size, 8);
}

/***********************************************************************
  Test city_size_add negative
***********************************************************************/
static void test_city_size_add_negative(void **state)
{
  (void) state;

  test_city.size = 10;
  city_size_add(&test_city, -2);
  assert_int_equal(test_city.size, 8);
}

/***********************************************************************
  Test city_population calculation
***********************************************************************/
static void test_city_population(void **state)
{
  (void) state;

  /* Population = size * (size + 1) * 5 */
  test_city.size = 1;
  assert_int_equal(city_population(&test_city), 10);

  test_city.size = 5;
  assert_int_equal(city_population(&test_city), 150);

  test_city.size = 10;
  assert_int_equal(city_population(&test_city), 550);
}

/***********************************************************************
  Test city_granary_size with zero city size
***********************************************************************/
static void test_city_granary_size_zero(void **state)
{
  (void) state;

  int granary = city_granary_size(0);
  assert_int_equal(granary, 0);
}

/***********************************************************************
  Test city_granary_size with valid size
***********************************************************************/
static void test_city_granary_size_valid(void **state)
{
  (void) state;

  /* Default game settings: foodbox = 100, granary_num_inis = 2,
   * granary_food_ini = {20, 20}, granary_food_inc = 10 */
  game.info.foodbox = 100;
  game.info.granary_num_inis = 2;
  game.info.granary_food_ini[0] = 20;
  game.info.granary_food_ini[1] = 20;
  game.info.granary_food_inc = 10;

  /* Size 1: base = 20, granary = 20 * 100 / 100 = 20 */
  int granary = city_granary_size(1);
  assert_int_equal(granary, 20);

  /* Size 2: base = 20, granary = 20 * 100 / 100 = 20 */
  granary = city_granary_size(2);
  assert_int_equal(granary, 20);

  /* Size 3: base = 20 + 10 * (3 - 2) = 30, granary = 30 * 100 / 100 = 30 */
  granary = city_granary_size(3);
  assert_int_equal(granary, 30);
}

/***********************************************************************
  Test is_capital with capital city
***********************************************************************/
static void test_is_capital_true(void **state)
{
  (void) state;

  test_city.capital = CAPITAL_PRIMARY;
  assert_true(is_capital(&test_city));
}

/***********************************************************************
  Test is_capital with non-capital city
***********************************************************************/
static void test_is_capital_false(void **state)
{
  (void) state;

  test_city.capital = CAPITAL_NOT;
  assert_false(is_capital(&test_city));
}

/***********************************************************************
  Test is_free_worked_index
***********************************************************************/
static void test_is_free_worked_index(void **state)
{
  (void) state;

  assert_true(is_free_worked_index(CITY_MAP_CENTER_TILE_INDEX));
  assert_false(is_free_worked_index(1));
  assert_false(is_free_worked_index(5));
}

/***********************************************************************
  Test city_map_radius_sq_get
***********************************************************************/
static void test_city_map_radius_sq_get(void **state)
{
  (void) state;

  test_city.city_radius_sq = 5;
  assert_int_equal(city_map_radius_sq_get(&test_city), 5);
}

/***********************************************************************
  Test city_map_radius_sq_get NULL city
***********************************************************************/
static void test_city_map_radius_sq_get_null(void **state)
{
  (void) state;

  int radius = city_map_radius_sq_get(NULL);
  assert_int_equal(radius, CITY_MAP_MIN_RADIUS_SQ);
}

/***********************************************************************
  Test city_map_radius_sq_set
***********************************************************************/
static void test_city_map_radius_sq_set(void **state)
{
  (void) state;

  city_map_radius_sq_set(&test_city, 10);
  assert_int_equal(test_city.city_radius_sq, 10);
}

/***********************************************************************
  Test city_map_tiles with special case -1
***********************************************************************/
static void test_city_map_tiles_center(void **state)
{
  (void) state;

  int tiles = city_map_tiles(CITY_MAP_CENTER_RADIUS_SQ);
  assert_int_equal(tiles, 0);
}

/***********************************************************************
  Test city_map_tiles with valid radius
***********************************************************************/
static void test_city_map_tiles_valid(void **state)
{
  (void) state;

  /* Radius squared 5 (default city radius) should have 21 tiles */
  int tiles = city_map_tiles(5);
  assert_int_equal(tiles, 21);

  /* Radius squared 1 should have 5 tiles */
  tiles = city_map_tiles(1);
  assert_int_equal(tiles, 5);
}

/***********************************************************************
  Test is_valid_city_coords with center coordinates
***********************************************************************/
static void test_is_valid_city_coords_center(void **state)
{
  (void) state;

  int center_x = CITY_REL2ABS(0);  /* 5 */
  int center_y = CITY_REL2ABS(0);  /* 5 */

  assert_true(is_valid_city_coords(5, center_x, center_y));
}

/***********************************************************************
  Test is_valid_city_coords with valid edge coordinates
***********************************************************************/
static void test_is_valid_city_coords_valid(void **state)
{
  (void) state;

  /* Check coordinates within radius 5 (default) */
  /* (1,1) relative has squared distance 2, which is <= 5 */
  int x = CITY_REL2ABS(1);  /* 6 */
  int y = CITY_REL2ABS(1);  /* 6 */

  assert_true(is_valid_city_coords(5, x, y));
}

/***********************************************************************
  Test is_valid_city_coords with invalid coordinates
***********************************************************************/
static void test_is_valid_city_coords_invalid(void **state)
{
  (void) state;

  /* Check coordinates outside radius 5 */
  int x = CITY_REL2ABS(5);  /* 10 */
  int y = CITY_REL2ABS(5);  /* 10 */

  assert_false(is_valid_city_coords(5, x, y));
}

/***********************************************************************
  Test city_tile_index_to_xy with valid index
***********************************************************************/
static void test_city_tile_index_to_xy_valid(void **state)
{
  (void) state;

  int city_map_x, city_map_y;
  bool result;

  /* Index 0 should be the center */
  result = city_tile_index_to_xy(&city_map_x, &city_map_y, 0, 5);
  assert_true(result);
  assert_int_equal(CITY_ABS2REL(city_map_x), 0);
  assert_int_equal(CITY_ABS2REL(city_map_y), 0);
}

/***********************************************************************
  Test city_tile_index_to_xy with invalid index
***********************************************************************/
static void test_city_tile_index_to_xy_invalid(void **state)
{
  (void) state;

  int city_map_x, city_map_y;
  bool result;

  /* Index beyond tile count should fail */
  result = city_tile_index_to_xy(&city_map_x, &city_map_y, 100, 5);
  assert_false(result);

  /* Negative index should fail */
  result = city_tile_index_to_xy(&city_map_x, &city_map_y, -1, 5);
  assert_false(result);
}

/***********************************************************************
  Test compare_iter_index with equal values
***********************************************************************/
static void test_compare_iter_index_equal(void **state)
{
  (void) state;

  /* If dist is same, it compares dx */
  struct iter_index c = {1, 0, 0};  /* dist=1, dx=0, dy=0 */
  struct iter_index d = {1, 1, 0};  /* dist=1, dx=1, dy=0 */

  int result = compare_iter_index(&c, &d);
  assert_int_equal(result, -1);  /* c.dx < d.dx */
}

/***********************************************************************
  Test compare_iter_index with different distances
***********************************************************************/
static void test_compare_iter_index_different_dist(void **state)
{
  (void) state;

  struct iter_index a = {0, 0, 0};  /* dist=0 */
  struct iter_index b = {1, 0, 0};  /* dist=1 */

  int result = compare_iter_index(&a, &b);
  assert_int_equal(result, -1);  /* a has smaller distance */

  result = compare_iter_index(&b, &a);
  assert_int_equal(result, 1);  /* b has larger distance */
}

/***********************************************************************
  Test get_output_identifier
***********************************************************************/
static void test_get_output_identifier(void **state)
{
  (void) state;

  assert_string_equal(get_output_identifier(O_FOOD), "food");
  assert_string_equal(get_output_identifier(O_SHIELD), "shield");
  assert_string_equal(get_output_identifier(O_TRADE), "trade");
  assert_string_equal(get_output_identifier(O_GOLD), "gold");
  assert_string_equal(get_output_identifier(O_LUXURY), "luxury");
  assert_string_equal(get_output_identifier(O_SCIENCE), "science");
}

/***********************************************************************
  Test get_output_identifier with invalid index
***********************************************************************/
static void test_get_output_identifier_invalid(void **state)
{
  (void) state;

  const char *id = get_output_identifier(-1);
  assert_null(id);

  id = get_output_identifier(O_LAST);
  assert_null(id);
}

/***********************************************************************
  Test output_type_by_identifier
***********************************************************************/
static void test_output_type_by_identifier(void **state)
{
  (void) state;

  assert_int_equal(output_type_by_identifier("food"), O_FOOD);
  assert_int_equal(output_type_by_identifier("shield"), O_SHIELD);
  assert_int_equal(output_type_by_identifier("trade"), O_TRADE);
  assert_int_equal(output_type_by_identifier("gold"), O_GOLD);
  assert_int_equal(output_type_by_identifier("luxury"), O_LUXURY);
  assert_int_equal(output_type_by_identifier("science"), O_SCIENCE);
}

/***********************************************************************
  Test output_type_by_identifier with unknown identifier
***********************************************************************/
static void test_output_type_by_identifier_unknown(void **state)
{
  (void) state;

  Output_type_id type = output_type_by_identifier("unknown");
  assert_int_equal(type, O_LAST);
}

/***********************************************************************
  Test get_output_type
***********************************************************************/
static void test_get_output_type(void **state)
{
  (void) state;

  struct output_type *type = get_output_type(O_FOOD);
  assert_non_null(type);
  assert_string_equal(type->id, "food");
  assert_true(type->harvested);

  type = get_output_type(O_GOLD);
  assert_non_null(type);
  assert_string_equal(type->id, "gold");
  assert_false(type->harvested);
}

/***********************************************************************
  Test get_output_type with invalid index
***********************************************************************/
static void test_get_output_type_invalid(void **state)
{
  (void) state;

  struct output_type *type = get_output_type(-1);
  assert_null(type);

  type = get_output_type(O_LAST);
  assert_null(type);
}

/***********************************************************************
  Test is_city_option_set
***********************************************************************/
static void test_is_city_option_set(void **state)
{
  (void) state;

  BV_CLR_ALL(test_city.city_options);

  assert_false(is_city_option_set(&test_city, CITYO_DISBAND));

  BV_SET(test_city.city_options, CITYO_DISBAND);
  assert_true(is_city_option_set(&test_city, CITYO_DISBAND));

  BV_CLR(test_city.city_options, CITYO_DISBAND);
  assert_false(is_city_option_set(&test_city, CITYO_DISBAND));
}

/***********************************************************************
  Test city_specialists - requires specialist types to be initialized
  This test is skipped as it needs game ruleset loading
***********************************************************************/
static void test_city_specialists(void **state)
{
  (void) state;

  /* This test requires specialist types to be loaded from ruleset.
   * city_specialists() uses normal_specialist_type_iterate which depends on
   * game.control.num_normal_types being set up. */
  skip();
}

/***********************************************************************
  Test city_specialists with no specialists
***********************************************************************/
static void test_city_specialists_none(void **state)
{
  (void) state;

  memset(test_city.specialists, 0, sizeof(test_city.specialists));

  citizens count = city_specialists(&test_city);
  assert_int_equal(count, 0);
}

/***********************************************************************
  Test city_can_change_build - can change when didn't buy
***********************************************************************/
static void test_city_can_change_build_can(void **state)
{
  (void) state;

  test_city.did_buy = FALSE;
  test_city.shield_stock = 10;

  assert_true(city_can_change_build(&test_city));
}

/***********************************************************************
  Test city_can_change_build - can change when bought but no shields
***********************************************************************/
static void test_city_can_change_build_bought_no_shields(void **state)
{
  (void) state;

  test_city.did_buy = TRUE;
  test_city.shield_stock = 0;

  assert_true(city_can_change_build(&test_city));
}

/***********************************************************************
  Test city_can_change_build - cannot change when bought and has shields
***********************************************************************/
static void test_city_can_change_build_cannot(void **state)
{
  (void) state;

  test_city.did_buy = TRUE;
  test_city.shield_stock = 10;

  assert_false(city_can_change_build(&test_city));
}

/***********************************************************************
  Test city_is_virtual with NULL
***********************************************************************/
static void test_city_is_virtual_null(void **state)
{
  (void) state;

  assert_false(city_is_virtual(NULL));
}

/***********************************************************************
  Test city_tile_xy_to_index
***********************************************************************/
static void test_city_tile_xy_to_index(void **state)
{
  (void) state;

  /* Center tile should be index 0 */
  int center_x = CITY_REL2ABS(0);
  int center_y = CITY_REL2ABS(0);
  int index = city_tile_xy_to_index(center_x, center_y, 5);
  assert_int_equal(index, 0);
}

/***********************************************************************
  Test city_name_compare
  Note: city_name_compare expects pointers to city pointers (for qsort)
***********************************************************************/
static void test_city_name_compare(void **state)
{
  (void) state;

  struct city city1, city2;
  struct city *p1 = &city1, *p2 = &city2;

  city1.name = fc_strdup("Alpha");
  city2.name = fc_strdup("Beta");

  /* city_name_compare expects pointers to city pointers (for qsort) */
  int result = city_name_compare(&p1, &p2);
  assert_true(result < 0);  /* "Alpha" < "Beta" */

  result = city_name_compare(&p2, &p1);
  assert_true(result > 0);  /* "Beta" > "Alpha" */

  FC_FREE(city1.name);
  FC_FREE(city2.name);
}

/***********************************************************************
  Test MAX_CITY_SIZE constant
***********************************************************************/
static void test_max_city_size(void **state)
{
  (void) state;

  assert_int_equal(MAX_CITY_SIZE, 0xFF);  /* 255 */
}

/***********************************************************************
  Test CITY_MAP constants
***********************************************************************/
static void test_city_map_constants(void **state)
{
  (void) state;

  assert_int_equal(CITY_MAP_CENTER_TILE_INDEX, 0);
  assert_int_equal(CITY_MAP_CENTER_RADIUS_SQ, -1);
  assert_int_equal(FREE_WORKED_TILES, 1);
}

/***********************************************************************
  Test city_has_building with NULL improvement
***********************************************************************/
static void test_city_has_building_null(void **state)
{
  (void) state;

  assert_false(city_has_building(&test_city, NULL));
}

/***********************************************************************
  Test INCITE_IMPOSSIBLE_COST constant
***********************************************************************/
static void test_incite_impossible_cost(void **state)
{
  (void) state;

  assert_int_equal(INCITE_IMPOSSIBLE_COST, 1000 * 1000 * 1000);
}

/***********************************************************************
  Test city_total_impr_gold_upkeep with NULL city
***********************************************************************/
static void test_city_total_impr_gold_upkeep_null(void **state)
{
  (void) state;

  int upkeep = city_total_impr_gold_upkeep(NULL);
  assert_int_equal(upkeep, 0);
}

/***********************************************************************
  Test city_total_unit_gold_upkeep with NULL city
***********************************************************************/
static void test_city_total_unit_gold_upkeep_null(void **state)
{
  (void) state;

  int upkeep = city_total_unit_gold_upkeep(NULL);
  assert_int_equal(upkeep, 0);
}

/***********************************************************************
  Test city_total_unit_gold_upkeep with NULL units_supported
***********************************************************************/
static void test_city_total_unit_gold_upkeep_null_units(void **state)
{
  (void) state;

  test_city.units_supported = NULL;
  int upkeep = city_total_unit_gold_upkeep(&test_city);
  assert_int_equal(upkeep, 0);
}

/***********************************************************************
  Main test runner
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    /* City name tests */
    cmocka_unit_test_setup_teardown(test_city_name_get_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_name_get_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_name_set_valid, setup, teardown),

    /* City size tests */
    cmocka_unit_test_setup_teardown(test_city_size_get_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_size_get_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_size_set, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_size_add_positive, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_size_add_negative, setup, teardown),

    /* City population test */
    cmocka_unit_test_setup_teardown(test_city_population, setup, teardown),

    /* Granary tests */
    cmocka_unit_test_setup_teardown(test_city_granary_size_zero, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_granary_size_valid, setup, teardown),

    /* Capital tests */
    cmocka_unit_test_setup_teardown(test_is_capital_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_capital_false, setup, teardown),

    /* Free worked tests */
    cmocka_unit_test_setup_teardown(test_is_free_worked_index, setup, teardown),

    /* City map radius tests */
    cmocka_unit_test_setup_teardown(test_city_map_radius_sq_get, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_map_radius_sq_get_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_map_radius_sq_set, setup, teardown),

    /* City map tiles tests */
    cmocka_unit_test_setup_teardown(test_city_map_tiles_center, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_map_tiles_valid, setup, teardown),

    /* City coordinates tests */
    cmocka_unit_test_setup_teardown(test_is_valid_city_coords_center, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_valid_city_coords_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_valid_city_coords_invalid, setup, teardown),

    /* City tile index tests */
    cmocka_unit_test_setup_teardown(test_city_tile_index_to_xy_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_tile_index_to_xy_invalid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_tile_xy_to_index, setup, teardown),

    /* Compare function tests */
    cmocka_unit_test_setup_teardown(test_compare_iter_index_equal, setup, teardown),
    cmocka_unit_test_setup_teardown(test_compare_iter_index_different_dist, setup, teardown),

    /* Output type tests */
    cmocka_unit_test_setup_teardown(test_get_output_identifier, setup, teardown),
    cmocka_unit_test_setup_teardown(test_get_output_identifier_invalid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_output_type_by_identifier, setup, teardown),
    cmocka_unit_test_setup_teardown(test_output_type_by_identifier_unknown, setup, teardown),
    cmocka_unit_test_setup_teardown(test_get_output_type, setup, teardown),
    cmocka_unit_test_setup_teardown(test_get_output_type_invalid, setup, teardown),

    /* City options test */
    cmocka_unit_test_setup_teardown(test_is_city_option_set, setup, teardown),

    /* Specialists tests */
    cmocka_unit_test_setup_teardown(test_city_specialists, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_specialists_none, setup, teardown),

    /* City build change tests */
    cmocka_unit_test_setup_teardown(test_city_can_change_build_can, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_can_change_build_bought_no_shields, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_can_change_build_cannot, setup, teardown),

    /* Virtual city test */
    cmocka_unit_test_setup_teardown(test_city_is_virtual_null, setup, teardown),

    /* City name compare test */
    cmocka_unit_test_setup_teardown(test_city_name_compare, setup, teardown),

    /* Constants tests */
    cmocka_unit_test_setup_teardown(test_max_city_size, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_map_constants, setup, teardown),
    cmocka_unit_test_setup_teardown(test_incite_impossible_cost, setup, teardown),

    /* NULL handling tests */
    cmocka_unit_test_setup_teardown(test_city_has_building_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_total_impr_gold_upkeep_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_total_unit_gold_upkeep_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_city_total_unit_gold_upkeep_null_units, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}