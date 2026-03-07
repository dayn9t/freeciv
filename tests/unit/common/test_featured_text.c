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

/* common */
#include "featured_text.h"

/* Test ft_color_construct() */
static void test_ft_color_construct(void **state)
{
  struct ft_color color;

  (void) state;

  color = ft_color_construct("#FF0000", "#00FF00");
  assert_string_equal(color.foreground, "#FF0000");
  assert_string_equal(color.background, "#00FF00");
}

/* Test ft_color_construct() with NULL values */
static void test_ft_color_construct_null(void **state)
{
  struct ft_color color;

  (void) state;

  color = ft_color_construct(NULL, NULL);
  assert_null(color.foreground);
  assert_null(color.background);
}

/* Test ft_color_construct() with partial values */
static void test_ft_color_construct_partial(void **state)
{
  struct ft_color color;

  (void) state;

  color = ft_color_construct("#FF0000", NULL);
  assert_string_equal(color.foreground, "#FF0000");
  assert_null(color.background);

  color = ft_color_construct(NULL, "#00FF00");
  assert_null(color.foreground);
  assert_string_equal(color.background, "#00FF00");
}

/* Test ft_color_requested() with both colors set */
static void test_ft_color_requested_both(void **state)
{
  struct ft_color color = FT_COLOR("#FF0000", "#00FF00");
  bool result;

  (void) state;

  result = ft_color_requested(color);
  assert_true(result);
}

/* Test ft_color_requested() with foreground only */
static void test_ft_color_requested_foreground(void **state)
{
  struct ft_color color = FT_COLOR("#FF0000", NULL);
  bool result;

  (void) state;

  result = ft_color_requested(color);
  assert_true(result);
}

/* Test ft_color_requested() with background only */
static void test_ft_color_requested_background(void **state)
{
  struct ft_color color = FT_COLOR(NULL, "#00FF00");
  bool result;

  (void) state;

  result = ft_color_requested(color);
  assert_true(result);
}

/* Test ft_color_requested() with neither color set */
static void test_ft_color_requested_none(void **state)
{
  struct ft_color color = FT_COLOR(NULL, NULL);
  bool result;

  (void) state;

  result = ft_color_requested(color);
  assert_false(result);
}

/* Test ft_color_requested() with empty strings */
static void test_ft_color_requested_empty(void **state)
{
  struct ft_color color = FT_COLOR("", "");
  bool result;

  (void) state;

  result = ft_color_requested(color);
  assert_false(result);
}

/* Test text_tag_new() for TTT_BOLD */
static void test_text_tag_new_bold(void **state)
{
  struct text_tag *tag;

  (void) state;

  tag = text_tag_new(TTT_BOLD, 0, 5);
  assert_non_null(tag);
  assert_int_equal(text_tag_type(tag), TTT_BOLD);
  assert_int_equal(text_tag_start_offset(tag), 0);
  assert_int_equal(text_tag_stop_offset(tag), 5);

  text_tag_destroy(tag);
}

/* Test text_tag_new() for TTT_ITALIC */
static void test_text_tag_new_italic(void **state)
{
  struct text_tag *tag;

  (void) state;

  tag = text_tag_new(TTT_ITALIC, 2, 8);
  assert_non_null(tag);
  assert_int_equal(text_tag_type(tag), TTT_ITALIC);
  assert_int_equal(text_tag_start_offset(tag), 2);
  assert_int_equal(text_tag_stop_offset(tag), 8);

  text_tag_destroy(tag);
}

/* Test text_tag_new() for TTT_STRIKE */
static void test_text_tag_new_strike(void **state)
{
  struct text_tag *tag;

  (void) state;

  tag = text_tag_new(TTT_STRIKE, 0, 10);
  assert_non_null(tag);
  assert_int_equal(text_tag_type(tag), TTT_STRIKE);
  assert_int_equal(text_tag_start_offset(tag), 0);
  assert_int_equal(text_tag_stop_offset(tag), 10);

  text_tag_destroy(tag);
}

