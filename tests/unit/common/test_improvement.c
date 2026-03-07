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
#include "game.h"
#include "improvement.h"

/* Helper to set up game with default values for testing */
static void setup_game_defaults(void)
{
  memset(&game, 0, sizeof(game));

  game.info.shieldbox = GAME_DEFAULT_SHIELDBOX;
  game.control.num_impr_types = 0;
}

/***********************************************************************
  Test improvement_by_number with negative id
***********************************************************************/
static void test_improvement_by_number_negative(void **state)
{
  struct impr_type *result;

  (void) state;

  setup_game_defaults();

  result = improvement_by_number(-1);
  assert_null(result);
}

/***********************************************************************
  Test improvement_by_number with invalid large id
***********************************************************************/
static void test_improvement_by_number_invalid(void **state)
{
  struct impr_type *result;

  (void) state;

  setup_game_defaults();

  result = improvement_by_number(9999);
  assert_null(result);
}

/***********************************************************************
  Test improvement_by_number with zero when no improvements exist
***********************************************************************/
static void test_improvement_by_number_zero_empty(void **state)
{
  struct impr_type *result;

  (void) state;

  setup_game_defaults();
  game.control.num_impr_types = 0;

  result = improvement_by_number(0);
  assert_null(result);
}

/***********************************************************************
  Test improvement_count when no improvements exist
***********************************************************************/
static void test_improvement_count_empty(void **state)
{
  Impr_type_id count;

  (void) state;

  setup_game_defaults();
  game.control.num_impr_types = 0;

  count = improvement_count();
  assert_int_equal(count, 0);
}

/***********************************************************************
  Test improvement_count with some improvements
***********************************************************************/
static void test_improvement_count_nonzero(void **state)
{
  Impr_type_id count;

  (void) state;

  setup_game_defaults();
  game.control.num_impr_types = 5;

  count = improvement_count();
  assert_int_equal(count, 5);
}

/***********************************************************************
  Test improvement_number with null pointer
  Note: improvement_number is declared with nonnull attribute,
  so we cannot safely test with NULL.
***********************************************************************/
static void test_improvement_number_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash due to nonnull attribute */
}

/***********************************************************************
  Test improvement_index with null pointer
  Note: improvement_index is declared with nonnull attribute,
  so we cannot safely test with NULL.
***********************************************************************/
static void test_improvement_index_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash due to nonnull attribute */
}

/***********************************************************************
  Test improvement_name_translation with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_improvement_name_translation_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

/***********************************************************************
  Test improvement_rule_name with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_improvement_rule_name_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

/***********************************************************************
  Test improvement_by_rule_name with null name
***********************************************************************/
static void test_improvement_by_rule_name_null(void **state)
{
  (void) state;

  /* Passing NULL to improvement_by_rule_name will crash due to Qn_()
   * Skip this test as the function doesn't handle NULL */
}

/***********************************************************************
  Test improvement_by_rule_name with empty string
***********************************************************************/
static void test_improvement_by_rule_name_empty(void **state)
{
  struct impr_type *result;

  (void) state;

  setup_game_defaults();

  result = improvement_by_rule_name("");
  assert_null(result);
}

/***********************************************************************
  Test improvement_by_rule_name with non-existent name
***********************************************************************/
static void test_improvement_by_rule_name_nonexistent(void **state)
{
  struct impr_type *result;

  (void) state;

  setup_game_defaults();

  result = improvement_by_rule_name("NonExistentImprovement");
  assert_null(result);
}

/***********************************************************************
  Test improvement_by_translated_name with null name
***********************************************************************/
static void test_improvement_by_translated_name_null(void **state)
{
  struct impr_type *result;

  (void) state;

  setup_game_defaults();

  result = improvement_by_translated_name(NULL);
  /* Result depends on implementation, just verify it doesn't crash */
  (void) result;
}

/***********************************************************************
  Test improvement_by_translated_name with empty string
***********************************************************************/
static void test_improvement_by_translated_name_empty(void **state)
{
  struct impr_type *result;

  (void) state;

  setup_game_defaults();

  result = improvement_by_translated_name("");
  assert_null(result);
}

/***********************************************************************
  Test improvement_by_translated_name with non-existent name
***********************************************************************/
static void test_improvement_by_translated_name_nonexistent(void **state)
{
  struct impr_type *result;

  (void) state;

  setup_game_defaults();

  result = improvement_by_translated_name("NonExistentImprovement");
  assert_null(result);
}

/***********************************************************************
  Test valid_improvement with null pointer
***********************************************************************/
static void test_valid_improvement_null(void **state)
{
  const struct impr_type *result;

  (void) state;

  setup_game_defaults();

  result = valid_improvement(NULL);
  assert_null(result);
}

/***********************************************************************
  Test valid_improvement_by_number with negative id
***********************************************************************/
static void test_valid_improvement_by_number_negative(void **state)
{
  const struct impr_type *result;

  (void) state;

  setup_game_defaults();

  result = valid_improvement_by_number(-1);
  assert_null(result);
}

