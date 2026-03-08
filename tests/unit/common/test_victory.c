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
#include "victory.h"
#include "game.h"

extern struct civ_game game;

static int setup(void **state)
{
  (void) state;
  memset(&game, 0, sizeof(game));
  return 0;
}

static int teardown(void **state)
{
  (void) state;
  return 0;
}

static void test_victory_enabled_none(void **state)
{
  (void) state;

  game.info.victory_conditions = 0;

  assert_false(victory_enabled(VC_ALLIED));
  assert_false(victory_enabled(VC_SPACERACE));
  assert_false(victory_enabled(VC_CULTURE));
  assert_false(victory_enabled(VC_WORLDPEACE));
}

static void test_victory_enabled_allied(void **state)
{
  (void) state;

  game.info.victory_conditions = (1 << VC_ALLIED);

  assert_true(victory_enabled(VC_ALLIED));
  assert_false(victory_enabled(VC_SPACERACE));
}

static void test_victory_enabled_spacerace(void **state)
{
  (void) state;

  game.info.victory_conditions = (1 << VC_SPACERACE);

  assert_false(victory_enabled(VC_ALLIED));
  assert_true(victory_enabled(VC_SPACERACE));
}

static void test_victory_enabled_multiple(void **state)
{
  (void) state;

  game.info.victory_conditions = (1 << VC_ALLIED) | (1 << VC_SPACERACE);

  assert_true(victory_enabled(VC_ALLIED));
  assert_true(victory_enabled(VC_SPACERACE));
  assert_false(victory_enabled(VC_CULTURE));
}

static void test_victory_enabled_all(void **state)
{
  (void) state;

  game.info.victory_conditions = (1 << VC_ALLIED) |
                                  (1 << VC_SPACERACE) |
                                  (1 << VC_CULTURE) |
                                  (1 << VC_WORLDPEACE);

  assert_true(victory_enabled(VC_ALLIED));
  assert_true(victory_enabled(VC_SPACERACE));
  assert_true(victory_enabled(VC_CULTURE));
  assert_true(victory_enabled(VC_WORLDPEACE));
}

static void test_victory_enabled_culture(void **state)
{
  (void) state;

  game.info.victory_conditions = (1 << VC_CULTURE);

  assert_true(victory_enabled(VC_CULTURE));
  assert_false(victory_enabled(VC_ALLIED));
  assert_false(victory_enabled(VC_SPACERACE));
  assert_false(victory_enabled(VC_WORLDPEACE));
}

static void test_victory_enabled_worldpeace(void **state)
{
  (void) state;

  game.info.victory_conditions = (1 << VC_WORLDPEACE);

  assert_true(victory_enabled(VC_WORLDPEACE));
  assert_false(victory_enabled(VC_ALLIED));
  assert_false(victory_enabled(VC_SPACERACE));
  assert_false(victory_enabled(VC_CULTURE));
}

static void test_victory_enabled_culture_worldpeace(void **state)
{
  (void) state;

  game.info.victory_conditions = (1 << VC_CULTURE) | (1 << VC_WORLDPEACE);

  assert_true(victory_enabled(VC_CULTURE));
  assert_true(victory_enabled(VC_WORLDPEACE));
  assert_false(victory_enabled(VC_ALLIED));
  assert_false(victory_enabled(VC_SPACERACE));
}

static void test_victory_enabled_spacerace_culture(void **state)
{
  (void) state;

  game.info.victory_conditions = (1 << VC_SPACERACE) | (1 << VC_CULTURE);

  assert_true(victory_enabled(VC_SPACERACE));
  assert_true(victory_enabled(VC_CULTURE));
  assert_false(victory_enabled(VC_ALLIED));
  assert_false(victory_enabled(VC_WORLDPEACE));
}

static void test_victory_enabled_allied_worldpeace(void **state)
{
  (void) state;

  game.info.victory_conditions = (1 << VC_ALLIED) | (1 << VC_WORLDPEACE);

  assert_true(victory_enabled(VC_ALLIED));
  assert_true(victory_enabled(VC_WORLDPEACE));
  assert_false(victory_enabled(VC_SPACERACE));
  assert_false(victory_enabled(VC_CULTURE));
}

