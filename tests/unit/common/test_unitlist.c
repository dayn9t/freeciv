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
#include "unitlist.h"

/* Mock unit structure for testing */
static struct unit mock_units[10];

static int setup(void **state)
{
  (void) state;
  memset(mock_units, 0, sizeof(mock_units));
  for (int i = 0; i < 10; i++) {
    mock_units[i].id = i;
  }
  return 0;
}

static int teardown(void **state)
{
  (void) state;
  return 0;
}

static void test_unit_list_new(void **state)
{
  (void) state;
  struct unit_list *plist;

  plist = unit_list_new();

  assert_non_null(plist);
  assert_int_equal(unit_list_size(plist), 0);

  unit_list_destroy(plist);
}

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

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_unit_list_new, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_append, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_remove, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_size, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_get, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_front_back, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_list_clear, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