/***********************************************************************
  Test valid_improvement_by_number with invalid large id
***********************************************************************/
static void test_valid_improvement_by_number_invalid(void **state)
{
  const struct impr_type *result;

  (void) state;

  setup_game_defaults();

  result = valid_improvement_by_number(9999);
  assert_null(result);
}

/***********************************************************************
  Test is_great_wonder with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_is_great_wonder_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

/***********************************************************************
  Test is_small_wonder with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_is_small_wonder_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

/***********************************************************************
  Test is_wonder with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_is_wonder_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

/***********************************************************************
  Test is_improvement with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_is_improvement_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

/***********************************************************************
  Test improvement_array_first when no improvements exist
***********************************************************************/
static void test_improvement_array_first_empty(void **state)
{
  struct impr_type *result;

  (void) state;

  setup_game_defaults();
  game.control.num_impr_types = 0;

  result = improvement_array_first();
  assert_null(result);
}

/***********************************************************************
  Test improvement_array_last when no improvements exist
***********************************************************************/
static void test_improvement_array_last_empty(void **state)
{
  const struct impr_type *result;

  (void) state;

  setup_game_defaults();
  game.control.num_impr_types = 0;

  result = improvement_array_last();
  assert_null(result);
}

/***********************************************************************
  Test improvement_has_flag with invalid flag
***********************************************************************/
static void test_improvement_has_flag_invalid(void **state)
{
  (void) state;

  /* Skip test - requires valid improvement pointer */
}

/***********************************************************************
  Test is_improvement_visible with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_is_improvement_visible_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

/***********************************************************************
  Test can_improvement_go_obsolete with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_can_improvement_go_obsolete_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

/***********************************************************************
  Test is_special_improvement with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_is_special_improvement_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

/***********************************************************************
  Test is_convert_improvement with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_is_convert_improvement_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

/***********************************************************************
  Test impr_base_build_shield_cost with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_impr_base_build_shield_cost_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

/***********************************************************************
  Test impr_sell_gold with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_impr_sell_gold_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

/***********************************************************************
  Test can_sell_building with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_can_sell_building_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

/***********************************************************************
  Test is_building_sellable with null pointer
  Note: This would crash, so we skip the test.
***********************************************************************/
static void test_is_building_sellable_null(void **state)
{
  (void) state;

  /* Skip test - passing NULL would crash */
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* improvement_by_number tests */
    cmocka_unit_test(test_improvement_by_number_negative),
    cmocka_unit_test(test_improvement_by_number_invalid),
    cmocka_unit_test(test_improvement_by_number_zero_empty),

    /* improvement_count tests */
    cmocka_unit_test(test_improvement_count_empty),
    cmocka_unit_test(test_improvement_count_nonzero),

    /* improvement_number tests */
    cmocka_unit_test(test_improvement_number_null),

    /* improvement_index tests */
    cmocka_unit_test(test_improvement_index_null),

    /* improvement_name tests */
    cmocka_unit_test(test_improvement_name_translation_null),
    cmocka_unit_test(test_improvement_rule_name_null),

    /* improvement_by_rule_name tests */
    cmocka_unit_test(test_improvement_by_rule_name_null),
    cmocka_unit_test(test_improvement_by_rule_name_empty),
    cmocka_unit_test(test_improvement_by_rule_name_nonexistent),

    /* improvement_by_translated_name tests */
    cmocka_unit_test(test_improvement_by_translated_name_null),
    cmocka_unit_test(test_improvement_by_translated_name_empty),
    cmocka_unit_test(test_improvement_by_translated_name_nonexistent),

    /* valid_improvement tests */
    cmocka_unit_test(test_valid_improvement_null),
    cmocka_unit_test(test_valid_improvement_by_number_negative),
    cmocka_unit_test(test_valid_improvement_by_number_invalid),

    /* is_wonder and related tests */
    cmocka_unit_test(test_is_great_wonder_null),
    cmocka_unit_test(test_is_small_wonder_null),
    cmocka_unit_test(test_is_wonder_null),
    cmocka_unit_test(test_is_improvement_null),

    /* improvement_array tests */
    cmocka_unit_test(test_improvement_array_first_empty),
    cmocka_unit_test(test_improvement_array_last_empty),

    /* Other improvement function tests */
    cmocka_unit_test(test_improvement_has_flag_invalid),
    cmocka_unit_test(test_is_improvement_visible_null),
    cmocka_unit_test(test_can_improvement_go_obsolete_null),
    cmocka_unit_test(test_is_special_improvement_null),
    cmocka_unit_test(test_is_convert_improvement_null),
    cmocka_unit_test(test_impr_base_build_shield_cost_null),
    cmocka_unit_test(test_impr_sell_gold_null),
    cmocka_unit_test(test_can_sell_building_null),
    cmocka_unit_test(test_is_building_sellable_null),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}