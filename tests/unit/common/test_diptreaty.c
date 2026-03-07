/***********************************************************************
 Freeciv - Copyright (C) 2025 - The Freeciv Project
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

/* utility */
#include "mem.h"
#include "shared.h"

/* common */
#include "game.h"
#include "player.h"
#include "diptreaty.h"

/* Test data */
static struct player test_player0;
static struct player test_player1;
static struct player_diplstate test_diplstate0;
static struct player_diplstate test_diplstate1;
static const struct player_diplstate *test_diplstates0[MAX_NUM_PLAYER_SLOTS];
static const struct player_diplstate *test_diplstates1[MAX_NUM_PLAYER_SLOTS];

/***********************************************************************
  Setup function - initialize minimal game state
***********************************************************************/
static int setup(void **state)
{
  (void) state;

  /* Initialize game state */
  memset(&game, 0, sizeof(game));
  game.info.diplomacy = DIPLO_FOR_ALL;
  game.info.trading_gold = TRUE;
  game.info.trading_tech = TRUE;
  game.info.trading_city = TRUE;

  /* Initialize player 0 */
  memset(&test_player0, 0, sizeof(test_player0));
  test_player0.slot = NULL;
  sz_strlcpy(test_player0.name, "Player0");
  test_player0.is_alive = TRUE;
  BV_CLR_ALL(test_player0.flags);

  /* Initialize player 1 */
  memset(&test_player1, 0, sizeof(test_player1));
  test_player1.slot = NULL;
  sz_strlcpy(test_player1.name, "Player1");
  test_player1.is_alive = TRUE;
  BV_CLR_ALL(test_player1.flags);

  /* Initialize diplomatic states */
  memset(&test_diplstate0, 0, sizeof(test_diplstate0));
  test_diplstate0.type = DS_NO_CONTACT;
  test_diplstate0.max_state = DS_NO_CONTACT;
  test_diplstate0.turns_left = 0;
  test_diplstate0.has_reason_to_cancel = 0;
  test_diplstate0.contact_turns_left = 0;
  test_diplstate0.auto_cancel_turn = -1;

  memset(&test_diplstate1, 0, sizeof(test_diplstate1));
  test_diplstate1.type = DS_NO_CONTACT;
  test_diplstate1.max_state = DS_NO_CONTACT;
  test_diplstate1.turns_left = 0;
  test_diplstate1.has_reason_to_cancel = 0;
  test_diplstate1.contact_turns_left = 0;
  test_diplstate1.auto_cancel_turn = -1;

  /* Set up diplstates arrays */
  memset(test_diplstates0, 0, sizeof(test_diplstates0));
  memset(test_diplstates1, 0, sizeof(test_diplstates1));
  test_diplstates0[0] = &test_diplstate0;
  test_diplstates1[1] = &test_diplstate1;
  test_player0.diplstates = (const struct player_diplstate **)test_diplstates0;
  test_player1.diplstates = (const struct player_diplstate **)test_diplstates1;

  /* Initialize clause infos */
  clause_infos_init();

  return 0;
}

/***********************************************************************
  Teardown function - cleanup
***********************************************************************/
static int teardown(void **state)
{
  (void) state;

  clause_infos_free();

  return 0;
}

/***********************************************************************
  Test init_treaty basic functionality
***********************************************************************/
static void test_init_treaty_basic(void **state)
{
  (void) state;

  struct treaty test_treaty;

  init_treaty(&test_treaty, &test_player0, &test_player1);

  assert_ptr_equal(test_treaty.plr0, &test_player0);
  assert_ptr_equal(test_treaty.plr1, &test_player1);
  assert_false(test_treaty.accept0);
  assert_false(test_treaty.accept1);
  assert_non_null(test_treaty.clauses);

  clear_treaty(&test_treaty);
}

/***********************************************************************
  Test clear_treaty with empty treaty
***********************************************************************/
static void test_clear_treaty_empty(void **state)
{
  (void) state;

  struct treaty test_treaty;

  init_treaty(&test_treaty, &test_player0, &test_player1);
  clear_treaty(&test_treaty);

  /* Should not crash - treaty is cleaned up */
}

