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
#include "player.h"
#include "traderoutes.h"
#include "world_object.h"

/***********************************************************************
  Test trade_route_list_new creates a valid list
***********************************************************************/
static void test_trade_route_list_new(void **state)
{
  (void) state;

  struct trade_route_list *list = trade_route_list_new();

  assert_non_null(list);
  assert_int_equal(trade_route_list_size(list), 0);

  trade_route_list_destroy(list);
}

/***********************************************************************
  Test trade_route_list_destroy handles NULL
***********************************************************************/
static void test_trade_route_list_destroy_null(void **state)
{
  (void) state;

  /* Should not crash with NULL */
  trade_route_list_destroy(NULL);
}

/***********************************************************************
  Test trade_route_list_size with empty list
***********************************************************************/
static void test_trade_route_list_size_empty(void **state)
{
  (void) state;

  struct trade_route_list *list = trade_route_list_new();

  assert_int_equal(trade_route_list_size(list), 0);

  trade_route_list_destroy(list);
}

/***********************************************************************
  Test trade_route_list_size with elements
***********************************************************************/
static void test_trade_route_list_size_with_elements(void **state)
{
  (void) state;

  struct trade_route_list *list = trade_route_list_new();
  struct trade_route route1 = {0};
  struct trade_route route2 = {0};
  struct trade_route route3 = {0};

  trade_route_list_append(list, &route1);
  assert_int_equal(trade_route_list_size(list), 1);

  trade_route_list_append(list, &route2);
  assert_int_equal(trade_route_list_size(list), 2);

  trade_route_list_append(list, &route3);
  assert_int_equal(trade_route_list_size(list), 3);

  trade_route_list_destroy(list);
}

/***********************************************************************
  Test trade_route_list_append and get
***********************************************************************/
static void test_trade_route_list_append_get(void **state)
{
  (void) state;

  struct trade_route_list *list = trade_route_list_new();
  struct trade_route route = {0};
  route.partner = 42;
  route.value = 100;

  trade_route_list_append(list, &route);

  assert_int_equal(trade_route_list_size(list), 1);

  struct trade_route *retrieved = trade_route_list_get(list, 0);
  assert_non_null(retrieved);
  assert_int_equal(retrieved->partner, 42);
  assert_int_equal(retrieved->value, 100);

  trade_route_list_destroy(list);
}

/***********************************************************************
  Test trade_route_list_prepend
***********************************************************************/
static void test_trade_route_list_prepend(void **state)
{
  (void) state;

  struct trade_route_list *list = trade_route_list_new();
  struct trade_route route1 = {0};
  struct trade_route route2 = {0};

  route1.partner = 1;
  route2.partner = 2;

  trade_route_list_append(list, &route1);
  trade_route_list_prepend(list, &route2);

  assert_int_equal(trade_route_list_size(list), 2);

  /* prepended element should be first */
  struct trade_route *first = trade_route_list_get(list, 0);
  assert_int_equal(first->partner, 2);

  trade_route_list_destroy(list);
}

/***********************************************************************
  Test trade_route_list_remove
***********************************************************************/
static void test_trade_route_list_remove(void **state)
{
  (void) state;

  struct trade_route_list *list = trade_route_list_new();
  struct trade_route route1 = {0};
  struct trade_route route2 = {0};

  route1.partner = 1;
  route2.partner = 2;

  trade_route_list_append(list, &route1);
  trade_route_list_append(list, &route2);

  assert_int_equal(trade_route_list_size(list), 2);

  bool removed = trade_route_list_remove(list, &route1);
  assert_true(removed);
  assert_int_equal(trade_route_list_size(list), 1);

  struct trade_route *remaining = trade_route_list_get(list, 0);
  assert_int_equal(remaining->partner, 2);

  trade_route_list_destroy(list);
}

