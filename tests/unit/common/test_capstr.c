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

#include <setjmp.h>
#include <cmocka.h>

/* common */
#include "capstr.h"

static void test_our_capability_exists(void **state)
{
  (void) state;

  /* our_capability is an extern const char *const */
  assert_non_null(our_capability);
}

static void test_init_our_capability(void **state)
{
  (void) state;

  /* init_our_capability() should not crash */
  init_our_capability();

  /* After initialization, our_capability should still be valid */
  assert_non_null(our_capability);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_our_capability_exists),
    cmocka_unit_test(test_init_our_capability),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
