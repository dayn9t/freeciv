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

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "idex.h"
#include "world_object.h"

/* Mock city and unit structures */
static struct city mock_cities[5];
static struct unit mock_units[5];

static int setup(void **state)
{
  (void) state;

  memset(mock_cities, 0, sizeof(mock_cities));
  memset(mock_units, 0, sizeof(mock_units));

  for (int i = 0; i < 5; i++) {
    mock_cities[i].id = i + 1;
    mock_units[i].id = i + 1;
  }

  idex_init(&wld);

  return 0;
}

static int teardown(void **state)
{
  (void) state;

  idex_free(&wld);

  return 0;
}

static void test_idex_register_city(void **state)
{
  (void) state;
  struct city *pcity;

  idex_register_city(&wld, &mock_cities[0]);

  pcity = idex_lookup_city(&wld, mock_cities[0].id);
  assert_ptr_equal(pcity, &mock_cities[0]);
}

static void test_idex_register_unit(void **state)
{
  (void) state;
  struct unit *punit;

  idex_register_unit(&wld, &mock_units[0]);

  punit = idex_lookup_unit(&wld, mock_units[0].id);
  assert_ptr_equal(punit, &mock_units[0]);
}

static void test_idex_lookup_city_not_found(void **state)
{
  (void) state;
  struct city *pcity;

  pcity = idex_lookup_city(&wld, 999);
  assert_null(pcity);
}

static void test_idex_lookup_unit_not_found(void **state)
{
  (void) state;
  struct unit *punit;

  punit = idex_lookup_unit(&wld, 999);
  assert_null(punit);
}

static void test_idex_unregister_city(void **state)
{
  (void) state;
  struct city *pcity;

  idex_register_city(&wld, &mock_cities[0]);
  pcity = idex_lookup_city(&wld, mock_cities[0].id);
  assert_ptr_equal(pcity, &mock_cities[0]);

  idex_unregister_city(&wld, &mock_cities[0]);
  pcity = idex_lookup_city(&wld, mock_cities[0].id);
  assert_null(pcity);
}

static void test_idex_unregister_unit(void **state)
{
  (void) state;
  struct unit *punit;

  idex_register_unit(&wld, &mock_units[0]);
  punit = idex_lookup_unit(&wld, mock_units[0].id);
  assert_ptr_equal(punit, &mock_units[0]);

  idex_unregister_unit(&wld, &mock_units[0]);
  punit = idex_lookup_unit(&wld, mock_units[0].id);
  assert_null(punit);
}

static void test_idex_multiple_cities(void **state)
{
  (void) state;
  struct city *pcity;

  for (int i = 0; i < 5; i++) {
    idex_register_city(&wld, &mock_cities[i]);
  }

  for (int i = 0; i < 5; i++) {
    pcity = idex_lookup_city(&wld, mock_cities[i].id);
    assert_ptr_equal(pcity, &mock_cities[i]);
  }
}

static void test_idex_multiple_units(void **state)
{
  (void) state;
  struct unit *punit;

  for (int i = 0; i < 5; i++) {
    idex_register_unit(&wld, &mock_units[i]);
  }

  for (int i = 0; i < 5; i++) {
    punit = idex_lookup_unit(&wld, mock_units[i].id);
    assert_ptr_equal(punit, &mock_units[i]);
  }
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_idex_register_city, setup, teardown),
    cmocka_unit_test_setup_teardown(test_idex_register_unit, setup, teardown),
    cmocka_unit_test_setup_teardown(test_idex_lookup_city_not_found, setup, teardown),
    cmocka_unit_test_setup_teardown(test_idex_lookup_unit_not_found, setup, teardown),
    cmocka_unit_test_setup_teardown(test_idex_unregister_city, setup, teardown),
    cmocka_unit_test_setup_teardown(test_idex_unregister_unit, setup, teardown),
    cmocka_unit_test_setup_teardown(test_idex_multiple_cities, setup, teardown),
    cmocka_unit_test_setup_teardown(test_idex_multiple_units, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
