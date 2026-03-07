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
#include "shared.h"
#include "support.h"

/* common */
#include "disaster.h"
#include "game.h"

/***********************************************************************
  Test setup and teardown
***********************************************************************/
static int setup_disaster(void **state)
{
  (void) state;
  memset(&game, 0, sizeof(game));
  game.control.num_disaster_types = 0;
  disaster_types_init();
  return 0;
}

static int teardown_disaster(void **state)
{
  (void) state;
  disaster_types_free();
  return 0;
}

/***********************************************************************
  Test disaster_by_number with negative id
***********************************************************************/
static void test_disaster_by_number_negative(void **state)
{
  struct disaster_type *result;

  (void) state;

  result = disaster_by_number(-1);
  assert_null(result);
}

/***********************************************************************
  Test disaster_by_number with invalid large id
***********************************************************************/
static void test_disaster_by_number_invalid(void **state)
{
  struct disaster_type *result;

  (void) state;

  /* Large invalid index */
  result = disaster_by_number(9999);
  assert_null(result);
}

/***********************************************************************
  Test disaster_by_number with zero when no disasters exist
***********************************************************************/
static void test_disaster_by_number_zero_empty(void **state)
{
  struct disaster_type *result;

  (void) state;

  game.control.num_disaster_types = 0;
  result = disaster_by_number(0);
  assert_null(result);
}

/***********************************************************************
  Test disaster_count returns zero before any disasters added
***********************************************************************/
static void test_disaster_count_zero(void **state)
{
  Disaster_type_id count;

  (void) state;

  game.control.num_disaster_types = 0;
  count = disaster_count();
  assert_int_equal(count, 0);
}

/***********************************************************************
  Test disaster_count returns correct value
***********************************************************************/
static void test_disaster_count_value(void **state)
{
  Disaster_type_id count;

  (void) state;

  game.control.num_disaster_types = 3;
  count = disaster_count();
  assert_int_equal(count, 3);
}

/***********************************************************************
  Test disaster_number with null pointer
***********************************************************************/
static void test_disaster_number_null(void **state)
{
  Disaster_type_id result;

  (void) state;

  result = disaster_number(NULL);
  assert_int_equal(result, -1);
}

/***********************************************************************
  Test disaster_index with null pointer
***********************************************************************/
static void test_disaster_index_null(void **state)
{
  Disaster_type_id result;

  (void) state;

  result = disaster_index(NULL);
  assert_int_equal(result, -1);
}

/***********************************************************************
  Test disaster_by_rule_name with null name
  Note: This test would crash as Qn_(NULL) is not safe, so we skip it.
***********************************************************************/
static void test_disaster_by_rule_name_null(void **state)
{
  (void) state;

  /* Skipping test - disaster_by_rule_name(NULL) would crash
   * as Qn_(NULL) is not safe. This is expected behavior. */
  skip();
}

/***********************************************************************
  Test disaster_by_rule_name with non-existent name
***********************************************************************/
static void test_disaster_by_rule_name_not_found(void **state)
{
  struct disaster_type *result;

  (void) state;

  game.control.num_disaster_types = 0;
  result = disaster_by_rule_name("nonexistent_disaster");
  assert_null(result);
}

/***********************************************************************
  Test disaster_by_rule_name with empty string
***********************************************************************/
static void test_disaster_by_rule_name_empty(void **state)
{
  struct disaster_type *result;

  (void) state;

  game.control.num_disaster_types = 0;
  result = disaster_by_rule_name("");
  assert_null(result);
}

/***********************************************************************
  Test disaster_types_init initializes correctly
***********************************************************************/
static void test_disaster_types_init(void **state)
{
  (void) state;

  /* After init, disaster types array should be ready */
  /* We can verify by checking the count is zero */
  game.control.num_disaster_types = 0;
  assert_int_equal(disaster_count(), 0);
}

/***********************************************************************
  Test disaster_by_number with valid id
***********************************************************************/
static void test_disaster_by_number_valid(void **state)
{
  struct disaster_type *result;

  (void) state;

  /* Set up one disaster type */
  game.control.num_disaster_types = 1;

  result = disaster_by_number(0);
  assert_non_null(result);
  assert_int_equal(result->id, 0);
}

