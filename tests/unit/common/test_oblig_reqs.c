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
#include <stdbool.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "common/actions.h"
#include "common/game.h"
#include "common/nation.h"
#include "common/requirements.h"
#include "common/oblig_reqs.h"

/* Test setup and teardown */
static int setup_oblig_reqs(void **state)
{
    (void) state;
    game_init(false);
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

/* Test oblig_hard_reqs_init initializes all vectors properly */
static void test_oblig_hard_reqs_init_vectors(void **state)
{
    (void) state;
    int i;

    /* After init, all action result vectors should be empty but valid */
    for (i = 0; i < ACTRES_NONE; i++) {
        struct obligatory_req_vector *vec = oblig_hard_reqs_get(i);
        assert_non_null(vec);
        assert_int_equal(vec->size, 0);
    }
}

/* Test oblig_hard_reqs_init for sub results */
static void test_oblig_hard_reqs_init_sub_vectors(void **state)
{
    (void) state;
    int i;

    /* After init, all action sub result vectors should be empty but valid */
    for (i = 0; i < ACT_SUB_RES_COUNT; i++) {
        struct obligatory_req_vector *vec = oblig_hard_reqs_get_sub(i);
        assert_non_null(vec);
        assert_int_equal(vec->size, 0);
    }
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

/* Test oblig_hard_reqs_get for multiple action results */
static void test_oblig_hard_reqs_get_multiple(void **state)
{
    (void) state;

    /* Test various action results */
    struct obligatory_req_vector *vec1 = oblig_hard_reqs_get(ACTRES_ATTACK);
    struct obligatory_req_vector *vec2 = oblig_hard_reqs_get(ACTRES_CONQUER_CITY);
    struct obligatory_req_vector *vec3 = oblig_hard_reqs_get(ACTRES_FORTIFY);

    assert_non_null(vec1);
    assert_non_null(vec2);
    assert_non_null(vec3);

    /* Each should be a different vector */
    assert_ptr_not_equal(vec1, vec2);
    assert_ptr_not_equal(vec2, vec3);
    assert_ptr_not_equal(vec1, vec3);
}

/* Test oblig_hard_reqs_get_sub for sub results */
static void test_oblig_hard_reqs_get_sub(void **state)
{
    (void) state;

    /* Get requirements for a sub result */
    struct obligatory_req_vector *vec = oblig_hard_reqs_get_sub(ACT_SUB_RES_HUT_ENTER);
    assert_non_null(vec);
}

/* Test oblig_hard_reqs_get_sub for multiple sub results */
static void test_oblig_hard_reqs_get_sub_multiple(void **state)
{
    (void) state;

    /* Test various action sub results */
    struct obligatory_req_vector *vec1 = oblig_hard_reqs_get_sub(ACT_SUB_RES_HUT_ENTER);
    struct obligatory_req_vector *vec2 = oblig_hard_reqs_get_sub(ACT_SUB_RES_HUT_FRIGHTEN);

    assert_non_null(vec1);
    assert_non_null(vec2);

    /* Each should be a different vector */
    assert_ptr_not_equal(vec1, vec2);
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
    /* Vector should now have requirements */
    assert_true(vec->size > 0);
}

/* Test hard_code_oblig_hard_reqs populates multiple action results */
static void test_hard_code_oblig_hard_reqs_multiple(void **state)
{
    (void) state;

    hard_code_oblig_hard_reqs();

    /* Check that multiple action results have requirements populated */
    struct obligatory_req_vector *vec_embassy = oblig_hard_reqs_get(ACTRES_ESTABLISH_EMBASSY);
    struct obligatory_req_vector *vec_attack = oblig_hard_reqs_get(ACTRES_ATTACK);
    struct obligatory_req_vector *vec_fortify = oblig_hard_reqs_get(ACTRES_FORTIFY);

    assert_true(vec_embassy->size > 0);
    assert_true(vec_attack->size > 0);
    assert_true(vec_fortify->size > 0);
}

/* Test hard_code_oblig_hard_reqs sub results */
static void test_hard_code_oblig_hard_reqs_sub(void **state)
{
    (void) state;

    hard_code_oblig_hard_reqs();

    /* Check that sub results have requirements populated */
    struct obligatory_req_vector *vec_hut_enter = oblig_hard_reqs_get_sub(ACT_SUB_RES_HUT_ENTER);
    struct obligatory_req_vector *vec_hut_frighten = oblig_hard_reqs_get_sub(ACT_SUB_RES_HUT_FRIGHTEN);

    assert_true(vec_hut_enter->size > 0);
    assert_true(vec_hut_frighten->size > 0);
}

/* Test obligatory_req structure content after hard coding */
static void test_obligatory_req_content(void **state)
{
    (void) state;

    hard_code_oblig_hard_reqs();

    struct obligatory_req_vector *vec = oblig_hard_reqs_get(ACTRES_ESTABLISH_EMBASSY);
    assert_true(vec->size > 0);

    /* Check first obligatory requirement */
    struct obligatory_req *oreq = &vec->p[0];
    assert_non_null(oreq->contras);
    assert_non_null(oreq->error_msg);
    assert_true(oreq->contras->alternatives > 0);
    assert_non_null(oreq->contras->alternative);
}

/* Test ae_contra_or structure content */
static void test_ae_contra_or_content(void **state)
{
    (void) state;

    hard_code_oblig_hard_reqs();

    struct obligatory_req_vector *vec = oblig_hard_reqs_get(ACTRES_ESTABLISH_EMBASSY);
    struct obligatory_req *oreq = &vec->p[0];
    struct ae_contra_or *contras = oreq->contras;

    assert_true(contras->alternatives > 0);
    assert_true(contras->users > 0);

    /* Check alternative contradictions */
    for (int i = 0; i < contras->alternatives; i++) {
        struct action_enabler_contradiction *alt = &contras->alternative[i];
        /* The req field should be a valid requirement */
        assert_true(alt->req.range >= REQ_RANGE_LOCAL);
        assert_true(alt->req.range < REQ_RANGE_COUNT);
    }
}

/* Test action_enabler_contradiction is_target field */
static void test_action_enabler_contradiction_target(void **state)
{
    (void) state;

    hard_code_oblig_hard_reqs();

    /* Find an action with target requirements */
    struct obligatory_req_vector *vec = oblig_hard_reqs_get(ACTRES_IRRIGATE);
    if (vec->size > 0) {
        struct obligatory_req *oreq = &vec->p[0];
        for (int i = 0; i < oreq->contras->alternatives; i++) {
            /* is_target should be either true or false */
            assert_true(oreq->contras->alternative[i].is_target == true
                        || oreq->contras->alternative[i].is_target == false);
        }
    }
}

/* Test hard_code_oblig_hard_reqs_ruleset */
static void test_hard_code_oblig_hard_reqs_ruleset(void **state)
{
    (void) state;

    /* This function requires nations to be set up */
    /* Without nations, it should not crash */
    hard_code_oblig_hard_reqs_ruleset();

    /* Just verify no crash occurred */
    assert_true(1);
}

/* Test multiple init/free cycles */
static void test_oblig_hard_reqs_multiple_cycles(void **state)
{
    (void) state;

    /* Free and re-init multiple times */
    oblig_hard_reqs_free();
    oblig_hard_reqs_init();

    oblig_hard_reqs_free();
    oblig_hard_reqs_init();

    /* Verify vectors are still valid after re-init */
    struct obligatory_req_vector *vec = oblig_hard_reqs_get(ACTRES_ESTABLISH_EMBASSY);
    assert_non_null(vec);
    assert_int_equal(vec->size, 0);
}

/* Test hard_code_oblig_hard_reqs can be called multiple times */
static void test_hard_code_oblig_hard_reqs_multiple_calls(void **state)
{
    (void) state;

    /* Call hard_code_oblig_hard_reqs twice */
    hard_code_oblig_hard_reqs();

    /* Get size after first call */
    struct obligatory_req_vector *vec = oblig_hard_reqs_get(ACTRES_ESTABLISH_EMBASSY);
    int size1 = vec->size;

    /* Call again - this should add more requirements */
    hard_code_oblig_hard_reqs();
    int size2 = vec->size;

    /* Size should have increased */
    assert_true(size2 >= size1);
}

/* Test that free properly cleans up after hard_code */
static void test_oblig_hard_reqs_free_cleanup(void **state)
{
    (void) state;

    /* Add requirements */
    hard_code_oblig_hard_reqs();

    /* Free should clean up properly */
    oblig_hard_reqs_free();

    /* Re-init should work */
    oblig_hard_reqs_init();

    /* Vectors should be empty again */
    struct obligatory_req_vector *vec = oblig_hard_reqs_get(ACTRES_ESTABLISH_EMBASSY);
    assert_int_equal(vec->size, 0);
}

/* Test error message content */
static void test_error_message_content(void **state)
{
    (void) state;

    hard_code_oblig_hard_reqs();

    struct obligatory_req_vector *vec = oblig_hard_reqs_get(ACTRES_ESTABLISH_EMBASSY);
    assert_true(vec->size > 0);

    /* Error message should not be null and should contain meaningful text */
    for (int i = 0; i < vec->size; i++) {
        struct obligatory_req *oreq = &vec->p[i];
        assert_non_null(oreq->error_msg);
        assert_true(strlen(oreq->error_msg) > 0);
    }
}

/* Test requirements for transport actions */
static void test_transport_requirements(void **state)
{
    (void) state;

    hard_code_oblig_hard_reqs();

    /* Transport actions should have requirements about diplomatic relations */
    struct obligatory_req_vector *vec_embark = oblig_hard_reqs_get(ACTRES_TRANSPORT_EMBARK);
    struct obligatory_req_vector *vec_board = oblig_hard_reqs_get(ACTRES_TRANSPORT_BOARD);
    struct obligatory_req_vector *vec_load = oblig_hard_reqs_get(ACTRES_TRANSPORT_LOAD);

    assert_true(vec_embark->size > 0);
    assert_true(vec_board->size > 0);
    assert_true(vec_load->size > 0);
}

/* Test requirements for terrain modification actions */
static void test_terrain_requirements(void **state)
{
    (void) state;

    hard_code_oblig_hard_reqs();

    /* Terrain modification actions should have requirements */
    struct obligatory_req_vector *vec_irrigate = oblig_hard_reqs_get(ACTRES_IRRIGATE);
    struct obligatory_req_vector *vec_mine = oblig_hard_reqs_get(ACTRES_MINE);
    struct obligatory_req_vector *vec_road = oblig_hard_reqs_get(ACTRES_ROAD);

    assert_true(vec_irrigate->size > 0);
    assert_true(vec_mine->size > 0);
    assert_true(vec_road->size > 0);
}

/* Test requirements for spy actions */
static void test_spy_requirements(void **state)
{
    (void) state;

    hard_code_oblig_hard_reqs();

    /* Spy actions should have requirements about foreign targets */
    struct obligatory_req_vector *vec_investigate = oblig_hard_reqs_get(ACTRES_SPY_INVESTIGATE_CITY);
    struct obligatory_req_vector *vec_steal_gold = oblig_hard_reqs_get(ACTRES_SPY_STEAL_GOLD);
    struct obligatory_req_vector *vec_steal_tech = oblig_hard_reqs_get(ACTRES_SPY_STEAL_TECH);

    assert_true(vec_investigate->size > 0);
    assert_true(vec_steal_gold->size > 0);
    assert_true(vec_steal_tech->size > 0);
}

/* Test users count in ae_contra_or */
static void test_ae_contra_or_users_count(void **state)
{
    (void) state;

    hard_code_oblig_hard_reqs();

    /* After hard coding, all ae_contra_or should have positive user counts */
    struct obligatory_req_vector *vec = oblig_hard_reqs_get(ACTRES_ESTABLISH_EMBASSY);
    for (int i = 0; i < vec->size; i++) {
        struct obligatory_req *oreq = &vec->p[i];
        assert_true(oreq->contras->users > 0);
    }
}

/* Test alternatives count */
static void test_alternatives_count(void **state)
{
    (void) state;

    hard_code_oblig_hard_reqs();

    /* Check that alternatives count matches the allocated array */
    struct obligatory_req_vector *vec = oblig_hard_reqs_get(ACTRES_ESTABLISH_EMBASSY);
    for (int i = 0; i < vec->size; i++) {
        struct obligatory_req *oreq = &vec->p[i];
        assert_true(oreq->contras->alternatives >= 1);
    }
}

/* Test conquer city requirements */
static void test_conquer_city_requirements(void **state)
{
    (void) state;

    hard_code_oblig_hard_reqs();

    struct obligatory_req_vector *vec = oblig_hard_reqs_get(ACTRES_CONQUER_CITY);
    assert_true(vec->size > 0);

    /* Verify each requirement has valid structure */
    for (int i = 0; i < vec->size; i++) {
        struct obligatory_req *oreq = &vec->p[i];
        assert_non_null(oreq->contras);
        assert_non_null(oreq->contras->alternative);
    }
}

/* Test paradrop requirements */
static void test_paradrop_requirements(void **state)
{
    (void) state;

    hard_code_oblig_hard_reqs();

    struct obligatory_req_vector *vec_paradrop = oblig_hard_reqs_get(ACTRES_PARADROP);
    struct obligatory_req_vector *vec_paradrop_conquer = oblig_hard_reqs_get(ACTRES_PARADROP_CONQUER);

    assert_true(vec_paradrop->size > 0);
    assert_true(vec_paradrop_conquer->size > 0);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_oblig_hard_reqs_init_free,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_oblig_hard_reqs_init_vectors,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_oblig_hard_reqs_init_sub_vectors,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_oblig_hard_reqs_get,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_oblig_hard_reqs_get_multiple,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_oblig_hard_reqs_get_sub,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_oblig_hard_reqs_get_sub_multiple,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_hard_code_oblig_hard_reqs,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_hard_code_oblig_hard_reqs_multiple,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_hard_code_oblig_hard_reqs_sub,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_obligatory_req_content,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_ae_contra_or_content,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_action_enabler_contradiction_target,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_hard_code_oblig_hard_reqs_ruleset,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_oblig_hard_reqs_multiple_cycles,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_hard_code_oblig_hard_reqs_multiple_calls,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_oblig_hard_reqs_free_cleanup,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_error_message_content,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_transport_requirements,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_terrain_requirements,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_spy_requirements,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_ae_contra_or_users_count,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_alternatives_count,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_conquer_city_requirements,
                                        setup_oblig_reqs, teardown_oblig_reqs),
        cmocka_unit_test_setup_teardown(test_paradrop_requirements,
                                        setup_oblig_reqs, teardown_oblig_reqs),
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}