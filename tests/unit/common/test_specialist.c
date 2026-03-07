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
#include "game.h"
#include "specialist.h"

/***********************************************************************
  Setup function - initialize minimal game state
***********************************************************************/
static int setup(void **state)
{
  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));

  return 0;
}

/***********************************************************************
  Teardown function - cleanup
***********************************************************************/
static int teardown(void **state)
{
  (void) state;
  return 0;
}

/***********************************************************************
  Test specialist_count with no specialist types
***********************************************************************/
static void test_specialist_count_zero(void **state)
{
  (void) state;

  game.control.num_specialist_types = 0;
  assert_int_equal(specialist_count(), 0);
}

/***********************************************************************
  Test specialist_count with specialist types
***********************************************************************/
static void test_specialist_count_positive(void **state)
{
  (void) state;

  game.control.num_specialist_types = 3;
  assert_int_equal(specialist_count(), 3);

  game.control.num_specialist_types = 5;
  assert_int_equal(specialist_count(), 5);
}

/***********************************************************************
  Test normal_specialist_count with no normal specialists
***********************************************************************/
static void test_normal_specialist_count_zero(void **state)
{
  (void) state;

  game.control.num_normal_specialists = 0;
  assert_int_equal(normal_specialist_count(), 0);
}

/***********************************************************************
  Test normal_specialist_count with normal specialists
***********************************************************************/
static void test_normal_specialist_count_positive(void **state)
{
  (void) state;

  game.control.num_normal_specialists = 2;
  assert_int_equal(normal_specialist_count(), 2);

  game.control.num_normal_specialists = 4;
  assert_int_equal(normal_specialist_count(), 4);
}

/***********************************************************************
  Test specialist_by_number with invalid negative id
***********************************************************************/
static void test_specialist_by_number_negative(void **state)
{
  (void) state;

  game.control.num_specialist_types = 5;

  struct specialist *sp = specialist_by_number(-1);
  assert_null(sp);
}

/***********************************************************************
  Test specialist_by_number with invalid large id
***********************************************************************/
static void test_specialist_by_number_out_of_range(void **state)
{
  (void) state;

  game.control.num_specialist_types = 5;

  struct specialist *sp = specialist_by_number(10);
  assert_null(sp);
}

/***********************************************************************
  Test specialist_by_number with valid id
  Note: This test requires the specialists array to be properly initialized.
  The function returns a pointer to the internal array, so we can test
  that it returns non-null for valid indices.
***********************************************************************/
static void test_specialist_by_number_valid(void **state)
{
  (void) state;

  /* This test requires proper initialization of specialists array.
   * Without ruleset data, the array exists but specialists are not
   * fully initialized. Skip this test as it requires full game init. */
  skip();
}

/***********************************************************************
  Test specialist_number with null pointer
  Note: specialist_number has an fc_assert_ret_val that returns -1 for null.
***********************************************************************/
static void test_specialist_number_null(void **state)
{
  (void) state;

  /* specialist_number returns -1 for null input due to fc_assert_ret_val */
  Specialist_type_id id = specialist_number(NULL);
  assert_int_equal(id, -1);
}

/***********************************************************************
  Test specialist_index with null pointer
  Note: specialist_index has an fc_assert_ret_val that returns -1 for null.
***********************************************************************/
static void test_specialist_index_null(void **state)
{
  (void) state;

  /* specialist_index returns -1 for null input due to fc_assert_ret_val */
  Specialist_type_id id = specialist_index(NULL);
  assert_int_equal(id, -1);
}

