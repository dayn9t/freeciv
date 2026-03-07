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

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_victory_enabled_none, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_allied, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_spacerace, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_multiple, setup, teardown),
    cmocka_unit_test_setup_teardown(test_victory_enabled_all, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
