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
#include <cmocka.h>

#include "common/effects.h"
#include "common/game.h"
#include "common/requirements.h"

/* Test setup and teardown */
static int setup_effects(void **state)
{
    (void) state;
    game_init(false);
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

    /* Destroy the empty list - effects created with effect_new are
     * managed by ruleset_cache and freed during teardown */
    effect_list_destroy(list);
}

/* Test effect creation and properties */
static void test_effect_creation(void **state)
{
    (void) state;

    /* Create an effect with value - this registers it in ruleset_cache */
    struct effect *peffect = effect_new(EFT_TURN_YEARS, 10, nullptr);
    assert_non_null(peffect);

    /* Verify effect properties */
    assert_int_equal(peffect->type, EFT_TURN_YEARS);
    assert_int_equal(peffect->value, 10);
    assert_null(peffect->multiplier);

    /* Note: effect_free is not called here because effect_new registers
     * the effect in ruleset_cache, which will free it during teardown */
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

    /* Note: effect_free is not called here because effect_new registers
     * the effect in ruleset_cache, which will free it during teardown */
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

    /* Note: effect_free is not called here because effect_new registers
     * the effects in ruleset_cache, which will free them during teardown */
}

/* Test effect_cumulative_max with no effects */
static void test_effect_cumulative_max_empty(void **state)
{
    (void) state;
    int max = effect_cumulative_max(EFT_TURN_YEARS, nullptr, 0);
    assert_int_equal(max, 0);
}

/* Test effect_cumulative_max with positive effects */
static void test_effect_cumulative_max_positive(void **state)
{
    (void) state;
    struct effect *eff1 = effect_new(EFT_TURN_YEARS, 10, nullptr);
    struct effect *eff2 = effect_new(EFT_TURN_YEARS, 20, nullptr);
    assert_non_null(eff1);
    assert_non_null(eff2);
    int max = effect_cumulative_max(EFT_TURN_YEARS, nullptr, 0);
    assert_int_equal(max, 30);
}

/* Test effect_cumulative_min with no effects */
static void test_effect_cumulative_min_empty(void **state)
{
    (void) state;
    int min = effect_cumulative_min(EFT_TURN_YEARS, nullptr);
    assert_int_equal(min, 0);
}

/* Test effect_cumulative_min with negative effects */
static void test_effect_cumulative_min_negative(void **state)
{
    (void) state;
    struct effect *eff1 = effect_new(EFT_DEFEND_BONUS, -10, nullptr);
    struct effect *eff2 = effect_new(EFT_DEFEND_BONUS, -20, nullptr);
    assert_non_null(eff1);
    assert_non_null(eff2);
    int min = effect_cumulative_min(EFT_DEFEND_BONUS, nullptr);
    assert_int_equal(min, -30);
}

/* Test get_world_bonus with effects */
static void test_get_world_bonus_with_effects(void **state)
{
    (void) state;
    struct effect *eff1 = effect_new(EFT_TURN_YEARS, 5, nullptr);
    struct effect *eff2 = effect_new(EFT_TURN_YEARS, 10, nullptr);
    assert_non_null(eff1);
    assert_non_null(eff2);
    int bonus = get_world_bonus(EFT_TURN_YEARS);
    assert_int_equal(bonus, 15);
}

/* Test get_player_bonus with null player */
static void test_get_player_bonus_null(void **state)
{
    (void) state;
    struct effect *eff = effect_new(EFT_TURN_YEARS, 10, nullptr);
    assert_non_null(eff);
    int bonus = get_player_bonus(nullptr, EFT_TURN_YEARS);
    assert_int_equal(bonus, 10);
}

/* Test iterate_effect_cache */
static bool iter_callback(struct effect *peffect, void *data)
{
    int *count = (int *)data;
    (*count)++;
    return TRUE;
}

static void test_iterate_effect_cache(void **state)
{
    (void) state;
    struct effect *eff1 = effect_new(EFT_TURN_YEARS, 5, nullptr);
    struct effect *eff2 = effect_new(EFT_DEFEND_BONUS, 10, nullptr);
    assert_non_null(eff1);
    assert_non_null(eff2);
    int count = 0;
    bool result = iterate_effect_cache(iter_callback, &count);
    assert_true(result);
    assert_int_equal(count, 2);
}

