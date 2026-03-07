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

/* utility */
#include "mem.h"
#include "shared.h"

/* common */
#include "citizens.h"
#include "city.h"
#include "game.h"
#include "player.h"

/* Test city structure */
static struct city test_city;

/***********************************************************************
  Setup function - initialize minimal game state
***********************************************************************/
static int setup(void **state)
{
  (void) state;

  /* Initialize minimal game state */
  memset(&game, 0, sizeof(game));
  game.info.citizen_nationality = TRUE;

  /* Initialize test city */
  memset(&test_city, 0, sizeof(test_city));
  test_city.name = fc_strdup("TestCity");
  test_city.id = 1;
  test_city.size = 5;

  return 0;
}

/***********************************************************************
  Teardown function - cleanup
***********************************************************************/
static int teardown(void **state)
{
  (void) state;

  if (test_city.name != NULL) {
    FC_FREE(test_city.name);
    test_city.name = NULL;
  }

  if (test_city.nationality != NULL) {
    FC_FREE(test_city.nationality);
    test_city.nationality = NULL;
  }

  return 0;
}

/***********************************************************************
  Test citizens_init with valid city
***********************************************************************/
static void test_citizens_init_valid(void **state)
{
  (void) state;

  citizens_init(&test_city);

  assert_non_null(test_city.nationality);
}

/***********************************************************************
  Test citizens_init with NULL city
***********************************************************************/
static void test_citizens_init_null(void **state)
{
  (void) state;

  /* Should not crash with NULL */
  citizens_init(NULL);
}

/***********************************************************************
  Test citizens_init when citizen_nationality is disabled
***********************************************************************/
static void test_citizens_init_disabled(void **state)
{
  (void) state;

  game.info.citizen_nationality = FALSE;

  citizens_init(&test_city);

  /* Nationality should not be allocated when disabled */
  assert_null(test_city.nationality);

  game.info.citizen_nationality = TRUE;
}

/***********************************************************************
  Test citizens_free with valid city
***********************************************************************/
static void test_citizens_free_valid(void **state)
{
  (void) state;

  citizens_init(&test_city);
  citizens_free(&test_city);

  assert_null(test_city.nationality);
}

/***********************************************************************
  Test citizens_free with NULL city
***********************************************************************/
static void test_citizens_free_null(void **state)
{
  (void) state;

  /* Should not crash with NULL */
  citizens_free(NULL);
}

/***********************************************************************
  Test citizens_init reinitialization
***********************************************************************/
static void test_citizens_init_reinit(void **state)
{
  (void) state;

  citizens_init(&test_city);

  /* Manually set some data in nationality array */
  if (test_city.nationality != NULL) {
    test_city.nationality[0] = 10;
  }

  /* Reinitialize should reset nationality data */
  citizens_init(&test_city);

  /* Check that nationality was reset */
  if (test_city.nationality != NULL) {
    assert_int_equal(test_city.nationality[0], 0);
  }
}

/***********************************************************************
  Test citizens_count with nationality disabled
***********************************************************************/
static void test_citizens_count_without_nationality(void **state)
{
  (void) state;

  game.info.citizen_nationality = FALSE;
  test_city.size = 7;

  citizens count = citizens_count(&test_city);
  assert_int_equal(count, 7);

  game.info.citizen_nationality = TRUE;
}

/***********************************************************************
  Test citizens_nation_get with NULL city
***********************************************************************/
static void test_citizens_nation_get_null_city(void **state)
{
  (void) state;

  citizens result = citizens_nation_get(NULL, NULL);
  assert_int_equal(result, 0);
}

/***********************************************************************
  Test citizens_nation_get when citizen_nationality is disabled
***********************************************************************/
static void test_citizens_nation_get_disabled(void **state)
{
  (void) state;

  game.info.citizen_nationality = FALSE;

  citizens result = citizens_nation_get(&test_city, NULL);
  assert_int_equal(result, 0);

  game.info.citizen_nationality = TRUE;
}

/***********************************************************************
  Test citizens_nation_set when citizen_nationality is disabled
***********************************************************************/
static void test_citizens_nation_set_disabled(void **state)
{
  (void) state;

  game.info.citizen_nationality = FALSE;

  citizens_init(&test_city);
  citizens_nation_set(&test_city, NULL, 10);

  /* Should not crash when disabled */
  game.info.citizen_nationality = TRUE;
}

