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

#include "common/effects.h"
#include "common/game.h"

/* Test setup and teardown */
static int setup_effects(void **state)
{
    (void) state;
    game_init();
    ruleset_cache_init();
    return 0;
}

static int teardown_effects(void **state)
{
    (void) state;
    ruleset_cache_free();
    game_free();
    return 0;
}

/* Test effect_type_exists for known effect types */
static void test_effect_type_exists(void **state)
{
    (void) state;

    /* Test common effect types exist */
    assert_true(EFT_TURN_YEARS >= 0);
    assert_true(EFT_CITY_VISION_RADIUS_SQ >= 0);
    assert_true(EFT_COUNT > 0);

    /* Test that EFT_COUNT is greater than specific types */
    assert_true(EFT_COUNT > EFT_TURN_YEARS);
    assert_true(EFT_COUNT > EFT_CITY_VISION_RADIUS_SQ);
}

/* Test effect_list operations */
static void test_effect_list_operations(void **state)
{
    (void) state;
    struct effect_list *list = nullptr;

    /* Create a new effect list */
    list = effect_list_new();
    assert_non_null(list);
    assert_int_equal(effect_list_size(list), 0);

    /* Create and append an effect */
    struct effect *peffect = effect_new(EFT_TURN_YEARS, 1, nullptr);
    assert_non_null(peffect);
    effect_list_append(list, peffect);
    assert_int_equal(effect_list_size(list), 1);

    /* Create and append another effect */
    struct effect *peffect2 = effect_new(EFT_CITY_VISION_RADIUS_SQ, 5, nullptr);
    assert_non_null(peffect2);
    effect_list_append(list, peffect2);
    assert_int_equal(effect_list_size(list), 2);

    /* Destroy the list */
    effect_list_destroy(list);
}

/* Test effect creation and properties */
static void test_effect_creation(void **state)
{
    (void) state;

    /* Create an effect with value */
    struct effect *peffect = effect_new(EFT_TURN_YEARS, 10, nullptr);
    assert_non_null(peffect);

    /* Verify effect properties */
    assert_int_equal(peffect->type, EFT_TURN_YEARS);
    assert_int_equal(peffect->value, 10);
    assert_null(peffect->multiplier);

    effect_free(peffect);
}

/* Test effect with requirements */
static void test_effect_with_requirements(void **state)
{
    (void) state;

    /* Create an effect */
    struct effect *peffect = effect_new(EFT_DEFEND_BONUS, 50, nullptr);
    assert_non_null(peffect);

    /* Add a requirement */
    struct requirement req;
    req.source.kind = VUT_NONE;
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    effect_req_append(peffect, req);

    /* Verify requirement was added */
    assert_int_equal(requirement_vector_size(&peffect->reqs), 1);

    effect_free(peffect);
}

/* Test get_effects returns a list */
static void test_get_effects(void **state)
{
    (void) state;

    /* Get effects for a type */
    struct effect_list *effects = get_effects(EFT_TURN_YEARS);
    /* List may be empty but should not crash */
    assert_non_null(effects);
}

/* Test get_world_bonus with empty cache */
static void test_get_world_bonus_empty(void **state)
{
    (void) state;

    /* Get world bonus for an effect type */
    int bonus = get_world_bonus(EFT_TURN_YEARS);

    /* With empty cache, should return 0 */
    assert_int_equal(bonus, 0);
}

/* Test user effect functions */
static void test_user_effects(void **state)
{
    (void) state;

    /* Test user effect check */
    assert_false(is_user_effect(EFT_TURN_YEARS));
    assert_false(is_user_effect(EFT_CITY_VISION_RADIUS_SQ));

    /* User effects should be identified correctly */
    if (EFT_USER_EFFECT_1 < EFT_COUNT) {
        assert_true(is_user_effect(EFT_USER_EFFECT_1));
    }
}

/* Test effect_copy */
static void test_effect_copy(void **state)
{
    (void) state;

    /* Create original effect */
    struct effect *original = effect_new(EFT_ATTACK_BONUS, 25, nullptr);
    assert_non_null(original);

    /* Copy the effect */
    struct effect *copy = effect_copy(original, EFT_DEFEND_BONUS);
    assert_non_null(copy);

    /* Verify copy has new type but same value */
    assert_int_equal(copy->type, EFT_DEFEND_BONUS);
    assert_int_equal(copy->value, original->value);

    effect_free(original);
    effect_free(copy);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_effect_type_exists,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_list_operations,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_creation,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_with_requirements,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_get_effects,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_get_world_bonus_empty,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_user_effects,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_copy,
                                        setup_effects, teardown_effects),
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}
