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

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "workertask.h"

static void test_worker_task_init(void **state)
{
  (void) state;
  struct worker_task task;

  worker_task_init(&task);

  assert_null(task.ptile);
  assert_int_equal(task.want, 0);
}

static void test_worker_task_is_sane_null(void **state)
{
  (void) state;

  assert_false(worker_task_is_sane(NULL));
}

static void test_worker_task_is_sane_no_tile(void **state)
{
  (void) state;
  struct worker_task task;

  worker_task_init(&task);

  assert_false(worker_task_is_sane(&task));
}

static void test_worker_task_list_new(void **state)
{
  (void) state;
  struct worker_task_list *plist;

  plist = worker_task_list_new();

  assert_non_null(plist);
  assert_int_equal(worker_task_list_size(plist), 0);

  worker_task_list_destroy(plist);
}

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
    cmocka_unit_test(test_worker_task_init),
    cmocka_unit_test(test_worker_task_is_sane_null),
    cmocka_unit_test(test_worker_task_is_sane_no_tile),
    cmocka_unit_test(test_worker_task_list_new),
    cmocka_unit_test(test_worker_task_list_append),
    cmocka_unit_test(test_worker_task_list_clear),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