/* Test text_tag_new() for TTT_UNDERLINE */
static void test_text_tag_new_underline(void **state)
{
  struct text_tag *tag;

  (void) state;

  tag = text_tag_new(TTT_UNDERLINE, 5, 15);
  assert_non_null(tag);
  assert_int_equal(text_tag_type(tag), TTT_UNDERLINE);
  assert_int_equal(text_tag_start_offset(tag), 5);
  assert_int_equal(text_tag_stop_offset(tag), 15);

  text_tag_destroy(tag);
}

/* Test text_tag_new() for TTT_COLOR with foreground */
static void test_text_tag_new_color_foreground(void **state)
{
  struct text_tag *tag;
  struct ft_color color = FT_COLOR("#FF0000", NULL);

  (void) state;

  tag = text_tag_new(TTT_COLOR, 0, 5, color);
  assert_non_null(tag);
  assert_int_equal(text_tag_type(tag), TTT_COLOR);
  assert_string_equal(text_tag_color_foreground(tag), "#FF0000");
  assert_string_equal(text_tag_color_background(tag), "");

  text_tag_destroy(tag);
}

/* Test text_tag_new() for TTT_COLOR with background */
static void test_text_tag_new_color_background(void **state)
{
  struct text_tag *tag;
  struct ft_color color = FT_COLOR(NULL, "#00FF00");

  (void) state;

  tag = text_tag_new(TTT_COLOR, 0, 5, color);
  assert_non_null(tag);
  assert_int_equal(text_tag_type(tag), TTT_COLOR);
  assert_string_equal(text_tag_color_foreground(tag), "");
  assert_string_equal(text_tag_color_background(tag), "#00FF00");

  text_tag_destroy(tag);
}

/* Test text_tag_new() for TTT_COLOR with both colors */
static void test_text_tag_new_color_both(void **state)
{
  struct text_tag *tag;
  struct ft_color color = FT_COLOR("#FF0000", "#0000FF");

  (void) state;

  tag = text_tag_new(TTT_COLOR, 3, 12, color);
  assert_non_null(tag);
  assert_int_equal(text_tag_type(tag), TTT_COLOR);
  assert_string_equal(text_tag_color_foreground(tag), "#FF0000");
  assert_string_equal(text_tag_color_background(tag), "#0000FF");

  text_tag_destroy(tag);
}

/* Test text_tag_new() for TTT_COLOR with no colors - should fail */
static void test_text_tag_new_color_none(void **state)
{
  struct text_tag *tag;
  struct ft_color color = FT_COLOR(NULL, NULL);

  (void) state;

  tag = text_tag_new(TTT_COLOR, 0, 5, color);
  assert_null(tag);
}

/* Test text_tag_new() with FT_OFFSET_UNSET for stop offset */
static void test_text_tag_new_unset_stop(void **state)
{
  struct text_tag *tag;

  (void) state;

  tag = text_tag_new(TTT_BOLD, 0, FT_OFFSET_UNSET);
  assert_non_null(tag);
  assert_int_equal(text_tag_start_offset(tag), 0);
  assert_int_equal(text_tag_stop_offset(tag), FT_OFFSET_UNSET);

  text_tag_destroy(tag);
}

/* Test text_tag_copy() */
static void test_text_tag_copy(void **state)
{
  struct text_tag *tag, *copy;

  (void) state;

  tag = text_tag_new(TTT_BOLD, 5, 10);
  assert_non_null(tag);

  copy = text_tag_copy(tag);
  assert_non_null(copy);
  assert_ptr_not_equal(copy, tag);
  assert_int_equal(text_tag_type(copy), TTT_BOLD);
  assert_int_equal(text_tag_start_offset(copy), 5);
  assert_int_equal(text_tag_stop_offset(copy), 10);

  text_tag_destroy(tag);
  text_tag_destroy(copy);
}

/* Test text_tag_copy() with color tag */
static void test_text_tag_copy_color(void **state)
{
  struct text_tag *tag, *copy;
  struct ft_color color = FT_COLOR("#ABCDEF", "#123456");

  (void) state;

  tag = text_tag_new(TTT_COLOR, 0, 10, color);
  assert_non_null(tag);

  copy = text_tag_copy(tag);
  assert_non_null(copy);
  assert_int_equal(text_tag_type(copy), TTT_COLOR);
  assert_string_equal(text_tag_color_foreground(copy), "#ABCDEF");
  assert_string_equal(text_tag_color_background(copy), "#123456");

  text_tag_destroy(tag);
  text_tag_destroy(copy);
}

