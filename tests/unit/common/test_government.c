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
#include "game.h"
#include "government.h"

/***********************************************************************
  Test setup and teardown
***********************************************************************/
static int setup_governments(void **state)
{
  (void) state;

  memset(&game, 0, sizeof(game));
  governments_alloc(3);

  return 0;
}

static int teardown_governments(void **state)
{
  (void) state;

  governments_free();
  memset(&game, 0, sizeof(game));

  return 0;
}

/***********************************************************************
  Test government_count
***********************************************************************/
static void test_government_count(void **state)
{
  (void) state;

  assert_int_equal(government_count(), 3);
}

/***********************************************************************
  Test government_by_number with valid indices
***********************************************************************/
static void test_government_by_number_valid(void **state)
{
  (void) state;

  struct government *gov0 = government_by_number(0);
  struct government *gov1 = government_by_number(1);
  struct government *gov2 = government_by_number(2);

  assert_non_null(gov0);
  assert_non_null(gov1);
  assert_non_null(gov2);

  /* Each government should be distinct */
  assert_ptr_not_equal(gov0, gov1);
  assert_ptr_not_equal(gov1, gov2);
  assert_ptr_not_equal(gov0, gov2);
}

/***********************************************************************
  Test government_by_number with invalid indices
***********************************************************************/
static void test_government_by_number_invalid(void **state)
{
  (void) state;

  /* Negative index should return NULL */
  assert_null(government_by_number(-1));

  /* Index beyond count should return NULL */
  assert_null(government_by_number(3));
  assert_null(government_by_number(100));
  assert_null(government_by_number(G_LAST));
}

/***********************************************************************
  Test government_number
***********************************************************************/
static void test_government_number(void **state)
{
  (void) state;

  struct government *gov0 = government_by_number(0);
  struct government *gov1 = government_by_number(1);
  struct government *gov2 = government_by_number(2);

  assert_int_equal(government_number(gov0), 0);
  assert_int_equal(government_number(gov1), 1);
  assert_int_equal(government_number(gov2), 2);
}

/***********************************************************************
  Test government_number with NULL input
***********************************************************************/
static void test_government_number_null(void **state)
{
  (void) state;

  /* NULL input should return -1 */
  assert_int_equal(government_number(NULL), -1);
}

/***********************************************************************
  Test government_index
***********************************************************************/
static void test_government_index(void **state)
{
  (void) state;

  struct government *gov0 = government_by_number(0);
  struct government *gov1 = government_by_number(1);
  struct government *gov2 = government_by_number(2);

  assert_int_equal(government_index(gov0), 0);
  assert_int_equal(government_index(gov1), 1);
  assert_int_equal(government_index(gov2), 2);
}

/***********************************************************************
  Test government_index with NULL input
***********************************************************************/
static void test_government_index_null(void **state)
{
  (void) state;

  /* NULL input should return -1 */
  assert_int_equal(government_index(NULL), -1);
}

/***********************************************************************
  Test government_rule_name - note that government name is not initialized
  so this tests the behavior with uninitialized data
***********************************************************************/
static void test_government_rule_name(void **state)
{
  (void) state;

  struct government *gov = government_by_number(0);

  /* Government name is not set, so rule_name may return NULL or empty */
  const char *name = government_rule_name(gov);
  /* Just verify it doesn't crash - result may be NULL for uninitialized */
  (void) name;
}

/***********************************************************************
  Test government_rule_name with NULL input
***********************************************************************/
static void test_government_rule_name_null(void **state)
{
  (void) state;

  /* NULL input should return NULL */
  assert_null(government_rule_name(NULL));
}

/***********************************************************************
  Test government_name_translation - note that government name is not
  initialized so this tests the behavior with uninitialized data
***********************************************************************/
static void test_government_name_translation(void **state)
{
  (void) state;

  struct government *gov = government_by_number(0);

  /* Government name is not set, so translation may return NULL */
  const char *name = government_name_translation(gov);
  /* Just verify it doesn't crash - result may be NULL for uninitialized */
  (void) name;
}

/***********************************************************************
  Test government_name_translation with NULL input
***********************************************************************/
static void test_government_name_translation_null(void **state)
{
  (void) state;

  /* NULL input should return NULL */
  assert_null(government_name_translation(NULL));
}

/***********************************************************************
  Test government_of_player with NULL input
***********************************************************************/
static void test_government_of_player_null(void **state)
{
  (void) state;

  assert_null(government_of_player(NULL));
}

/***********************************************************************
  Test government_of_city with NULL input
***********************************************************************/
static void test_government_of_city_null(void **state)
{
  (void) state;

  assert_null(government_of_city(NULL));
}

/***********************************************************************
  Test government_ruler_titles with valid input
***********************************************************************/
static void test_government_ruler_titles(void **state)
{
  (void) state;

  struct government *gov = government_by_number(0);

  const struct ruler_title_hash *titles = government_ruler_titles(gov);
  assert_non_null(titles);
}

/***********************************************************************
  Test government_ruler_titles with NULL input
***********************************************************************/
static void test_government_ruler_titles_null(void **state)
{
  (void) state;

  assert_null(government_ruler_titles(NULL));
}

/***********************************************************************
  Test government_has_flag
***********************************************************************/
static void test_government_has_flag(void **state)
{
  (void) state;

  struct government *gov = government_by_number(0);

  /* By default, flags should not be set */
  /* Test that the function doesn't crash */
  (void) government_has_flag(gov, GOVF_USER_FLAG_1);
}

