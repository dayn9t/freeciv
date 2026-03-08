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
#include "calendar.h"
#include "effects.h"
#include "game.h"
#include "support.h"
#include "victory.h"

/* Test setup and teardown for calendar tests */
static int setup_calendar(void **state)
{
  (void) state;
  game_init(false);
  ruleset_cache_init();
  return 0;
}

static int teardown_calendar(void **state)
{
  (void) state;
  ruleset_cache_free();
  game_free();
  return 0;
}

/* ==================== textyear tests ==================== */

static void test_textyear_negative(void **state)
{
  const char *result;

  (void) state;

  fc_snprintf(game.calendar.negative_year_label,
              sizeof(game.calendar.negative_year_label), "BC");
  fc_snprintf(game.calendar.positive_year_label,
              sizeof(game.calendar.positive_year_label), "AD");

  result = textyear(-1000);
  assert_non_null(result);
  assert_string_not_equal(result, "");
  assert_true(strstr(result, "1000") != NULL);
  assert_true(strstr(result, "BC") != NULL);
}

static void test_textyear_positive(void **state)
{
  const char *result;

  (void) state;

  fc_snprintf(game.calendar.negative_year_label,
              sizeof(game.calendar.negative_year_label), "BC");
  fc_snprintf(game.calendar.positive_year_label,
              sizeof(game.calendar.positive_year_label), "AD");

  result = textyear(1000);
  assert_non_null(result);
  assert_string_not_equal(result, "");
  assert_true(strstr(result, "1000") != NULL);
  assert_true(strstr(result, "AD") != NULL);
}

static void test_textyear_zero(void **state)
{
  const char *result;

  (void) state;

  fc_snprintf(game.calendar.negative_year_label,
              sizeof(game.calendar.negative_year_label), "BC");
  fc_snprintf(game.calendar.positive_year_label,
              sizeof(game.calendar.positive_year_label), "AD");

  result = textyear(0);
  assert_non_null(result);
  assert_string_not_equal(result, "");
  assert_true(strstr(result, "0") != NULL);
}

static void test_textyear_custom_labels(void **state)
{
  const char *result;

  (void) state;

  fc_strlcpy(game.calendar.negative_year_label, "BCE",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "CE",
             sizeof(game.calendar.positive_year_label));

  result = textyear(-500);
  assert_non_null(result);
  assert_true(strstr(result, "BCE") != NULL);

  result = textyear(500);
  assert_non_null(result);
  assert_true(strstr(result, "CE") != NULL);
}

static void test_textyear_large_negative(void **state)
{
  const char *result;

  (void) state;

  fc_strlcpy(game.calendar.negative_year_label, "BC",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "AD",
             sizeof(game.calendar.positive_year_label));

  result = textyear(-10000);
  assert_non_null(result);
  assert_true(strstr(result, "10000") != NULL);
  assert_true(strstr(result, "BC") != NULL);
}

static void test_textyear_large_positive(void **state)
{
  const char *result;

  (void) state;

  fc_strlcpy(game.calendar.negative_year_label, "BC",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "AD",
             sizeof(game.calendar.positive_year_label));

  result = textyear(10000);
  assert_non_null(result);
  assert_true(strstr(result, "10000") != NULL);
  assert_true(strstr(result, "AD") != NULL);
}

static void test_textyear_one(void **state)
{
  const char *result;

  (void) state;

  fc_strlcpy(game.calendar.negative_year_label, "BC",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "AD",
             sizeof(game.calendar.positive_year_label));

  result = textyear(1);
  assert_non_null(result);
  assert_true(strstr(result, "1") != NULL);
  assert_true(strstr(result, "AD") != NULL);

  result = textyear(-1);
  assert_non_null(result);
  assert_true(strstr(result, "1") != NULL);
  assert_true(strstr(result, "BC") != NULL);
}

/* ==================== textcalfrag tests ==================== */

static void test_textcalfrag_with_fragments(void **state)
{
  const char *result;

  (void) state;

  game.calendar.calendar_fragments = 12;
  game.calendar.calendar_fragment_name[0][0] = '\0';
  game.calendar.calendar_fragment_name[1][0] = '\0';

  result = textcalfrag(0);
  assert_non_null(result);
  assert_string_equal(result, "1");

  result = textcalfrag(1);
  assert_non_null(result);
  assert_string_equal(result, "2");
}