/* Test text_tag_copy() with NULL */
static void test_text_tag_copy_null(void **state)
{
  struct text_tag *copy;

  (void) state;

  copy = text_tag_copy(NULL);
  assert_null(copy);
}

/* Test text_tag_destroy() with NULL - should not crash */
static void test_text_tag_destroy_null(void **state)
{
  (void) state;

  text_tag_destroy(NULL);
  /* Should not crash */
}

/* Test featured_text_to_plain_text() with plain text */
static void test_featured_text_to_plain_text_plain(void **state)
{
  const char *input = "Hello World";
  char output[64];
  size_t len;
  struct text_tag_list *tags = NULL;

  (void) state;

  len = featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_int_equal(len, strlen("Hello World"));
  assert_string_equal(output, "Hello World");
  assert_non_null(tags);
  assert_int_equal(text_tag_list_size(tags), 0);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with bold tags */
static void test_featured_text_to_plain_text_bold(void **state)
{
  const char *input = "[b]Hello[/b] World";
  char output[64];
  size_t len;
  struct text_tag_list *tags = NULL;

  (void) state;

  len = featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_int_equal(len, strlen("Hello World"));
  assert_string_equal(output, "Hello World");
  assert_non_null(tags);
  assert_int_equal(text_tag_list_size(tags), 1);

  if (text_tag_list_size(tags) > 0) {
    struct text_tag *tag = text_tag_list_get(tags, 0);
    assert_int_equal(text_tag_type(tag), TTT_BOLD);
    assert_int_equal(text_tag_start_offset(tag), 0);
    assert_int_equal(text_tag_stop_offset(tag), 5);
  }

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with italic tags */
static void test_featured_text_to_plain_text_italic(void **state)
{
  const char *input = "Hello [i]World[/i]";
  char output[64];
  size_t len;
  struct text_tag_list *tags = NULL;

  (void) state;

  len = featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_int_equal(len, strlen("Hello World"));
  assert_string_equal(output, "Hello World");
  assert_non_null(tags);
  assert_int_equal(text_tag_list_size(tags), 1);

  if (text_tag_list_size(tags) > 0) {
    struct text_tag *tag = text_tag_list_get(tags, 0);
    assert_int_equal(text_tag_type(tag), TTT_ITALIC);
    assert_int_equal(text_tag_start_offset(tag), 6);
    assert_int_equal(text_tag_stop_offset(tag), 11);
  }

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with color tags */
static void test_featured_text_to_plain_text_color(void **state)
{
  const char *input = "[color fg=\"#FF0000\"]Red Text[/color]";
  char output[64];
  size_t len;
  struct text_tag_list *tags = NULL;

  (void) state;

  len = featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_int_equal(len, strlen("Red Text"));
  assert_string_equal(output, "Red Text");
  assert_non_null(tags);
  assert_int_equal(text_tag_list_size(tags), 1);

  if (text_tag_list_size(tags) > 0) {
    struct text_tag *tag = text_tag_list_get(tags, 0);
    assert_int_equal(text_tag_type(tag), TTT_COLOR);
    assert_string_equal(text_tag_color_foreground(tag), "#FF0000");
  }

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() without tags list */
static void test_featured_text_to_plain_text_no_tags(void **state)
{
  const char *input = "[b]Hello[/b] World";
  char output[64];
  size_t len;

  (void) state;

  len = featured_text_to_plain_text(input, output, sizeof(output), NULL, FALSE);
  assert_int_equal(len, strlen("Hello World"));
  assert_string_equal(output, "Hello World");
}

/* Test featured_text_apply_tag() for TTT_BOLD */
static void test_featured_text_apply_tag_bold(void **state)
{
  const char *input = "Hello World";
  char output[64];
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_BOLD, 0, 5);
  assert_true(len > 0);
  assert_string_equal(output, "[b]Hello[/b] World");
}

/* Test featured_text_apply_tag() for TTT_ITALIC */
static void test_featured_text_apply_tag_italic(void **state)
{
  const char *input = "Hello World";
  char output[64];
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_ITALIC, 6, 11);
  assert_true(len > 0);
  assert_string_equal(output, "Hello [i]World[/i]");
}

/* Test featured_text_apply_tag() for TTT_STRIKE */
static void test_featured_text_apply_tag_strike(void **state)
{
  const char *input = "Hello World";
  char output[64];
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_STRIKE, 0, 11);
  assert_true(len > 0);
  assert_string_equal(output, "[s]Hello World[/s]");
}

/* Test featured_text_apply_tag() for TTT_UNDERLINE */
static void test_featured_text_apply_tag_underline(void **state)
{
  const char *input = "Hello World";
  char output[64];
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_UNDERLINE, 3, 8);
  assert_true(len > 0);
  assert_string_equal(output, "Hel[u]lo Wo[/u]rld");
}

