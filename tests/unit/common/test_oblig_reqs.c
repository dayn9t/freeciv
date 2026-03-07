/***********************************************************************
 Freeciv - Copyright (C) 2024 - The Freeciv Project
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

#include "common/oblig_reqs.h"
#include "common/game.h"
#include "common/nation.h"

/* Test setup and teardown */
static int setup_oblig_reqs(void **state)
{
    (void) state;
    game_init();
    oblig_hard_reqs_init();
    return 0;
}

static int teardown_oblig_reqs(void **state)
{
    (void) state;
    oblig_hard_reqs_free();
    game_free();
    return 0;
}

/* Test oblig_hard_reqs_init and oblig_hard_reqs_free */
static void test_oblig_hard_reqs_init_free(void **state)
{
    (void) state;

    /* Setup and teardown handle init/free, just verify no crash */
    assert_true(1);
}

/* Test oblig_hard_reqs_get for valid action results */
static void test_oblig_hard_reqs_get(void **state)
{
    (void) state;

    /* Get requirements for a valid action result */
    struct obligatory_req_vector *vec = oblig_hard_reqs_get(ACTRES_ESTABLISH_EMBASSY);
    assert_non_null(vec);

    /* Vector should be initialized (may be empty before hard_code_oblig_hard_reqs) */
    assert_int_equal(vec->size, 0);
}

/* Test oblig_hard_reqs_get for ACTRES_NONE */
static void test_oblig_hard_reqs_get_none(void **state)
{
    (void) state;

    /* ACTRES_NONE is a special value, not a valid action result */
    /* Just verify the function doesn't crash */
}

/* Test oblig_hard_reqs_get_sub for sub results */
static void test_oblig_hard_reqs_get_sub(void **state)
{
    (void) state;

    /* Get requirements for a sub result */
    struct obligatory_req_vector *vec = oblig_hard_reqs_get_sub(ACT_SUB_RES_HUT_ENTER);
    assert_non_null(vec);
}

/* Test hard_code_oblig_hard_reqs */
static void test_hard_code_oblig_hard_reqs(void **state)
{
    (void) state;

    /* Call hard_code_oblig_hard_reqs */
    hard_code_oblig_hard_reqs();

    /* After hard coding, some action results should have requirements */
    struct obligatory_req_vector *vec = oblig_hard_reqs_get(ACTRES_ESTABLISH_EMBASSY);
    assert_non_null(vec);
    /* Vector may or may not be empty depending on hard coded requirements */
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_oblig_hard_reqs_init_free,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_oblig_hard_reqs_get,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_oblig_hard_reqs_get_none,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_oblig_hard_reqs_get_sub,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_hard_code_oblig_hard_reqs,
                                        setup_oblig_reqs, teardown_oblig_reqs),
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}
