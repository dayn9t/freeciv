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
#include "mapimg.h"
#include "tile.h"

/* Mock callback functions for mapimg_init */
static enum known_type mock_tile_known(const struct tile *ptile,
                                       const struct player *pplayer,
                                       bool knowledge)
{
  (void) ptile;
  (void) pplayer;
  (void) knowledge;
  return TILE_KNOWN_SEEN;
}

static struct terrain *mock_tile_terrain(const struct tile *ptile,
                                         const struct player *pplayer,
                                         bool knowledge)
{
  (void) ptile;
  (void) pplayer;
  (void) knowledge;
  return NULL;
}

static struct player *mock_tile_player(const struct tile *ptile,
                                       const struct player *pplayer,
                                       bool knowledge)
{
  (void) ptile;
  (void) pplayer;
  (void) knowledge;
  return NULL;
}

static int mock_plrcolor_count(void)
{
  return 0;
}

static struct rgbcolor *mock_plrcolor_get(int id)
{
  (void) id;
  return NULL;
}

/* Test: mapimg_initialised returns FALSE before initialization */
static void test_mapimg_initialised_before_init(void **state)
{
  bool result;

  (void) state;

  /* If already initialized, free first to get clean state */
  if (mapimg_initialised()) {
    mapimg_free();
  }

  result = mapimg_initialised();
  assert_false(result);
}

/* Test: mapimg_init initializes the subsystem */
static void test_mapimg_init(void **state)
{
  bool initialised;
  int count;

  (void) state;

  /* Ensure clean state */
  if (mapimg_initialised()) {
    mapimg_free();
  }

  /* Initialize */
  mapimg_init(mock_tile_known,
              mock_tile_terrain,
              mock_tile_player,
              mock_tile_player,
              mock_tile_player,
              mock_plrcolor_count,
              mock_plrcolor_get);

  initialised = mapimg_initialised();
  assert_true(initialised);

  /* After init, count should be 0 (no map definitions) */
  count = mapimg_count();
  assert_int_equal(count, 0);

  /* Clean up */
  mapimg_free();
}

/* Test: mapimg_init is idempotent (calling twice does nothing) */
static void test_mapimg_init_idempotent(void **state)
{
  bool initialised;

  (void) state;

  /* Ensure clean state */
  if (mapimg_initialised()) {
    mapimg_free();
  }

  /* Initialize twice */
  mapimg_init(mock_tile_known,
              mock_tile_terrain,
              mock_tile_player,
              mock_tile_player,
              mock_tile_player,
              mock_plrcolor_count,
              mock_plrcolor_get);

  mapimg_init(mock_tile_known,
              mock_tile_terrain,
              mock_tile_player,
              mock_tile_player,
              mock_tile_player,
              mock_plrcolor_count,
              mock_plrcolor_get);

  initialised = mapimg_initialised();
  assert_true(initialised);

  /* Clean up */
  mapimg_free();
}

/* Test: mapimg_free works correctly */
static void test_mapimg_free(void **state)
{
  bool initialised;

  (void) state;

  /* Ensure clean state */
  if (mapimg_initialised()) {
    mapimg_free();
  }

  /* Initialize then free */
  mapimg_init(mock_tile_known,
              mock_tile_terrain,
              mock_tile_player,
              mock_tile_player,
              mock_tile_player,
              mock_plrcolor_count,
              mock_plrcolor_get);

  mapimg_free();

  initialised = mapimg_initialised();
  assert_false(initialised);
}

/* Test: mapimg_free is safe to call when not initialized */
static void test_mapimg_free_uninitialized(void **state)
{
  (void) state;

  /* Ensure clean state */
  if (mapimg_initialised()) {
    mapimg_free();
  }

  /* Calling free on uninitialized subsystem should not crash */
  mapimg_free();

  /* Should still be uninitialized */
  assert_false(mapimg_initialised());
}

/* Test: mapimg_count returns 0 when not initialized */
static void test_mapimg_count_uninitialized(void **state)
{
  int count;

  (void) state;

  /* Ensure clean state */
  if (mapimg_initialised()) {
    mapimg_free();
  }

  count = mapimg_count();
  assert_int_equal(count, 0);
}