/* Test featured_text_apply_tag() for TTT_COLOR */
static void test_featured_text_apply_tag_color(void **state)
{
  const char *input = "Hello World";
  char output[64];
  struct ft_color color = FT_COLOR("#FF0000", NULL);
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_COLOR, 0, 5, color);
  assert_true(len > 0);
  assert_string_equal(output, "[c fg=\"#FF0000\"]Hello[/c] World");
}

/* Test featured_text_apply_tag() for TTT_COLOR with both colors */
static void test_featured_text_apply_tag_color_both(void **state)
{
  const char *input = "Hello";
  char output[64];
  struct ft_color color = FT_COLOR("#FF0000", "#0000FF");
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_COLOR, 0, 5, color);
  assert_true(len > 0);
  assert_true(strstr(output, "fg=\"#FF0000\"") != NULL);
  assert_true(strstr(output, "bg=\"#0000FF\"") != NULL);
}

/* Test featured_text_apply_tag() with invalid offset */
static void test_featured_text_apply_tag_invalid_offset(void **state)
{
  const char *input = "Hello";
  char output[64];
  size_t len;

  (void) state;

  /* Start offset beyond string length */
  len = featured_text_apply_tag(input, output, sizeof(output), TTT_BOLD, 100, 200);
  assert_int_equal(len, 0);
}

/* Test featured_text_apply_tag() with FT_OFFSET_UNSET start */
static void test_featured_text_apply_tag_unset_start(void **state)
{
  const char *input = "Hello";
  char output[64];
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_BOLD, FT_OFFSET_UNSET, 5);
  assert_int_equal(len, 0);
}

/* Test text_tag_color_foreground() with non-color tag */
static void test_text_tag_color_foreground_wrong_type(void **state)
{
  struct text_tag *tag;
  const char *result;

  (void) state;

  tag = text_tag_new(TTT_BOLD, 0, 5);
  assert_non_null(tag);

  result = text_tag_color_foreground(tag);
  assert_null(result);

  text_tag_destroy(tag);
}

/* Test text_tag_color_background() with non-color tag */
static void test_text_tag_color_background_wrong_type(void **state)
{
  struct text_tag *tag;
  const char *result;

  (void) state;

  tag = text_tag_new(TTT_ITALIC, 0, 5);
  assert_non_null(tag);

  result = text_tag_color_background(tag);
  assert_null(result);

  text_tag_destroy(tag);
}

/* Test text_tag_link_type() with non-link tag */
static void test_text_tag_link_type_wrong_type(void **state)
{
  struct text_tag *tag;
  int result;

  (void) state;

  tag = text_tag_new(TTT_BOLD, 0, 5);
  assert_non_null(tag);

  result = text_tag_link_type(tag);
  assert_int_equal(result, -1);

  text_tag_destroy(tag);
}

/* Test text_tag_link_id() with non-link tag */
static void test_text_tag_link_id_wrong_type(void **state)
{
  struct text_tag *tag;
  int result;

  (void) state;

  tag = text_tag_new(TTT_BOLD, 0, 5);
  assert_non_null(tag);

  result = text_tag_link_id(tag);
  assert_int_equal(result, -1);

  text_tag_destroy(tag);
}

