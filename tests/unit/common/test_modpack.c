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
#include "shared.h"

/* common */
#include "fc_interface.h"
#include "modpack.h"

/***********************************************************************
  Test MODPACK_CAPSTR constant
***********************************************************************/
static void test_modpack_capstr_exists(void **state)
{
  (void) state;

  /* MODPACK_CAPSTR should be a valid string */
  assert_non_null(MODPACK_CAPSTR);
  assert_true(strlen(MODPACK_CAPSTR) > 0);
}

static void test_modpack_capstr_format(void **state)
{
  (void) state;

  /* MODPACK_CAPSTR should start with "+" for capability string */
  assert_true(MODPACK_CAPSTR[0] == '+');
  /* Should contain "Freeciv" */
  assert_true(strstr(MODPACK_CAPSTR, "Freeciv") != NULL);
  /* Should contain "modpack" */
  assert_true(strstr(MODPACK_CAPSTR, "modpack") != NULL);
}

/***********************************************************************
  Test MODPACK_SUFFIX constant
***********************************************************************/
static void test_modpack_suffix_exists(void **state)
{
  (void) state;

  /* MODPACK_SUFFIX should be a valid string */
  assert_non_null(MODPACK_SUFFIX);
  assert_true(strlen(MODPACK_SUFFIX) > 0);
}

static void test_modpack_suffix_value(void **state)
{
  (void) state;

  /* MODPACK_SUFFIX should be ".modpack" */
  assert_string_equal(MODPACK_SUFFIX, ".modpack");
}

/***********************************************************************
  Test modpack_has_ruleset with null input
***********************************************************************/
static void test_modpack_has_ruleset_null(void **state)
{
  const char *result;

  (void) state;

  result = modpack_has_ruleset(NULL);
  assert_null(result);
}

/***********************************************************************
  Test modpack_has_tileset with null input
***********************************************************************/
static void test_modpack_has_tileset_null(void **state)
{
  const char *result;

  (void) state;

  result = modpack_has_tileset(NULL);
  assert_null(result);
}

/***********************************************************************
  Test modpack_serv_file with null input
***********************************************************************/
static void test_modpack_serv_file_null(void **state)
{
  const char *result;

  (void) state;

  result = modpack_serv_file(NULL);
  assert_null(result);
}

/***********************************************************************
  Test modpack_rulesetdir with null input
***********************************************************************/
static void test_modpack_rulesetdir_null(void **state)
{
  const char *result;

  (void) state;

  result = modpack_rulesetdir(NULL);
  assert_null(result);
}

/***********************************************************************
  Test modpack_tilespec with null input
***********************************************************************/
static void test_modpack_tilespec_null(void **state)
{
  const char *result;

  (void) state;

  result = modpack_tilespec(NULL);
  assert_null(result);
}

/***********************************************************************
  Test modpacks_init and modpacks_free for server
***********************************************************************/
static void test_modpacks_init_free_server(void **state)
{
  (void) state;

  /* Set as server */
  i_am_server();

  /* Init should not crash */
  modpacks_init();

  /* Free should not crash */
  modpacks_free();
}

/***********************************************************************
  Test modpacks_init and modpacks_free for client
***********************************************************************/
static void test_modpacks_init_free_client(void **state)
{
  (void) state;

  /* Set as client */
  i_am_client();

  /* Init should not crash */
  modpacks_init();

  /* Free should not crash */
  modpacks_free();
}

/***********************************************************************
  Test cache lookup with non-existent name (server)
***********************************************************************/
static void test_modpack_file_from_ruleset_cache_not_found(void **state)
{
  const char *result;

  (void) state;

  /* Set as server and initialize */
  i_am_server();
  modpacks_init();

  result = modpack_file_from_ruleset_cache("nonexistent_modpack");
  assert_null(result);

  modpacks_free();
}

/***********************************************************************
  Test cache lookup with non-existent name (client)
***********************************************************************/
static void test_modpack_file_from_tileset_cache_not_found(void **state)
{
  const char *result;

  (void) state;

  /* Set as client and initialize */
  i_am_client();
  modpacks_init();

  result = modpack_file_from_tileset_cache("nonexistent_modpack");
  assert_null(result);

  modpacks_free();
}

/***********************************************************************
  Test modpack_tileset_target with non-existent name
***********************************************************************/
static void test_modpack_tileset_target_not_found(void **state)
{
  const char *result;

  (void) state;

  /* Set as client and initialize */
  i_am_client();
  modpacks_init();

  result = modpack_tileset_target("nonexistent_modpack");
  assert_null(result);

  modpacks_free();
}

