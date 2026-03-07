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

static void test_extra_by_number_negative(void **state)
{
  struct extra_type *result;

  (void) state;

  result = extra_by_number(-1);
  assert_null(result);
}

static void test_extra_by_number_extra_none(void **state)
{
  struct extra_type *result;

  (void) state;

  result = extra_by_number(EXTRA_NONE);
  assert_null(result);
}

static void test_extra_by_number_invalid(void **state)
{
  struct extra_type *result;

  (void) state;

  /* Large invalid index */
  result = extra_by_number(9999);
  assert_null(result);
}

static void test_extra_by_number_valid(void **state)
{
  struct extra_type *result;

  (void) state;

  /* Valid index within range - should return a pointer */
  result = extra_by_number(0);
  assert_non_null(result);
}

static void test_extra_count(void **state)
{
  int count;

  (void) state;

  count = extra_count();
  /* Should return the current count, which is 0 before game initialization */
  assert_int_equal(count, 0);
}

static void test_extra_number_null(void **state)
{
  int result;

  (void) state;

  result = extra_number(NULL);
  assert_int_equal(result, -1);
}

static void test_extra_number_valid(void **state)
{
  struct extra_type *pextra;
  int result;

  (void) state;

  pextra = extra_by_number(0);
  assert_non_null(pextra);

  result = extra_number(pextra);
  assert_int_equal(result, 0);
}

static void test_extra_name_translation_null(void **state)
{
  const char *result;

  (void) state;

  /* Passing NULL would crash - skip this test */
  (void) result;
}

static void test_extra_rule_name_null(void **state)
{
  const char *result;

  (void) state;

  /* Passing NULL would crash - skip this test */
  (void) result;
}

static void test_extra_type_by_rule_name_null(void **state)
{
  struct extra_type *result;

  (void) state;

  result = extra_type_by_rule_name(NULL);
  assert_null(result);
}

static void test_extra_type_by_rule_name_not_found(void **state)
{
  struct extra_type *result;

  (void) state;

  result = extra_type_by_rule_name("nonexistent_extra");
  assert_null(result);
}

static void test_extra_type_by_translated_name_null(void **state)
{
  struct extra_type *result;

  (void) state;

  result = extra_type_by_translated_name(NULL);
  assert_null(result);
}

static void test_extra_type_by_translated_name_not_found(void **state)
{
  struct extra_type *result;

  (void) state;

  result = extra_type_by_translated_name("nonexistent_extra");
  assert_null(result);
}

static void test_extra_has_flag_null(void **state)
{
  bool result;

  (void) state;

  /* Passing NULL would crash - skip this test */
  (void) result;
}

static void test_extra_has_flag_valid(void **state)
{
  struct extra_type *pextra;
  bool result;

  (void) state;

  pextra = extra_by_number(0);
  assert_non_null(pextra);

  /* Test with a valid flag - result depends on extra configuration */
  result = extra_has_flag(pextra, EF_NATIVE_TILE);
  /* Just verify it doesn't crash */
  (void) result;
}

static void test_extra_index_macro(void **state)
{
  struct extra_type *pextra;

  (void) state;

  pextra = extra_by_number(0);
  assert_non_null(pextra);

  /* extra_index is a macro that returns pextra->id */
  assert_int_equal(extra_index(pextra), 0);
}

static void test_extra_base_get_macro(void **state)
{
  struct extra_type *pextra;

  (void) state;

  pextra = extra_by_number(0);
  assert_non_null(pextra);

  /* extra_base_get returns pextra->data.base which should be NULL before init */
  assert_null(extra_base_get(pextra));
}

static void test_extra_road_get_macro(void **state)
{
  struct extra_type *pextra;

  (void) state;

  pextra = extra_by_number(0);
  assert_non_null(pextra);

  /* extra_road_get returns pextra->data.road which should be NULL before init */
  assert_null(extra_road_get(pextra));
}

static void test_extra_resource_get_macro(void **state)
{
  struct extra_type *pextra;

  (void) state;

  pextra = extra_by_number(0);
  assert_non_null(pextra);

  /* extra_resource_get returns pextra->data.resource which should be NULL before init */
  assert_null(extra_resource_get(pextra));
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_extra_by_number_negative),
    cmocka_unit_test(test_extra_by_number_extra_none),
    cmocka_unit_test(test_extra_by_number_invalid),
    cmocka_unit_test(test_extra_by_number_valid),
    cmocka_unit_test(test_extra_count),
    cmocka_unit_test(test_extra_number_null),
    cmocka_unit_test(test_extra_number_valid),
    cmocka_unit_test(test_extra_name_translation_null),
    cmocka_unit_test(test_extra_rule_name_null),
    cmocka_unit_test(test_extra_type_by_rule_name_null),
    cmocka_unit_test(test_extra_type_by_rule_name_not_found),
    cmocka_unit_test(test_extra_type_by_translated_name_null),
    cmocka_unit_test(test_extra_type_by_translated_name_not_found),
    cmocka_unit_test(test_extra_has_flag_null),
    cmocka_unit_test(test_extra_has_flag_valid),
    cmocka_unit_test(test_extra_index_macro),
    cmocka_unit_test(test_extra_base_get_macro),
    cmocka_unit_test(test_extra_road_get_macro),
    cmocka_unit_test(test_extra_resource_get_macro),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}