/* Test effect_remove */
static void test_effect_remove(void **state)
{
    (void) state;
    struct effect *eff = effect_new(EFT_TURN_YEARS, 10, nullptr);
    assert_non_null(eff);
    struct effect_list *list = get_effects(EFT_TURN_YEARS);
    assert_int_equal(effect_list_size(list), 1);
    effect_remove(eff);
    assert_int_equal(effect_list_size(list), 0);
    effect_free(eff);
}

/* Test effect stacking */
static void test_effect_stacking(void **state)
{
    (void) state;
    struct effect *eff1 = effect_new(EFT_TURN_YEARS, 10, nullptr);
    struct effect *eff2 = effect_new(EFT_TURN_YEARS, 20, nullptr);
    struct effect *eff3 = effect_new(EFT_TURN_YEARS, 30, nullptr);
    assert_non_null(eff1);
    assert_non_null(eff2);
    assert_non_null(eff3);
    int bonus = get_world_bonus(EFT_TURN_YEARS);
    assert_int_equal(bonus, 60);
}

/* Test get_target_bonus_effects */
static void test_get_target_bonus_effects(void **state)
{
    (void) state;
    struct effect *eff = effect_new(EFT_TURN_YEARS, 10, nullptr);
    assert_non_null(eff);
    int bonus = get_target_bonus_effects(nullptr, nullptr, nullptr, EFT_TURN_YEARS);
    assert_int_equal(bonus, 10);
}

/* Test effect_list_append */
static void test_effect_list_append(void **state)
{
    (void) state;
    struct effect_list *list = effect_list_new();
    assert_non_null(list);
    assert_int_equal(effect_list_size(list), 0);
    struct effect *eff1 = effect_new(EFT_TURN_YEARS, 5, nullptr);
    effect_list_append(list, eff1);
    assert_int_equal(effect_list_size(list), 1);
    effect_list_destroy(list);
}

/* Test effect_list_front_back */
static void test_effect_list_front_back(void **state)
{
    (void) state;
    struct effect_list *list = effect_list_new();
    assert_non_null(list);
    struct effect *eff1 = effect_new(EFT_TURN_YEARS, 5, nullptr);
    struct effect *eff2 = effect_new(EFT_TURN_YEARS, 10, nullptr);
    effect_list_append(list, eff1);
    effect_list_append(list, eff2);
    struct effect *front = effect_list_front(list);
    struct effect *back = effect_list_back(list);
    assert_non_null(front);
    assert_non_null(back);
    assert_int_equal(front->value, 5);
    assert_int_equal(back->value, 10);
    effect_list_destroy(list);
}

/* Test effect_cumulative_max with negative effects */
static void test_effect_cumulative_max_negative(void **state)
{
    (void) state;
    struct effect *eff = effect_new(EFT_TURN_YEARS, -5, nullptr);
    assert_non_null(eff);
    int max = effect_cumulative_max(EFT_TURN_YEARS, nullptr, 0);
    assert_int_equal(max, -5);
}

/* Test effect_cumulative_min with positive effects */
static void test_effect_cumulative_min_positive(void **state)
{
    (void) state;
    struct effect *eff = effect_new(EFT_ATTACK_BONUS, 15, nullptr);
    assert_non_null(eff);
    int min = effect_cumulative_min(EFT_ATTACK_BONUS, nullptr);
    assert_int_equal(min, 15);
}

/* Test get_world_bonus with mixed effects */
static void test_get_world_bonus_mixed(void **state)
{
    (void) state;
    struct effect *eff1 = effect_new(EFT_CITY_VISION_RADIUS_SQ, 5, nullptr);
    struct effect *eff2 = effect_new(EFT_CITY_VISION_RADIUS_SQ, -3, nullptr);
    assert_non_null(eff1);
    assert_non_null(eff2);
    int bonus = get_world_bonus(EFT_CITY_VISION_RADIUS_SQ);
    assert_int_equal(bonus, 2);
}