/***********************************************************************
  Test cache lookup with null name
***********************************************************************/
static void test_modpack_file_from_ruleset_cache_null(void **state)
{
  const char *result;

  (void) state;

  /* Set as server and initialize */
  i_am_server();
  modpacks_init();

  /* Passing NULL should be safe (returns NULL, doesn't crash) */
  result = modpack_file_from_ruleset_cache(NULL);
  /* Result depends on implementation - just verify it doesn't crash */
  (void) result;

  modpacks_free();
}

static void test_modpack_file_from_tileset_cache_null(void **state)
{
  const char *result;

  (void) state;

  /* Set as client and initialize */
  i_am_client();
  modpacks_init();

  /* Passing NULL should be safe (returns NULL, doesn't crash) */
  result = modpack_file_from_tileset_cache(NULL);
  /* Result depends on implementation - just verify it doesn't crash */
  (void) result;

  modpacks_free();
}

static void test_modpack_tileset_target_null(void **state)
{
  const char *result;

  (void) state;

  /* Set as client and initialize */
  i_am_client();
  modpacks_init();

  /* Passing NULL should be safe (returns NULL, doesn't crash) */
  result = modpack_tileset_target(NULL);
  /* Result depends on implementation - just verify it doesn't crash */
  (void) result;

  modpacks_free();
}

/***********************************************************************
  Test cache iteration with callback
***********************************************************************/
static int cache_iterate_count;

static void cache_iterate_cb(const char *name, const char *filename, void *data)
{
  (void) name;
  (void) filename;
  (void) data;
  cache_iterate_count++;
}

static void test_modpack_ruleset_cache_iterate_empty(void **state)
{
  (void) state;

  /* Set as server and initialize */
  i_am_server();
  modpacks_init();

  cache_iterate_count = 0;
  modpack_ruleset_cache_iterate(cache_iterate_cb, NULL);
  assert_int_equal(cache_iterate_count, 0);

  modpacks_free();
}

static void test_modpack_tileset_cache_iterate_empty(void **state)
{
  (void) state;

  /* Set as client and initialize */
  i_am_client();
  modpacks_init();

  cache_iterate_count = 0;
  modpack_tileset_cache_iterate(cache_iterate_cb, NULL);
  assert_int_equal(cache_iterate_count, 0);

  modpacks_free();
}

/***********************************************************************
  Test multiple init/free cycles
***********************************************************************/
static void test_modpacks_multiple_init_free(void **state)
{
  (void) state;

  /* Set as server */
  i_am_server();

  /* Multiple init/free cycles should not leak or crash */
  modpacks_init();
  modpacks_free();

  modpacks_init();
  modpacks_free();

  modpacks_init();
  modpacks_free();
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* MODPACK_CAPSTR tests */
    cmocka_unit_test(test_modpack_capstr_exists),
    cmocka_unit_test(test_modpack_capstr_format),

    /* MODPACK_SUFFIX tests */
    cmocka_unit_test(test_modpack_suffix_exists),
    cmocka_unit_test(test_modpack_suffix_value),

    /* Null input tests */
    cmocka_unit_test(test_modpack_has_ruleset_null),
    cmocka_unit_test(test_modpack_has_tileset_null),
    cmocka_unit_test(test_modpack_serv_file_null),
    cmocka_unit_test(test_modpack_rulesetdir_null),
    cmocka_unit_test(test_modpack_tilespec_null),

    /* Init/free tests */
    cmocka_unit_test(test_modpacks_init_free_server),
    cmocka_unit_test(test_modpacks_init_free_client),

    /* Cache lookup tests */
    cmocka_unit_test(test_modpack_file_from_ruleset_cache_not_found),
    cmocka_unit_test(test_modpack_file_from_tileset_cache_not_found),
    cmocka_unit_test(test_modpack_tileset_target_not_found),
    cmocka_unit_test(test_modpack_file_from_ruleset_cache_null),
    cmocka_unit_test(test_modpack_file_from_tileset_cache_null),
    cmocka_unit_test(test_modpack_tileset_target_null),

    /* Cache iteration tests */
    cmocka_unit_test(test_modpack_ruleset_cache_iterate_empty),
    cmocka_unit_test(test_modpack_tileset_cache_iterate_empty),

    /* Multiple init/free cycles */
    cmocka_unit_test(test_modpacks_multiple_init_free),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}