/***********************************************************************
  Test clause_infos_init and clause_infos_free
***********************************************************************/
static void test_clause_infos_init_free(void **state)
{
  (void) state;

  /* Already called in setup - just verify clause_info_get works */
  struct clause_info *info = clause_info_get(CLAUSE_ADVANCE);
  assert_non_null(info);
  assert_int_equal(info->type, CLAUSE_ADVANCE);
}

/***********************************************************************
  Test clause_info_get for all clause types
***********************************************************************/
static void test_clause_info_get_all_types(void **state)
{
  (void) state;

  for (int i = 0; i < CLAUSE_COUNT; i++) {
    struct clause_info *info = clause_info_get(i);
    assert_non_null(info);
    assert_int_equal(info->type, i);
  }
}

/***********************************************************************
  Test clause_enabled when disabled by default
***********************************************************************/
static void test_clause_enabled_disabled(void **state)
{
  (void) state;

  /* Clause types are disabled by default after clause_infos_init */
  assert_false(clause_enabled(CLAUSE_ADVANCE));
  assert_false(clause_enabled(CLAUSE_GOLD));
  assert_false(clause_enabled(CLAUSE_CITY));
}

/***********************************************************************
  Test clause_enabled when trading is disabled
***********************************************************************/
static void test_clause_enabled_trading_disabled(void **state)
{
  (void) state;

  struct clause_info *info;

  /* Enable all clause types */
  for (int i = 0; i < CLAUSE_COUNT; i++) {
    info = clause_info_get(i);
    info->enabled = TRUE;
  }

  /* Test trading flags */
  game.info.trading_gold = FALSE;
  assert_false(clause_enabled(CLAUSE_GOLD));

  game.info.trading_gold = TRUE;
  assert_true(clause_enabled(CLAUSE_GOLD));

  game.info.trading_tech = FALSE;
  assert_false(clause_enabled(CLAUSE_ADVANCE));

  game.info.trading_tech = TRUE;
  assert_true(clause_enabled(CLAUSE_ADVANCE));

  game.info.trading_city = FALSE;
  assert_false(clause_enabled(CLAUSE_CITY));

  game.info.trading_city = TRUE;
  assert_true(clause_enabled(CLAUSE_CITY));

  /* Reset to disabled for other tests */
  for (int i = 0; i < CLAUSE_COUNT; i++) {
    info = clause_info_get(i);
    info->enabled = FALSE;
  }
  game.info.trading_gold = TRUE;
  game.info.trading_tech = TRUE;
  game.info.trading_city = TRUE;
}

/***********************************************************************
  Test is_pact_clause macro
***********************************************************************/
static void test_is_pact_clause(void **state)
{
  (void) state;

  assert_true(is_pact_clause(CLAUSE_CEASEFIRE));
  assert_true(is_pact_clause(CLAUSE_PEACE));
  assert_true(is_pact_clause(CLAUSE_ALLIANCE));
  assert_false(is_pact_clause(CLAUSE_ADVANCE));
  assert_false(is_pact_clause(CLAUSE_GOLD));
  assert_false(is_pact_clause(CLAUSE_MAP));
  assert_false(is_pact_clause(CLAUSE_SEAMAP));
  assert_false(is_pact_clause(CLAUSE_CITY));
  assert_false(is_pact_clause(CLAUSE_VISION));
  assert_false(is_pact_clause(CLAUSE_EMBASSY));
  assert_false(is_pact_clause(CLAUSE_SHARED_TILES));
}

/***********************************************************************
  Test clause_type values
***********************************************************************/
static void test_clause_type_values(void **state)
{
  (void) state;

  assert_int_equal(CLAUSE_ADVANCE, 0);
  assert_int_equal(CLAUSE_GOLD, 1);
  assert_int_equal(CLAUSE_MAP, 2);
  assert_int_equal(CLAUSE_SEAMAP, 3);
  assert_int_equal(CLAUSE_CITY, 4);
  assert_int_equal(CLAUSE_CEASEFIRE, 5);
  assert_int_equal(CLAUSE_PEACE, 6);
  assert_int_equal(CLAUSE_ALLIANCE, 7);
  assert_int_equal(CLAUSE_VISION, 8);
  assert_int_equal(CLAUSE_EMBASSY, 9);
  assert_int_equal(CLAUSE_SHARED_TILES, 10);
  assert_true(CLAUSE_COUNT > CLAUSE_SHARED_TILES);
}

