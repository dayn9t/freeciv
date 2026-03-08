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

/* utility */
#include "mem.h"

/* common */
#include "extras.h"
#include "fc_types.h"
#include "map.h"
#include "tile.h"
#include "unit.h"
#include "world_object.h"
#include "workertask.h"

#define TEST_MAP_WIDTH 10
#define TEST_MAP_HEIGHT 10

/* Setup/teardown for tests requiring a map */
static int setup_map(void **state)
{
  (void) state;

  memset(&wld, 0, sizeof(wld));
  wld.map.xsize = TEST_MAP_WIDTH;
  wld.map.ysize = TEST_MAP_HEIGHT;
  wld.map.topology_id = 0;
  wld.map.wrap_id = WRAP_X;

  wld.map.tiles = fc_calloc(MAP_INDEX_SIZE, sizeof(struct tile));
  for (int i = 0; i < MAP_INDEX_SIZE; i++) {
    wld.map.tiles[i].index = i;
    wld.map.tiles[i].units = unit_list_new();
  }

  return 0;
}

static int teardown_map(void **state)
{
  (void) state;

  if (wld.map.tiles != NULL) {
    for (int i = 0; i < MAP_INDEX_SIZE; i++) {
      unit_list_destroy(wld.map.tiles[i].units);
    }
    free(wld.map.tiles);
    wld.map.tiles = NULL;
  }
  memset(&wld, 0, sizeof(wld));

  return 0;
}

/* Test: worker_task_init() initializes fields correctly */
static void test_worker_task_init(void **state)
{
  (void) state;
  struct worker_task task;

  worker_task_init(&task);

  assert_null(task.ptile);
  assert_int_equal(task.want, 0);
}

/* Test: worker_task_is_sane() returns FALSE for NULL task */
static void test_worker_task_is_sane_null(void **state)
{
  (void) state;

  assert_false(worker_task_is_sane(NULL));
}