/* Test effect_copy with same type */
static void test_effect_copy_same_type(void **state)
{
    (void) state;
    struct effect *original = effect_new(EFT_TURN_YEARS, 15, nullptr);
    assert_non_null(original);
    struct effect *copy = effect_copy(original, effect_type_invalid());
    assert_non_null(copy);
    assert_int_equal(copy->type, original->type);
    assert_int_equal(copy->value, original->value);
}

/* Test effect_copy with requirements */
static void test_effect_copy_with_requirements(void **state)
{
    (void) state;
    struct effect *original = effect_new(EFT_DEFEND_BONUS, 30, nullptr);
    assert_non_null(original);
    struct requirement req;
    req.source.kind = VUT_NONE;
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = true;
    req.quiet = false;
    effect_req_append(original, req);
    struct effect *copy = effect_copy(original, EFT_ATTACK_BONUS);
    assert_non_null(copy);
    assert_int_equal(requirement_vector_size(&copy->reqs), 1);
}

/* Test iterate_effect_cache with early stop */
static bool iter_callback_stop(struct effect *peffect, void *data)
{
    int *count = (int *)data;
    (*count)++;
    return FALSE;
}

static void test_iterate_effect_cache_stop(void **state)
{
    (void) state;
    struct effect *eff1 = effect_new(EFT_ATTACK_BONUS, 5, nullptr);
    struct effect *eff2 = effect_new(EFT_CITY_VISION_RADIUS_SQ, 10, nullptr);
    assert_non_null(eff1);
    assert_non_null(eff2);
    int count = 0;
    bool result = iterate_effect_cache(iter_callback_stop, &count);
    assert_false(result);
    assert_int_equal(count, 1);
}

/* Test effect with multiple requirements */
static void test_effect_multiple_requirements(void **state)
{
    (void) state;
    struct effect *peffect = effect_new(EFT_DEFEND_BONUS, 50, nullptr);
    assert_non_null(peffect);
    struct requirement req1, req2;
    req1.source.kind = VUT_NONE;
    req1.range = REQ_RANGE_LOCAL;
    req1.survives = false;
    req1.present = true;
    req1.quiet = false;
    req2.source.kind = VUT_NONE;
    req2.range = REQ_RANGE_CITY;
    req2.survives = false;
    req2.present = false;
    req2.quiet = false;
    effect_req_append(peffect, req1);
    effect_req_append(peffect, req2);
    assert_int_equal(requirement_vector_size(&peffect->reqs), 2);
}

/* Test get_effects for different types */
static void test_get_effects_different_types(void **state)
{
    (void) state;
    struct effect *eff1 = effect_new(EFT_TURN_YEARS, 5, nullptr);
    struct effect *eff2 = effect_new(EFT_DEFEND_BONUS, 10, nullptr);
    struct effect *eff3 = effect_new(EFT_ATTACK_BONUS, 15, nullptr);
    assert_non_null(eff1);
    assert_non_null(eff2);
    assert_non_null(eff3);
    struct effect_list *list1 = get_effects(EFT_TURN_YEARS);
    struct effect_list *list2 = get_effects(EFT_DEFEND_BONUS);
    struct effect_list *list3 = get_effects(EFT_ATTACK_BONUS);
    assert_non_null(list1);
    assert_non_null(list2);
    assert_non_null(list3);
    assert_int_equal(effect_list_size(list1), 1);
    assert_int_equal(effect_list_size(list2), 1);
    assert_int_equal(effect_list_size(list3), 1);
}

/* Test effect_value_from_universals with no effects */
static void test_effect_value_from_universals_empty(void **state)
{
    (void) state;
    int value = effect_value_from_universals(EFT_TURN_YEARS, nullptr, 0);
    assert_int_equal(value, 0);
}

/* Test effect_universals_value_never_below */
static void test_effect_universals_value_never_below(void **state)
{
    (void) state;
    struct effect *eff = effect_new(EFT_TURN_YEARS, 10, nullptr);
    assert_non_null(eff);
    bool result = effect_universals_value_never_below(EFT_TURN_YEARS, nullptr, 0, 5);
    assert_true(result);
    result = effect_universals_value_never_below(EFT_TURN_YEARS, nullptr, 0, 15);
    assert_false(result);
}

