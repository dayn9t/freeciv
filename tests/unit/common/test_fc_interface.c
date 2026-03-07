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
#include "fc_interface.h"

/* External variables from fc_interface.c */
extern bool fc_funcs_defined;
extern const struct functions *fc_funcs;

static void test_i_am_server(void **state)
{
  (void) state;

  i_am_server();
  assert_true(is_server());
}

static void test_i_am_client(void **state)
{
  (void) state;

  i_am_client();
  assert_false(is_server());
}

static void test_i_am_tool(void **state)
{
  (void) state;

  /* i_am_tool() calls i_am_server() internally */
  i_am_tool();
  assert_true(is_server());
}

static void test_is_server_after_server(void **state)
{
  (void) state;

  i_am_server();
  assert_true(is_server());
}

static void test_is_server_after_client(void **state)
{
  (void) state;

  i_am_client();
  assert_false(is_server());
}

static void test_fc_interface_funcs_returns_non_null(void **state)
{
  struct functions *funcs;

  (void) state;

  /* Reset state for this test */
  fc_funcs_defined = FALSE;
  fc_funcs = NULL;

  funcs = fc_interface_funcs();
  assert_non_null(funcs);
}

static void test_fc_interface_funcs_returns_same_pointer(void **state)
{
  struct functions *funcs1;
  struct functions *funcs2;

  (void) state;

  /* Reset state for this test */
  fc_funcs_defined = FALSE;
  fc_funcs = NULL;

  funcs1 = fc_interface_funcs();
  funcs2 = fc_interface_funcs();

  assert_ptr_equal(funcs1, funcs2);
}

static int test_fc_interface_setup(void **state)
{
  (void) state;

  /* Reset fc_interface state before each test */
  fc_funcs_defined = FALSE;
  fc_funcs = NULL;

  return 0;
}

static int test_fc_interface_teardown(void **state)
{
  (void) state;

  /* Reset fc_interface state after each test */
  fc_funcs_defined = FALSE;
  fc_funcs = NULL;

  return 0;
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_i_am_server),
    cmocka_unit_test(test_i_am_client),
    cmocka_unit_test(test_i_am_tool),
    cmocka_unit_test(test_is_server_after_server),
    cmocka_unit_test(test_is_server_after_client),
    cmocka_unit_test_setup_teardown(test_fc_interface_funcs_returns_non_null,
                                     test_fc_interface_setup,
                                     test_fc_interface_teardown),
    cmocka_unit_test_setup_teardown(test_fc_interface_funcs_returns_same_pointer,
                                     test_fc_interface_setup,
                                     test_fc_interface_teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}