/* Test roundtrip: apply tag then convert to plain text */
static void test_featured_text_roundtrip_bold(void **state)
{
  const char *original = "Hello World";
  char featured[64];
  char plain[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  /* Apply bold tag */
  featured_text_apply_tag(original, featured, sizeof(featured), TTT_BOLD, 0, 5);

  /* Convert back to plain text */
  featured_text_to_plain_text(featured, plain, sizeof(plain), &tags, FALSE);

  assert_string_equal(plain, original);
  assert_int_equal(text_tag_list_size(tags), 1);

  text_tag_list_destroy(tags);
}

/* Test predefined color ftc_warning */
static void test_predefined_color_warning(void **state)
{
  (void) state;

  assert_string_equal(ftc_warning.foreground, "#FF0000");
  assert_null(ftc_warning.background);
}

/* Test predefined color ftc_server */
static void test_predefined_color_server(void **state)
{
  (void) state;

  assert_string_equal(ftc_server.foreground, "#8B0000");
  assert_null(ftc_server.background);
}

/* Test predefined color ftc_chat_public */
static void test_predefined_color_chat_public(void **state)
{
  (void) state;

  assert_string_equal(ftc_chat_public.foreground, "#00008B");
  assert_null(ftc_chat_public.background);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* ft_color tests */
    cmocka_unit_test(test_ft_color_construct),
    cmocka_unit_test(test_ft_color_construct_null),
    cmocka_unit_test(test_ft_color_construct_partial),
    cmocka_unit_test(test_ft_color_requested_both),
    cmocka_unit_test(test_ft_color_requested_foreground),
    cmocka_unit_test(test_ft_color_requested_background),
    cmocka_unit_test(test_ft_color_requested_none),
    cmocka_unit_test(test_ft_color_requested_empty),

    /* text_tag_new tests */
    cmocka_unit_test(test_text_tag_new_bold),
    cmocka_unit_test(test_text_tag_new_italic),
    cmocka_unit_test(test_text_tag_new_strike),
    cmocka_unit_test(test_text_tag_new_underline),
    cmocka_unit_test(test_text_tag_new_color_foreground),
    cmocka_unit_test(test_text_tag_new_color_background),
    cmocka_unit_test(test_text_tag_new_color_both),
    cmocka_unit_test(test_text_tag_new_color_none),
    cmocka_unit_test(test_text_tag_new_unset_stop),

    /* text_tag_copy tests */
    cmocka_unit_test(test_text_tag_copy),
    cmocka_unit_test(test_text_tag_copy_color),
    cmocka_unit_test(test_text_tag_copy_null),

    /* text_tag_destroy tests */
    cmocka_unit_test(test_text_tag_destroy_null),

    /* featured_text_to_plain_text tests */
    cmocka_unit_test(test_featured_text_to_plain_text_plain),
    cmocka_unit_test(test_featured_text_to_plain_text_bold),
    cmocka_unit_test(test_featured_text_to_plain_text_italic),
    cmocka_unit_test(test_featured_text_to_plain_text_color),
    cmocka_unit_test(test_featured_text_to_plain_text_no_tags),

    /* featured_text_apply_tag tests */
    cmocka_unit_test(test_featured_text_apply_tag_bold),
    cmocka_unit_test(test_featured_text_apply_tag_italic),
    cmocka_unit_test(test_featured_text_apply_tag_strike),
    cmocka_unit_test(test_featured_text_apply_tag_underline),
    cmocka_unit_test(test_featured_text_apply_tag_color),
    cmocka_unit_test(test_featured_text_apply_tag_color_both),
    cmocka_unit_test(test_featured_text_apply_tag_invalid_offset),
    cmocka_unit_test(test_featured_text_apply_tag_unset_start),

    /* Wrong type accessor tests */
    cmocka_unit_test(test_text_tag_color_foreground_wrong_type),
    cmocka_unit_test(test_text_tag_color_background_wrong_type),
    cmocka_unit_test(test_text_tag_link_type_wrong_type),
    cmocka_unit_test(test_text_tag_link_id_wrong_type),

    /* Roundtrip tests */
    cmocka_unit_test(test_featured_text_roundtrip_bold),

    /* Predefined colors tests */
    cmocka_unit_test(test_predefined_color_warning),
    cmocka_unit_test(test_predefined_color_server),
    cmocka_unit_test(test_predefined_color_chat_public),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}