/* Test get_target_bonus_effects with list */
static void test_get_target_bonus_effects_with_list(void **state)
{
    (void) state;
    struct effect *eff1 = effect_new(EFT_TURN_YEARS, 5, nullptr);
    struct effect *eff2 = effect_new(EFT_TURN_YEARS, 10, nullptr);
    assert_non_null(eff1);
    assert_non_null(eff2);
    struct effect_list *list = effect_list_new();
    assert_non_null(list);
    int bonus = get_target_bonus_effects(list, nullptr, nullptr, EFT_TURN_YEARS);
    assert_int_equal(bonus, 15);
    assert_int_equal(effect_list_size(list), 2);
    effect_list_destroy(list);
}

/* Test effect with present=false requirement */
static void test_effect_negative_requirement(void **state)
{
    (void) state;
    struct effect *peffect = effect_new(EFT_DEFEND_BONUS, 50, nullptr);
    assert_non_null(peffect);
    struct requirement req;
    req.source.kind = VUT_NONE;
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = false;
    req.quiet = false;
    effect_req_append(peffect, req);
    assert_int_equal(requirement_vector_size(&peffect->reqs), 1);
    struct requirement *first_req = requirement_vector_get(&peffect->reqs, 0);
    assert_non_null(first_req);
    assert_false(first_req->present);
}

/* Test effect rulesave fields */
static void test_effect_rulesave(void **state)
{
    (void) state;
    struct effect *peffect = effect_new(EFT_TURN_YEARS, 10, nullptr);
    assert_non_null(peffect);
    assert_false(peffect->rulesave.do_not_save);
    assert_null(peffect->rulesave.comment);
}

/* Test effect_list clear */
static void test_effect_list_clear(void **state)
{
    (void) state;
    struct effect_list *list = effect_list_new();
    assert_non_null(list);
    struct effect *eff = effect_new(EFT_TURN_YEARS, 5, nullptr);
    effect_list_append(list, eff);
    assert_int_equal(effect_list_size(list), 1);
    effect_list_clear(list);
    assert_int_equal(effect_list_size(list), 0);
    effect_list_destroy(list);
}

/* Test is_user_effect boundary conditions */
static void test_is_user_effect_boundary(void **state)
{
    (void) state;
    assert_false(is_user_effect(EFT_TURN_YEARS));
    assert_false(is_user_effect(EFT_DEFEND_BONUS));
    assert_false(is_user_effect(EFT_ATTACK_BONUS));
    if (EFT_USER_EFFECT_1 < EFT_COUNT && EFT_USER_EFFECT_LAST < EFT_COUNT) {
        assert_true(is_user_effect(EFT_USER_EFFECT_1));
        assert_true(is_user_effect(EFT_USER_EFFECT_LAST));
    }
}

/* Test effect with different ranges */
static void test_effect_different_ranges(void **state)
{
    (void) state;
    struct effect *eff1 = effect_new(EFT_DEFEND_BONUS, 10, nullptr);
    struct effect *eff2 = effect_new(EFT_DEFEND_BONUS, 20, nullptr);
    assert_non_null(eff1);
    assert_non_null(eff2);
    struct requirement req1, req2;
    req1.source.kind = VUT_NONE;
    req1.range = REQ_RANGE_LOCAL;
    req1.survives = false;
    req1.present = true;
    req1.quiet = false;
    req2.source.kind = VUT_NONE;
    req2.range = REQ_RANGE_WORLD;
    req2.survives = false;
    req2.present = true;
    req2.quiet = false;
    effect_req_append(eff1, req1);
    effect_req_append(eff2, req2);
    struct requirement *r1 = requirement_vector_get(&eff1->reqs, 0);
    struct requirement *r2 = requirement_vector_get(&eff2->reqs, 0);
    assert_int_equal(r1->range, REQ_RANGE_LOCAL);
    assert_int_equal(r2->range, REQ_RANGE_WORLD);
}