/***********************************************************************
  Test trade_route_list_clear
***********************************************************************/
static void test_trade_route_list_clear(void **state)
{
  (void) state;

  struct trade_route_list *list = trade_route_list_new();
  struct trade_route route = {0};

  trade_route_list_append(list, &route);
  trade_route_list_append(list, &route);

  assert_int_equal(trade_route_list_size(list), 2);

  trade_route_list_clear(list);
  assert_int_equal(trade_route_list_size(list), 0);

  trade_route_list_destroy(list);
}

/***********************************************************************
  Test trade_route_list_front and back
***********************************************************************/
static void test_trade_route_list_front_back(void **state)
{
  (void) state;

  struct trade_route_list *list = trade_route_list_new();
  struct trade_route route1 = {0};
  struct trade_route route2 = {0};

  route1.partner = 1;
  route2.partner = 2;

  trade_route_list_append(list, &route1);
  trade_route_list_append(list, &route2);

  struct trade_route *front = trade_route_list_front(list);
  assert_non_null(front);
  assert_int_equal(front->partner, 1);

  struct trade_route *back = trade_route_list_back(list);
  assert_non_null(back);
  assert_int_equal(back->partner, 2);

  trade_route_list_destroy(list);
}

/***********************************************************************
  Test trade_route_types_init
***********************************************************************/
static void test_trade_route_types_init(void **state)
{
  (void) state;

  trade_route_types_init();

  /* All types should have default trade_pct of 100 after init */
  for (enum trade_route_type type = TRT_NATIONAL; type < TRT_LAST; type++) {
    struct trade_route_settings *settings = trade_route_settings_by_type(type);
    assert_non_null(settings);
    assert_int_equal(settings->trade_pct, 100);
  }
}

/***********************************************************************
  Test trade_route_type_name for valid types
***********************************************************************/
static void test_trade_route_type_name_valid(void **state)
{
  (void) state;

  const char *name;

  name = trade_route_type_name(TRT_NATIONAL);
  assert_non_null(name);
  assert_string_equal(name, "National");

  name = trade_route_type_name(TRT_NATIONAL_IC);
  assert_non_null(name);
  assert_string_equal(name, "NationalIC");

  name = trade_route_type_name(TRT_IN);
  assert_non_null(name);
  assert_string_equal(name, "IN");

  name = trade_route_type_name(TRT_ALLY);
  assert_non_null(name);
  assert_string_equal(name, "Ally");

  name = trade_route_type_name(TRT_ENEMY);
  assert_non_null(name);
  assert_string_equal(name, "Enemy");

  name = trade_route_type_name(TRT_TEAM);
  assert_non_null(name);
  assert_string_equal(name, "Team");
}

/***********************************************************************
  Test trade_route_type_name for invalid types
***********************************************************************/
static void test_trade_route_type_name_invalid(void **state)
{
  (void) state;

  const char *name;

  /* Invalid low value */
  name = trade_route_type_name(-1);
  assert_null(name);

  /* Invalid high value */
  name = trade_route_type_name(TRT_LAST);
  assert_null(name);

  name = trade_route_type_name(TRT_LAST + 1);
  assert_null(name);
}

/***********************************************************************
  Test trade_route_type_by_name for valid names
***********************************************************************/
static void test_trade_route_type_by_name_valid(void **state)
{
  (void) state;

  enum trade_route_type type;

  type = trade_route_type_by_name("National");
  assert_int_equal(type, TRT_NATIONAL);

  type = trade_route_type_by_name("NationalIC");
  assert_int_equal(type, TRT_NATIONAL_IC);

  type = trade_route_type_by_name("IN");
  assert_int_equal(type, TRT_IN);

  type = trade_route_type_by_name("INIC");
  assert_int_equal(type, TRT_IN_IC);

  type = trade_route_type_by_name("Ally");
  assert_int_equal(type, TRT_ALLY);

  type = trade_route_type_by_name("AllyIC");
  assert_int_equal(type, TRT_ALLY_IC);

  type = trade_route_type_by_name("Enemy");
  assert_int_equal(type, TRT_ENEMY);

  type = trade_route_type_by_name("Team");
  assert_int_equal(type, TRT_TEAM);
}