/* Test: mapimg_count returns correct value after init */
static void test_mapimg_count_after_init(void **state)
{
  int count;

  (void) state;

  /* Ensure clean state */
  if (mapimg_initialised()) {
    mapimg_free();
  }

  mapimg_init(mock_tile_known,
              mock_tile_terrain,
              mock_tile_player,
              mock_tile_player,
              mock_tile_player,
              mock_plrcolor_count,
              mock_plrcolor_get);

  count = mapimg_count();
  assert_int_equal(count, 0);

  /* Clean up */
  mapimg_free();
}

/* Test: mapimg_reset is safe when not initialized */
static void test_mapimg_reset_uninitialized(void **state)
{
  (void) state;

  /* Ensure clean state */
  if (mapimg_initialised()) {
    mapimg_free();
  }

  /* Calling reset on uninitialized subsystem should not crash */
  mapimg_reset();

  /* Should still be uninitialized */
  assert_false(mapimg_initialised());
}

/* Test: mapimg_reset clears map definitions */
static void test_mapimg_reset(void **state)
{
  int count;

  (void) state;

  /* Ensure clean state */
  if (mapimg_initialised()) {
    mapimg_free();
  }

  mapimg_init(mock_tile_known,
              mock_tile_terrain,
              mock_tile_player,
              mock_tile_player,
              mock_tile_player,
              mock_plrcolor_count,
              mock_plrcolor_get);

  /* Reset should keep subsystem initialized but clear definitions */
  mapimg_reset();

  /* Should still be initialized */
  assert_true(mapimg_initialised());

  /* Count should still be 0 */
  count = mapimg_count();
  assert_int_equal(count, 0);

  /* Clean up */
  mapimg_free();
}

/* Test: mapimg_error returns NULL or empty when no error */
static void test_mapimg_error_no_error(void **state)
{
  const char *error;

  (void) state;

  /* Ensure clean state */
  if (mapimg_initialised()) {
    mapimg_free();
  }

  mapimg_init(mock_tile_known,
              mock_tile_terrain,
              mock_tile_player,
              mock_tile_player,
              mock_tile_player,
              mock_plrcolor_count,
              mock_plrcolor_get);

  error = mapimg_error();
  /* Error should be NULL or empty string when no error has occurred */
  assert_true(error == NULL || error[0] == '\0');

  /* Clean up */
  mapimg_free();
}

/* Test: init-free cycle can be repeated */
static void test_mapimg_init_free_cycle(void **state)
{
  (void) state;

  /* Ensure clean state */
  if (mapimg_initialised()) {
    mapimg_free();
  }

  /* First cycle */
  mapimg_init(mock_tile_known,
              mock_tile_terrain,
              mock_tile_player,
              mock_tile_player,
              mock_tile_player,
              mock_plrcolor_count,
              mock_plrcolor_get);
  assert_true(mapimg_initialised());
  mapimg_free();
  assert_false(mapimg_initialised());

  /* Second cycle */
  mapimg_init(mock_tile_known,
              mock_tile_terrain,
              mock_tile_player,
              mock_tile_player,
              mock_tile_player,
              mock_plrcolor_count,
              mock_plrcolor_get);
  assert_true(mapimg_initialised());
  mapimg_free();
  assert_false(mapimg_initialised());
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_mapimg_initialised_before_init),
    cmocka_unit_test(test_mapimg_init),
    cmocka_unit_test(test_mapimg_init_idempotent),
    cmocka_unit_test(test_mapimg_free),
    cmocka_unit_test(test_mapimg_free_uninitialized),
    cmocka_unit_test(test_mapimg_count_uninitialized),
    cmocka_unit_test(test_mapimg_count_after_init),
    cmocka_unit_test(test_mapimg_reset_uninitialized),
    cmocka_unit_test(test_mapimg_reset),
    cmocka_unit_test(test_mapimg_error_no_error),
    cmocka_unit_test(test_mapimg_init_free_cycle),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}