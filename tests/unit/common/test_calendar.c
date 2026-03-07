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
#include "calendar.h"
#include "game.h"

static void test_textyear_negative(void **state)
{
  const char *result;

  (void) state;

  /* Setup game calendar for negative year test */
  game.calendar.negative_year_label = N_("BC");
  game.calendar.positive_year_label = N_("AD");

  result = textyear(-1000);
  assert_non_null(result);
  assert_string_not_equal(result, "");
  /* The result should contain the year number */
  assert_true(strstr(result, "1000") != NULL);
}

static void test_textyear_positive(void **state)
{
  const char *result;

  (void) state;

  game.calendar.negative_year_label = N_("BC");
  game.calendar.positive_year_label = N_("AD");

  result = textyear(1000);
  assert_non_null(result);
  assert_string_not_equal(result, "");
  assert_true(strstr(result, "1000") != NULL);
}

static void test_textyear_zero(void **state)
{
  const char *result;

  (void) state;

  game.calendar.negative_year_label = N_("BC");
  game.calendar.positive_year_label = N_("AD");

  /* Year 0 should be handled as positive (0 AD) */
  result = textyear(0);
  assert_non_null(result);
  assert_string_not_equal(result, "");
  assert_true(strstr(result, "0") != NULL);
}

static void test_textcalfrag_with_fragments(void **state)
{
  const char *result;

  (void) state;

  /* Setup calendar with fragments */
  game.calendar.calendar_fragments = 12;
  game.calendar.calendar_fragment_name[0][0] = '\0';

  result = textcalfrag(0);
  assert_non_null(result);
  /* First fragment should display as 1 (human readable) */
  assert_string_equal(result, "1");
}

static void test_textcalfrag_with_name(void **state)
{
  const char *result;

  (void) state;

  game.calendar.calendar_fragments = 4;
  fc_strlcpy(game.calendar.calendar_fragment_name[0], "Spring",
             sizeof(game.calendar.calendar_fragment_name[0]));

  result = textcalfrag(0);
  assert_non_null(result);
}

static void test_calendar_text_no_fragments(void **state)
{
  const char *result;

  (void) state;

  game.calendar.calendar_fragments = 0;
  game.calendar.negative_year_label = N_("BC");
  game.calendar.positive_year_label = N_("AD");
  game.info.year = 100;

  result = calendar_text();
  assert_non_null(result);
  assert_string_not_equal(result, "");
}

static void test_calendar_text_with_fragments(void **state)
{
  const char *result;

  (void) state;

  game.calendar.calendar_fragments = 12;
  game.calendar.negative_year_label = N_("BC");
  game.calendar.positive_year_label = N_("AD");
  game.info.year = 100;
  game.info.fragment_count = 5;

  result = calendar_text();
  assert_non_null(result);
  assert_string_not_equal(result, "");
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_textyear_negative),
    cmocka_unit_test(test_textyear_positive),
    cmocka_unit_test(test_textyear_zero),
    cmocka_unit_test(test_textcalfrag_with_fragments),
    cmocka_unit_test(test_textcalfrag_with_name),
    cmocka_unit_test(test_calendar_text_no_fragments),
    cmocka_unit_test(test_calendar_text_with_fragments),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
