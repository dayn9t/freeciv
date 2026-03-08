/***********************************************************************
 Freeciv - Copyright (C) 2026 - The Freeciv Project
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

#include "common/aicore/pf_tools.h"
#include "common/aicore/path_finding.h"
#include "common/aicore/aiactions.h"
#include "common/game.h"
#include "common/map.h"
#include "common/player.h"
#include "common/unittype.h"

/* Test setup and teardown */
static int setup_pf_tools(void **state)
{
    (void) state;
    game_init(false);
    return 0;
}

static int teardown_pf_tools(void **state)
{
    (void) state;
    game_free();
    return 0;
}

/* Test pft_fill_unit_parameter */
static void test_pft_fill_unit_parameter(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    struct unit_type *utype = utype_by_number(0);
    if (utype == NULL) {
        skip();
        return;
    }

    struct unit unit;
    memset(&unit, 0, sizeof(unit));
    unit.owner = p1;
    unit.utype = utype;
    unit.moves_left = utype->move_rate;
    unit.hp = utype->hp;

    struct pf_parameter param;
    pft_fill_unit_parameter(&param, &wld.map, &unit);

    assert_ptr_equal(param.map, &wld.map);
    assert_ptr_equal(param.owner, p1);
    assert_true(param.move_rate > 0);
}

/* Test pft_fill_utype_parameter */
static void test_pft_fill_utype_parameter(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    struct unit_type *utype = utype_by_number(0);
    if (utype == NULL) {
        skip();
        return;
    }

    struct tile *start = map_pos_to_tile(&wld.map, 8, 8);
    if (start == NULL) {
        skip();
        return;
    }

    struct pf_parameter param;
    pft_fill_utype_parameter(&param, &wld.map, utype, start, p1);

    assert_ptr_equal(param.map, &wld.map);
    assert_ptr_equal(param.owner, p1);
    assert_ptr_equal(param.start_tile, start);
    assert_ptr_equal(param.utype, utype);
}

/* Test pft_fill_unit_overlap_param */
static void test_pft_fill_unit_overlap_param(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    struct unit_type *utype = utype_by_number(0);
    if (utype == NULL) {
        skip();
        return;
    }

    struct unit unit;
    memset(&unit, 0, sizeof(unit));
    unit.owner = p1;
    unit.utype = utype;
    unit.moves_left = utype->move_rate;

    struct pf_parameter param;
    pft_fill_unit_overlap_param(&param, &wld.map, &unit);

    assert_ptr_not_equal(param.get_MC, NULL);
    assert_false(param.ignore_none_scopes);
}

/* Test pft_fill_unit_attack_param */
static void test_pft_fill_unit_attack_param(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    struct unit_type *utype = utype_by_number(0);
    if (utype == NULL) {
        skip();
        return;
    }

    struct unit unit;
    memset(&unit, 0, sizeof(unit));
    unit.owner = p1;
    unit.utype = utype;

    struct pf_parameter param;
    pft_fill_unit_attack_param(&param, &wld.map, &unit);

    assert_true(param.actions & PF_AA_UNIT_ATTACK);
}

/* Test pft_fill_reverse_parameter */
static void test_pft_fill_reverse_parameter(void **state)
{
    (void) state;

    struct tile *target = map_pos_to_tile(&wld.map, 10, 10);
    if (target == NULL) {
        skip();
        return;
    }

    struct pf_parameter param;
    pft_fill_reverse_parameter(&wld.map, &param, target);

    assert_ptr_equal(param.map, &wld.map);
    assert_ptr_equal(param.data, target);
    assert_int_equal(param.fuel, 1);
}

/* Test no_fights_or_unknown */
static void test_no_fights_or_unknown(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    struct unit_type *utype = utype_by_number(0);
    if (utype == NULL) {
        skip();
        return;
    }

    struct tile *tile = map_pos_to_tile(&wld.map, 5, 5);
    if (tile == NULL) {
        skip();
        return;
    }

    struct pf_parameter param;
    pft_fill_utype_parameter(&param, &wld.map, utype, tile, p1);
    param.omniscience = TRUE;

    enum tile_behavior tb = no_fights_or_unknown(tile, TILE_KNOWN_SEEN, &param);
    assert_int_equal(tb, TB_NORMAL);

    tb = no_fights_or_unknown(tile, TILE_UNKNOWN, &param);
    assert_int_equal(tb, TB_IGNORE);
}