static void test_textcalfrag_with_name(void **state)
{
  const char *result;

  (void) state;

  game.calendar.calendar_fragments = 4;
  fc_strlcpy(game.calendar.calendar_fragment_name[0], "Spring",
             sizeof(game.calendar.calendar_fragment_name[0]));
  fc_strlcpy(game.calendar.calendar_fragment_name[1], "Summer",
             sizeof(game.calendar.calendar_fragment_name[1]));
  game.calendar.calendar_fragment_name[2][0] = '\0';

  result = textcalfrag(0);
  assert_non_null(result);
  assert_string_equal(result, "Spring");

  result = textcalfrag(1);
  assert_non_null(result);
  assert_string_equal(result, "Summer");

  result = textcalfrag(2);
  assert_non_null(result);
  assert_string_equal(result, "3");
}

static void test_textcalfrag_zero_fragments(void **state)
{
  const char *result;

  (void) state;

  game.calendar.calendar_fragments = 0;

  result = textcalfrag(0);
  assert_non_null(result);
  assert_string_equal(result, "");
}

static void test_textcalfrag_high_fragment(void **state)
{
  const char *result;

  (void) state;

  game.calendar.calendar_fragments = 12;
  game.calendar.calendar_fragment_name[11][0] = '\0';

  result = textcalfrag(11);
  assert_non_null(result);
  assert_string_equal(result, "12");
}

static void test_textcalfrag_multiple_with_names(void **state)
{
  const char *result;

  (void) state;

  game.calendar.calendar_fragments = 4;
  fc_strlcpy(game.calendar.calendar_fragment_name[0], "Q1",
             sizeof(game.calendar.calendar_fragment_name[0]));
  fc_strlcpy(game.calendar.calendar_fragment_name[1], "Q2",
             sizeof(game.calendar.calendar_fragment_name[1]));
  fc_strlcpy(game.calendar.calendar_fragment_name[2], "Q3",
             sizeof(game.calendar.calendar_fragment_name[2]));
  fc_strlcpy(game.calendar.calendar_fragment_name[3], "Q4",
             sizeof(game.calendar.calendar_fragment_name[3]));

  result = textcalfrag(0);
  assert_string_equal(result, "Q1");

  result = textcalfrag(1);
  assert_string_equal(result, "Q2");

  result = textcalfrag(2);
  assert_string_equal(result, "Q3");

  result = textcalfrag(3);
  assert_string_equal(result, "Q4");
}

/* ==================== calendar_text tests ==================== */

static void test_calendar_text_no_fragments(void **state)
{
  const char *result;

  (void) state;

  game.calendar.calendar_fragments = 0;
  fc_snprintf(game.calendar.negative_year_label,
              sizeof(game.calendar.negative_year_label), "BC");
  fc_snprintf(game.calendar.positive_year_label,
              sizeof(game.calendar.positive_year_label), "AD");
  game.info.year = 100;

  result = calendar_text();
  assert_non_null(result);
  assert_string_not_equal(result, "");
  assert_true(strstr(result, "100") != NULL);
}

static void test_calendar_text_with_fragments(void **state)
{
  const char *result;

  (void) state;

  game.calendar.calendar_fragments = 12;
  fc_snprintf(game.calendar.negative_year_label,
              sizeof(game.calendar.negative_year_label), "BC");
  fc_snprintf(game.calendar.positive_year_label,
              sizeof(game.calendar.positive_year_label), "AD");
  game.info.year = 100;
  game.info.fragment_count = 5;
  game.calendar.calendar_fragment_name[5][0] = '\0';

  result = calendar_text();
  assert_non_null(result);
  assert_string_not_equal(result, "");
  assert_true(strstr(result, "100") != NULL);
  assert_true(strstr(result, "6") != NULL);
}