/* Test: worker_task_is_sane() returns FALSE for NULL tile */
static void test_worker_task_is_sane_no_tile(void **state)
{
  (void) state;
  struct worker_task task;

  worker_task_init(&task);
  task.act = ACTIVITY_IDLE;

  assert_false(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns FALSE for invalid activity (ACTIVITY_LAST) */
static void test_worker_task_is_sane_invalid_activity(void **state)
{
  (void) state;
  struct worker_task task;
  struct tile tile;

  worker_task_init(&task);
  task.ptile = &tile;
  task.act = ACTIVITY_LAST;  /* Invalid activity */

  assert_false(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns FALSE when activity requires target but target is NULL */
static void test_worker_task_is_sane_missing_target(void **state)
{
  (void) state;
  struct worker_task task;
  struct tile tile;

  worker_task_init(&task);
  task.ptile = &tile;
  task.act = ACTIVITY_IRRIGATE;  /* Requires target */
  task.tgt = NULL;

  assert_false(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns FALSE when target doesn't match activity cause */
static void test_worker_task_is_sane_wrong_target(void **state)
{
  (void) state;
  struct worker_task task;
  struct tile tile;
  struct extra_type extra;

  /* Initialize extra with no matching causes */
  memset(&extra, 0, sizeof(extra));
  extra.causes = 0;  /* No causes set */
  extra.rmcauses = 0; /* No rmcauses set */

  worker_task_init(&task);
  task.ptile = &tile;
  task.act = ACTIVITY_IRRIGATE;  /* Requires target with EC_IRRIGATION cause */
  task.tgt = &extra;

  assert_false(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE for activity without target requirement */
static void test_worker_task_is_sane_activity_no_target(void **state)
{
  struct worker_task task;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_IDLE;  /* Does not require target */
  task.tgt = NULL;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE for ACTIVITY_TRANSFORM */
static void test_worker_task_is_sane_activity_transform(void **state)
{
  struct worker_task task;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_TRANSFORM;
  task.tgt = NULL;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE for ACTIVITY_CULTIVATE */
static void test_worker_task_is_sane_activity_cultivate(void **state)
{
  struct worker_task task;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_CULTIVATE;
  task.tgt = NULL;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE for ACTIVITY_PLANT */
static void test_worker_task_is_sane_activity_plant(void **state)
{
  struct worker_task task;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_PLANT;
  task.tgt = NULL;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE for ACTIVITY_FORTIFIED */
static void test_worker_task_is_sane_activity_fortified(void **state)
{
  struct worker_task task;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_FORTIFIED;
  task.tgt = NULL;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE for ACTIVITY_SENTRY */
static void test_worker_task_is_sane_activity_sentry(void **state)
{
  struct worker_task task;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_SENTRY;
  task.tgt = NULL;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE for ACTIVITY_GOTO */
static void test_worker_task_is_sane_activity_goto(void **state)
{
  struct worker_task task;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_GOTO;
  task.tgt = NULL;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE for ACTIVITY_EXPLORE */
static void test_worker_task_is_sane_activity_explore(void **state)
{
  struct worker_task task;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_EXPLORE;
  task.tgt = NULL;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE for ACTIVITY_FORTIFYING */
static void test_worker_task_is_sane_activity_fortifying(void **state)
{
  struct worker_task task;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_FORTIFYING;
  task.tgt = NULL;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE for ACTIVITY_CONVERT */
static void test_worker_task_is_sane_activity_convert(void **state)
{
  struct worker_task task;
  struct tile *ptile;

  ptile = &wld.map.tiles[0];

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_CONVERT;
  task.tgt = NULL;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE with matching cause for ACTIVITY_IRRIGATE */
static void test_worker_task_is_sane_irrigate_matching_target(void **state)
{
  struct worker_task task;
  struct tile *ptile;
  struct extra_type extra;

  ptile = &wld.map.tiles[0];

  /* Initialize extra with EC_IRRIGATION cause */
  memset(&extra, 0, sizeof(extra));
  extra.causes = (1 << EC_IRRIGATION);  /* Set irrigation cause */

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_IRRIGATE;
  task.tgt = &extra;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE with matching cause for ACTIVITY_MINE */
static void test_worker_task_is_sane_mine_matching_target(void **state)
{
  struct worker_task task;
  struct tile *ptile;
  struct extra_type extra;

  ptile = &wld.map.tiles[0];

  /* Initialize extra with EC_MINE cause */
  memset(&extra, 0, sizeof(extra));
  extra.causes = (1 << EC_MINE);

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_MINE;
  task.tgt = &extra;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE with matching cause for ACTIVITY_BASE */
static void test_worker_task_is_sane_base_matching_target(void **state)
{
  struct worker_task task;
  struct tile *ptile;
  struct extra_type extra;

  ptile = &wld.map.tiles[0];

  /* Initialize extra with EC_BASE cause */
  memset(&extra, 0, sizeof(extra));
  extra.causes = (1 << EC_BASE);

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_BASE;
  task.tgt = &extra;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE with matching cause for ACTIVITY_GEN_ROAD */
static void test_worker_task_is_sane_gen_road_matching_target(void **state)
{
  struct worker_task task;
  struct tile *ptile;
  struct extra_type extra;

  ptile = &wld.map.tiles[0];

  /* Initialize extra with EC_ROAD cause */
  memset(&extra, 0, sizeof(extra));
  extra.causes = (1 << EC_ROAD);

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_GEN_ROAD;
  task.tgt = &extra;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE with matching rmcause for ACTIVITY_PILLAGE */
static void test_worker_task_is_sane_pillage_matching_target(void **state)
{
  struct worker_task task;
  struct tile *ptile;
  struct extra_type extra;

  ptile = &wld.map.tiles[0];

  /* Initialize extra with ERM_PILLAGE rmcause */
  memset(&extra, 0, sizeof(extra));
  extra.rmcauses = (1 << ERM_PILLAGE);

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_PILLAGE;
  task.tgt = &extra;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns TRUE with matching rmcause for ACTIVITY_CLEAN */
static void test_worker_task_is_sane_clean_matching_target(void **state)
{
  struct worker_task task;
  struct tile *ptile;
  struct extra_type extra;

  ptile = &wld.map.tiles[0];

  /* Initialize extra with ERM_CLEAN rmcause */
  memset(&extra, 0, sizeof(extra));
  extra.rmcauses = (1 << ERM_CLEAN);

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_CLEAN;
  task.tgt = &extra;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns FALSE for PILLAGE with wrong rmcause */
static void test_worker_task_is_sane_pillage_wrong_target(void **state)
{
  struct worker_task task;
  struct tile *ptile;
  struct extra_type extra;

  ptile = &wld.map.tiles[0];

  /* Initialize extra with wrong rmcause */
  memset(&extra, 0, sizeof(extra));
  extra.rmcauses = (1 << ERM_CLEAN);  /* Not ERM_PILLAGE */
  extra.causes = 0;

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_PILLAGE;
  task.tgt = &extra;

  assert_false(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() returns FALSE for CLEAN with wrong rmcause */
static void test_worker_task_is_sane_clean_wrong_target(void **state)
{
  struct worker_task task;
  struct tile *ptile;
  struct extra_type extra;

  ptile = &wld.map.tiles[0];

  /* Initialize extra with wrong rmcause */
  memset(&extra, 0, sizeof(extra));
  extra.rmcauses = (1 << ERM_PILLAGE);  /* Not ERM_CLEAN */
  extra.causes = 0;

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_CLEAN;
  task.tgt = &extra;

  assert_false(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() with extra having multiple causes */
static void test_worker_task_is_sane_extra_multiple_causes(void **state)
{
  struct worker_task task;
  struct tile *ptile;
  struct extra_type extra;

  ptile = &wld.map.tiles[0];

  /* Initialize extra with multiple causes including EC_IRRIGATION */
  memset(&extra, 0, sizeof(extra));
  extra.causes = (1 << EC_IRRIGATION) | (1 << EC_MINE);

  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_IRRIGATE;
  task.tgt = &extra;

  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_is_sane() with extra having both causes and rmcauses */
static void test_worker_task_is_sane_extra_causes_and_rmcauses(void **state)
{
  struct worker_task task;
  struct tile *ptile;
  struct extra_type extra;

  ptile = &wld.map.tiles[0];

  /* Initialize extra with both causes and rmcauses */
  memset(&extra, 0, sizeof(extra));
  extra.causes = (1 << EC_IRRIGATION);
  extra.rmcauses = (1 << ERM_PILLAGE);

  /* Test with matching cause */
  worker_task_init(&task);
  task.ptile = ptile;
  task.act = ACTIVITY_IRRIGATE;
  task.tgt = &extra;
  assert_true(worker_task_is_sane(&task));

  /* Test with matching rmcause */
  task.act = ACTIVITY_PILLAGE;
  assert_true(worker_task_is_sane(&task));
}

/* Test: worker_task_list_new() creates empty list */
static void test_worker_task_list_new(void **state)
{
  (void) state;
  struct worker_task_list *plist;

  plist = worker_task_list_new();

  assert_non_null(plist);
  assert_int_equal(worker_task_list_size(plist), 0);

  worker_task_list_destroy(plist);
}

/* Test: worker_task_list_append() adds element */
static void test_worker_task_list_append(void **state)
{
  (void) state;
  struct worker_task_list *plist;
  struct worker_task task;

  plist = worker_task_list_new();
  worker_task_init(&task);

  worker_task_list_append(plist, &task);

  assert_int_equal(worker_task_list_size(plist), 1);

  worker_task_list_destroy(plist);
}

/* Test: worker_task_list_clear() removes all elements */
static void test_worker_task_list_clear(void **state)
{
  (void) state;
  struct worker_task_list *plist;
  struct worker_task task1, task2;

  plist = worker_task_list_new();
  worker_task_init(&task1);
  worker_task_init(&task2);

  worker_task_list_append(plist, &task1);
  worker_task_list_append(plist, &task2);

  assert_int_equal(worker_task_list_size(plist), 2);

  worker_task_list_clear(plist);

  assert_int_equal(worker_task_list_size(plist), 0);

  worker_task_list_destroy(plist);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* Basic initialization tests */
    cmocka_unit_test(test_worker_task_init),

    /* worker_task_is_sane() error cases */
    cmocka_unit_test(test_worker_task_is_sane_null),
    cmocka_unit_test(test_worker_task_is_sane_no_tile),
    cmocka_unit_test(test_worker_task_is_sane_invalid_activity),
    cmocka_unit_test(test_worker_task_is_sane_missing_target),
    cmocka_unit_test(test_worker_task_is_sane_wrong_target),

    /* worker_task_is_sane() activities without target requirement */
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_activity_no_target, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_activity_transform, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_activity_cultivate, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_activity_plant, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_activity_fortified, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_activity_sentry, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_activity_goto, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_activity_explore, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_activity_fortifying, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_activity_convert, setup_map, teardown_map),

    /* worker_task_is_sane() activities with matching targets */
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_irrigate_matching_target, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_mine_matching_target, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_base_matching_target, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_gen_road_matching_target, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_pillage_matching_target, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_clean_matching_target, setup_map, teardown_map),

    /* worker_task_is_sane() with wrong targets */
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_pillage_wrong_target, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_clean_wrong_target, setup_map, teardown_map),

    /* worker_task_is_sane() edge cases */
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_extra_multiple_causes, setup_map, teardown_map),
    cmocka_unit_test_setup_teardown(test_worker_task_is_sane_extra_causes_and_rmcauses, setup_map, teardown_map),

    /* List operations */
    cmocka_unit_test(test_worker_task_list_new),
    cmocka_unit_test(test_worker_task_list_append),
    cmocka_unit_test(test_worker_task_list_clear),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}