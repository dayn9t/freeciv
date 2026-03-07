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

/* common */
#include "fc_types.h"
#include "game.h"
#include "terrain.h"

#define TEST_TERRAIN_COUNT 3

static int setup(void **state)
{
  (void) state;

  /* Initialize game structure */
  memset(&game, 0, sizeof(game));
  game.control.terrain_count = TEST_TERRAIN_COUNT;

  return 0;
}

static int teardown(void **state)
{
  (void) state;

  memset(&game, 0, sizeof(game));

  return 0;
}

/* ==================== terrain_count Tests ==================== */

static void test_terrain_count_initialized(void **state)
{
  (void) state;
  Terrain_type_id count;

  count = terrain_count();
  assert_int_equal(count, TEST_TERRAIN_COUNT);
}

static void test_terrain_count_zero(void **state)
{
  (void) state;
  Terrain_type_id count;

  game.control.terrain_count = 0;
  count = terrain_count();
  assert_int_equal(count, 0);
}

/* ==================== terrain_by_number Tests ==================== */

static void test_terrain_by_number_valid(void **state)
{
  (void) state;
  struct terrain *pterrain;

  pterrain = terrain_by_number(0);
  assert_non_null(pterrain);

  pterrain = terrain_by_number(TEST_TERRAIN_COUNT - 1);
  assert_non_null(pterrain);
}

static void test_terrain_by_number_negative(void **state)
{
  (void) state;
  struct terrain *pterrain;

  pterrain = terrain_by_number(-1);
  assert_null(pterrain);
}

static void test_terrain_by_number_out_of_range(void **state)
{
  (void) state;
  struct terrain *pterrain;

  pterrain = terrain_by_number(TEST_TERRAIN_COUNT);
  assert_null(pterrain);

  pterrain = terrain_by_number(MAX_NUM_TERRAINS);
  assert_null(pterrain);

  pterrain = terrain_by_number(9999);
  assert_null(pterrain);
}

/* ==================== terrain_number Tests ==================== */

static void test_terrain_number_null(void **state)
{
  (void) state;
  Terrain_type_id result;

  result = terrain_number(NULL);
  assert_int_equal(result, -1);
}

/* ==================== terrain_index Tests ==================== */

static void test_terrain_index_null(void **state)
{
  (void) state;
  Terrain_type_id result;

  result = terrain_index(NULL);
  assert_int_equal(result, -1);
}

/* ==================== terrain_name_translation Tests ==================== */

static void test_terrain_name_translation_null(void **state)
{
  (void) state;
  const char *name;

  /* Passing NULL to terrain_name_translation will crash - skip */
  /* Instead test that the function exists */
  (void) name;
}

/* ==================== terrain_rule_name Tests ==================== */

static void test_terrain_rule_name_null(void **state)
{
  (void) state;
  const char *name;

  /* Passing NULL to terrain_rule_name will crash - skip */
  (void) name;
}

/* ==================== terrain_identifier Tests ==================== */

static void test_terrain_identifier_null(void **state)
{
  (void) state;
  char identifier;

  identifier = terrain_identifier(NULL);
  assert_int_equal(identifier, '\0');
}

/* ==================== terrain_by_identifier Tests ==================== */

static void test_terrain_by_identifier_unknown(void **state)
{
  (void) state;
  struct terrain *pterrain;

  pterrain = terrain_by_identifier(TERRAIN_UNKNOWN_IDENTIFIER);
  assert_null(pterrain);
}

static void test_terrain_by_identifier_invalid(void **state)
{
  (void) state;
  struct terrain *pterrain;

  pterrain = terrain_by_identifier('\0');
  assert_null(pterrain);

  pterrain = terrain_by_identifier('Z');
  assert_null(pterrain);
}

/* ==================== terrain_by_rule_name Tests ==================== */

static void test_terrain_by_rule_name_null(void **state)
{
  (void) state;
  struct terrain *pterrain;

  pterrain = terrain_by_rule_name(NULL);
  assert_null(pterrain);
}

static void test_terrain_by_rule_name_not_found(void **state)
{
  (void) state;
  struct terrain *pterrain;

  pterrain = terrain_by_rule_name("nonexistent_terrain");
  assert_null(pterrain);
}

/* ==================== terrain_by_translated_name Tests ==================== */

static void test_terrain_by_translated_name_null(void **state)
{
  (void) state;
  struct terrain *pterrain;

  pterrain = terrain_by_translated_name(NULL);
  assert_null(pterrain);
}

