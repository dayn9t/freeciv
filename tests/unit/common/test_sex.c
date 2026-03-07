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
#include "sex.h"

static void test_sex_by_name_male(void **state)
{
  sex_t result;

  (void) state;

  result = sex_by_name("Male");
  assert_int_equal(result, SEX_MALE);
}

static void test_sex_by_name_male_lowercase(void **state)
{
  sex_t result;

  (void) state;

  result = sex_by_name("male");
  assert_int_equal(result, SEX_MALE);
}

static void test_sex_by_name_male_uppercase(void **state)
{
  sex_t result;

  (void) state;

  result = sex_by_name("MALE");
  assert_int_equal(result, SEX_MALE);
}

static void test_sex_by_name_female(void **state)
{
  sex_t result;

  (void) state;

  result = sex_by_name("Female");
  assert_int_equal(result, SEX_FEMALE);
}

static void test_sex_by_name_female_lowercase(void **state)
{
  sex_t result;

  (void) state;

  result = sex_by_name("female");
  assert_int_equal(result, SEX_FEMALE);
}

static void test_sex_by_name_female_uppercase(void **state)
{
  sex_t result;

  (void) state;

  result = sex_by_name("FEMALE");
  assert_int_equal(result, SEX_FEMALE);
}

static void test_sex_by_name_unknown(void **state)
{
  sex_t result;

  (void) state;

  result = sex_by_name("Unknown");
  assert_int_equal(result, SEX_UNKNOWN);
}

static void test_sex_by_name_invalid(void **state)
{
  sex_t result;

  (void) state;

  result = sex_by_name("invalid");
  assert_int_equal(result, SEX_UNKNOWN);
}

static void test_sex_by_name_empty(void **state)
{
  sex_t result;

  (void) state;

  result = sex_by_name("");
  assert_int_equal(result, SEX_UNKNOWN);
}

static void test_sex_rule_name_male(void **state)
{
  const char *result;

  (void) state;

  result = sex_rule_name(SEX_MALE);
  assert_non_null(result);
  assert_string_equal(result, "Male");
}

static void test_sex_rule_name_female(void **state)
{
  const char *result;

  (void) state;

  result = sex_rule_name(SEX_FEMALE);
  assert_non_null(result);
  assert_string_equal(result, "Female");
}

static void test_sex_rule_name_unknown(void **state)
{
  const char *result;

  (void) state;

  result = sex_rule_name(SEX_UNKNOWN);
  assert_null(result);
}

static void test_sex_name_translation_male(void **state)
{
  const char *result;

  (void) state;

  result = sex_name_translation(SEX_MALE);
  assert_non_null(result);
}

static void test_sex_name_translation_female(void **state)
{
  const char *result;

  (void) state;

  result = sex_name_translation(SEX_FEMALE);
  assert_non_null(result);
}

static void test_sex_name_translation_unknown(void **state)
{
  const char *result;

  (void) state;

  result = sex_name_translation(SEX_UNKNOWN);
  assert_null(result);
}

static void test_sex_name_mnemonic_male(void **state)
{
  const char *result;

  (void) state;

  result = sex_name_mnemonic(SEX_MALE, "&");
  assert_non_null(result);
}

static void test_sex_name_mnemonic_female(void **state)
{
  const char *result;

  (void) state;

  result = sex_name_mnemonic(SEX_FEMALE, "&");
  assert_non_null(result);
}

static void test_sex_name_mnemonic_unknown(void **state)
{
  const char *result;

  (void) state;

  result = sex_name_mnemonic(SEX_UNKNOWN, "&");
  assert_null(result);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_sex_by_name_male),
    cmocka_unit_test(test_sex_by_name_male_lowercase),
    cmocka_unit_test(test_sex_by_name_male_uppercase),
    cmocka_unit_test(test_sex_by_name_female),
    cmocka_unit_test(test_sex_by_name_female_lowercase),
    cmocka_unit_test(test_sex_by_name_female_uppercase),
    cmocka_unit_test(test_sex_by_name_unknown),
    cmocka_unit_test(test_sex_by_name_invalid),
    cmocka_unit_test(test_sex_by_name_empty),
    cmocka_unit_test(test_sex_rule_name_male),
    cmocka_unit_test(test_sex_rule_name_female),
    cmocka_unit_test(test_sex_rule_name_unknown),
    cmocka_unit_test(test_sex_name_translation_male),
    cmocka_unit_test(test_sex_name_translation_female),
    cmocka_unit_test(test_sex_name_translation_unknown),
    cmocka_unit_test(test_sex_name_mnemonic_male),
    cmocka_unit_test(test_sex_name_mnemonic_female),
    cmocka_unit_test(test_sex_name_mnemonic_unknown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
