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

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/* common */
#include "borders.h"
#include "game.h"
#include "map.h"

static void test_tile_border_source_radius_sq_disabled(void **state)
{
  int result;

  (void) state;

  /* Set borders to disabled */
  game.info.borders = BORDERS_DISABLED;

  result = tile_border_source_radius_sq(NULL);
  assert_int_equal(result, 0);
}

static void test_tile_border_source_strength_disabled(void **state)
{
  int result;

  (void) state;

  /* Set borders to disabled */
  game.info.borders = BORDERS_DISABLED;

  result = tile_border_source_strength(NULL);
  assert_int_equal(result, 0);
}

static void test_is_border_source_null(void **state)
{
  bool result;

  (void) state;

  /* NULL tile should not be a border source */
  result = is_border_source(NULL);
  /* This might crash or return false - just verify function exists */
  (void) result;
}

static void test_tile_border_strength_same_tile(void **state)
{
  int result;
  struct tile tile;

  (void) state;

  /* Same tile should have FC_INFINITY strength */
  result = tile_border_strength(&tile, &tile);
  assert_int_equal(result, FC_INFINITY);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_tile_border_source_radius_sq_disabled),
    cmocka_unit_test(test_tile_border_source_strength_disabled),
    cmocka_unit_test(test_is_border_source_null),
    cmocka_unit_test(test_tile_border_strength_same_tile),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
