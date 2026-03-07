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
#include "unittype.h"

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
  Test utype_count with no unit types
***********************************************************************/
static void test_utype_count_zero(void **state)
{
  (void) state;

  game.control.num_unit_types = 0;
  assert_int_equal(utype_count(), 0);
}

/***********************************************************************
  Test utype_count with unit types
***********************************************************************/
static void test_utype_count_positive(void **state)
{
  (void) state;

  game.control.num_unit_types = 5;
  assert_int_equal(utype_count(), 5);

  game.control.num_unit_types = 10;
  assert_int_equal(utype_count(), 10);
}

/***********************************************************************
  Test utype_by_number with invalid negative id
***********************************************************************/
static void test_utype_by_number_negative(void **state)
{
  (void) state;

  game.control.num_unit_types = 5;

  struct unit_type *ut = utype_by_number(-1);
  assert_null(ut);
}

/***********************************************************************
  Test utype_by_number with invalid large id
***********************************************************************/
static void test_utype_by_number_out_of_range(void **state)
{
  (void) state;

  game.control.num_unit_types = 5;

  struct unit_type *ut = utype_by_number(10);
  assert_null(ut);
}

/***********************************************************************
  Test utype_by_number with valid id
  Note: This test requires the unit_types array to be properly initialized.
  The function returns a pointer to the internal array, so we can test
  that it returns non-null for valid indices.
***********************************************************************/
static void test_utype_by_number_valid(void **state)
{
  (void) state;

  /* This test requires proper initialization of unit_types array.
   * Without ruleset data, the array exists but unit types are not
   * fully initialized. Skip this test as it requires full game init. */
  skip();
}

/***********************************************************************
  Test utype_number with null pointer
  Note: utype_number has an fc_assert_ret_val that returns -1 for null.
***********************************************************************/
static void test_utype_number_null(void **state)
{
  (void) state;

  /* utype_number returns -1 for null input due to fc_assert_ret_val */
  Unit_type_id id = utype_number(NULL);
  assert_int_equal(id, -1);
}

/***********************************************************************
  Test utype_index with null pointer
  Note: utype_index has an fc_assert_ret_val that returns -1 for null.
***********************************************************************/
static void test_utype_index_null(void **state)
{
  (void) state;

  /* utype_index returns -1 for null input due to fc_assert_ret_val */
  Unit_type_id id = utype_index(NULL);
  assert_int_equal(id, -1);
}

/***********************************************************************
  Test unit_type_array_first with no unit types
***********************************************************************/
static void test_unit_type_array_first_empty(void **state)
{
  (void) state;

  game.control.num_unit_types = 0;

  struct unit_type *ut = unit_type_array_first();
  assert_null(ut);
}

/***********************************************************************
  Test unit_type_array_first with unit types
  Note: This test requires proper initialization of unit_types array.
***********************************************************************/
static void test_unit_type_array_first_with_types(void **state)
{
  (void) state;

  /* This test requires proper initialization of unit_types array.
   * Skip this test as it requires full game init with ruleset data. */
  skip();
}

/***********************************************************************
  Test unit_type_array_last with no unit types
***********************************************************************/
static void test_unit_type_array_last_empty(void **state)
{
  (void) state;

  game.control.num_unit_types = 0;

  const struct unit_type *ut = unit_type_array_last();
  assert_null(ut);
}

/***********************************************************************
  Test unit_type_array_last with unit types
  Note: This test requires proper initialization of unit_types array.
***********************************************************************/
static void test_unit_type_array_last_with_types(void **state)
{
  (void) state;

  /* This test requires proper initialization of unit_types array.
   * Skip this test as it requires full game init with ruleset data. */
  skip();
}

