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
#include "extras.h"
#include "fc_types.h"
#include "game.h"
#include "name_translation.h"
#include "tiledef.h"

static int setup(void **state)
{
  (void) state;

  /* Initialize game structure for testing */
  memset(&game, 0, sizeof(game));
  game.control.num_tiledef_types = 2;

  /* Initialize tiledefs */
  tiledefs_init();

  return 0;
}

static int teardown(void **state)
{
  (void) state;

  /* Free tiledefs */
  tiledefs_free();
  game.control.num_tiledef_types = 0;

  return 0;
}

static void test_tiledef_by_number_valid(void **state)
{
  struct tiledef *td;

  (void) state;

  /* Test valid ID 0 */
  td = tiledef_by_number(0);
  assert_non_null(td);
  assert_int_equal(td->id, 0);

  /* Test valid ID 1 */
  td = tiledef_by_number(1);
  assert_non_null(td);
  assert_int_equal(td->id, 1);
}

static void test_tiledef_by_number_negative(void **state)
{
  struct tiledef *td;

  (void) state;

  /* Negative ID should return NULL */
  td = tiledef_by_number(-1);
  assert_null(td);
}

static void test_tiledef_by_number_out_of_range(void **state)
{
  struct tiledef *td;

  (void) state;

  /* ID >= MAX_TILEDEFS should return NULL */
  td = tiledef_by_number(MAX_TILEDEFS);
  assert_null(td);

  /* Large invalid ID */
  td = tiledef_by_number(9999);
  assert_null(td);
}

static void test_tiledef_number(void **state)
{
  struct tiledef *td;
  int result;

  (void) state;

  td = tiledef_by_number(0);
  assert_non_null(td);

  result = tiledef_number(td);
  assert_int_equal(result, 0);

  td = tiledef_by_number(3);
  assert_non_null(td);

  result = tiledef_number(td);
  assert_int_equal(result, 3);
}

static void test_tiledef_number_null(void **state)
{
  int result;

  (void) state;

  /* NULL tiledef should return -1 */
  result = tiledef_number(NULL);
  assert_int_equal(result, -1);
}

static void test_tiledef_index(void **state)
{
  struct tiledef *td;

  (void) state;

  td = tiledef_by_number(0);
  assert_non_null(td);
  assert_int_equal(tiledef_index(td), 0);

  td = tiledef_by_number(5);
  assert_non_null(td);
  assert_int_equal(tiledef_index(td), 5);
}

static void test_tiledef_count(void **state)
{
  int count;

  (void) state;

  count = tiledef_count();
  assert_int_equal(count, 2);  /* Set in setup */
}

static void test_tiledef_name_translation(void **state)
{
  struct tiledef *td;
  const char *name;

  (void) state;

  td = tiledef_by_number(0);
  assert_non_null(td);

  /* Set a name */
  names_set(&td->name, NULL, "TestTiledef", NULL);

  name = tiledef_name_translation(td);
  assert_non_null(name);
}

static void test_tiledef_rule_name(void **state)
{
  struct tiledef *td;
  const char *name;

  (void) state;

  td = tiledef_by_number(0);
  assert_non_null(td);

  /* Set a name */
  names_set(&td->name, NULL, "TestTiledef", "test_tiledef");

  name = tiledef_rule_name(td);
  assert_non_null(name);
  assert_string_equal(name, "test_tiledef");
}

static void test_tiledef_by_rule_name(void **state)
{
  struct tiledef *td, *found;

  (void) state;

  td = tiledef_by_number(0);
  assert_non_null(td);

  /* Set a name for the tiledef */
  names_set(&td->name, NULL, "TestTiledef", "test_td");

  /* Find by rule name */
  found = tiledef_by_rule_name("test_td");
  assert_non_null(found);
  assert_int_equal(found->id, 0);

  /* Non-existent name should return NULL */
  found = tiledef_by_rule_name("nonexistent");
  assert_null(found);

  /* NULL name should return NULL */
  found = tiledef_by_rule_name(NULL);
  assert_null(found);
}

static void test_tiledef_by_translated_name(void **state)
{
  struct tiledef *td, *found;

  (void) state;

  /* Set names for all tiledefs to avoid NULL pointer in iteration */
  td = tiledef_by_number(0);
  assert_non_null(td);
  names_set(&td->name, NULL, "MyTiledef", NULL);

  td = tiledef_by_number(1);
  assert_non_null(td);
  names_set(&td->name, NULL, "OtherTiledef", NULL);

  /* Find by translated name */
  td = tiledef_by_number(0);
  found = tiledef_by_translated_name("MyTiledef");
  assert_non_null(found);
  assert_int_equal(found->id, 0);

  /* Non-existent name should return NULL */
  found = tiledef_by_translated_name("nonexistent");
  assert_null(found);
}

static void test_tiledef_extras_list(void **state)
{
  struct tiledef *td;

  (void) state;

  td = tiledef_by_number(0);
  assert_non_null(td);

  /* Extras list should be initialized */
  assert_non_null(td->extras);
}

static void test_tiledef_iterate(void **state)
{
  int count = 0;

  (void) state;

  tiledef_iterate(td) {
    assert_non_null(td);
    count++;
  } tiledef_iterate_end;

  /* Should iterate over game.control.num_tiledef_types items */
  assert_int_equal(count, 2);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_tiledef_by_number_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tiledef_by_number_negative, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tiledef_by_number_out_of_range, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tiledef_number, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tiledef_number_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tiledef_index, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tiledef_count, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tiledef_name_translation, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tiledef_rule_name, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tiledef_by_rule_name, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tiledef_by_translated_name, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tiledef_extras_list, setup, teardown),
    cmocka_unit_test_setup_teardown(test_tiledef_iterate, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}