/***********************************************************************
  Test treaties_init and treaties_free
***********************************************************************/
static void test_treaties_init_free(void **state)
{
  (void) state;

  treaties_init();
  treaties_free();

  /* Should not crash */
}

/***********************************************************************
  Test treaty_add and treaty_remove
***********************************************************************/
static void test_treaty_add_remove(void **state)
{
  (void) state;

  struct treaty *test_treaty;

  treaties_init();

  /* Allocate treaty on heap since treaty_remove calls free() */
  test_treaty = fc_malloc(sizeof(struct treaty));
  init_treaty(test_treaty, &test_player0, &test_player1);
  treaty_add(test_treaty);

  /* Find the treaty */
  struct treaty *found = find_treaty(&test_player0, &test_player1);
  assert_non_null(found);
  assert_ptr_equal(found, test_treaty);

  /* Find with reversed players */
  found = find_treaty(&test_player1, &test_player0);
  assert_non_null(found);
  assert_ptr_equal(found, test_treaty);

  treaty_remove(test_treaty);

  /* Treaty should be gone */
  found = find_treaty(&test_player0, &test_player1);
  assert_null(found);

  treaties_free();
}

/***********************************************************************
  Test find_treaty returns NULL when not found
***********************************************************************/
static void test_find_treaty_not_found(void **state)
{
  (void) state;

  treaties_init();

  struct treaty *found = find_treaty(&test_player0, &test_player1);
  assert_null(found);

  treaties_free();
}

/***********************************************************************
  Test remove_clause from empty treaty
***********************************************************************/
static void test_remove_clause_empty(void **state)
{
  (void) state;

  struct treaty test_treaty;

  init_treaty(&test_treaty, &test_player0, &test_player1);

  bool result = remove_clause(&test_treaty, &test_player0, CLAUSE_GOLD, 100);
  assert_false(result);

  clear_treaty(&test_treaty);
}

/***********************************************************************
  Test treaty structure fields
***********************************************************************/
static void test_treaty_structure_fields(void **state)
{
  (void) state;

  struct treaty test_treaty;

  init_treaty(&test_treaty, &test_player0, &test_player1);

  /* Verify initial accept states */
  test_treaty.accept0 = TRUE;
  test_treaty.accept1 = TRUE;
  assert_true(test_treaty.accept0);
  assert_true(test_treaty.accept1);

  /* Reset for cleanup */
  test_treaty.accept0 = FALSE;
  test_treaty.accept1 = FALSE;

  clear_treaty(&test_treaty);
}

/***********************************************************************
  Test Clause structure
***********************************************************************/
static void test_clause_structure(void **state)
{
  (void) state;

  struct Clause test_clause;

  test_clause.type = CLAUSE_GOLD;
  test_clause.from = &test_player0;
  test_clause.value = 100;

  assert_int_equal(test_clause.type, CLAUSE_GOLD);
  assert_ptr_equal(test_clause.from, &test_player0);
  assert_int_equal(test_clause.value, 100);
}

/***********************************************************************
  Main test function
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_init_treaty_basic,
                                    setup, teardown),
    cmocka_unit_test_setup_teardown(test_clear_treaty_empty,
                                    setup, teardown),
    cmocka_unit_test_setup_teardown(test_clause_infos_init_free,
                                    setup, teardown),
    cmocka_unit_test_setup_teardown(test_clause_info_get_all_types,
                                    setup, teardown),
    cmocka_unit_test_setup_teardown(test_clause_enabled_disabled,
                                    setup, teardown),
    cmocka_unit_test_setup_teardown(test_clause_enabled_trading_disabled,
                                    setup, teardown),
    cmocka_unit_test_setup_teardown(test_is_pact_clause,
                                    setup, teardown),
    cmocka_unit_test_setup_teardown(test_clause_type_values,
                                    setup, teardown),
    cmocka_unit_test_setup_teardown(test_treaties_init_free,
                                    setup, teardown),
    cmocka_unit_test_setup_teardown(test_treaty_add_remove,
                                    setup, teardown),
    cmocka_unit_test_setup_teardown(test_find_treaty_not_found,
                                    setup, teardown),
    cmocka_unit_test_setup_teardown(test_remove_clause_empty,
                                    setup, teardown),
    cmocka_unit_test_setup_teardown(test_treaty_structure_fields,
                                    setup, teardown),
    cmocka_unit_test_setup_teardown(test_clause_structure,
                                    setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}