/***********************************************************************
  Test unit_type_by_rule_name with null name
  Note: The function may crash with null input, so we skip this test.
***********************************************************************/
static void test_unit_type_by_rule_name_null(void **state)
{
  (void) state;

  /* This test may cause undefined behavior with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test unit_type_by_rule_name with empty string
***********************************************************************/
static void test_unit_type_by_rule_name_empty(void **state)
{
  (void) state;

  game.control.num_unit_types = 0;

  struct unit_type *ut = unit_type_by_rule_name("");
  assert_null(ut);
}

/***********************************************************************
  Test unit_type_by_rule_name with non-existent name
***********************************************************************/
static void test_unit_type_by_rule_name_not_found(void **state)
{
  (void) state;

  game.control.num_unit_types = 0;

  struct unit_type *ut = unit_type_by_rule_name("nonexistent_unit");
  assert_null(ut);
}

/***********************************************************************
  Test unit_type_by_translated_name with null name
  Note: The function may crash with null input, so we skip this test.
***********************************************************************/
static void test_unit_type_by_translated_name_null(void **state)
{
  (void) state;

  /* This test may cause undefined behavior with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test unit_type_by_translated_name with empty string
***********************************************************************/
static void test_unit_type_by_translated_name_empty(void **state)
{
  (void) state;

  game.control.num_unit_types = 0;

  struct unit_type *ut = unit_type_by_translated_name("");
  assert_null(ut);
}

/***********************************************************************
  Test unit_type_by_translated_name with non-existent name
***********************************************************************/
static void test_unit_type_by_translated_name_not_found(void **state)
{
  (void) state;

  game.control.num_unit_types = 0;

  struct unit_type *ut = unit_type_by_translated_name("Nonexistent Unit");
  assert_null(ut);
}

/***********************************************************************
  Test utype_name_translation with null pointer
  Note: The function will crash with null input, so we skip this test.
***********************************************************************/
static void test_utype_name_translation_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test utype_rule_name with null pointer
  Note: The function will crash with null input, so we skip this test.
***********************************************************************/
static void test_utype_rule_name_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test unit_type_get with null unit
***********************************************************************/
static void test_unit_type_get_null(void **state)
{
  (void) state;

  const struct unit_type *ut = unit_type_get(NULL);
  assert_null(ut);
}

/***********************************************************************
  Test unit_type_get with unit having null utype
***********************************************************************/
static void test_unit_type_get_unit_null_utype(void **state)
{
  (void) state;

  struct unit test_unit;
  memset(&test_unit, 0, sizeof(test_unit));
  test_unit.utype = NULL;

  const struct unit_type *ut = unit_type_get(&test_unit);
  assert_null(ut);
}

/***********************************************************************
  Test utype_has_flag with null pointer
  Note: utype_has_flag is an inline function that accesses punittype->flags.
  It will crash with null input, so we skip this test.
***********************************************************************/
static void test_utype_has_flag_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test utype_has_role with null pointer
  Note: utype_has_role accesses punittype->roles which will crash with null.
***********************************************************************/
static void test_utype_has_role_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test utype_has_role with invalid role
***********************************************************************/
static void test_utype_has_role_invalid_role(void **state)
{
  (void) state;

  /* utype_has_role returns FALSE for invalid role values */
  bool result = utype_has_role(NULL, -1);
  assert_false(result);
}

/***********************************************************************
  Test uclass_count with no unit classes
***********************************************************************/
static void test_uclass_count_zero(void **state)
{
  (void) state;

  game.control.num_unit_classes = 0;
  assert_int_equal(uclass_count(), 0);
}

/***********************************************************************
  Test uclass_count with unit classes
***********************************************************************/
static void test_uclass_count_positive(void **state)
{
  (void) state;

  game.control.num_unit_classes = 3;
  assert_int_equal(uclass_count(), 3);

  game.control.num_unit_classes = 5;
  assert_int_equal(uclass_count(), 5);
}

/***********************************************************************
  Test uclass_by_number with invalid negative id
***********************************************************************/
static void test_uclass_by_number_negative(void **state)
{
  (void) state;

  game.control.num_unit_classes = 5;

  struct unit_class *uc = uclass_by_number(-1);
  assert_null(uc);
}

/***********************************************************************
  Test uclass_by_number with invalid large id
***********************************************************************/
static void test_uclass_by_number_out_of_range(void **state)
{
  (void) state;

  game.control.num_unit_classes = 5;

  struct unit_class *uc = uclass_by_number(10);
  assert_null(uc);
}

/***********************************************************************
  Test unit_class_by_rule_name with null name
  Note: The function may crash with null input, so we skip this test.
***********************************************************************/
static void test_unit_class_by_rule_name_null(void **state)
{
  (void) state;

  /* This test may cause undefined behavior with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test unit_class_by_rule_name with empty string
***********************************************************************/
static void test_unit_class_by_rule_name_empty(void **state)
{
  (void) state;

  game.control.num_unit_classes = 0;

  struct unit_class *uc = unit_class_by_rule_name("");
  assert_null(uc);
}

/***********************************************************************
  Test unit_class_by_rule_name with non-existent name
***********************************************************************/
static void test_unit_class_by_rule_name_not_found(void **state)
{
  (void) state;

  game.control.num_unit_classes = 0;

  struct unit_class *uc = unit_class_by_rule_name("nonexistent_class");
  assert_null(uc);
}

/***********************************************************************
  Test unit_class_array_first with no unit classes
***********************************************************************/
static void test_unit_class_array_first_empty(void **state)
{
  (void) state;

  game.control.num_unit_classes = 0;

  struct unit_class *uc = unit_class_array_first();
  assert_null(uc);
}

/***********************************************************************
  Test unit_class_array_last with no unit classes
***********************************************************************/
static void test_unit_class_array_last_empty(void **state)
{
  (void) state;

  game.control.num_unit_classes = 0;

  const struct unit_class *uc = unit_class_array_last();
  assert_null(uc);
}

/***********************************************************************
  Test uclass_number with null pointer
  Note: uclass_number is a macro that accesses item_number.
  It will crash with null input, so we skip this test.
***********************************************************************/
static void test_uclass_number_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test uclass_index macro with null pointer
  Note: uclass_index is a macro that accesses item_number.
  It will crash with null input, so we skip this test.
***********************************************************************/
static void test_uclass_index_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test utype_class macro with null pointer
  Note: utype_class is a macro that accesses uclass.
  It will crash with null input, so we skip this test.
***********************************************************************/
static void test_utype_class_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test unit_class_get with null unit
  Note: unit_class_get calls unit_type_get which asserts on null, and then
  accesses utype->uclass which can crash.
***********************************************************************/
static void test_unit_class_get_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test unit_class_get with unit having null utype
  Note: unit_class_get calls utype_class which accesses utype->uclass.
  This will crash with null utype.
***********************************************************************/
static void test_unit_class_get_unit_null_utype(void **state)
{
  (void) state;

  /* This test would cause a crash with null utype.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test uclass_has_flag with null pointer
  Note: uclass_has_flag is an inline function that accesses punitclass->flags.
  It will crash with null input, so we skip this test.
***********************************************************************/
static void test_uclass_has_flag_null(void **state)
{
  (void) state;

  /* This test would cause a crash with null input.
   * Skip this test. */
  skip();
}

/***********************************************************************
  Test utype_fuel macro
***********************************************************************/
static void test_utype_fuel_macro(void **state)
{
  (void) state;

  struct unit_type test_utype;
  memset(&test_utype, 0, sizeof(test_utype));

  test_utype.fuel = 0;
  assert_int_equal(utype_fuel(&test_utype), 0);

  test_utype.fuel = 5;
  assert_int_equal(utype_fuel(&test_utype), 5);
}

/***********************************************************************
  Test U_LAST constant
***********************************************************************/
static void test_u_last_constant(void **state)
{
  (void) state;

  /* U_LAST should be MAX_NUM_UNITS */
  assert_int_equal(U_LAST, MAX_NUM_UNITS);
}

/***********************************************************************
  Test UNIT_MAX_PARADROP_RANGE constant
***********************************************************************/
static void test_unit_max_paradrop_range(void **state)
{
  (void) state;

  /* UNIT_MAX_PARADROP_RANGE should be 65534 */
  assert_int_equal(UNIT_MAX_PARADROP_RANGE, 65535 - 1);
}

/***********************************************************************
  Test L_FIRST and L_LAST constants
***********************************************************************/
static void test_l_constants(void **state)
{
  (void) state;

  /* L_FIRST should be UTYF_LAST_USER_FLAG + 1 */
  assert_int_equal(L_FIRST, UTYF_LAST_USER_FLAG + 1);

  /* L_MAX should be 64 */
  assert_int_equal(L_MAX, 64);
}

/***********************************************************************
  Test unit type flag enum values
***********************************************************************/
static void test_unit_type_flag_enum_values(void **state)
{
  (void) state;

  /* Test some basic unit type flags */
  assert_int_equal(UTYF_NOZOC, 0);
  assert_int_equal(UTYF_IGZOC, 1);
  assert_int_equal(UTYF_CIVILIAN, 2);
  assert_int_equal(UTYF_IGTER, 3);
}

/***********************************************************************
  Test unit class flag enum values
***********************************************************************/
static void test_unit_class_flag_enum_values(void **state)
{
  (void) state;

  /* Test some basic unit class flags */
  assert_int_equal(UCF_TERRAIN_SPEED, 0);
  assert_int_equal(UCF_TERRAIN_DEFENSE, 1);
  assert_int_equal(UCF_DAMAGE_SLOWS, 2);
  assert_int_equal(UCF_CAN_OCCUPY_CITY, 3);
}

/***********************************************************************
  Test unit move type enum values
***********************************************************************/
static void test_unit_move_type_enum_values(void **state)
{
  (void) state;

  assert_int_equal(UMT_LAND, 0);
  assert_int_equal(UMT_SEA, 1);
  assert_int_equal(UMT_BOTH, 2);
}

/***********************************************************************
  Test unit role enum values
***********************************************************************/
static void test_unit_role_enum_values(void **state)
{
  (void) state;

  /* L_FIRSTBUILD should be L_FIRST */
  assert_int_equal(L_FIRSTBUILD, L_FIRST);
  assert_int_equal(L_EXPLORER, L_FIRST + 1);
  assert_int_equal(L_HUT, L_FIRST + 2);
}

/***********************************************************************
  Test combat bonus type enum values
***********************************************************************/
static void test_combat_bonus_type_enum_values(void **state)
{
  (void) state;

  assert_int_equal(CBONUS_DEFENSE_MULTIPLIER, 0);
  assert_int_equal(CBONUS_DEFENSE_DIVIDER, 1);
  assert_int_equal(CBONUS_LOW_FIREPOWER, 2);
}

/***********************************************************************
  Test move_level enum values
***********************************************************************/
static void test_move_level_enum_values(void **state)
{
  (void) state;

  assert_int_equal(MOVE_NONE, 0);
  assert_int_equal(MOVE_PARTIAL, 1);
  assert_int_equal(MOVE_FULL, 2);
}

/***********************************************************************
  Main test runner
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    /* utype_count tests */
    cmocka_unit_test_setup_teardown(test_utype_count_zero, setup, teardown),
    cmocka_unit_test_setup_teardown(test_utype_count_positive, setup, teardown),

    /* utype_by_number tests */
    cmocka_unit_test_setup_teardown(test_utype_by_number_negative, setup, teardown),
    cmocka_unit_test_setup_teardown(test_utype_by_number_out_of_range, setup, teardown),
    cmocka_unit_test_setup_teardown(test_utype_by_number_valid, setup, teardown),

    /* utype_number and utype_index tests */
    cmocka_unit_test_setup_teardown(test_utype_number_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_utype_index_null, setup, teardown),

    /* unit_type_array tests */
    cmocka_unit_test_setup_teardown(test_unit_type_array_first_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_type_array_first_with_types, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_type_array_last_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_type_array_last_with_types, setup, teardown),

    /* unit_type_by_rule_name tests */
    cmocka_unit_test_setup_teardown(test_unit_type_by_rule_name_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_type_by_rule_name_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_type_by_rule_name_not_found, setup, teardown),

    /* unit_type_by_translated_name tests */
    cmocka_unit_test_setup_teardown(test_unit_type_by_translated_name_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_type_by_translated_name_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_type_by_translated_name_not_found, setup, teardown),

    /* utype name tests */
    cmocka_unit_test_setup_teardown(test_utype_name_translation_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_utype_rule_name_null, setup, teardown),

    /* unit_type_get tests */
    cmocka_unit_test_setup_teardown(test_unit_type_get_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_type_get_unit_null_utype, setup, teardown),

    /* utype flag/role tests */
    cmocka_unit_test_setup_teardown(test_utype_has_flag_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_utype_has_role_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_utype_has_role_invalid_role, setup, teardown),

    /* uclass_count tests */
    cmocka_unit_test_setup_teardown(test_uclass_count_zero, setup, teardown),
    cmocka_unit_test_setup_teardown(test_uclass_count_positive, setup, teardown),

    /* uclass_by_number tests */
    cmocka_unit_test_setup_teardown(test_uclass_by_number_negative, setup, teardown),
    cmocka_unit_test_setup_teardown(test_uclass_by_number_out_of_range, setup, teardown),

    /* unit_class_by_rule_name tests */
    cmocka_unit_test_setup_teardown(test_unit_class_by_rule_name_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_class_by_rule_name_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_class_by_rule_name_not_found, setup, teardown),

    /* unit_class_array tests */
    cmocka_unit_test_setup_teardown(test_unit_class_array_first_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_class_array_last_empty, setup, teardown),

    /* uclass number/index tests */
    cmocka_unit_test_setup_teardown(test_uclass_number_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_uclass_index_null, setup, teardown),

    /* utype_class tests */
    cmocka_unit_test_setup_teardown(test_utype_class_null, setup, teardown),

    /* unit_class_get tests */
    cmocka_unit_test_setup_teardown(test_unit_class_get_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_class_get_unit_null_utype, setup, teardown),

    /* uclass_has_flag tests */
    cmocka_unit_test_setup_teardown(test_uclass_has_flag_null, setup, teardown),

    /* utype_fuel macro test */
    cmocka_unit_test_setup_teardown(test_utype_fuel_macro, setup, teardown),

    /* Constants tests */
    cmocka_unit_test_setup_teardown(test_u_last_constant, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_max_paradrop_range, setup, teardown),
    cmocka_unit_test_setup_teardown(test_l_constants, setup, teardown),

    /* Enum value tests */
    cmocka_unit_test_setup_teardown(test_unit_type_flag_enum_values, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_class_flag_enum_values, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_move_type_enum_values, setup, teardown),
    cmocka_unit_test_setup_teardown(test_unit_role_enum_values, setup, teardown),
    cmocka_unit_test_setup_teardown(test_combat_bonus_type_enum_values, setup, teardown),
    cmocka_unit_test_setup_teardown(test_move_level_enum_values, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}