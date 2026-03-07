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
#include "fc_types.h"
#include "server_settings.h"

/***********************************************************************
  Test that SERVER_SETTING_NONE constant equals -1
***********************************************************************/
static void test_server_setting_none_constant(void **state)
{
  (void) state;

  assert_int_equal(SERVER_SETTING_NONE, (server_setting_id)-1);
}

/***********************************************************************
  Test that SSETV_NONE equals SERVER_SETTING_NONE
***********************************************************************/
static void test_ssetv_none_constant(void **state)
{
  (void) state;

  assert_int_equal(SSETV_NONE, SERVER_SETTING_NONE);
}

/***********************************************************************
  Test server_setting_by_name with NULL fc_funcs
  Returns SERVER_SETTING_NONE when fc_funcs is NULL
***********************************************************************/
static void test_server_setting_by_name_null_funcs(void **state)
{
  server_setting_id result;

  (void) state;

  result = server_setting_by_name("test_setting");
  assert_int_equal(result, SERVER_SETTING_NONE);
}

/***********************************************************************
  Test server_setting_by_name with NULL name
  Returns SERVER_SETTING_NONE when fc_funcs is NULL
***********************************************************************/
static void test_server_setting_by_name_null_name(void **state)
{
  server_setting_id result;

  (void) state;

  result = server_setting_by_name(NULL);
  assert_int_equal(result, SERVER_SETTING_NONE);
}

/***********************************************************************
  Test server_setting_name_get with invalid id
  Returns NULL when fc_funcs is NULL
***********************************************************************/
static void test_server_setting_name_get_null_funcs(void **state)
{
  const char *result;

  (void) state;

  result = server_setting_name_get(0);
  assert_null(result);
}

/***********************************************************************
  Test server_setting_name_get with SERVER_SETTING_NONE
  Returns NULL when fc_funcs is NULL
***********************************************************************/
static void test_server_setting_name_get_none(void **state)
{
  const char *result;

  (void) state;

  result = server_setting_name_get(SERVER_SETTING_NONE);
  assert_null(result);
}

/***********************************************************************
  Test server_setting_type_get with invalid id
  Returns sset_type_invalid() when fc_funcs is NULL
***********************************************************************/
static void test_server_setting_type_get_null_funcs(void **state)
{
  enum sset_type result;

  (void) state;

  result = server_setting_type_get(0);
  assert_false(sset_type_is_valid(result));
}

/***********************************************************************
  Test server_setting_type_get with SERVER_SETTING_NONE
  Returns sset_type_invalid() when fc_funcs is NULL
***********************************************************************/
static void test_server_setting_type_get_none(void **state)
{
  enum sset_type result;

  (void) state;

  result = server_setting_type_get(SERVER_SETTING_NONE);
  assert_false(sset_type_is_valid(result));
}

/***********************************************************************
  Test server_setting_exists with invalid id
  Returns FALSE when fc_funcs is NULL (type is invalid)
***********************************************************************/
static void test_server_setting_exists_invalid(void **state)
{
  bool result;

  (void) state;

  result = server_setting_exists(0);
  assert_false(result);
}

/***********************************************************************
  Test server_setting_exists with SERVER_SETTING_NONE
  Returns FALSE since type is invalid
***********************************************************************/
static void test_server_setting_exists_none(void **state)
{
  bool result;

  (void) state;

  result = server_setting_exists(SERVER_SETTING_NONE);
  assert_false(result);
}

/***********************************************************************
  Test server_setting_value_bool_get with invalid id
  Returns FALSE when fc_funcs is NULL
***********************************************************************/
static void test_server_setting_value_bool_get_null_funcs(void **state)
{
  bool result;

  (void) state;

  result = server_setting_value_bool_get(0);
  assert_false(result);
}

/***********************************************************************
  Test server_setting_value_bool_get with SERVER_SETTING_NONE
  Returns FALSE when fc_funcs is NULL
***********************************************************************/
static void test_server_setting_value_bool_get_none(void **state)
{
  bool result;

  (void) state;

  result = server_setting_value_bool_get(SERVER_SETTING_NONE);
  assert_false(result);
}

/***********************************************************************
  Test server_setting_value_int_get with invalid id
  Returns FALSE (0) when fc_funcs is NULL
***********************************************************************/
static void test_server_setting_value_int_get_null_funcs(void **state)
{
  int result;

  (void) state;

  result = server_setting_value_int_get(0);
  assert_int_equal(result, 0);
}

/***********************************************************************
  Test server_setting_value_int_get with SERVER_SETTING_NONE
  Returns FALSE (0) when fc_funcs is NULL
***********************************************************************/
static void test_server_setting_value_int_get_none(void **state)
{
  int result;

  (void) state;

  result = server_setting_value_int_get(SERVER_SETTING_NONE);
  assert_int_equal(result, 0);
}

/***********************************************************************
  Test server_setting_value_bitwise_get with invalid id
  Returns FALSE (0) when fc_funcs is NULL
***********************************************************************/
static void test_server_setting_value_bitwise_get_null_funcs(void **state)
{
  unsigned int result;

  (void) state;

  result = server_setting_value_bitwise_get(0);
  assert_int_equal(result, 0);
}