/***********************************************************************
  Test government iteration
***********************************************************************/
static void test_government_iteration(void **state)
{
  (void) state;

  int count = 0;

  governments_iterate(gov) {
    assert_non_null(gov);
    count++;
  } governments_iterate_end;

  assert_int_equal(count, 3);
}

/***********************************************************************
  Test government_iter_sizeof
***********************************************************************/
static void test_government_iter_sizeof(void **state)
{
  (void) state;

  size_t size = government_iter_sizeof();
  assert_true(size > 0);
}

/***********************************************************************
  Test governments_free when governments is NULL
***********************************************************************/
static void test_governments_free_null(void **state)
{
  (void) state;

  /* After teardown, governments will be NULL. This test just verifies
   * that calling governments_free when NULL is handled gracefully. */
  governments_free();
  governments_free();  /* Should not crash */

  /* Re-allocate for other tests */
  governments_alloc(3);
}

/***********************************************************************
  Test untargeted_revolution_allowed
***********************************************************************/
static void test_untargeted_revolution_allowed(void **state)
{
  (void) state;

  /* Default game.info.revolentype should allow untargeted revolution */
  memset(&game, 0, sizeof(game));
  game.info.revolentype = REVOLEN_FIXED;

  assert_true(untargeted_revolution_allowed());

  game.info.revolentype = REVOLEN_RANDOM;

  assert_true(untargeted_revolution_allowed());
}

/***********************************************************************
  Test untargeted_revolution_allowed quickening
***********************************************************************/
static void test_untargeted_revolution_not_allowed(void **state)
{
  (void) state;

  memset(&game, 0, sizeof(game));
  game.info.revolentype = REVOLEN_QUICKENING;

  assert_false(untargeted_revolution_allowed());

  game.info.revolentype = REVOLEN_RANDQUICK;

  assert_false(untargeted_revolution_allowed());
}

/***********************************************************************
  Test can_change_to_government with NULL player
***********************************************************************/
static void test_can_change_to_government_null_player(void **state)
{
  (void) state;

  struct government *gov = government_by_number(0);

  assert_false(can_change_to_government(NULL, gov));
}

/***********************************************************************
  Test can_change_to_government with NULL government
***********************************************************************/
static void test_can_change_to_government_null_government(void **state)
{
  (void) state;

  assert_false(can_change_to_government(NULL, NULL));
}

/***********************************************************************
  Test G_LAST constant
***********************************************************************/
static void test_government_last_constant(void **state)
{
  (void) state;

  /* G_LAST should be larger than any valid government index */
  assert_true(G_LAST > 2);
  assert_true(G_LAST > 100);
  assert_int_equal(G_LAST, 127);
}

/***********************************************************************
  Main test suite
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    /* Basic government tests with setup/teardown */
    cmocka_unit_test_setup_teardown(test_government_count,
                                     setup_governments, teardown_governments),
    cmocka_unit_test_setup_teardown(test_government_by_number_valid,
                                     setup_governments, teardown_governments),
    cmocka_unit_test_setup_teardown(test_government_by_number_invalid,
                                     setup_governments, teardown_governments),
    cmocka_unit_test_setup_teardown(test_government_number,
                                     setup_governments, teardown_governments),
    cmocka_unit_test_setup_teardown(test_government_number_null,
                                     setup_governments, teardown_governments),
    cmocka_unit_test_setup_teardown(test_government_index,
                                     setup_governments, teardown_governments),
    cmocka_unit_test_setup_teardown(test_government_index_null,
                                     setup_governments, teardown_governments),

    /* Name tests */
    cmocka_unit_test_setup_teardown(test_government_rule_name,
                                     setup_governments, teardown_governments),
    cmocka_unit_test_setup_teardown(test_government_rule_name_null,
                                     setup_governments, teardown_governments),
    cmocka_unit_test_setup_teardown(test_government_name_translation,
                                     setup_governments, teardown_governments),
    cmocka_unit_test_setup_teardown(test_government_name_translation_null,
                                     setup_governments, teardown_governments),

    /* Player/city government tests */
    cmocka_unit_test_setup_teardown(test_government_of_player_null,
                                     setup_governments, teardown_governments),
    cmocka_unit_test_setup_teardown(test_government_of_city_null,
                                     setup_governments, teardown_governments),

    /* Ruler titles tests */
    cmocka_unit_test_setup_teardown(test_government_ruler_titles,
                                     setup_governments, teardown_governments),
    cmocka_unit_test_setup_teardown(test_government_ruler_titles_null,
                                     setup_governments, teardown_governments),

    /* Flag tests */
    cmocka_unit_test_setup_teardown(test_government_has_flag,
                                     setup_governments, teardown_governments),

    /* Iteration tests */
    cmocka_unit_test_setup_teardown(test_government_iteration,
                                     setup_governments, teardown_governments),
    cmocka_unit_test_setup_teardown(test_government_iter_sizeof,
                                     setup_governments, teardown_governments),

    /* Allocation tests */
    cmocka_unit_test_setup_teardown(test_governments_free_null,
                                     setup_governments, teardown_governments),

    /* Revolution tests */
    cmocka_unit_test(test_untargeted_revolution_allowed),
    cmocka_unit_test(test_untargeted_revolution_not_allowed),

    /* Can change government tests */
    cmocka_unit_test_setup_teardown(test_can_change_to_government_null_player,
                                     setup_governments, teardown_governments),
    cmocka_unit_test_setup_teardown(test_can_change_to_government_null_government,
                                     setup_governments, teardown_governments),

    /* Constant tests */
    cmocka_unit_test(test_government_last_constant),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}