/* Test effect survives flag */
static void test_effect_survives_flag(void **state)
{
    (void) state;
    struct effect *peffect = effect_new(EFT_DEFEND_BONUS, 50, nullptr);
    assert_non_null(peffect);
    struct requirement req;
    req.source.kind = VUT_NONE;
    req.range = REQ_RANGE_WORLD;
    req.survives = true;
    req.present = true;
    req.quiet = false;
    effect_req_append(peffect, req);
    struct requirement *r = requirement_vector_get(&peffect->reqs, 0);
    assert_true(r->survives);
}

/* Test effect quiet flag */
static void test_effect_quiet_flag(void **state)
{
    (void) state;
    struct effect *peffect = effect_new(EFT_DEFEND_BONUS, 50, nullptr);
    assert_non_null(peffect);
    struct requirement req;
    req.source.kind = VUT_NONE;
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = true;
    req.quiet = true;
    effect_req_append(peffect, req);
    struct requirement *r = requirement_vector_get(&peffect->reqs, 0);
    assert_true(r->quiet);
}

/* Test effect value edge cases */
static void test_effect_value_edge_cases(void **state)
{
    (void) state;
    struct effect *eff_zero = effect_new(EFT_TURN_YEARS, 0, nullptr);
    assert_non_null(eff_zero);
    assert_int_equal(eff_zero->value, 0);
    struct effect *eff_large = effect_new(EFT_TURN_YEARS, 1000000, nullptr);
    assert_non_null(eff_large);
    assert_int_equal(eff_large->value, 1000000);
    struct effect *eff_neg = effect_new(EFT_TURN_YEARS, -1000000, nullptr);
    assert_non_null(eff_neg);
    assert_int_equal(eff_neg->value, -1000000);
}

/* Test effect_list_remove */
static void test_effect_list_remove(void **state)
{
    (void) state;
    struct effect_list *list = effect_list_new();
    assert_non_null(list);
    struct effect *eff1 = effect_new(EFT_TURN_YEARS, 5, nullptr);
    struct effect *eff2 = effect_new(EFT_TURN_YEARS, 10, nullptr);
    effect_list_append(list, eff1);
    effect_list_append(list, eff2);
    assert_int_equal(effect_list_size(list), 2);
    effect_list_remove(list, eff1);
    assert_int_equal(effect_list_size(list), 1);
    struct effect *remaining = effect_list_front(list);
    assert_int_equal(remaining->value, 10);
    effect_list_destroy(list);
}

/* Test ruleset_cache reinit */
static void test_ruleset_cache_reinit(void **state)
{
    (void) state;
    struct effect *eff = effect_new(EFT_TURN_YEARS, 10, nullptr);
    assert_non_null(eff);
    struct effect_list *list = get_effects(EFT_TURN_YEARS);
    assert_int_equal(effect_list_size(list), 1);
    ruleset_cache_free();
    ruleset_cache_init();
    list = get_effects(EFT_TURN_YEARS);
    assert_int_equal(effect_list_size(list), 0);
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
        cmocka_unit_test_setup_teardown(test_effect_cumulative_max_empty,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_cumulative_max_positive,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_cumulative_min_empty,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_cumulative_min_negative,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_get_world_bonus_with_effects,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_get_player_bonus_null,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_iterate_effect_cache,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_remove,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_stacking,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_get_target_bonus_effects,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_list_append,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_list_front_back,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_cumulative_max_negative,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_cumulative_min_positive,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_get_world_bonus_mixed,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_copy_same_type,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_copy_with_requirements,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_iterate_effect_cache_stop,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_multiple_requirements,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_get_effects_different_types,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_value_from_universals_empty,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_universals_value_never_below,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_get_target_bonus_effects_with_list,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_negative_requirement,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_rulesave,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_list_clear,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_is_user_effect_boundary,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_different_ranges,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_survives_flag,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_quiet_flag,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_value_edge_cases,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_effect_list_remove,
                                        setup_effects, teardown_effects),
        cmocka_unit_test_setup_teardown(test_ruleset_cache_reinit,
                                        setup_effects, teardown_effects),
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}