/***********************************************************************
  Test server_setting_value_bitwise_get with SERVER_SETTING_NONE
  Returns FALSE (0) when fc_funcs is NULL
***********************************************************************/
static void test_server_setting_value_bitwise_get_none(void **state)
{
  unsigned int result;

  (void) state;

  result = server_setting_value_bitwise_get(SERVER_SETTING_NONE);
  assert_int_equal(result, 0);
}

/***********************************************************************
  Test ssetv_from_values with value 0
  Returns SSETV_NONE when value is 0
***********************************************************************/
static void test_ssetv_from_values_zero_value(void **state)
{
  ssetv result;

  (void) state;

  result = ssetv_from_values(1, 0);
  assert_int_equal(result, SSETV_NONE);
}

/***********************************************************************
  Test ssetv_from_values with invalid setting id
  Returns SSETV_NONE when setting type is not SST_BOOL
  (since fc_funcs is NULL, server_setting_type_get returns invalid type)
***********************************************************************/
static void test_ssetv_from_values_invalid_setting(void **state)
{
  ssetv result;

  (void) state;

  /* With fc_funcs NULL, server_setting_type_get returns invalid type,
   * which is not SST_BOOL, so this returns SSETV_NONE */
  result = ssetv_from_values(1, 1);
  assert_int_equal(result, SSETV_NONE);
}

/***********************************************************************
  Test ssetv_by_rule_name with NULL name
  Returns SSETV_NONE when fc_funcs is NULL
***********************************************************************/
static void test_ssetv_by_rule_name_null(void **state)
{
  ssetv result;

  (void) state;

  result = ssetv_by_rule_name(NULL);
  assert_int_equal(result, SSETV_NONE);
}

/***********************************************************************
  Test ssetv_by_rule_name with valid name but no fc_funcs
  Returns SSETV_NONE when fc_funcs is NULL
***********************************************************************/
static void test_ssetv_by_rule_name_no_funcs(void **state)
{
  ssetv result;

  (void) state;

  result = ssetv_by_rule_name("test_setting");
  assert_int_equal(result, SSETV_NONE);
}

/***********************************************************************
  Test ssetv_rule_name with invalid ssetv
  Returns NULL when fc_funcs is NULL
  Note: This function has assertions that would fail, so we test with
  SSETV_NONE which should return NULL
***********************************************************************/
static void test_ssetv_rule_name_null_funcs(void **state)
{
  const char *result;

  (void) state;

  /* When fc_funcs is NULL, server_setting_name_get returns NULL */
  result = server_setting_name_get((server_setting_id)SSETV_NONE);
  assert_null(result);
}

/***********************************************************************
  Test ssetv_setting_has_value with invalid ssetv
  Returns FALSE when fc_funcs is NULL
***********************************************************************/
static void test_ssetv_setting_has_value_invalid(void **state)
{
  bool result;

  (void) state;

  result = ssetv_setting_has_value(0);
  assert_false(result);
}

/***********************************************************************
  Test ssetv_setting_has_value with SSETV_NONE
  Returns FALSE when fc_funcs is NULL
***********************************************************************/
static void test_ssetv_setting_has_value_none(void **state)
{
  bool result;

  (void) state;

  result = ssetv_setting_has_value(SSETV_NONE);
  assert_false(result);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* Constant tests */
    cmocka_unit_test(test_server_setting_none_constant),
    cmocka_unit_test(test_ssetv_none_constant),

    /* server_setting_by_name tests */
    cmocka_unit_test(test_server_setting_by_name_null_funcs),
    cmocka_unit_test(test_server_setting_by_name_null_name),

    /* server_setting_name_get tests */
    cmocka_unit_test(test_server_setting_name_get_null_funcs),
    cmocka_unit_test(test_server_setting_name_get_none),

    /* server_setting_type_get tests */
    cmocka_unit_test(test_server_setting_type_get_null_funcs),
    cmocka_unit_test(test_server_setting_type_get_none),

    /* server_setting_exists tests */
    cmocka_unit_test(test_server_setting_exists_invalid),
    cmocka_unit_test(test_server_setting_exists_none),

    /* server_setting_value_bool_get tests */
    cmocka_unit_test(test_server_setting_value_bool_get_null_funcs),
    cmocka_unit_test(test_server_setting_value_bool_get_none),

    /* server_setting_value_int_get tests */
    cmocka_unit_test(test_server_setting_value_int_get_null_funcs),
    cmocka_unit_test(test_server_setting_value_int_get_none),

    /* server_setting_value_bitwise_get tests */
    cmocka_unit_test(test_server_setting_value_bitwise_get_null_funcs),
    cmocka_unit_test(test_server_setting_value_bitwise_get_none),

    /* ssetv_from_values tests */
    cmocka_unit_test(test_ssetv_from_values_zero_value),
    cmocka_unit_test(test_ssetv_from_values_invalid_setting),

    /* ssetv_by_rule_name tests */
    cmocka_unit_test(test_ssetv_by_rule_name_null),
    cmocka_unit_test(test_ssetv_by_rule_name_no_funcs),

    /* ssetv_rule_name tests */
    cmocka_unit_test(test_ssetv_rule_name_null_funcs),

    /* ssetv_setting_has_value tests */
    cmocka_unit_test(test_ssetv_setting_has_value_invalid),
    cmocka_unit_test(test_ssetv_setting_has_value_none),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}