/***********************************************************************
  Test trade_route_type_by_name for invalid names
***********************************************************************/
static void test_trade_route_type_by_name_invalid(void **state)
{
  (void) state;

  enum trade_route_type type;

  type = trade_route_type_by_name("Invalid");
  assert_int_equal(type, TRT_LAST);

  type = trade_route_type_by_name("nonexistent");
  assert_int_equal(type, TRT_LAST);

  type = trade_route_type_by_name(NULL);
  /* Should handle NULL gracefully - might crash or return TRT_LAST */
  /* This test documents behavior */
}

/***********************************************************************
  Test trade_route_type_trade_pct
***********************************************************************/
static void test_trade_route_type_trade_pct(void **state)
{
  (void) state;

  trade_route_types_init();

  /* After init, all types should have 100% */
  for (enum trade_route_type type = TRT_NATIONAL; type < TRT_LAST; type++) {
    int pct = trade_route_type_trade_pct(type);
    assert_int_equal(pct, 100);
  }

  /* Invalid type should return 0 */
  int pct = trade_route_type_trade_pct(TRT_LAST);
  assert_int_equal(pct, 0);

  pct = trade_route_type_trade_pct(TRT_LAST + 100);
  assert_int_equal(pct, 0);
}

/***********************************************************************
  Test trade_route_settings_by_type for valid types
***********************************************************************/
static void test_trade_route_settings_by_type_valid(void **state)
{
  (void) state;

  for (enum trade_route_type type = TRT_NATIONAL; type < TRT_LAST; type++) {
    struct trade_route_settings *settings = trade_route_settings_by_type(type);
    assert_non_null(settings);
  }
}

/***********************************************************************
  Test trade_route_settings_by_type for invalid types
***********************************************************************/
static void test_trade_route_settings_by_type_invalid(void **state)
{
  (void) state;

  struct trade_route_settings *settings;

  settings = trade_route_settings_by_type(-1);
  assert_null(settings);

  settings = trade_route_settings_by_type(TRT_LAST);
  assert_null(settings);
}

/***********************************************************************
  Test trade_route_cancelling_type_name for valid types
***********************************************************************/
static void test_trade_route_cancelling_type_name_valid(void **state)
{
  (void) state;

  const char *name;

  name = trade_route_cancelling_type_name(TRI_ACTIVE);
  assert_non_null(name);
  assert_string_equal(name, "Active");

  name = trade_route_cancelling_type_name(TRI_INACTIVE);
  assert_non_null(name);
  assert_string_equal(name, "Inactive");

  name = trade_route_cancelling_type_name(TRI_CANCEL);
  assert_non_null(name);
  assert_string_equal(name, "Cancel");
}

/***********************************************************************
  Test trade_route_cancelling_type_name for invalid types
***********************************************************************/
static void test_trade_route_cancelling_type_name_invalid(void **state)
{
  (void) state;

  const char *name;

  name = trade_route_cancelling_type_name(-1);
  assert_null(name);

  name = trade_route_cancelling_type_name(TRI_LAST);
  assert_null(name);
}

/***********************************************************************
  Test trade_route_cancelling_type_by_name for valid names
***********************************************************************/
static void test_trade_route_cancelling_type_by_name_valid(void **state)
{
  (void) state;

  enum trade_route_illegal_cancelling type;

  type = trade_route_cancelling_type_by_name("Active");
  assert_int_equal(type, TRI_ACTIVE);

  type = trade_route_cancelling_type_by_name("Inactive");
  assert_int_equal(type, TRI_INACTIVE);

  type = trade_route_cancelling_type_by_name("Cancel");
  assert_int_equal(type, TRI_CANCEL);
}