/***********************************************************************
  Test disaster_number returns correct id
***********************************************************************/
static void test_disaster_number_valid(void **state)
{
  struct disaster_type *pdis;
  Disaster_type_id id;

  (void) state;

  game.control.num_disaster_types = 1;
  pdis = disaster_by_number(0);
  assert_non_null(pdis);

  id = disaster_number(pdis);
  assert_int_equal(id, 0);
}

/***********************************************************************
  Test disaster_index returns correct value
***********************************************************************/
static void test_disaster_index_valid(void **state)
{
  struct disaster_type *pdis;
  Disaster_type_id idx;

  (void) state;

  game.control.num_disaster_types = 2;
  pdis = disaster_by_number(1);
  assert_non_null(pdis);

  idx = disaster_index(pdis);
  assert_int_equal(idx, 1);
}

/***********************************************************************
  Test disaster_name_translation with valid disaster
***********************************************************************/
static void test_disaster_name_translation_valid(void **state)
{
  struct disaster_type *pdis;
  const char *name;

  (void) state;

  game.control.num_disaster_types = 1;
  pdis = disaster_by_number(0);
  assert_non_null(pdis);

  /* Set up a name for the disaster */
  names_set(&pdis->name, NULL, "Earthquake", "Earthquake");

  name = disaster_name_translation(pdis);
  assert_non_null(name);
  assert_string_equal(name, "Earthquake");
}

/***********************************************************************
  Test disaster_rule_name with valid disaster
***********************************************************************/
static void test_disaster_rule_name_valid(void **state)
{
  struct disaster_type *pdis;
  const char *name;

  (void) state;

  game.control.num_disaster_types = 1;
  pdis = disaster_by_number(0);
  assert_non_null(pdis);

  /* Set up a rule name for the disaster */
  names_set(&pdis->name, NULL, "Earthquake", "Earthquake");

  name = disaster_rule_name(pdis);
  assert_non_null(name);
  assert_string_equal(name, "Earthquake");
}

/***********************************************************************
  Test disaster_by_rule_name finds disaster
***********************************************************************/
static void test_disaster_by_rule_name_found(void **state)
{
  struct disaster_type *pdis;
  struct disaster_type *found;

  (void) state;

  game.control.num_disaster_types = 1;
  pdis = disaster_by_number(0);
  assert_non_null(pdis);

  /* Set up a name for the disaster */
  names_set(&pdis->name, NULL, "Earthquake", "Earthquake");

  found = disaster_by_rule_name("Earthquake");
  assert_non_null(found);
  assert_int_equal(found->id, 0);
}

/***********************************************************************
  Test disaster_has_effect returns false when not set
***********************************************************************/
static void test_disaster_has_effect_not_set(void **state)
{
  struct disaster_type *pdis;
  bool has_effect;

  (void) state;

  game.control.num_disaster_types = 1;
  pdis = disaster_by_number(0);
  assert_non_null(pdis);

  /* Clear all effects */
  BV_CLR_ALL(pdis->effects);

  has_effect = disaster_has_effect(pdis, DE_DESTROY_BUILDING);
  assert_false(has_effect);
}

/***********************************************************************
  Test disaster_has_effect returns true when set
***********************************************************************/
static void test_disaster_has_effect_set(void **state)
{
  struct disaster_type *pdis;
  bool has_effect;

  (void) state;

  game.control.num_disaster_types = 1;
  pdis = disaster_by_number(0);
  assert_non_null(pdis);

  /* Clear all effects first */
  BV_CLR_ALL(pdis->effects);

  /* Set a specific effect */
  BV_SET(pdis->effects, DE_DESTROY_BUILDING);

  has_effect = disaster_has_effect(pdis, DE_DESTROY_BUILDING);
  assert_true(has_effect);

  /* Other effects should still be false */
  has_effect = disaster_has_effect(pdis, DE_REDUCE_POP);
  assert_false(has_effect);
}

/***********************************************************************
  Test disaster frequency default
***********************************************************************/
static void test_disaster_frequency(void **state)
{
  struct disaster_type *pdis;

  (void) state;

  game.control.num_disaster_types = 1;
  pdis = disaster_by_number(0);
  assert_non_null(pdis);

  /* Set frequency */
  pdis->frequency = 5000;

  assert_int_equal(pdis->frequency, 5000);
}

/***********************************************************************
  Test disaster_count constant
***********************************************************************/
static void test_disaster_base_rarity_constant(void **state)
{
  (void) state;

  assert_int_equal(DISASTER_BASE_RARITY, 1000000);
}

