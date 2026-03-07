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

#include "common/requirements.h"
#include "common/game.h"

/* Test setup and teardown */
static int setup_requirements(void **state)
{
    (void) state;
    game_init(false);
    requirements_init();
    return 0;
}

static int teardown_requirements(void **state)
{
    (void) state;
    requirements_free();
    game_free();
    return 0;
}

/* Test req_from_str with invalid input */
static void test_req_from_str_invalid(void **state)
{
    (void) state;

    /* Test with invalid type */
    struct requirement req = req_from_str("InvalidType", "Local",
                                           false, true, false, "None");
    assert_int_equal(req.source.kind, VUT_NONE);
}

/* Test req_copy */
static void test_req_copy(void **state)
{
    (void) state;

    /* Create a requirement */
    struct requirement src;
    src.source.kind = VUT_NONE;
    src.range = REQ_RANGE_LOCAL;
    src.survives = false;
    src.present = true;
    src.quiet = false;

    /* Copy it */
    struct requirement dst;
    req_copy(&dst, &src);

    /* Verify copy matches original */
    assert_int_equal(dst.source.kind, src.source.kind);
    assert_int_equal(dst.range, src.range);
    assert_int_equal(dst.survives, src.survives);
    assert_int_equal(dst.present, src.present);
    assert_int_equal(dst.quiet, src.quiet);
}

/* Test are_requirements_equal */
static void test_are_requirements_equal(void **state)
{
    (void) state;

    /* Create two identical requirements */
    struct requirement req1;
    req1.source.kind = VUT_NONE;
    req1.range = REQ_RANGE_LOCAL;
    req1.survives = false;
    req1.present = true;
    req1.quiet = false;

    struct requirement req2;
    req2.source.kind = VUT_NONE;
    req2.range = REQ_RANGE_LOCAL;
    req2.survives = false;
    req2.present = true;
    req2.quiet = false;

    /* They should be equal */
    assert_true(are_requirements_equal(&req1, &req2));

    /* Change one field */
    req2.present = false;
    assert_false(are_requirements_equal(&req1, &req2));
}

/* Test req_range enumeration */
static void test_req_range_enum(void **state)
{
    (void) state;

    /* Test range values are ordered correctly */
    assert_int_equal(REQ_RANGE_LOCAL, 0);
    assert_true(REQ_RANGE_TILE > REQ_RANGE_LOCAL);
    assert_true(REQ_RANGE_CITY > REQ_RANGE_TILE);
    assert_true(REQ_RANGE_PLAYER > REQ_RANGE_CITY);
    assert_true(REQ_RANGE_WORLD > REQ_RANGE_PLAYER);
    assert_true(REQ_RANGE_COUNT > REQ_RANGE_WORLD);
}

/* Test req_context_empty */
static void test_req_context_empty(void **state)
{
    (void) state;

    /* Get empty context */
    const struct req_context *ctx = req_context_empty();
    assert_non_null(ctx);

    /* All fields should be nullptr */
    assert_null(ctx->player);
    assert_null(ctx->city);
    assert_null(ctx->tile);
    assert_null(ctx->unit);
}

/* Test req_is_impossible_to_fulfill */
static void test_req_is_impossible_to_fulfill(void **state)
{
    (void) state;

    /* Create a requirement with VUT_NONE - should be impossible */
    struct requirement req;
    req.source.kind = VUT_NONE;
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* VUT_NONE requirement should be impossible to fulfill */
    assert_true(req_is_impossible_to_fulfill(&req));
}

/* Test universal_by_number and back */
static void test_universal_by_number(void **state)
{
    (void) state;

    /* Test getting universal by number for VUT_NONE */
    struct universal univ = universal_by_number(VUT_NONE, 0);
    assert_int_equal(univ.kind, VUT_NONE);
}

/* Test req_vec_change operations */
static void test_req_vec_change(void **state)
{
    (void) state;

    /* Create a change structure */
    struct req_vec_change change;
    change.operation = RVCO_REMOVE;
    change.vector_number = 0;

    /* Initialize the requirement in the change */
    change.req.source.kind = VUT_NONE;
    change.req.range = REQ_RANGE_LOCAL;
    change.req.survives = false;
    change.req.present = true;
    change.req.quiet = false;

    /* Verify change structure */
    assert_int_equal(change.operation, RVCO_REMOVE);
    assert_int_equal(change.vector_number, 0);
}

/* Test req_vec_problem_new and free */
static void test_req_vec_problem(void **state)
{
    (void) state;

    /* Create a problem with no solutions */
    struct req_vec_problem *problem = req_vec_problem_new(0, "Test problem");
    assert_non_null(problem);
    assert_int_equal(problem->num_suggested_solutions, 0);
    assert_string_equal(problem->description, "Test problem");

    req_vec_problem_free(problem);
}

/* Test requirement_vector operations */
static void test_requirement_vector(void **state)
{
    (void) state;

    /* Create a requirement vector */
    struct requirement_vector vec;
    requirement_vector_init(&vec);

    /* Add a requirement */
    struct requirement req;
    req.source.kind = VUT_NONE;
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    requirement_vector_append(&vec, req);
    assert_int_equal(requirement_vector_size(&vec), 1);

    /* Free the vector */
    requirement_vector_free(&vec);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_req_from_str_invalid,
                                        setup_requirements, teardown_requirements),
        cmocka_unit_test_setup_teardown(test_req_copy,
                                        setup_requirements, teardown_requirements),
        cmocka_unit_test_setup_teardown(test_are_requirements_equal,
                                        setup_requirements, teardown_requirements),
        cmocka_unit_test_setup_teardown(test_req_range_enum,
                                        setup_requirements, teardown_requirements),
        cmocka_unit_test_setup_teardown(test_req_context_empty,
                                        setup_requirements, teardown_requirements),
        cmocka_unit_test_setup_teardown(test_req_is_impossible_to_fulfill,
                                        setup_requirements, teardown_requirements),
        cmocka_unit_test_setup_teardown(test_universal_by_number,
                                        setup_requirements, teardown_requirements),
        cmocka_unit_test_setup_teardown(test_req_vec_change,
                                        setup_requirements, teardown_requirements),
        cmocka_unit_test_setup_teardown(test_req_vec_problem,
                                        setup_requirements, teardown_requirements),
        cmocka_unit_test_setup_teardown(test_requirement_vector,
                                        setup_requirements, teardown_requirements),
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}