static void test_victory_enabled_three_conditions(void **state)
{
  (void) state;

  /* Test with three conditions enabled, one disabled */
  game.info.victory_conditions = (1 << VC_SPACERACE) |
                                  (1 << VC_ALLIED) |
                                  (1 << VC_CULTURE);

  assert_true(victory_enabled(VC_SPACERACE));
  assert_true(victory_enabled(VC_ALLIED));
  assert_true(victory_enabled(VC_CULTURE));
  assert_false(victory_enabled(VC_WORLDPEACE));

  /* Different combination */
  game.info.victory_conditions = (1 << VC_SPACERACE) |
                                  (1 << VC_ALLIED) |
                                  (1 << VC_WORLDPEACE);

  assert_true(victory_enabled(VC_SPACERACE));
  assert_true(victory_enabled(VC_ALLIED));
  assert_true(victory_enabled(VC_WORLDPEACE));
  assert_false(victory_enabled(VC_CULTURE));
}

static void test_victory_enabled_default_conditions(void **state)
{
  (void) state;

  /* Test default victory conditions as defined in game.h */
  game.info.victory_conditions = GAME_DEFAULT_VICTORY_CONDITIONS;

  assert_true(victory_enabled(VC_SPACERACE));
  assert_true(victory_enabled(VC_ALLIED));
  assert_false(victory_enabled(VC_CULTURE));
  assert_false(victory_enabled(VC_WORLDPEACE));
}

static void test_victory_enabled_toggle_conditions(void **state)
{
  (void) state;

  /* Start with no conditions */
  game.info.victory_conditions = 0;
  assert_false(victory_enabled(VC_ALLIED));
  assert_false(victory_enabled(VC_SPACERACE));
  assert_false(victory_enabled(VC_CULTURE));
  assert_false(victory_enabled(VC_WORLDPEACE));

  /* Enable SPACERACE */
  game.info.victory_conditions |= (1 << VC_SPACERACE);
  assert_true(victory_enabled(VC_SPACERACE));
  assert_false(victory_enabled(VC_ALLIED));

  /* Enable ALLIED */
  game.info.victory_conditions |= (1 << VC_ALLIED);
  assert_true(victory_enabled(VC_SPACERACE));
  assert_true(victory_enabled(VC_ALLIED));

  /* Disable SPACERACE */
  game.info.victory_conditions &= ~(1 << VC_SPACERACE);
  assert_false(victory_enabled(VC_SPACERACE));
  assert_true(victory_enabled(VC_ALLIED));

  /* Enable all */
  game.info.victory_conditions |= (1 << VC_SPACERACE) |
                                   (1 << VC_CULTURE) |
                                   (1 << VC_WORLDPEACE);
  assert_true(victory_enabled(VC_SPACERACE));
  assert_true(victory_enabled(VC_ALLIED));
  assert_true(victory_enabled(VC_CULTURE));
  assert_true(victory_enabled(VC_WORLDPEACE));

  /* Disable all */
  game.info.victory_conditions = 0;
  assert_false(victory_enabled(VC_SPACERACE));
  assert_false(victory_enabled(VC_ALLIED));
  assert_false(victory_enabled(VC_CULTURE));
  assert_false(victory_enabled(VC_WORLDPEACE));
}

static void test_victory_enabled_bit_values(void **state)
{
  (void) state;

  /* Verify enum values match expected bit positions */
  assert_int_equal(VC_SPACERACE, 0);
  assert_int_equal(VC_ALLIED, 1);
  assert_int_equal(VC_CULTURE, 2);
  assert_int_equal(VC_WORLDPEACE, 3);

  /* Verify bit shift values */
  assert_int_equal((1 << VC_SPACERACE), 1);
  assert_int_equal((1 << VC_ALLIED), 2);
  assert_int_equal((1 << VC_CULTURE), 4);
  assert_int_equal((1 << VC_WORLDPEACE), 8);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_victory_enabled_none, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_allied, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_spacerace, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_culture, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_worldpeace, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_multiple, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_culture_worldpeace, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_spacerace_culture, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_allied_worldpeace, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_three_conditions, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_all, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_default_conditions, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_toggle_conditions, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_bit_values, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