static void test_calendar_text_negative_year_with_fragments(void **state)
{
  const char *result;

  (void) state;

  game.calendar.calendar_fragments = 4;
  fc_strlcpy(game.calendar.negative_year_label, "BC",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "AD",
             sizeof(game.calendar.positive_year_label));
  game.info.year = -500;
  game.info.fragment_count = 2;
  game.calendar.calendar_fragment_name[2][0] = '\0';

  result = calendar_text();
  assert_non_null(result);
  assert_true(strstr(result, "500") != NULL);
  assert_true(strstr(result, "BC") != NULL);
}

static void test_calendar_text_with_named_fragment(void **state)
{
  const char *result;

  (void) state;

  game.calendar.calendar_fragments = 4;
  fc_strlcpy(game.calendar.negative_year_label, "BC",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "AD",
             sizeof(game.calendar.positive_year_label));
  game.info.year = 2020;
  game.info.fragment_count = 1;
  fc_strlcpy(game.calendar.calendar_fragment_name[1], "Summer",
             sizeof(game.calendar.calendar_fragment_name[1]));

  result = calendar_text();
  assert_non_null(result);
  assert_true(strstr(result, "2020") != NULL);
  assert_true(strstr(result, "Summer") != NULL);
}

static void test_calendar_text_zero_year(void **state)
{
  const char *result;

  (void) state;

  game.calendar.calendar_fragments = 0;
  fc_strlcpy(game.calendar.negative_year_label, "BC",
             sizeof(game.calendar.negative_year_label));
  fc_strlcpy(game.calendar.positive_year_label, "AD",
             sizeof(game.calendar.positive_year_label));
  game.info.year = 0;

  result = calendar_text();
  assert_non_null(result);
  assert_true(strstr(result, "0") != NULL);
}

/* ==================== game_next_year tests ==================== */

static void test_game_next_year_basic(void **state)
{
  struct packet_game_info info;

  (void) state;

  memset(&info, 0, sizeof(info));
  info.year = -4000;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, -4000);
}

static void test_game_next_year_with_turn_years(void **state)
{
  struct packet_game_info info;

  (void) state;

  effect_new(EFT_TURN_YEARS, 50, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = -1000;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, -950);
}

static void test_game_next_year_slowdown_1(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.info.victory_conditions = (1 << VC_SPACERACE);

  effect_new(EFT_TURN_YEARS, 10, nullptr);
  effect_new(EFT_SLOW_DOWN_TIMELINE, 1, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 105);
}

static void test_game_next_year_slowdown_2(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.info.victory_conditions = (1 << VC_SPACERACE);

  effect_new(EFT_TURN_YEARS, 10, nullptr);
  effect_new(EFT_SLOW_DOWN_TIMELINE, 2, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 102);
}

static void test_game_next_year_slowdown_3(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.info.victory_conditions = (1 << VC_SPACERACE);

  effect_new(EFT_TURN_YEARS, 10, nullptr);
  effect_new(EFT_SLOW_DOWN_TIMELINE, 3, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 101);
}

static void test_game_next_year_slowdown_no_spacerace(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.info.victory_conditions = 0;

  effect_new(EFT_TURN_YEARS, 10, nullptr);
  effect_new(EFT_SLOW_DOWN_TIMELINE, 3, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 110);
}

static void test_game_next_year_slowdown_low_increase(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.info.victory_conditions = (1 << VC_SPACERACE);

  effect_new(EFT_TURN_YEARS, 1, nullptr);
  effect_new(EFT_SLOW_DOWN_TIMELINE, 3, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 101);
}

static void test_game_next_year_slowdown_boundary_1(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.info.victory_conditions = (1 << VC_SPACERACE);

  effect_new(EFT_TURN_YEARS, 5, nullptr);
  effect_new(EFT_SLOW_DOWN_TIMELINE, 1, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 105);
}

static void test_game_next_year_slowdown_boundary_2(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.info.victory_conditions = (1 << VC_SPACERACE);

  effect_new(EFT_TURN_YEARS, 2, nullptr);
  effect_new(EFT_SLOW_DOWN_TIMELINE, 2, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 102);
}

static void test_game_next_year_slowdown_boundary_3(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.info.victory_conditions = (1 << VC_SPACERACE);

  effect_new(EFT_TURN_YEARS, 1, nullptr);
  effect_new(EFT_SLOW_DOWN_TIMELINE, 3, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 101);
}