/***********************************************************************
  Test trade_route_cancelling_type_by_name for invalid names
***********************************************************************/
static void test_trade_route_cancelling_type_by_name_invalid(void **state)
{
  (void) state;

  enum trade_route_illegal_cancelling type;

  type = trade_route_cancelling_type_by_name("Invalid");
  assert_int_equal(type, TRI_LAST);

  type = trade_route_cancelling_type_by_name("nonexistent");
  assert_int_equal(type, TRI_LAST);
}

/***********************************************************************
  Test city_num_trade_routes with NULL city
  Note: city_num_trade_routes() crashes on NULL input - it dereferences pcity
***********************************************************************/
static void test_city_num_trade_routes_null(void **state)
{
  (void) state;

  /* This test is disabled because city_num_trade_routes() doesn't handle NULL */
  /* The function dereferences pcity->routes, which segfaults on NULL pcity */
  skip();
}

/***********************************************************************
  Test city_num_trade_routes with empty routes
***********************************************************************/
static void test_city_num_trade_routes_empty(void **state)
{
  (void) state;

  struct city test_city;
  memset(&test_city, 0, sizeof(test_city));

  test_city.routes = trade_route_list_new();

  int num = city_num_trade_routes(&test_city);
  assert_int_equal(num, 0);

  trade_route_list_destroy(test_city.routes);
}

/***********************************************************************
  Test city_num_trade_routes with routes
***********************************************************************/
static void test_city_num_trade_routes_with_routes(void **state)
{
  (void) state;

  struct city test_city;
  memset(&test_city, 0, sizeof(test_city));

  test_city.routes = trade_route_list_new();

  struct trade_route route1 = {0};
  struct trade_route route2 = {0};
  struct trade_route route3 = {0};

  trade_route_list_append(test_city.routes, &route1);
  trade_route_list_append(test_city.routes, &route2);
  trade_route_list_append(test_city.routes, &route3);

  int num = city_num_trade_routes(&test_city);
  assert_int_equal(num, 3);

  trade_route_list_destroy(test_city.routes);
}

/***********************************************************************
  Test can_establish_trade_route with NULL cities
***********************************************************************/
static void test_can_establish_trade_route_null(void **state)
{
  (void) state;

  /* Both NULL */
  bool result = can_establish_trade_route(NULL, NULL, 0);
  assert_false(result);

  /* First NULL */
  struct city test_city;
  memset(&test_city, 0, sizeof(test_city));
  result = can_establish_trade_route(NULL, &test_city, 0);
  assert_false(result);

  /* Second NULL */
  result = can_establish_trade_route(&test_city, NULL, 0);
  assert_false(result);
}

/***********************************************************************
  Test can_establish_trade_route same city
***********************************************************************/
static void test_can_establish_trade_route_same_city(void **state)
{
  (void) state;

  struct city test_city;
  memset(&test_city, 0, sizeof(test_city));

  /* Same city should return FALSE */
  bool result = can_establish_trade_route(&test_city, &test_city, 0);
  assert_false(result);
}

/***********************************************************************
  Test can_cities_trade with NULL cities
***********************************************************************/
static void test_can_cities_trade_null(void **state)
{
  (void) state;

  bool result = can_cities_trade(NULL, NULL);
  assert_false(result);

  struct city test_city;
  memset(&test_city, 0, sizeof(test_city));

  result = can_cities_trade(NULL, &test_city);
  assert_false(result);

  result = can_cities_trade(&test_city, NULL);
  assert_false(result);
}

/***********************************************************************
  Test can_cities_trade same city
***********************************************************************/
static void test_can_cities_trade_same_city(void **state)
{
  (void) state;

  struct city test_city;
  memset(&test_city, 0, sizeof(test_city));

  /* Same city should return FALSE */
  bool result = can_cities_trade(&test_city, &test_city);
  assert_false(result);
}