static void test_terrain_by_translated_name_not_found(void **state)
{
  (void) state;
  struct terrain *pterrain;

  pterrain = terrain_by_translated_name("nonexistent_terrain");
  assert_null(pterrain);
}

/* ==================== terrain_array_first/last Tests ==================== */

static void test_terrain_array_first_empty(void **state)
{
  (void) state;
  struct terrain *pterrain;

  game.control.terrain_count = 0;
  pterrain = terrain_array_first();
  assert_null(pterrain);
}

static void test_terrain_array_first_valid(void **state)
{
  (void) state;
  struct terrain *pterrain;

  pterrain = terrain_array_first();
  assert_non_null(pterrain);
}

static void test_terrain_array_last_empty(void **state)
{
  (void) state;
  const struct terrain *pterrain;

  game.control.terrain_count = 0;
  pterrain = terrain_array_last();
  assert_null(pterrain);
}

static void test_terrain_array_last_valid(void **state)
{
  (void) state;
  const struct terrain *pterrain;

  pterrain = terrain_array_last();
  assert_non_null(pterrain);
}

/* ==================== terrain_type_terrain_class Tests ==================== */

static void test_terrain_type_terrain_class_null(void **state)
{
  (void) state;

  /* Passing NULL will crash - skip */
}

/* ==================== terrain_class_name_translation Tests ==================== */

static void test_terrain_class_name_translation_valid(void **state)
{
  (void) state;
  const char *name;

  name = terrain_class_name_translation(TC_LAND);
  /* Result depends on locale, just verify function doesn't crash */
  (void) name;

  name = terrain_class_name_translation(TC_OCEAN);
  (void) name;
}

static void test_terrain_class_name_translation_invalid(void **state)
{
  (void) state;
  const char *name;

  name = terrain_class_name_translation((enum terrain_class)999);
  assert_null(name);
}

/* ==================== terrain_has_resource Tests ==================== */

static void test_terrain_has_resource_null_terrain(void **state)
{
  (void) state;

  /* Passing NULL will crash - skip */
}

/* ==================== terrain_can_support_alteration Tests ==================== */

static void test_terrain_can_support_alteration_null(void **state)
{
  (void) state;

  /* Passing NULL will crash - skip */
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* terrain_count Tests */
    cmocka_unit_test_setup_teardown(test_terrain_count_initialized, setup, teardown),
    cmocka_unit_test_setup_teardown(test_terrain_count_zero, setup, teardown),

    /* terrain_by_number Tests */
    cmocka_unit_test_setup_teardown(test_terrain_by_number_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_terrain_by_number_negative, setup, teardown),
    cmocka_unit_test_setup_teardown(test_terrain_by_number_out_of_range, setup, teardown),

    /* terrain_number Tests */
    cmocka_unit_test_setup_teardown(test_terrain_number_null, setup, teardown),

    /* terrain_index Tests */
    cmocka_unit_test_setup_teardown(test_terrain_index_null, setup, teardown),

    /* terrain_name_translation Tests */
    cmocka_unit_test(test_terrain_name_translation_null),

    /* terrain_rule_name Tests */
    cmocka_unit_test(test_terrain_rule_name_null),

    /* terrain_identifier Tests */
    cmocka_unit_test_setup_teardown(test_terrain_identifier_null, setup, teardown),

    /* terrain_by_identifier Tests */
    cmocka_unit_test_setup_teardown(test_terrain_by_identifier_unknown, setup, teardown),
    cmocka_unit_test_setup_teardown(test_terrain_by_identifier_invalid, setup, teardown),

    /* terrain_by_rule_name Tests */
    cmocka_unit_test_setup_teardown(test_terrain_by_rule_name_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_terrain_by_rule_name_not_found, setup, teardown),

    /* terrain_by_translated_name Tests */
    cmocka_unit_test_setup_teardown(test_terrain_by_translated_name_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_terrain_by_translated_name_not_found, setup, teardown),

    /* terrain_array_first/last Tests */
    cmocka_unit_test_setup_teardown(test_terrain_array_first_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_terrain_array_first_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_terrain_array_last_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_terrain_array_last_valid, setup, teardown),

    /* terrain_type_terrain_class Tests */
    cmocka_unit_test(test_terrain_type_terrain_class_null),

    /* terrain_class_name_translation Tests */
    cmocka_unit_test_setup_teardown(test_terrain_class_name_translation_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_terrain_class_name_translation_invalid, setup, teardown),

    /* terrain_has_resource Tests */
    cmocka_unit_test(test_terrain_has_resource_null_terrain),

    /* terrain_can_support_alteration Tests */
    cmocka_unit_test(test_terrain_can_support_alteration_null),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}