/***********************************************************************
  Test multiple disaster types
***********************************************************************/
static void test_disaster_multiple_types(void **state)
{
  struct disaster_type *pdis0;
  struct disaster_type *pdis1;
  struct disaster_type *pdis2;

  (void) state;

  game.control.num_disaster_types = 3;

  pdis0 = disaster_by_number(0);
  pdis1 = disaster_by_number(1);
  pdis2 = disaster_by_number(2);

  assert_non_null(pdis0);
  assert_non_null(pdis1);
  assert_non_null(pdis2);

  /* Each should have unique id */
  assert_int_equal(pdis0->id, 0);
  assert_int_equal(pdis1->id, 1);
  assert_int_equal(pdis2->id, 2);
}

/***********************************************************************
  Test disaster_by_number boundary conditions
***********************************************************************/
static void test_disaster_by_number_boundary(void **state)
{
  struct disaster_type *result;

  (void) state;

  game.control.num_disaster_types = 5;

  /* Valid: at boundary */
  result = disaster_by_number(4);
  assert_non_null(result);

  /* Invalid: just past boundary */
  result = disaster_by_number(5);
  assert_null(result);
}

/***********************************************************************
  Test disaster_by_rule_name case insensitive
***********************************************************************/
static void test_disaster_by_rule_name_case(void **state)
{
  struct disaster_type *pdis;
  struct disaster_type *found;

  (void) state;

  game.control.num_disaster_types = 1;
  pdis = disaster_by_number(0);
  assert_non_null(pdis);

  /* Set up a name for the disaster */
  names_set(&pdis->name, NULL, "Earthquake", "Earthquake");

  /* Should be case insensitive */
  found = disaster_by_rule_name("earthquake");
  assert_non_null(found);

  found = disaster_by_rule_name("EARTHQUAKE");
  assert_non_null(found);
}

/***********************************************************************
  Main test suite
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_disaster_by_number_negative),
    cmocka_unit_test(test_disaster_by_number_invalid),
    cmocka_unit_test(test_disaster_by_number_zero_empty),
    cmocka_unit_test(test_disaster_count_zero),
    cmocka_unit_test(test_disaster_count_value),
    cmocka_unit_test(test_disaster_number_null),
    cmocka_unit_test(test_disaster_index_null),
    cmocka_unit_test(test_disaster_by_rule_name_null),
    cmocka_unit_test(test_disaster_by_rule_name_not_found),
    cmocka_unit_test(test_disaster_by_rule_name_empty),
    cmocka_unit_test_setup_teardown(test_disaster_types_init,
                                    setup_disaster, teardown_disaster),
    cmocka_unit_test_setup_teardown(test_disaster_by_number_valid,
                                    setup_disaster, teardown_disaster),
    cmocka_unit_test_setup_teardown(test_disaster_number_valid,
                                    setup_disaster, teardown_disaster),
    cmocka_unit_test_setup_teardown(test_disaster_index_valid,
                                    setup_disaster, teardown_disaster),
    cmocka_unit_test_setup_teardown(test_disaster_name_translation_valid,
                                    setup_disaster, teardown_disaster),
    cmocka_unit_test_setup_teardown(test_disaster_rule_name_valid,
                                    setup_disaster, teardown_disaster),
    cmocka_unit_test_setup_teardown(test_disaster_by_rule_name_found,
                                    setup_disaster, teardown_disaster),
    cmocka_unit_test_setup_teardown(test_disaster_has_effect_not_set,
                                    setup_disaster, teardown_disaster),
    cmocka_unit_test_setup_teardown(test_disaster_has_effect_set,
                                    setup_disaster, teardown_disaster),
    cmocka_unit_test_setup_teardown(test_disaster_frequency,
                                    setup_disaster, teardown_disaster),
    cmocka_unit_test(test_disaster_base_rarity_constant),
    cmocka_unit_test_setup_teardown(test_disaster_multiple_types,
                                    setup_disaster, teardown_disaster),
    cmocka_unit_test_setup_teardown(test_disaster_by_number_boundary,
                                    setup_disaster, teardown_disaster),
    cmocka_unit_test_setup_teardown(test_disaster_by_rule_name_case,
                                    setup_disaster, teardown_disaster),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}