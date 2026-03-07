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
#include "rgbcolor.h"

static void test_rgbcolor_new_valid(void **state)
{
  struct rgbcolor *color;

  (void) state;

  color = rgbcolor_new(255, 128, 64);
  assert_non_null(color);
  assert_int_equal(color->r, 255);
  assert_int_equal(color->g, 128);
  assert_int_equal(color->b, 64);

  rgbcolor_destroy(color);
}

static void test_rgbcolor_new_black(void **state)
{
  struct rgbcolor *color;

  (void) state;

  color = rgbcolor_new(0, 0, 0);
  assert_non_null(color);
  assert_int_equal(color->r, 0);
  assert_int_equal(color->g, 0);
  assert_int_equal(color->b, 0);

  rgbcolor_destroy(color);
}

static void test_rgbcolor_new_white(void **state)
{
  struct rgbcolor *color;

  (void) state;

  color = rgbcolor_new(255, 255, 255);
  assert_non_null(color);
  assert_int_equal(color->r, 255);
  assert_int_equal(color->g, 255);
  assert_int_equal(color->b, 255);

  rgbcolor_destroy(color);
}

static void test_rgbcolor_to_hex(void **state)
{
  struct rgbcolor *color;
  char hex[16];
  bool result;

  (void) state;

  color = rgbcolor_new(255, 128, 64);
  result = rgbcolor_to_hex(color, hex, sizeof(hex));
  assert_true(result);
  assert_string_equal(hex, "#ff8040");

  rgbcolor_destroy(color);
}

static void test_rgbcolor_to_hex_red(void **state)
{
  struct rgbcolor *color;
  char hex[16];
  bool result;

  (void) state;

  color = rgbcolor_new(255, 0, 0);
  result = rgbcolor_to_hex(color, hex, sizeof(hex));
  assert_true(result);
  assert_string_equal(hex, "#ff0000");

  rgbcolor_destroy(color);
}

static void test_rgbcolor_to_hex_buffer_too_small(void **state)
{
  struct rgbcolor *color;
  char hex[4];
  bool result;

  (void) state;

  color = rgbcolor_new(255, 0, 0);
  /* Buffer too small (needs at least 8 bytes for #RRGGBB\0) */
  result = rgbcolor_to_hex(color, hex, sizeof(hex));
  assert_false(result);

  rgbcolor_destroy(color);
}

static void test_rgbcolor_from_hex_valid(void **state)
{
  struct rgbcolor *color = NULL;
  bool result;

  (void) state;

  result = rgbcolor_from_hex(&color, "#ff0000");
  assert_true(result);
  assert_non_null(color);
  assert_int_equal(color->r, 255);
  assert_int_equal(color->g, 0);
  assert_int_equal(color->b, 0);

  rgbcolor_destroy(color);
}

static void test_rgbcolor_from_hex_no_hash(void **state)
{
  struct rgbcolor *color = NULL;
  bool result;

  (void) state;

  result = rgbcolor_from_hex(&color, "00ff00");
  assert_true(result);
  assert_non_null(color);
  assert_int_equal(color->r, 0);
  assert_int_equal(color->g, 255);
  assert_int_equal(color->b, 0);

  rgbcolor_destroy(color);
}

static void test_rgbcolor_from_hex_blue(void **state)
{
  struct rgbcolor *color = NULL;
  bool result;

  (void) state;

  result = rgbcolor_from_hex(&color, "#0000ff");
  assert_true(result);
  assert_non_null(color);
  assert_int_equal(color->r, 0);
  assert_int_equal(color->g, 0);
  assert_int_equal(color->b, 255);

  rgbcolor_destroy(color);
}

static void test_rgbcolor_from_hex_invalid_short(void **state)
{
  struct rgbcolor *color = NULL;
  bool result;

  (void) state;

  result = rgbcolor_from_hex(&color, "#f00");
  assert_false(result);
  assert_null(color);
}

static void test_rgbcolor_from_hex_invalid_long(void **state)
{
  struct rgbcolor *color = NULL;
  bool result;

  (void) state;

  result = rgbcolor_from_hex(&color, "#ff00000");
  assert_false(result);
  assert_null(color);
}

static void test_rgbcolor_from_hex_invalid_chars(void **state)
{
  struct rgbcolor *color = NULL;
  bool result;

  (void) state;

  result = rgbcolor_from_hex(&color, "invalid");
  assert_false(result);
  assert_null(color);
}

static void test_rgbcolor_copy(void **state)
{
  struct rgbcolor *original, *copy;

  (void) state;

  original = rgbcolor_new(100, 150, 200);
  copy = rgbcolor_copy(original);

  assert_non_null(copy);
  assert_ptr_not_equal(copy, original);
  assert_int_equal(copy->r, original->r);
  assert_int_equal(copy->g, original->g);
  assert_int_equal(copy->b, original->b);

  rgbcolor_destroy(original);
  rgbcolor_destroy(copy);
}

static void test_rgbcolor_copy_null(void **state)
{
  struct rgbcolor *copy;

  (void) state;

  copy = rgbcolor_copy(NULL);
  assert_null(copy);
}

static void test_rgbcolors_are_equal(void **state)
{
  struct rgbcolor *color1, *color2;

  (void) state;

  color1 = rgbcolor_new(50, 100, 150);
  color2 = rgbcolor_new(50, 100, 150);

  assert_true(rgbcolors_are_equal(color1, color2));

  rgbcolor_destroy(color1);
  rgbcolor_destroy(color2);
}

static void test_rgbcolors_are_not_equal(void **state)
{
  struct rgbcolor *color1, *color2;

  (void) state;

  color1 = rgbcolor_new(50, 100, 150);
  color2 = rgbcolor_new(51, 100, 150);

  assert_false(rgbcolors_are_equal(color1, color2));

  rgbcolor_destroy(color1);
  rgbcolor_destroy(color2);
}

static void test_rgbcolor_brightness_score(void **state)
{
  struct rgbcolor *color;
  int score;

  (void) state;

  /* Black should have brightness 0 */
  color = rgbcolor_new(0, 0, 0);
  score = rgbcolor_brightness_score(color);
  assert_int_equal(score, 0);
  rgbcolor_destroy(color);

  /* White should have maximum brightness */
  color = rgbcolor_new(255, 255, 255);
  score = rgbcolor_brightness_score(color);
  assert_int_equal(score, 255);
  rgbcolor_destroy(color);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_rgbcolor_new_valid),
    cmocka_unit_test(test_rgbcolor_new_black),
    cmocka_unit_test(test_rgbcolor_new_white),
    cmocka_unit_test(test_rgbcolor_to_hex),
    cmocka_unit_test(test_rgbcolor_to_hex_red),
    cmocka_unit_test(test_rgbcolor_to_hex_buffer_too_small),
    cmocka_unit_test(test_rgbcolor_from_hex_valid),
    cmocka_unit_test(test_rgbcolor_from_hex_no_hash),
    cmocka_unit_test(test_rgbcolor_from_hex_blue),
    cmocka_unit_test(test_rgbcolor_from_hex_invalid_short),
    cmocka_unit_test(test_rgbcolor_from_hex_invalid_long),
    cmocka_unit_test(test_rgbcolor_from_hex_invalid_chars),
    cmocka_unit_test(test_rgbcolor_copy),
    cmocka_unit_test(test_rgbcolor_copy_null),
    cmocka_unit_test(test_rgbcolors_are_equal),
    cmocka_unit_test(test_rgbcolors_are_not_equal),
    cmocka_unit_test(test_rgbcolor_brightness_score),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