/* Test no_fights */
static void test_no_fights(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    struct unit_type *utype = utype_by_number(0);
    if (utype == NULL) {
        skip();
        return;
    }

    struct tile *tile = map_pos_to_tile(&wld.map, 5, 5);
    if (tile == NULL) {
        skip();
        return;
    }

    struct pf_parameter param;
    pft_fill_utype_parameter(&param, &wld.map, utype, tile, p1);

    enum tile_behavior tb = no_fights(tile, TILE_KNOWN_SEEN, &param);
    assert_int_equal(tb, TB_NORMAL);
}

/* Test no_intermediate_fights */
static void test_no_intermediate_fights(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    struct unit_type *utype = utype_by_number(0);
    if (utype == NULL) {
        skip();
        return;
    }

    struct tile *tile = map_pos_to_tile(&wld.map, 5, 5);
    if (tile == NULL) {
        skip();
        return;
    }

    struct pf_parameter param;
    pft_fill_utype_parameter(&param, &wld.map, utype, tile, p1);

    enum tile_behavior tb = no_intermediate_fights(tile, TILE_KNOWN_SEEN, &param);
    assert_int_equal(tb, TB_NORMAL);
}

/* Test pft_fill_amphibious_parameter */
static void test_pft_fill_amphibious_parameter(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    struct unit_type *utype1 = utype_by_number(0);
    struct unit_type *utype2 = utype_by_number(1);

    if (utype1 == NULL || utype2 == NULL) {
        skip();
        return;
    }

    struct tile *start = map_pos_to_tile(&wld.map, 8, 8);
    if (start == NULL) {
        skip();
        return;
    }

    struct pft_amphibious amphibious;
    pft_fill_utype_parameter(&amphibious.land, &wld.map, utype1, start, p1);
    pft_fill_utype_parameter(&amphibious.sea, &wld.map, utype2, start, p1);
    pft_fill_amphibious_parameter(&amphibious);

    assert_ptr_not_equal(amphibious.combined.get_MC, NULL);
    assert_true(amphibious.land_scale > 0);
    assert_true(amphibious.sea_scale > 0);
}

/* Test pft_fill_utype_overlap_param */
static void test_pft_fill_utype_overlap_param(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    struct unit_type *utype = utype_by_number(0);
    if (utype == NULL) {
        skip();
        return;
    }

    struct tile *start = map_pos_to_tile(&wld.map, 8, 8);
    if (start == NULL) {
        skip();
        return;
    }

    struct pf_parameter param;
    pft_fill_utype_overlap_param(&param, &wld.map, utype, start, p1);

    assert_ptr_equal(param.map, &wld.map);
    assert_ptr_equal(param.utype, utype);
}

/* Test pft_fill_utype_attack_param */
static void test_pft_fill_utype_attack_param(void **state)
{
    (void) state;

    struct player *p1 = player_by_number(0);
    if (p1 == NULL) {
        skip();
        return;
    }

    struct unit_type *utype = utype_by_number(0);
    if (utype == NULL) {
        skip();
        return;
    }

    struct tile *start = map_pos_to_tile(&wld.map, 8, 8);
    if (start == NULL) {
        skip();
        return;
    }

    struct pf_parameter param;
    pft_fill_utype_attack_param(&param, &wld.map, utype, start, p1);

    assert_ptr_equal(param.utype, utype);
    assert_false(param.actions & PF_AA_CITY_ATTACK);
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_pft_fill_unit_parameter,
                                        setup_pf_tools, teardown_pf_tools),
        cmocka_unit_test_setup_teardown(test_pft_fill_utype_parameter,
                                        setup_pf_tools, teardown_pf_tools),
        cmocka_unit_test_setup_teardown(test_pft_fill_unit_overlap_param,
                                        setup_pf_tools, teardown_pf_tools),
        cmocka_unit_test_setup_teardown(test_pft_fill_unit_attack_param,
                                        setup_pf_tools, teardown_pf_tools),
        cmocka_unit_test_setup_teardown(test_pft_fill_reverse_parameter,
                                        setup_pf_tools, teardown_pf_tools),
        cmocka_unit_test_setup_teardown(test_no_fights_or_unknown,
                                        setup_pf_tools, teardown_pf_tools),
        cmocka_unit_test_setup_teardown(test_no_fights,
                                        setup_pf_tools, teardown_pf_tools),
        cmocka_unit_test_setup_teardown(test_no_intermediate_fights,
                                        setup_pf_tools, teardown_pf_tools),
        cmocka_unit_test_setup_teardown(test_pft_fill_amphibious_parameter,
                                        setup_pf_tools, teardown_pf_tools),
        cmocka_unit_test_setup_teardown(test_pft_fill_utype_overlap_param,
                                        setup_pf_tools, teardown_pf_tools),
        cmocka_unit_test_setup_teardown(test_pft_fill_utype_attack_param,
                                        setup_pf_tools, teardown_pf_tools),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
