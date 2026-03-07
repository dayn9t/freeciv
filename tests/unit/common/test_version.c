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

#include <setjmp.h>
#include <cmocka.h>

/* common */
#include "version.h"

static void test_freeciv_name_version(void **state)
{
  const char *result;

  (void) state;

  result = freeciv_name_version();
  assert_non_null(result);
  assert_string_not_equal(result, "");
  /* Should contain "Freeciv" */
  assert_true(strstr(result, "Freeciv") != NULL);
}

static void test_word_version(void **state)
{
  const char *result;

  (void) state;

  result = word_version();
  assert_non_null(result);
  assert_string_not_equal(result, "");
}

static void test_fc_comparable_version(void **state)
{
  const char *result;

  (void) state;

  result = fc_comparable_version();
  assert_non_null(result);
  assert_string_not_equal(result, "");
}

static void test_freeciv_motto(void **state)
{
  const char *result;

  (void) state;

  result = freeciv_motto();
  assert_non_null(result);
  assert_string_not_equal(result, "");
}

static void test_freeciv_datafile_version(void **state)
{
  const char *result;

  (void) state;

  result = freeciv_datafile_version();
  assert_non_null(result);
  assert_string_not_equal(result, "");
}

static void test_fc_git_revision(void **state)
{
  const char *result;

  (void) state;

  /* May return NULL if git revision is not available */
  result = fc_git_revision();
  /* Just check it doesn't crash - result can be NULL or a valid string */
  (void) result;
}

static void test_beta_message(void **state)
{
  const char *result;

  (void) state;

  /* May return NULL if not a beta version */
  result = beta_message();
  /* Just check it doesn't crash - result can be NULL or a valid string */
  (void) result;
}

static void test_alpha_message(void **state)
{
  const char *result;

  (void) state;

  /* May return NULL if not a development version */
  result = alpha_message();
  /* Just check it doesn't crash - result can be NULL or a valid string */
  (void) result;
}

static void test_unstable_message(void **state)
{
  const char *result;

  (void) state;

  /* May return NULL if not an unstable version */
  result = unstable_message();
  /* Just check it doesn't crash - result can be NULL or a valid string */
  (void) result;
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_freeciv_name_version),
    cmocka_unit_test(test_word_version),
    cmocka_unit_test(test_fc_comparable_version),
    cmocka_unit_test(test_freeciv_motto),
    cmocka_unit_test(test_freeciv_datafile_version),
    cmocka_unit_test(test_fc_git_revision),
    cmocka_unit_test(test_beta_message),
    cmocka_unit_test(test_alpha_message),
    cmocka_unit_test(test_unstable_message),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
