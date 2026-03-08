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
#include "city.h"
#include "tile.h"
#include "unit.h"
#include "unittype.h"

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

/* Test featured_text_to_plain_text() with strike tags */
static void test_featured_text_to_plain_text_strike(void **state)
{
  const char *input = "[s]Deleted[/s] text";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Deleted text");
  assert_int_equal(text_tag_list_size(tags), 1);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with underline tags */
static void test_featured_text_to_plain_text_underline(void **state)
{
  const char *input = "Some [u]underlined[/u] text";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Some underlined text");
  assert_int_equal(text_tag_list_size(tags), 1);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with color background */
static void test_featured_text_to_plain_text_color_bg(void **state)
{
  const char *input = "[c bg=\"#0000FF\"]Blue BG[/c]";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Blue BG");
  assert_int_equal(text_tag_list_size(tags), 1);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with multiple tags */
static void test_featured_text_to_plain_text_multiple(void **state)
{
  const char *input = "[b]Bold[/b] and [i]italic[/i] text";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Bold and italic text");
  assert_int_equal(text_tag_list_size(tags), 2);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with nested tags */
static void test_featured_text_to_plain_text_nested(void **state)
{
  const char *input = "[b]Bold [i]and italic[/i][/b]";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Bold and italic");
  assert_int_equal(text_tag_list_size(tags), 2);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with unclosed tag */
static void test_featured_text_to_plain_text_unclosed(void **state)
{
  const char *input = "[b]Bold text without end";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Bold text without end");
  assert_int_equal(text_tag_list_size(tags), 1);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with full tag names */
static void test_featured_text_to_plain_text_full_names(void **state)
{
  const char *input = "[bold]Bold[/bold] [italic]Italic[/italic]";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Bold Italic");
  assert_int_equal(text_tag_list_size(tags), 2);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with empty string */
static void test_featured_text_to_plain_text_empty(void **state)
{
  const char *input = "";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "");

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with case insensitive tags */
static void test_featured_text_to_plain_text_case_insensitive(void **state)
{
  const char *input = "[B]Bold[/B] [I]Italic[/I]";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Bold Italic");
  assert_int_equal(text_tag_list_size(tags), 2);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with link single sequence */
static void test_featured_text_to_plain_text_link_single(void **state)
{
  const char *input = "[link tgt=\"city\" id=1 name=\"TestCity\" /]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_int_equal(text_tag_list_size(tags), 1);

  if (text_tag_list_size(tags) > 0) {
    struct text_tag *tag = text_tag_list_get(tags, 0);
    assert_int_equal(text_tag_type(tag), TTT_LINK);
    assert_int_equal(text_tag_link_type(tag), TLT_CITY);
    assert_int_equal(text_tag_link_id(tag), 1);
  }

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with unit link */
static void test_featured_text_to_plain_text_link_unit(void **state)
{
  const char *input = "[l tgt=\"unit\" id=5 name=\"Warrior\" /]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_int_equal(text_tag_list_size(tags), 1);

  if (text_tag_list_size(tags) > 0) {
    struct text_tag *tag = text_tag_list_get(tags, 0);
    assert_int_equal(text_tag_link_type(tag), TLT_UNIT);
    assert_int_equal(text_tag_link_id(tag), 5);
  }

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with link pair */
static void test_featured_text_to_plain_text_link_pair(void **state)
{
  const char *input = "[link tgt=\"city\" id=1]Click here[/link]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Click here");
  assert_int_equal(text_tag_list_size(tags), 1);

  text_tag_list_destroy(tags);
}

/* Test featured_text_apply_tag() with stop before start */
static void test_featured_text_apply_tag_stop_before_start(void **state)
{
  const char *input = "Hello";
  char output[64];
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_BOLD, 3, 1);
  assert_int_equal(len, 0);
}

/* Test featured_text_apply_tag() with FT_OFFSET_UNSET stop */
static void test_featured_text_apply_tag_unset_stop(void **state)
{
  const char *input = "Hello World";
  char output[64];
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_BOLD, 0, FT_OFFSET_UNSET);
  assert_true(len > 0);
}

/* Test featured_text_apply_tag() with TTT_COLOR no colors */
static void test_featured_text_apply_tag_color_none(void **state)
{
  const char *input = "Hello";
  char output[64];
  struct ft_color color = FT_COLOR(NULL, NULL);
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_COLOR, 0, 5, color);
  assert_int_equal(len, 0);
}

/* Test featured_text_apply_tag() with TTT_LINK city null */
static void test_featured_text_apply_tag_link_city_null(void **state)
{
  const char *input = "Hello";
  char output[64];
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_LINK, 0, 5, TLT_CITY, NULL);
  assert_int_equal(len, 0);
}

/* Test featured_text_apply_tag() with TTT_LINK tile null */
static void test_featured_text_apply_tag_link_tile_null(void **state)
{
  const char *input = "Hello";
  char output[64];
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_LINK, 0, 5, TLT_TILE, NULL);
  assert_int_equal(len, 0);
}

/* Test featured_text_apply_tag() with TTT_LINK unit null */
static void test_featured_text_apply_tag_link_unit_null(void **state)
{
  const char *input = "Hello";
  char output[64];
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_LINK, 0, 5, TLT_UNIT, NULL);
  assert_int_equal(len, 0);
}

/* Test text_tag_list operations */
static void test_text_tag_list_basic(void **state)
{
  struct text_tag_list *list;

  (void) state;

  list = text_tag_list_new();
  assert_non_null(list);
  assert_int_equal(text_tag_list_size(list), 0);

  text_tag_list_destroy(list);
}

/* Test text_tag_list_append */
static void test_text_tag_list_append(void **state)
{
  struct text_tag_list *list;
  struct text_tag *tag1, *tag2;

  (void) state;

  list = text_tag_list_new();
  assert_non_null(list);

  tag1 = text_tag_new(TTT_BOLD, 0, 5);
  tag2 = text_tag_new(TTT_ITALIC, 10, 15);

  text_tag_list_append(list, tag1);
  assert_int_equal(text_tag_list_size(list), 1);

  text_tag_list_append(list, tag2);
  assert_int_equal(text_tag_list_size(list), 2);

  text_tag_list_destroy(list);
}

/* Test text_tag_list_copy */
static void test_text_tag_list_copy(void **state)
{
  struct text_tag_list *list, *copy;
  struct text_tag *tag;

  (void) state;

  list = text_tag_list_new();
  tag = text_tag_new(TTT_BOLD, 0, 5);
  text_tag_list_append(list, tag);

  copy = text_tag_list_copy(list);
  assert_non_null(copy);
  assert_int_equal(text_tag_list_size(copy), 1);
  assert_ptr_not_equal(list, copy);

  text_tag_list_destroy(list);
  text_tag_list_destroy(copy);
}

/* Test predefined color ftc_any */
static void test_predefined_color_any(void **state)
{
  (void) state;

  assert_null(ftc_any.foreground);
  assert_null(ftc_any.background);
}

/* Test predefined color ftc_log */
static void test_predefined_color_log(void **state)
{
  (void) state;

  assert_string_equal(ftc_log.foreground, "#7F7F7F");
  assert_null(ftc_log.background);
}

/* Test predefined color ftc_client */
static void test_predefined_color_client(void **state)
{
  (void) state;

  assert_string_equal(ftc_client.foreground, "#EF7F00");
  assert_null(ftc_client.background);
}

/* Test predefined color ftc_editor */
static void test_predefined_color_editor(void **state)
{
  (void) state;

  assert_string_equal(ftc_editor.foreground, "#0000FF");
  assert_null(ftc_editor.background);
}

/* Test predefined color ftc_command */
static void test_predefined_color_command(void **state)
{
  (void) state;

  assert_string_equal(ftc_command.foreground, "#006400");
  assert_null(ftc_command.background);
}

/* Test predefined color ftc_changed */
static void test_predefined_color_changed(void **state)
{
  (void) state;

  assert_string_equal(ftc_changed.foreground, "#FF0000");
  assert_null(ftc_changed.background);
}

/* Test predefined color ftc_server_prompt */
static void test_predefined_color_server_prompt(void **state)
{
  (void) state;

  assert_string_equal(ftc_server_prompt.foreground, "#FF0000");
  assert_string_equal(ftc_server_prompt.background, "#BEBEBE");
}

/* Test predefined color ftc_player_lost */
static void test_predefined_color_player_lost(void **state)
{
  (void) state;

  assert_string_equal(ftc_player_lost.foreground, "#FFFFFF");
  assert_string_equal(ftc_player_lost.background, "#000000");
}

/* Test predefined color ftc_game_start */
static void test_predefined_color_game_start(void **state)
{
  (void) state;

  assert_string_equal(ftc_game_start.foreground, "#00FF00");
  assert_string_equal(ftc_game_start.background, "#115511");
}

/* Test predefined color ftc_chat_ally */
static void test_predefined_color_chat_ally(void **state)
{
  (void) state;

  assert_string_equal(ftc_chat_ally.foreground, "#551166");
  assert_null(ftc_chat_ally.background);
}

/* Test predefined color ftc_chat_private */
static void test_predefined_color_chat_private(void **state)
{
  (void) state;

  assert_string_equal(ftc_chat_private.foreground, "#A020F0");
  assert_null(ftc_chat_private.background);
}

/* Test predefined color ftc_chat_luaconsole */
static void test_predefined_color_chat_luaconsole(void **state)
{
  (void) state;

  assert_string_equal(ftc_chat_luaconsole.foreground, "#006400");
  assert_null(ftc_chat_luaconsole.background);
}

/* Test predefined color ftc_vote_public */
static void test_predefined_color_vote_public(void **state)
{
  (void) state;

  assert_string_equal(ftc_vote_public.foreground, "#FFFFFF");
  assert_string_equal(ftc_vote_public.background, "#AA0000");
}

/* Test predefined color ftc_vote_team */
static void test_predefined_color_vote_team(void **state)
{
  (void) state;

  assert_string_equal(ftc_vote_team.foreground, "#FFFFFF");
  assert_string_equal(ftc_vote_team.background, "#5555CC");
}

/* Test predefined color ftc_vote_passed */
static void test_predefined_color_vote_passed(void **state)
{
  (void) state;

  assert_string_equal(ftc_vote_passed.foreground, "#006400");
  assert_string_equal(ftc_vote_passed.background, "#AAFFAA");
}

/* Test predefined color ftc_vote_failed */
static void test_predefined_color_vote_failed(void **state)
{
  (void) state;

  assert_string_equal(ftc_vote_failed.foreground, "#8B0000");
  assert_string_equal(ftc_vote_failed.background, "#FFAAAA");
}

/* Test predefined color ftc_vote_yes */
static void test_predefined_color_vote_yes(void **state)
{
  (void) state;

  assert_string_equal(ftc_vote_yes.foreground, "#000000");
  assert_string_equal(ftc_vote_yes.background, "#C8FFD5");
}

/* Test predefined color ftc_vote_no */
static void test_predefined_color_vote_no(void **state)
{
  (void) state;

  assert_string_equal(ftc_vote_no.foreground, "#000000");
  assert_string_equal(ftc_vote_no.background, "#FFD2D2");
}

/* Test predefined color ftc_vote_abstain */
static void test_predefined_color_vote_abstain(void **state)
{
  (void) state;

  assert_string_equal(ftc_vote_abstain.foreground, "#000000");
  assert_string_equal(ftc_vote_abstain.background, "#E8E8E8");
}

/* Test predefined color ftc_luaconsole_input */
static void test_predefined_color_luaconsole_input(void **state)
{
  (void) state;

  assert_string_equal(ftc_luaconsole_input.foreground, "#2B008B");
  assert_null(ftc_luaconsole_input.background);
}

/* Test predefined color ftc_luaconsole_error */
static void test_predefined_color_luaconsole_error(void **state)
{
  (void) state;

  assert_string_equal(ftc_luaconsole_error.foreground, "#FF0000");
  assert_null(ftc_luaconsole_error.background);
}

/* Test predefined color ftc_luaconsole_warn */
static void test_predefined_color_luaconsole_warn(void **state)
{
  (void) state;

  assert_string_equal(ftc_luaconsole_warn.foreground, "#CF2020");
  assert_null(ftc_luaconsole_warn.background);
}

/* Test predefined color ftc_luaconsole_normal */
static void test_predefined_color_luaconsole_normal(void **state)
{
  (void) state;

  assert_string_equal(ftc_luaconsole_normal.foreground, "#006400");
  assert_null(ftc_luaconsole_normal.background);
}

/* Test predefined color ftc_luaconsole_verbose */
static void test_predefined_color_luaconsole_verbose(void **state)
{
  (void) state;

  assert_string_equal(ftc_luaconsole_verbose.foreground, "#B8B8B8");
  assert_null(ftc_luaconsole_verbose.background);
}

/* Test predefined color ftc_luaconsole_debug */
static void test_predefined_color_luaconsole_debug(void **state)
{
  (void) state;

  assert_string_equal(ftc_luaconsole_debug.foreground, "#B87676");
  assert_null(ftc_luaconsole_debug.background);
}

/* Test featured_text_to_plain_text() with unclosed quote */
static void test_featured_text_to_plain_text_unclosed_quote(void **state)
{
  const char *input = "[c fg=\"unclosed]Text[/c]";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_non_null(tags);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with empty quoted value */
static void test_featured_text_to_plain_text_empty_quoted_value(void **state)
{
  const char *input = "[c fg=\"\"]Text[/c]";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Text");
  assert_non_null(tags);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with unquoted color value */
static void test_featured_text_to_plain_text_unquoted_color(void **state)
{
  const char *input = "[c fg=red123 bg=blue456]Text[/c]";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Text");
  assert_non_null(tags);

  if (text_tag_list_size(tags) > 0) {
    struct text_tag *tag = text_tag_list_get(tags, 0);
    assert_string_equal(text_tag_color_foreground(tag), "red123");
    assert_string_equal(text_tag_color_background(tag), "blue456");
  }

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with tile link invalid x format */
static void test_featured_text_to_plain_text_tile_invalid_x_format(void **state)
{
  const char *input = "[l tgt=\"tile\" x=abc y=5 /]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_non_null(tags);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with tile link invalid y format */
static void test_featured_text_to_plain_text_tile_invalid_y_format(void **state)
{
  const char *input = "[l tgt=\"tile\" x=5 y=abc /]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_non_null(tags);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with unit link invalid id format */
static void test_featured_text_to_plain_text_unit_invalid_id_format(void **state)
{
  const char *input = "[l tgt=\"unit\" id=abc /]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_non_null(tags);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with invalid target type string */
static void test_featured_text_to_plain_text_invalid_target_type(void **state)
{
  const char *input = "[l tgt=\"unknown\" id=1 /]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_non_null(tags);
  assert_int_equal(text_tag_list_size(tags), 0);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with multiple options in link */
static void test_featured_text_to_plain_text_link_multiple_options(void **state)
{
  const char *input = "[l tgt=\"city\" id=42 name=\"BigCity\" /]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_non_null(tags);
  if (text_tag_list_size(tags) > 0) {
    struct text_tag *tag = text_tag_list_get(tags, 0);
    assert_int_equal(text_tag_type(tag), TTT_LINK);
    assert_int_equal(text_tag_link_type(tag), TLT_CITY);
    assert_int_equal(text_tag_link_id(tag), 42);
  }

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with extra whitespace in sequence */
static void test_featured_text_to_plain_text_extra_whitespace(void **state)
{
  const char *input = "[  b  ]Hello[  /b  ]";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with color using both fg and background options */
static void test_featured_text_to_plain_text_color_option_fallback(void **state)
{
  const char *input = "[color fg=\"#FF0000\" background=\"#0000FF\"]Text[/color]";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Text");
  assert_non_null(tags);

  if (text_tag_list_size(tags) > 0) {
    struct text_tag *tag = text_tag_list_get(tags, 0);
    assert_string_equal(text_tag_color_foreground(tag), "#FF0000");
    assert_string_equal(text_tag_color_background(tag), "#0000FF");
  }

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with only background option */
static void test_featured_text_to_plain_text_color_only_bg_option(void **state)
{
  const char *input = "[c bg=\"#00FF00\"]Text[/c]";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Text");
  assert_non_null(tags);

  if (text_tag_list_size(tags) > 0) {
    struct text_tag *tag = text_tag_list_get(tags, 0);
    assert_string_equal(text_tag_color_foreground(tag), "");
    assert_string_equal(text_tag_color_background(tag), "#00FF00");
  }

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with link pair having content */
static void test_featured_text_to_plain_text_link_pair_content(void **state)
{
  const char *input = "[l tgt=\"city\" id=1]Click here[/l]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Click here");
  assert_non_null(tags);
  assert_int_equal(text_tag_list_size(tags), 1);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with city link missing name */
static void test_featured_text_to_plain_text_city_missing_name(void **state)
{
  const char *input = "[l tgt=\"city\" id=5 /]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_non_null(tags);
  if (text_tag_list_size(tags) > 0) {
    struct text_tag *tag = text_tag_list_get(tags, 0);
    assert_int_equal(text_tag_link_id(tag), 5);
  }

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with unit link missing name */
static void test_featured_text_to_plain_text_unit_missing_name(void **state)
{
  const char *input = "[l tgt=\"unit\" id=10 /]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_non_null(tags);
  if (text_tag_list_size(tags) > 0) {
    struct text_tag *tag = text_tag_list_get(tags, 0);
    assert_int_equal(text_tag_link_id(tag), 10);
  }

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with negative id values */
static void test_featured_text_to_plain_text_negative_id(void **state)
{
  const char *input = "[l tgt=\"city\" id=-1 name=\"Invalid\" /]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_non_null(tags);
  if (text_tag_list_size(tags) > 0) {
    struct text_tag *tag = text_tag_list_get(tags, 0);
    assert_int_equal(text_tag_link_id(tag), -1);
  }

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with large id values */
static void test_featured_text_to_plain_text_large_id(void **state)
{
  const char *input = "[l tgt=\"unit\" id=999999 name=\"LargeID\" /]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_non_null(tags);
  if (text_tag_list_size(tags) > 0) {
    struct text_tag *tag = text_tag_list_get(tags, 0);
    assert_int_equal(text_tag_link_id(tag), 999999);
  }

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with negative tile coordinates */
static void test_featured_text_to_plain_text_negative_tile_coords(void **state)
{
  const char *input = "[l tgt=\"tile\" x=-1 y=-1 /]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_non_null(tags);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with zero tile coordinates */
static void test_featured_text_to_plain_text_zero_tile_coords(void **state)
{
  const char *input = "[l tgt=\"tile\" x=0 y=0 /]";
  char output[128];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_non_null(tags);

  text_tag_list_destroy(tags);
}

/* Test featured_text_apply_tag() with FT_OFFSET_UNSET for both */
static void test_featured_text_apply_tag_both_unset(void **state)
{
  const char *input = "Hello";
  char output[64];
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_BOLD, FT_OFFSET_UNSET, FT_OFFSET_UNSET);
  assert_int_equal(len, 0);
}

/* Test featured_text_to_plain_text() with mixed case tag names */
static void test_featured_text_to_plain_text_mixed_case(void **state)
{
  const char *input = "[BoLd]Hello[/bOlD]";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_string_equal(output, "Hello");
  assert_non_null(tags);
  assert_int_equal(text_tag_list_size(tags), 1);

  text_tag_list_destroy(tags);
}

/* Test featured_text_to_plain_text() with mixed case color options */
static void test_featured_text_to_plain_text_mixed_case_options(void **state)
{
  const char *input = "[c FG=\"#FF0000\" BG=\"#0000FF\"]Text[/c]";
  char output[64];
  struct text_tag_list *tags = NULL;

  (void) state;

  featured_text_to_plain_text(input, output, sizeof(output), &tags, FALSE);
  assert_non_null(tags);

  text_tag_list_destroy(tags);
}

/* Test text_tag_list_destroy with NULL */
static void test_text_tag_list_destroy_null(void **state)
{
  (void) state;

  text_tag_list_destroy(NULL);
}

/***********************************************************************
  Link function tests - these tests cover city_link, tile_link, unit_link,
  city_tile_link, and unit_tile_link functions
***********************************************************************/

/* Test city_link() basic functionality */
static void test_city_link_basic(void **state)
{
  struct city test_city;
  const char *link;

  (void) state;

  /* Setup minimal city data */
  memset(&test_city, 0, sizeof(test_city));
  test_city.id = 123;
  test_city.name = "TestCity";
  test_city.tile = NULL;  /* No tile for basic test */

  link = city_link(&test_city);

  /* Verify link contains expected components */
  assert_non_null(link);
  assert_true(strstr(link, "l") != NULL);  /* link type short name */
  assert_true(strstr(link, "tgt=\"city\"") != NULL);
  assert_true(strstr(link, "id=123") != NULL);
  assert_true(strstr(link, "name=\"TestCity\"") != NULL);
}

/* Test tile_link() basic functionality
 * Skip - requires valid map state for TILE_XY macro */
static void test_tile_link_basic(void **state)
{
  (void) state;
  /* Skip - requires full map initialization */
  skip();
}

/* Test unit_link() basic functionality */
static void test_unit_link_basic(void **state)
{
  struct unit test_unit;
  struct unit_type test_utype;
  const char *link;

  (void) state;

  /* Setup minimal unit data */
  memset(&test_unit, 0, sizeof(test_unit));
  memset(&test_utype, 0, sizeof(test_utype));
  test_unit.id = 456;
  test_unit.utype = &test_utype;

  link = unit_link(&test_unit);

  /* Verify link contains expected components */
  assert_non_null(link);
  assert_true(strstr(link, "l") != NULL);  /* link type short name */
  assert_true(strstr(link, "tgt=\"unit\"") != NULL);
  assert_true(strstr(link, "id=456") != NULL);
}

/* Test city_tile_link() basic functionality
 * Skip - requires valid map state for TILE_XY macro */
static void test_city_tile_link_basic(void **state)
{
  (void) state;
  /* Skip - requires full map initialization */
  skip();
}

/* Test unit_tile_link() basic functionality
 * Skip - requires valid map state for TILE_XY macro */
static void test_unit_tile_link_basic(void **state)
{
  (void) state;
  /* Skip - requires full map initialization */
  skip();
}

/* Test featured_text_apply_tag() with color empty strings */
static void test_featured_text_apply_tag_color_empty_strings(void **state)
{
  const char *input = "Hello";
  char output[64];
  struct ft_color color = FT_COLOR("", "");
  size_t len;

  (void) state;

  len = featured_text_apply_tag(input, output, sizeof(output), TTT_COLOR, 0, 5, color);
  assert_int_equal(len, 0);
}

/* Test text_tag_new() with FT_OFFSET_UNSET for both offsets */
static void test_text_tag_new_both_offsets_unset(void **state)
{
  struct text_tag *tag;

  (void) state;

  tag = text_tag_new(TTT_BOLD, FT_OFFSET_UNSET, FT_OFFSET_UNSET);
  assert_non_null(tag);
  assert_int_equal(text_tag_start_offset(tag), FT_OFFSET_UNSET);
  assert_int_equal(text_tag_stop_offset(tag), FT_OFFSET_UNSET);

  text_tag_destroy(tag);
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

    /* Additional featured_text_to_plain_text tests */
    cmocka_unit_test(test_featured_text_to_plain_text_strike),
    cmocka_unit_test(test_featured_text_to_plain_text_underline),
    cmocka_unit_test(test_featured_text_to_plain_text_color_bg),
    cmocka_unit_test(test_featured_text_to_plain_text_multiple),
    cmocka_unit_test(test_featured_text_to_plain_text_nested),
    cmocka_unit_test(test_featured_text_to_plain_text_unclosed),
    cmocka_unit_test(test_featured_text_to_plain_text_full_names),
    cmocka_unit_test(test_featured_text_to_plain_text_empty),
    cmocka_unit_test(test_featured_text_to_plain_text_case_insensitive),
    cmocka_unit_test(test_featured_text_to_plain_text_link_single),
    cmocka_unit_test(test_featured_text_to_plain_text_link_unit),
    cmocka_unit_test(test_featured_text_to_plain_text_link_pair),

    /* Additional featured_text_apply_tag tests */
    cmocka_unit_test(test_featured_text_apply_tag_stop_before_start),
    cmocka_unit_test(test_featured_text_apply_tag_unset_stop),
    cmocka_unit_test(test_featured_text_apply_tag_color_none),
    cmocka_unit_test(test_featured_text_apply_tag_link_city_null),
    cmocka_unit_test(test_featured_text_apply_tag_link_tile_null),
    cmocka_unit_test(test_featured_text_apply_tag_link_unit_null),

    /* text_tag_list tests */
    cmocka_unit_test(test_text_tag_list_basic),
    cmocka_unit_test(test_text_tag_list_append),
    cmocka_unit_test(test_text_tag_list_copy),

    /* More predefined colors tests */
    cmocka_unit_test(test_predefined_color_any),
    cmocka_unit_test(test_predefined_color_log),
    cmocka_unit_test(test_predefined_color_client),
    cmocka_unit_test(test_predefined_color_editor),
    cmocka_unit_test(test_predefined_color_command),
    cmocka_unit_test(test_predefined_color_changed),
    cmocka_unit_test(test_predefined_color_server_prompt),
    cmocka_unit_test(test_predefined_color_player_lost),
    cmocka_unit_test(test_predefined_color_game_start),
    cmocka_unit_test(test_predefined_color_chat_ally),
    cmocka_unit_test(test_predefined_color_chat_private),
    cmocka_unit_test(test_predefined_color_chat_luaconsole),
    cmocka_unit_test(test_predefined_color_vote_public),
    cmocka_unit_test(test_predefined_color_vote_team),
    cmocka_unit_test(test_predefined_color_vote_passed),
    cmocka_unit_test(test_predefined_color_vote_failed),
    cmocka_unit_test(test_predefined_color_vote_yes),
    cmocka_unit_test(test_predefined_color_vote_no),
    cmocka_unit_test(test_predefined_color_vote_abstain),
    cmocka_unit_test(test_predefined_color_luaconsole_input),
    cmocka_unit_test(test_predefined_color_luaconsole_error),
    cmocka_unit_test(test_predefined_color_luaconsole_warn),
    cmocka_unit_test(test_predefined_color_luaconsole_normal),
    cmocka_unit_test(test_predefined_color_luaconsole_verbose),
    cmocka_unit_test(test_predefined_color_luaconsole_debug),

    /* Link function tests - city_link, tile_link, unit_link, etc. */
    cmocka_unit_test(test_city_link_basic),
    cmocka_unit_test(test_tile_link_basic),
    cmocka_unit_test(test_unit_link_basic),
    cmocka_unit_test(test_city_tile_link_basic),
    cmocka_unit_test(test_unit_tile_link_basic),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}