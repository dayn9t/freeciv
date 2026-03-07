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
#include "worklist.h"

static void test_worklist_init(void **state)
{
  (void) state;
  struct worklist wl;

  worklist_init(&wl);

  assert_int_equal(worklist_length(&wl), 0);
  assert_true(worklist_is_empty(&wl));
}

static void test_worklist_append(void **state)
{
  (void) state;
  struct worklist wl;
  struct universal prod;

  worklist_init(&wl);

  prod.kind = VUT_UTYPE;
  prod.value.utype = NULL;

  assert_true(worklist_append(&wl, &prod));
  assert_int_equal(worklist_length(&wl), 1);
  assert_false(worklist_is_empty(&wl));
}

static void test_worklist_peek(void **state)
{
  (void) state;
  struct worklist wl;
  struct universal prod;
  struct universal peeked;

  worklist_init(&wl);

  prod.kind = VUT_UTYPE;
  prod.value.utype = NULL;

  worklist_append(&wl, &prod);

  assert_true(worklist_peek(&wl, &peeked));
  assert_int_equal(peeked.kind, VUT_UTYPE);
  assert_int_equal(worklist_length(&wl), 1);
}

static void test_worklist_remove(void **state)
{
  (void) state;
  struct worklist wl;
  struct universal prod1, prod2;

  worklist_init(&wl);

  prod1.kind = VUT_UTYPE;
  prod1.value.utype = NULL;
  prod2.kind = VUT_IMPROVEMENT;
  prod2.value.building = NULL;

  worklist_append(&wl, &prod1);
  worklist_append(&wl, &prod2);

  assert_int_equal(worklist_length(&wl), 2);

  worklist_remove(&wl, 0);

  assert_int_equal(worklist_length(&wl), 1);
}

static void test_worklist_is_empty(void **state)
{
  (void) state;
  struct worklist wl;

  worklist_init(&wl);
  assert_true(worklist_is_empty(&wl));

  struct universal prod;
  prod.kind = VUT_UTYPE;
  prod.value.utype = NULL;
  worklist_append(&wl, &prod);

  assert_false(worklist_is_empty(&wl));
}

static void test_worklist_append_max(void **state)
{
  (void) state;
  struct worklist wl;
  struct universal prod;
  int i;

  worklist_init(&wl);

  prod.kind = VUT_UTYPE;
  prod.value.utype = NULL;

  for (i = 0; i < MAX_LEN_WORKLIST; i++) {
    assert_true(worklist_append(&wl, &prod));
  }

  assert_int_equal(worklist_length(&wl), MAX_LEN_WORKLIST);
  assert_false(worklist_append(&wl, &prod));
}

static void test_worklist_advance(void **state)
{
  (void) state;
  struct worklist wl;
  struct universal prod1, prod2, peeked;

  worklist_init(&wl);

  prod1.kind = VUT_UTYPE;
  prod1.value.utype = NULL;
  prod2.kind = VUT_IMPROVEMENT;
  prod2.value.building = NULL;

  worklist_append(&wl, &prod1);
  worklist_append(&wl, &prod2);

  worklist_advance(&wl);

  assert_int_equal(worklist_length(&wl), 1);
  assert_true(worklist_peek(&wl, &peeked));
  assert_int_equal(peeked.kind, VUT_IMPROVEMENT);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_worklist_init),
    cmocka_unit_test(test_worklist_append),
    cmocka_unit_test(test_worklist_peek),
    cmocka_unit_test(test_worklist_remove),
    cmocka_unit_test(test_worklist_is_empty),
    cmocka_unit_test(test_worklist_append_max),
    cmocka_unit_test(test_worklist_advance),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
