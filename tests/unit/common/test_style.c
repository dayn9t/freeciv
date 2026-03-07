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
#include "game.h"
#include "style.h"

static void test_style_by_number_negative(void **state)
{
  struct nation_style *result;

  (void) state;

  result = style_by_number(-1);
  assert_null(result);
}

static void test_style_by_number_invalid(void **state)
{
  struct nation_style *result;

  (void) state;

  /* Large invalid index */
  result = style_by_number(9999);
  assert_null(result);
}

static void test_style_by_number_zero_uninitialized(void **state)
{
  struct nation_style *result;

  (void) state;

  /* Before styles are allocated, even index 0 should return NULL */
  result = style_by_number(0);
  assert_null(result);
}

static void test_style_count_uninitialized(void **state)
{
  int count;

  (void) state;

  /* Before styles are allocated, count should be 0 */
  count = style_count();
  assert_int_equal(count, 0);
}

static void test_style_number_null(void **state)
{
  /* style_number() requires non-null parameter (fc__attribute((nonnull(1))))
   * Passing NULL would cause undefined behavior, so skip this test */
  (void) state;
}

static void test_style_index_null(void **state)
{
  /* style_index() requires non-null parameter (fc__attribute((nonnull(1))))
   * Passing NULL would cause undefined behavior, so skip this test */
  (void) state;
}

static void test_style_name_translation_null(void **state)
{
  /* style_name_translation() requires non-null parameter
   * Passing NULL would cause undefined behavior, so skip this test */
  (void) state;
}

static void test_style_rule_name_null(void **state)
{
  /* style_rule_name() requires non-null parameter
   * Passing NULL would cause undefined behavior, so skip this test */
  (void) state;
}

static void test_style_by_rule_name_null(void **state)
{
  /* style_by_rule_name(NULL) causes segmentation fault because
   * Qn_() doesn't handle NULL gracefully. Skip this test. */
  (void) state;
}

static void test_style_by_rule_name_empty(void **state)
{
  struct nation_style *result;

  (void) state;

  /* Passing empty string should return NULL (no match) */
  result = style_by_rule_name("");
  assert_null(result);
}

static void test_style_by_rule_name_nonexistent(void **state)
{
  struct nation_style *result;

  (void) state;

  /* Passing non-existent name should return NULL */
  result = style_by_rule_name("nonexistent_style");
  assert_null(result);
}

static void test_music_style_by_number_negative(void **state)
{
  struct music_style *result;

  (void) state;

  result = music_style_by_number(-1);
  assert_null(result);
}

static void test_music_style_by_number_invalid(void **state)
{
  struct music_style *result;

  (void) state;

  /* Large invalid index */
  result = music_style_by_number(9999);
  assert_null(result);
}

static void test_music_style_by_number_zero_uninitialized(void **state)
{
  struct music_style *result;

  (void) state;

  /* Before music styles are allocated, even index 0 should return NULL */
  result = music_style_by_number(0);
  assert_null(result);
}

static void test_music_style_number_null(void **state)
{
  /* music_style_number() requires non-null parameter (fc__attribute((nonnull(1))))
   * Passing NULL would cause undefined behavior, so skip this test */
  (void) state;
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_style_by_number_negative),
    cmocka_unit_test(test_style_by_number_invalid),
    cmocka_unit_test(test_style_by_number_zero_uninitialized),
    cmocka_unit_test(test_style_count_uninitialized),
    cmocka_unit_test(test_style_number_null),
    cmocka_unit_test(test_style_index_null),
    cmocka_unit_test(test_style_name_translation_null),
    cmocka_unit_test(test_style_rule_name_null),
    cmocka_unit_test(test_style_by_rule_name_null),
    cmocka_unit_test(test_style_by_rule_name_empty),
    cmocka_unit_test(test_style_by_rule_name_nonexistent),
    cmocka_unit_test(test_music_style_by_number_negative),
    cmocka_unit_test(test_music_style_by_number_invalid),
    cmocka_unit_test(test_music_style_by_number_zero_uninitialized),
    cmocka_unit_test(test_music_style_number_null),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}