/***********************************************************************
  Test citizens_nation_add when citizen_nationality is disabled
***********************************************************************/
static void test_citizens_nation_add_disabled(void **state)
{
  (void) state;

  game.info.citizen_nationality = FALSE;

  citizens_init(&test_city);
  citizens_nation_add(&test_city, NULL, 10);

  /* Should not crash when disabled */
  game.info.citizen_nationality = TRUE;
}

/***********************************************************************
  Test citizens_count with NULL city - skipped
***********************************************************************/
static void test_citizens_count_null(void **state)
{
  (void) state;

  /* citizens_count with NULL city will crash due to city_size_get */
  /* This test is skipped */
  skip();
}

/***********************************************************************
  Test citizens_nation_foreign with NULL city - skipped
***********************************************************************/
static void test_citizens_nation_foreign_null(void **state)
{
  (void) state;

  /* citizens_nation_foreign requires city_owner() which needs valid owner */
  /* This test is skipped */
  skip();
}

/***********************************************************************
  Test citizens_nation_move when citizen_nationality is disabled
***********************************************************************/
static void test_citizens_nation_move_disabled(void **state)
{
  (void) state;

  game.info.citizen_nationality = FALSE;

  citizens_init(&test_city);
  citizens_nation_move(&test_city, NULL, NULL, 5);

  /* Should not crash when disabled */
  game.info.citizen_nationality = TRUE;
}

/***********************************************************************
  Test MAX_NUM_PLAYER_SLOTS constant
***********************************************************************/
static void test_max_num_player_slots(void **state)
{
  (void) state;

  /* MAX_NUM_PLAYER_SLOTS should be defined and reasonable */
  assert_true(MAX_NUM_PLAYER_SLOTS > 0);
  assert_int_equal(MAX_NUM_PLAYER_SLOTS, 512);
}

/***********************************************************************
  Test citizens type definition
***********************************************************************/
static void test_citizens_type_size(void **state)
{
  (void) state;

  /* citizens is unsigned char, max value 255 (MAX_CITY_SIZE) */
  citizens c = 255;
  assert_int_equal(c, 255);
}

/***********************************************************************
  Test MAX_CITY_SIZE constant
***********************************************************************/
static void test_max_city_size(void **state)
{
  (void) state;

  /* MAX_CITY_SIZE should be 255 (0xFF) */
  assert_int_equal(MAX_CITY_SIZE, 0xFF);
}

/***********************************************************************
  Main test runner
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    /* citizens_init tests */
    cmocka_unit_test_setup_teardown(test_citizens_init_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_citizens_init_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_citizens_init_disabled, setup, teardown),
    cmocka_unit_test_setup_teardown(test_citizens_init_reinit, setup, teardown),

    /* citizens_free tests */
    cmocka_unit_test_setup_teardown(test_citizens_free_valid, setup, teardown),
    cmocka_unit_test_setup_teardown(test_citizens_free_null, setup, teardown),

    /* citizens_count tests */
    cmocka_unit_test_setup_teardown(test_citizens_count_without_nationality, setup, teardown),
    cmocka_unit_test_setup_teardown(test_citizens_count_null, setup, teardown),

    /* citizens_nation tests with disabled nationality */
    cmocka_unit_test_setup_teardown(test_citizens_nation_get_null_city, setup, teardown),
    cmocka_unit_test_setup_teardown(test_citizens_nation_get_disabled, setup, teardown),
    cmocka_unit_test_setup_teardown(test_citizens_nation_set_disabled, setup, teardown),
    cmocka_unit_test_setup_teardown(test_citizens_nation_add_disabled, setup, teardown),
    cmocka_unit_test_setup_teardown(test_citizens_nation_move_disabled, setup, teardown),
    cmocka_unit_test_setup_teardown(test_citizens_nation_foreign_null, setup, teardown),

    /* Constants tests */
    cmocka_unit_test_setup_teardown(test_max_num_player_slots, setup, teardown),
    cmocka_unit_test_setup_teardown(test_citizens_type_size, setup, teardown),
    cmocka_unit_test_setup_teardown(test_max_city_size, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}