static void test_game_next_year_slowdown_1_cap_from_6(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.info.victory_conditions = (1 << VC_SPACERACE);

  effect_new(EFT_TURN_YEARS, 6, nullptr);
  effect_new(EFT_SLOW_DOWN_TIMELINE, 1, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 105);
}

static void test_game_next_year_slowdown_2_cap_from_3(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.info.victory_conditions = (1 << VC_SPACERACE);

  effect_new(EFT_TURN_YEARS, 3, nullptr);
  effect_new(EFT_SLOW_DOWN_TIMELINE, 2, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 102);
}

static void test_game_next_year_slowdown_3_cap_from_2(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.info.victory_conditions = (1 << VC_SPACERACE);

  effect_new(EFT_TURN_YEARS, 2, nullptr);
  effect_new(EFT_SLOW_DOWN_TIMELINE, 3, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 101);
}

static void test_game_next_year_slowdown_high(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.info.victory_conditions = (1 << VC_SPACERACE);

  effect_new(EFT_TURN_YEARS, 10, nullptr);
  effect_new(EFT_SLOW_DOWN_TIMELINE, 5, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 101);
}

static void test_game_next_year_year_0_hack(void **state)
{
  struct packet_game_info info;

  (void) state;

  effect_new(EFT_TURN_YEARS, 5, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 0;
  info.year_0_hack = TRUE;

  game_next_year(&info);

  assert_int_equal(info.year, 5);
  assert_false(info.year_0_hack);
}

static void test_game_next_year_calendar_skip_0(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.calendar.calendar_skip_0 = TRUE;
  game.calendar.calendar_fragments = 0;

  effect_new(EFT_TURN_YEARS, 1, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = -1;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 1);
  assert_true(info.year_0_hack);
}

static void test_game_next_year_calendar_skip_0_next_turn(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.calendar.calendar_skip_0 = TRUE;
  game.calendar.calendar_fragments = 0;

  effect_new(EFT_TURN_YEARS, 1, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 1;
  info.year_0_hack = TRUE;

  game_next_year(&info);

  assert_int_equal(info.year, 1);
  assert_false(info.year_0_hack);
}

static void test_game_next_year_calendar_skip_0_disabled(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.calendar.calendar_skip_0 = FALSE;
  game.calendar.calendar_fragments = 0;

  effect_new(EFT_TURN_YEARS, 1, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = -1;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 0);
  assert_false(info.year_0_hack);
}

static void test_game_next_year_calendar_skip_0_large_jump(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.calendar.calendar_skip_0 = TRUE;
  game.calendar.calendar_fragments = 0;

  effect_new(EFT_TURN_YEARS, 10, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = -5;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 5);
  assert_false(info.year_0_hack);
}

static void test_game_next_year_fragments(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.calendar.calendar_fragments = 12;
  game.calendar.calendar_skip_0 = FALSE;

  effect_new(EFT_TURN_YEARS, 1, nullptr);
  effect_new(EFT_TURN_FRAGMENTS, 5, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.fragment_count = 0;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 101);
  assert_int_equal(info.fragment_count, 5);

  game_next_year(&info);

  assert_int_equal(info.year, 102);
  assert_int_equal(info.fragment_count, 10);

  game_next_year(&info);

  assert_int_equal(info.year, 104);
  assert_int_equal(info.fragment_count, 3);
}

static void test_game_next_year_fragments_exact_multiple(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.calendar.calendar_fragments = 4;
  game.calendar.calendar_skip_0 = FALSE;

  effect_new(EFT_TURN_YEARS, 0, nullptr);
  effect_new(EFT_TURN_FRAGMENTS, 4, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.fragment_count = 0;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 101);
  assert_int_equal(info.fragment_count, 0);
}

static void test_game_next_year_fragments_large(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.calendar.calendar_fragments = 12;
  game.calendar.calendar_skip_0 = FALSE;

  effect_new(EFT_TURN_YEARS, 0, nullptr);
  effect_new(EFT_TURN_FRAGMENTS, 25, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.fragment_count = 0;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 102);
  assert_int_equal(info.fragment_count, 1);
}

static void test_game_next_year_fragments_no_turn_years(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.calendar.calendar_fragments = 4;
  game.calendar.calendar_skip_0 = FALSE;

  effect_new(EFT_TURN_YEARS, 0, nullptr);
  effect_new(EFT_TURN_FRAGMENTS, 2, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.fragment_count = 0;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 100);
  assert_int_equal(info.fragment_count, 2);

  game_next_year(&info);

  assert_int_equal(info.year, 101);
  assert_int_equal(info.fragment_count, 0);
}

static void test_game_next_year_fragments_accumulate(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.calendar.calendar_fragments = 10;
  game.calendar.calendar_skip_0 = FALSE;

  effect_new(EFT_TURN_YEARS, 0, nullptr);
  effect_new(EFT_TURN_FRAGMENTS, 3, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.fragment_count = 0;
  info.year_0_hack = FALSE;

  game_next_year(&info);
  assert_int_equal(info.year, 100);
  assert_int_equal(info.fragment_count, 3);

  game_next_year(&info);
  assert_int_equal(info.year, 100);
  assert_int_equal(info.fragment_count, 6);

  game_next_year(&info);
  assert_int_equal(info.year, 100);
  assert_int_equal(info.fragment_count, 9);

  game_next_year(&info);
  assert_int_equal(info.year, 101);
  assert_int_equal(info.fragment_count, 2);
}

static void test_game_next_year_combined_slowdown_and_fragments(void **state)
{
  struct packet_game_info info;

  (void) state;

  game.calendar.calendar_fragments = 4;
  game.calendar.calendar_skip_0 = FALSE;
  game.info.victory_conditions = (1 << VC_SPACERACE);

  effect_new(EFT_TURN_YEARS, 10, nullptr);
  effect_new(EFT_SLOW_DOWN_TIMELINE, 2, nullptr);
  effect_new(EFT_TURN_FRAGMENTS, 2, nullptr);

  memset(&info, 0, sizeof(info));
  info.year = 100;
  info.fragment_count = 0;
  info.year_0_hack = FALSE;

  game_next_year(&info);

  assert_int_equal(info.year, 102);
  assert_int_equal(info.fragment_count, 2);
}

/* ==================== game_advance_year tests ==================== */

static void test_game_advance_year_basic(void **state)
{
  (void) state;

  effect_new(EFT_TURN_YEARS, 10, nullptr);

  game.info.year = -1000;
  game.info.turn = 1;

  game_advance_year();

  assert_int_equal(game.info.year, -990);
  assert_int_equal(game.info.turn, 2);
}

static void test_game_advance_year_multiple(void **state)
{
  (void) state;

  effect_new(EFT_TURN_YEARS, 5, nullptr);

  game.info.year = 0;
  game.info.turn = 0;

  game_advance_year();
  assert_int_equal(game.info.year, 5);
  assert_int_equal(game.info.turn, 1);

  game_advance_year();
  assert_int_equal(game.info.year, 10);
  assert_int_equal(game.info.turn, 2);

  game_advance_year();
  assert_int_equal(game.info.year, 15);
  assert_int_equal(game.info.turn, 3);
}

static void test_game_advance_year_with_fragments(void **state)
{
  (void) state;

  game.calendar.calendar_fragments = 12;
  game.calendar.calendar_skip_0 = FALSE;

  effect_new(EFT_TURN_YEARS, 1, nullptr);
  effect_new(EFT_TURN_FRAGMENTS, 6, nullptr);

  game.info.year = 100;
  game.info.turn = 0;
  game.info.fragment_count = 0;

  game_advance_year();
  assert_int_equal(game.info.year, 101);
  assert_int_equal(game.info.turn, 1);
  assert_int_equal(game.info.fragment_count, 6);

  game_advance_year();
  assert_int_equal(game.info.year, 103);
  assert_int_equal(game.info.turn, 2);
  assert_int_equal(game.info.fragment_count, 0);
}

static void test_game_advance_year_turn_increment(void **state)
{
  (void) state;

  effect_new(EFT_TURN_YEARS, 1, nullptr);

  game.info.year = 2000;
  game.info.turn = 100;

  game_advance_year();
  assert_int_equal(game.info.year, 2001);
  assert_int_equal(game.info.turn, 101);

  game_advance_year();
  assert_int_equal(game.info.year, 2002);
  assert_int_equal(game.info.turn, 102);
}

static void test_game_advance_year_negative_year(void **state)
{
  (void) state;

  effect_new(EFT_TURN_YEARS, 50, nullptr);

  game.info.year = -4000;
  game.info.turn = 0;

  game_advance_year();
  assert_int_equal(game.info.year, -3950);
  assert_int_equal(game.info.turn, 1);

  game_advance_year();
  assert_int_equal(game.info.year, -3900);
  assert_int_equal(game.info.turn, 2);
}

static void test_game_advance_year_cross_zero(void **state)
{
  (void) state;

  game.calendar.calendar_skip_0 = FALSE;
  game.calendar.calendar_fragments = 0;

  effect_new(EFT_TURN_YEARS, 50, nullptr);

  game.info.year = -25;
  game.info.turn = 0;

  game_advance_year();
  assert_int_equal(game.info.year, 25);
  assert_int_equal(game.info.turn, 1);
}

static void test_game_advance_year_with_skip_0(void **state)
{
  (void) state;

  game.calendar.calendar_skip_0 = TRUE;
  game.calendar.calendar_fragments = 0;

  effect_new(EFT_TURN_YEARS, 1, nullptr);

  game.info.year = -1;
  game.info.turn = 0;
  game.info.year_0_hack = FALSE;

  game_advance_year();
  assert_int_equal(game.info.year, 1);
  assert_int_equal(game.info.turn, 1);
  assert_true(game.info.year_0_hack);

  game_advance_year();
  assert_int_equal(game.info.year, 1);
  assert_int_equal(game.info.turn, 2);
  assert_false(game.info.year_0_hack);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* textyear tests */
    cmocka_unit_test_setup_teardown(test_textyear_negative,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_textyear_positive,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_textyear_zero,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_textyear_custom_labels,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_textyear_large_negative,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_textyear_large_positive,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_textyear_one,
                                     setup_calendar, teardown_calendar),

    /* textcalfrag tests */
    cmocka_unit_test_setup_teardown(test_textcalfrag_with_fragments,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_textcalfrag_with_name,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_textcalfrag_zero_fragments,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_textcalfrag_high_fragment,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_textcalfrag_multiple_with_names,
                                     setup_calendar, teardown_calendar),

    /* calendar_text tests */
    cmocka_unit_test_setup_teardown(test_calendar_text_no_fragments,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_calendar_text_with_fragments,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_calendar_text_negative_year_with_fragments,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_calendar_text_with_named_fragment,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_calendar_text_zero_year,
                                     setup_calendar, teardown_calendar),

    /* game_next_year tests */
    cmocka_unit_test_setup_teardown(test_game_next_year_basic,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_with_turn_years,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_slowdown_1,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_slowdown_2,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_slowdown_3,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_slowdown_no_spacerace,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_slowdown_low_increase,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_slowdown_boundary_1,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_slowdown_boundary_2,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_slowdown_boundary_3,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_slowdown_1_cap_from_6,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_slowdown_2_cap_from_3,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_slowdown_3_cap_from_2,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_slowdown_high,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_year_0_hack,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_calendar_skip_0,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_calendar_skip_0_next_turn,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_calendar_skip_0_disabled,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_calendar_skip_0_large_jump,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_fragments,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_fragments_exact_multiple,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_fragments_large,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_fragments_no_turn_years,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_fragments_accumulate,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_next_year_combined_slowdown_and_fragments,
                                     setup_calendar, teardown_calendar),

    /* game_advance_year tests */
    cmocka_unit_test_setup_teardown(test_game_advance_year_basic,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_advance_year_multiple,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_advance_year_with_fragments,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_advance_year_turn_increment,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_advance_year_negative_year,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_advance_year_cross_zero,
                                     setup_calendar, teardown_calendar),
    cmocka_unit_test_setup_teardown(test_game_advance_year_with_skip_0,
                                     setup_calendar, teardown_calendar),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}