/***********************************************************************
  Test specialist_by_rule_name with null name
  Note: The function may crash with null input, so we skip this test.
***********************************************************************/
static void test_specialist_by_rule_name_null(void **state)
{
  (void) state;

  /* This test may cause undefined behavior with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test specialist_by_rule_name with empty string
***********************************************************************/
static void test_specialist_by_rule_name_empty(void **state)
{
  (void) state;

  game.control.num_specialist_types = 0;

  struct specialist *sp = specialist_by_rule_name("");
  assert_null(sp);
}

/***********************************************************************
  Test specialist_by_rule_name with non-existent name
***********************************************************************/
static void test_specialist_by_rule_name_not_found(void **state)
{
  (void) state;

  game.control.num_specialist_types = 0;

  struct specialist *sp = specialist_by_rule_name("nonexistent_specialist");
  assert_null(sp);
}

/***********************************************************************
  Test specialist_by_translated_name with null name
  Note: The function may crash with null input, so we skip this test.
***********************************************************************/
static void test_specialist_by_translated_name_null(void **state)
{
  (void) state;

  /* This test may cause undefined behavior with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test specialist_by_translated_name with empty string
***********************************************************************/
static void test_specialist_by_translated_name_empty(void **state)
{
  (void) state;

  game.control.num_specialist_types = 0;

  struct specialist *sp = specialist_by_translated_name("");
  assert_null(sp);
}

/***********************************************************************
  Test specialist_by_translated_name with non-existent name
***********************************************************************/
static void test_specialist_by_translated_name_not_found(void **state)
{
  (void) state;

  game.control.num_specialist_types = 0;

  struct specialist *sp = specialist_by_translated_name("Nonexistent Specialist");
  assert_null(sp);
}

/***********************************************************************
  Test specialist_rule_name with null pointer
  Note: The function will crash with null input, so we skip this test.
***********************************************************************/
static void test_specialist_rule_name_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test specialist_plural_translation with null pointer
  Note: The function will crash with null input, so we skip this test.
***********************************************************************/
static void test_specialist_plural_translation_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test specialist_abbreviation_translation with null pointer
  Note: The function will crash with null input, so we skip this test.
***********************************************************************/
static void test_specialist_abbreviation_translation_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test is_normal_specialist_id with negative id
***********************************************************************/
static void test_is_normal_specialist_id_negative(void **state)
{
  (void) state;

  game.control.num_normal_specialists = 3;

  bool result = is_normal_specialist_id(-1);
  assert_false(result);
}

/***********************************************************************
  Test is_normal_specialist_id with valid id
***********************************************************************/
static void test_is_normal_specialist_id_valid(void **state)
{
  (void) state;

  game.control.num_normal_specialists = 3;

  bool result = is_normal_specialist_id(0);
  assert_true(result);

  result = is_normal_specialist_id(2);
  assert_true(result);
}

/***********************************************************************
  Test is_normal_specialist_id with out of range id
***********************************************************************/
static void test_is_normal_specialist_id_out_of_range(void **state)
{
  (void) state;

  game.control.num_normal_specialists = 3;

  bool result = is_normal_specialist_id(3);
  assert_false(result);

  result = is_normal_specialist_id(5);
  assert_false(result);
}

/***********************************************************************
  Test is_super_specialist_id with normal specialist id
***********************************************************************/
static void test_is_super_specialist_id_normal(void **state)
{
  (void) state;

  game.control.num_specialist_types = 5;
  game.control.num_normal_specialists = 3;

  bool result = is_super_specialist_id(0);
  assert_false(result);

  result = is_super_specialist_id(2);
  assert_false(result);
}

/***********************************************************************
  Test is_super_specialist_id with super specialist id
***********************************************************************/
static void test_is_super_specialist_id_super(void **state)
{
  (void) state;

  game.control.num_specialist_types = 5;
  game.control.num_normal_specialists = 3;

  bool result = is_super_specialist_id(3);
  assert_true(result);

  result = is_super_specialist_id(4);
  assert_true(result);
}

/***********************************************************************
  Test is_super_specialist_id with out of range id
***********************************************************************/
static void test_is_super_specialist_id_out_of_range(void **state)
{
  (void) state;

  game.control.num_specialist_types = 5;
  game.control.num_normal_specialists = 3;

  bool result = is_super_specialist_id(5);
  assert_false(result);

  result = is_super_specialist_id(10);
  assert_false(result);
}

/***********************************************************************
  Test is_normal_specialist with null pointer
  Note: The function calls specialist_index which handles null, but then
  accesses the pointer. Skip this test.
***********************************************************************/
static void test_is_normal_specialist_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test is_super_specialist with null pointer
  Note: The function calls specialist_index which handles null, but then
  accesses the pointer. Skip this test.
***********************************************************************/
static void test_is_super_specialist_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test specialists_abbreviation_string with no specialists
***********************************************************************/
static void test_specialists_abbreviation_string_empty(void **state)
{
  (void) state;

  game.control.num_specialist_types = 0;

  const char *result = specialists_abbreviation_string();
  assert_string_equal(result, "");
}

/***********************************************************************
  Test specialists_string with null list
  Note: The function may crash with null input, so we skip this test.
***********************************************************************/
static void test_specialists_string_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test specialists_string with empty list
***********************************************************************/
static void test_specialists_string_empty(void **state)
{
  (void) state;

  game.control.num_specialist_types = 0;

  citizens list[SP_MAX] = {0};
  const char *result = specialists_string(list);
  assert_string_equal(result, "");
}

/***********************************************************************
  Test SP_MAX constant
***********************************************************************/
static void test_sp_max_constant(void **state)
{
  (void) state;

  /* SP_MAX should be a positive value */
  assert_true(SP_MAX > 0);
}

/***********************************************************************
  Main test runner
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    /* specialist_count tests */
    cmocka_unit_test_setup_teardown(test_specialist_count_zero, setup, teardown),
    cmocka_unit_test_setup_teardown(test_specialist_count_positive, setup, teardown),

    /* normal_specialist_count tests */
    cmocka_unit_test_setup_teardown(test_normal_specialist_count_zero, setup, teardown),
    cmocka_unit_test_setup_teardown(test_normal_specialist_count_positive, setup, teardown),

    /* specialist_by_number tests */
    cmocka_unit_test_setup_teardown(test_specialist_by_number_negative, setup, teardown),
    cmocka_unit_test_setup_teardown(test_specialist_by_number_out_of_range, setup, teardown),
    cmocka_unit_test_setup_teardown(test_specialist_by_number_valid, setup, teardown),

    /* specialist_number and specialist_index tests */
    cmocka_unit_test_setup_teardown(test_specialist_number_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_specialist_index_null, setup, teardown),

    /* specialist_by_rule_name tests */
    cmocka_unit_test_setup_teardown(test_specialist_by_rule_name_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_specialist_by_rule_name_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_specialist_by_rule_name_not_found, setup, teardown),

    /* specialist_by_translated_name tests */
    cmocka_unit_test_setup_teardown(test_specialist_by_translated_name_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_specialist_by_translated_name_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_specialist_by_translated_name_not_found, setup, teardown),

    /* specialist name translation tests */
    cmocka_unit_test_setup_teardown(test_specialist_rule_name_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_specialist_plural_translation_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_specialist_abbreviation_translation_null, setup, teardown),

    /* is_normal_specialist_id tests */
    cmocka_unit_test_setup_teardown(test_is_normal_specialist_id_negative, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_normal_specialist_id_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_normal_specialist_id_out_of_range, setup, teardown),

    /* is_super_specialist_id tests */
    cmocka_unit_test_setup_teardown(test_is_super_specialist_id_normal, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_super_specialist_id_super, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_super_specialist_id_out_of_range, setup, teardown),

    /* is_normal_specialist and is_super_specialist tests */
    cmocka_unit_test_setup_teardown(test_is_normal_specialist_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_super_specialist_null, setup, teardown),

    /* specialists_abbreviation_string tests */
    cmocka_unit_test_setup_teardown(test_specialists_abbreviation_string_empty, setup, teardown),

    /* specialists_string tests */
    cmocka_unit_test_setup_teardown(test_specialists_string_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_specialists_string_empty, setup, teardown),

    /* Constants tests */
    cmocka_unit_test_setup_teardown(test_sp_max_constant, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}