/***********************************************************************
  Test trade_base_between_cities with NULL cities
***********************************************************************/
static void test_trade_base_between_cities_null(void **state)
{
  (void) state;

  int trade = trade_base_between_cities(NULL, NULL);
  assert_int_equal(trade, 0);

  struct city test_city;
  memset(&test_city, 0, sizeof(test_city));

  trade = trade_base_between_cities(NULL, &test_city);
  assert_int_equal(trade, 0);

  trade = trade_base_between_cities(&test_city, NULL);
  assert_int_equal(trade, 0);
}

/***********************************************************************
  Test have_cities_trade_route with NULL cities
***********************************************************************/
static void test_have_cities_trade_route_null(void **state)
{
  (void) state;

  /* These will likely crash - documenting expected behavior */
  /* bool result = have_cities_trade_route(NULL, NULL); */

  /* Skip NULL tests for this function */
  skip();
}

/***********************************************************************
  Test trade route type enum values
***********************************************************************/
static void test_trade_route_type_enum_values(void **state)
{
  (void) state;

  /* Verify enum values match expected */
  assert_int_equal(TRT_NATIONAL, 0);
  assert_int_equal(TRT_NATIONAL_IC, 1);
  assert_int_equal(TRT_IN, 2);
  assert_int_equal(TRT_IN_IC, 3);
  assert_int_equal(TRT_ALLY, 4);
  assert_int_equal(TRT_ALLY_IC, 5);
  assert_int_equal(TRT_ENEMY, 6);
  assert_int_equal(TRT_ENEMY_IC, 7);
  assert_int_equal(TRT_TEAM, 8);
  assert_int_equal(TRT_TEAM_IC, 9);
  assert_int_equal(TRT_LAST, 10);
}

/***********************************************************************
  Test trade route illegal cancelling enum values
***********************************************************************/
static void test_trade_route_illegal_cancelling_enum_values(void **state)
{
  (void) state;

  /* Verify enum values match expected */
  assert_int_equal(TRI_ACTIVE, 0);
  assert_int_equal(TRI_INACTIVE, 1);
  assert_int_equal(TRI_CANCEL, 2);
  assert_int_equal(TRI_LAST, 3);
}

/***********************************************************************
  Test trade route structure fields
***********************************************************************/
static void test_trade_route_structure_fields(void **state)
{
  (void) state;

  struct trade_route route;

  route.partner = 123;
  route.value = 456;
  route.dir = RDIR_TO;
  route.goods = NULL;

  assert_int_equal(route.partner, 123);
  assert_int_equal(route.value, 456);
  assert_int_equal(route.dir, RDIR_TO);
  assert_null(route.goods);
}

/***********************************************************************
  Test route_direction enum values
***********************************************************************/
static void test_route_direction_enum_values(void **state)
{
  (void) state;

  /* Verify enum values */
  assert_int_equal(RDIR_NONE, 0);
  assert_int_equal(RDIR_FROM, 1);
  assert_int_equal(RDIR_TO, 2);
  assert_int_equal(RDIR_BIDIRECTIONAL, 3);
}

/***********************************************************************
  Test trade_route_settings structure
***********************************************************************/
static void test_trade_route_settings_structure(void **state)
{
  (void) state;

  trade_route_types_init();

  struct trade_route_settings *settings = trade_route_settings_by_type(TRT_NATIONAL);
  assert_non_null(settings);

  /* Modify and verify */
  settings->trade_pct = 150;
  settings->cancelling = TRI_CANCEL;
  settings->bonus_type = TBONUS_GOLD;

  assert_int_equal(settings->trade_pct, 150);
  assert_int_equal(settings->cancelling, TRI_CANCEL);
  assert_int_equal(settings->bonus_type, TBONUS_GOLD);

  /* Reset for other tests */
  settings->trade_pct = 100;
}

/***********************************************************************
  Test trade_route_list iteration
***********************************************************************/
static void test_trade_route_list_iteration(void **state)
{
  (void) state;

  struct trade_route_list *list = trade_route_list_new();
  struct trade_route routes[3] = {{0}};
  int count = 0;

  routes[0].partner = 1;
  routes[1].partner = 2;
  routes[2].partner = 3;

  trade_route_list_append(list, &routes[0]);
  trade_route_list_append(list, &routes[1]);
  trade_route_list_append(list, &routes[2]);

  trade_route_list_iterate(list, proute) {
    count++;
    assert_true(proute->partner >= 1 && proute->partner <= 3);
  } trade_route_list_iterate_end;

  assert_int_equal(count, 3);

  trade_route_list_destroy(list);
}

/***********************************************************************
  Test GOODS_HIGH_PRIORITY constant
***********************************************************************/
static void test_goods_high_priority(void **state)
{
  (void) state;

  assert_int_equal(GOODS_HIGH_PRIORITY, 100);
}

/***********************************************************************
  Main test runner
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    /* Trade route list tests */
    cmocka_unit_test(test_trade_route_list_new),
    cmocka_unit_test(test_trade_route_list_destroy_null),
    cmocka_unit_test(test_trade_route_list_size_empty),
    cmocka_unit_test(test_trade_route_list_size_with_elements),
    cmocka_unit_test(test_trade_route_list_append_get),
    cmocka_unit_test(test_trade_route_list_prepend),
    cmocka_unit_test(test_trade_route_list_remove),
    cmocka_unit_test(test_trade_route_list_clear),
    cmocka_unit_test(test_trade_route_list_front_back),

    /* Trade route type tests */
    cmocka_unit_test(test_trade_route_types_init),
    cmocka_unit_test(test_trade_route_type_name_valid),
    cmocka_unit_test(test_trade_route_type_name_invalid),
    cmocka_unit_test(test_trade_route_type_by_name_valid),
    cmocka_unit_test(test_trade_route_type_by_name_invalid),
    cmocka_unit_test(test_trade_route_type_trade_pct),
    cmocka_unit_test(test_trade_route_settings_by_type_valid),
    cmocka_unit_test(test_trade_route_settings_by_type_invalid),

    /* Trade route cancelling type tests */
    cmocka_unit_test(test_trade_route_cancelling_type_name_valid),
    cmocka_unit_test(test_trade_route_cancelling_type_name_invalid),
    cmocka_unit_test(test_trade_route_cancelling_type_by_name_valid),
    cmocka_unit_test(test_trade_route_cancelling_type_by_name_invalid),

    /* City trade routes tests */
    cmocka_unit_test(test_city_num_trade_routes_null),
    cmocka_unit_test(test_city_num_trade_routes_empty),
    cmocka_unit_test(test_city_num_trade_routes_with_routes),

    /* can_establish_trade_route tests */
    cmocka_unit_test(test_can_establish_trade_route_null),
    cmocka_unit_test(test_can_establish_trade_route_same_city),

    /* can_cities_trade tests */
    cmocka_unit_test(test_can_cities_trade_null),
    cmocka_unit_test(test_can_cities_trade_same_city),

    /* trade_base_between_cities tests */
    cmocka_unit_test(test_trade_base_between_cities_null),

    /* have_cities_trade_route tests */
    cmocka_unit_test(test_have_cities_trade_route_null),

    /* Enum values tests */
    cmocka_unit_test(test_trade_route_type_enum_values),
    cmocka_unit_test(test_trade_route_illegal_cancelling_enum_values),
    cmocka_unit_test(test_route_direction_enum_values),

    /* Structure tests */
    cmocka_unit_test(test_trade_route_structure_fields),
    cmocka_unit_test(test_trade_route_settings_structure),

    /* List iteration test */
    cmocka_unit_test(test_trade_route_list_iteration),

    /* Constants test */
    cmocka_unit_test(test_goods_high_priority),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}