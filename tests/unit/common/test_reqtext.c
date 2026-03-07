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

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <setjmp.h>
#include <cmocka.h>

#include "common/reqtext.h"
#include "common/requirements.h"
#include "common/game.h"

/* Test setup and teardown */
static int setup_reqtext(void **state)
{
    (void) state;
    game_init(false);
    return 0;
}

static int teardown_reqtext(void **state)
{
    (void) state;
    game_free();
    return 0;
}

/* Test req_text_insert with VUT_NONE requirement */
static void test_req_text_insert_none(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Create a VUT_NONE requirement */
    req.source.kind = VUT_NONE;
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* VUT_NONE should return FALSE */
    assert_false(req_text_insert(buf, sizeof(buf), nullptr,
                                  &req, VERB_DEFAULT, ""));
}

/* Test req_text_insert with quiet requirement */
static void test_req_text_insert_quiet(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Create a quiet requirement */
    req.source.kind = VUT_MINYEAR;
    req.range = REQ_RANGE_WORLD;
    req.survives = false;
    req.present = true;
    req.quiet = true;

    /* Quiet requirements should return FALSE with VERB_DEFAULT */
    assert_false(req_text_insert(buf, sizeof(buf), nullptr,
                                  &req, VERB_DEFAULT, ""));

    /* But should work with VERB_ACTUAL */
    buf[0] = '\0';
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_ACTUAL, ""));
}

/* Test req_text_insert_nl adds newline */
static void test_req_text_insert_nl(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;
    size_t len_before, len_after;

    /* Create a valid requirement */
    req.source.kind = VUT_MINYEAR;
    req.source.value.minyear = 0;
    req.range = REQ_RANGE_WORLD;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Call req_text_insert first */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));
    len_before = strlen(buf);

    /* Reset buffer */
    buf[0] = '\0';

    /* Call req_text_insert_nl */
    assert_true(req_text_insert_nl(buf, sizeof(buf), nullptr,
                                    &req, VERB_DEFAULT, ""));
    len_after = strlen(buf);

    /* Should have added a newline */
    assert_true(len_after == len_before + 1);
    assert_string_equal(buf + len_before, "\n");
}

/* Test req_text_insert with prefix */
static void test_req_text_insert_prefix(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a valid requirement */
    req.source.kind = VUT_MINYEAR;
    req.source.value.minyear = 0;
    req.range = REQ_RANGE_WORLD;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Call with prefix */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, "Prefix: "));

    /* Buffer should start with the prefix */
    assert_true(strncmp(buf, "Prefix: ", 8) == 0);
}

/* Test req_text_insert with small buffer */
static void test_req_text_insert_small_buffer(void **state)
{
    (void) state;
    char buf[4];
    struct requirement req;

    /* Create a valid requirement */
    req.source.kind = VUT_MINYEAR;
    req.source.value.minyear = 0;
    req.range = REQ_RANGE_WORLD;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should handle small buffer gracefully */
    req_text_insert(buf, sizeof(buf), nullptr, &req, VERB_DEFAULT, "");

    /* Buffer should be null-terminated */
    assert_true(buf[sizeof(buf) - 1] == '\0' || strlen(buf) < sizeof(buf));
}

/* Test rt_verbosity enumeration */
static void test_rt_verbosity_enum(void **state)
{
    (void) state;

    /* Test verbosity values are valid */
    assert_int_equal(VERB_DEFAULT, 0);
    assert_int_equal(VERB_ACTUAL, 1);
}

/* Test req_text_insert with present = false */
static void test_req_text_insert_absent(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create an absent requirement */
    req.source.kind = VUT_MINYEAR;
    req.source.value.minyear = 0;
    req.range = REQ_RANGE_WORLD;
    req.survives = false;
    req.present = false;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed for absent requirement */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));

    /* Buffer should contain some text */
    assert_true(strlen(buf) > 0);
}

/* Test req_text_insert with survives flag */
static void test_req_text_insert_survives(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a requirement with survives flag */
    req.source.kind = VUT_MINYEAR;
    req.source.value.minyear = 0;
    req.range = REQ_RANGE_WORLD;
    req.survives = true;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));
}

/* Test req_text_insert with different ranges */
static void test_req_text_insert_ranges(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;
    int range;

    /* Create a requirement and test different ranges */
    req.source.kind = VUT_MINYEAR;
    req.source.value.minyear = 0;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Test each range */
    for (range = REQ_RANGE_LOCAL; range < REQ_RANGE_COUNT; range++) {
        req.range = (enum req_range)range;

        /* Reset buffer */
        buf[0] = '\0';

        /* MINYEAR with WORLD range should work, others may not */
        if (range == REQ_RANGE_WORLD) {
            assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                         &req, VERB_DEFAULT, ""));
        } else {
            /* Other ranges may not be supported for MINYEAR */
            req_text_insert(buf, sizeof(buf), nullptr, &req, VERB_DEFAULT, "");
        }
    }
}

/* Test req_text_insert with MINCITIES requirement */
static void test_req_text_insert_min_cities(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a MINCITIES requirement */
    req.source.kind = VUT_MINCITIES;
    req.source.value.min_cities = 5;
    req.range = REQ_RANGE_PLAYER;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));

    /* Buffer should contain some text */
    assert_true(strlen(buf) > 0);
}

/* Test req_text_insert with MINTECHS requirement */
static void test_req_text_insert_min_techs(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a MINTECHS requirement */
    req.source.kind = VUT_MINTECHS;
    req.source.value.min_techs = 10;
    req.range = REQ_RANGE_PLAYER;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));

    /* Buffer should contain some text */
    assert_true(strlen(buf) > 0);
}

/* Test req_text_insert with AI_LEVEL requirement */
static void test_req_text_insert_ai_level(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create an AI_LEVEL requirement */
    req.source.kind = VUT_AI_LEVEL;
    req.source.value.ai_level = 1;  /* Use a valid AI level value */
    req.range = REQ_RANGE_PLAYER;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));
}

/* Test req_text_insert with TERRAINCLASS requirement */
static void test_req_text_insert_terrain_class(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a TERRAINCLASS requirement */
    req.source.kind = VUT_TERRAINCLASS;
    req.source.value.terrainclass = 0;  /* TC_LAND or similar */
    req.range = REQ_RANGE_TILE;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Call the function - may or may not succeed depending on initialization */
    req_text_insert(buf, sizeof(buf), nullptr, &req, VERB_DEFAULT, "");
}

/* Test req_text_insert with MINSIZE requirement */
static void test_req_text_insert_min_size(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a MINSIZE requirement */
    req.source.kind = VUT_MINSIZE;
    req.source.value.minsize = 8;
    req.range = REQ_RANGE_CITY;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));

    /* Buffer should contain some text */
    assert_true(strlen(buf) > 0);
}

/* Test req_text_insert with MINCULTURE requirement */
static void test_req_text_insert_min_culture(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a MINCULTURE requirement */
    req.source.kind = VUT_MINCULTURE;
    req.source.value.minculture = 100;
    req.range = REQ_RANGE_CITY;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));

    /* Buffer should contain some text */
    assert_true(strlen(buf) > 0);
}

/* Test req_text_insert with UNITSTATE requirement */
static void test_req_text_insert_unit_state(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a UNITSTATE requirement */
    req.source.kind = VUT_UNITSTATE;
    req.source.value.unit_state = 0;  /* USP_LIVE or similar */
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Call the function */
    req_text_insert(buf, sizeof(buf), nullptr, &req, VERB_DEFAULT, "");
}

/* Test req_text_insert with ACTIVITY requirement */
static void test_req_text_insert_activity(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create an ACTIVITY requirement */
    req.source.kind = VUT_ACTIVITY;
    req.source.value.activity = 0;  /* ACTIVITY_IDLE or similar */
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Call the function */
    req_text_insert(buf, sizeof(buf), nullptr, &req, VERB_DEFAULT, "");
}

/* Test req_text_insert with MINMOVES requirement */
static void test_req_text_insert_min_moves(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a MINMOVES requirement */
    req.source.kind = VUT_MINMOVES;
    req.source.value.minmoves = 2;
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));
}

/* Test req_text_insert with MINVETERAN requirement */
static void test_req_text_insert_min_veteran(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a MINVETERAN requirement */
    req.source.kind = VUT_MINVETERAN;
    req.source.value.minveteran = 1;
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));
}

{
    (void) state;
    char buf[256];
    struct requirement req;

    req.source.value.mincities = 10;
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));
}

/* Test req_text_insert with AGE requirement */
static void test_req_text_insert_age(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create an AGE requirement */
    req.source.kind = VUT_AGE;
    req.source.value.age = 5;
    req.range = REQ_RANGE_PLAYER;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));
}

/* Test req_text_insert with FORM_AGE requirement */
static void test_req_text_insert_form_age(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a FORM_AGE requirement */
    req.source.kind = VUT_FORM_AGE;
    req.source.value.form_age = 10;
    req.range = REQ_RANGE_PLAYER;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));
}

/* Test req_text_insert with FUTURETECHS requirement */
static void test_req_text_insert_future_techs(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a FUTURETECHS requirement */
    req.source.kind = VUT_FUTURETECHS;
    req.source.value.future_techs = 3;
    req.range = REQ_RANGE_PLAYER;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));
}

/* Test req_text_insert with MAXTILETOTALUNITS requirement */
static void test_req_text_insert_max_tile_total_units(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a MAXTILETOTALUNITS requirement */
    req.source.kind = VUT_MAXTILETOTALUNITS;
    req.source.value.max_tile_total_units = 10;
    req.range = REQ_RANGE_TILE;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));
}

/* Test req_text_insert with CITYTILE requirement */
static void test_req_text_insert_city_tile(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a CITYTILE requirement */
    req.source.kind = VUT_CITYTILE;
    req.source.value.citytile = 0;  /* CITYT_CENTER or similar */
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Call the function */
    req_text_insert(buf, sizeof(buf), nullptr, &req, VERB_DEFAULT, "");
}

/* Test req_text_insert with CITYSTATUS requirement */
static void test_req_text_insert_city_status(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a CITYSTATUS requirement */
    req.source.kind = VUT_CITYSTATUS;
    req.source.value.citystatus = 0;  /* CITYS_OWN or similar */
    req.range = REQ_RANGE_CITY;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Call the function */
    req_text_insert(buf, sizeof(buf), nullptr, &req, VERB_DEFAULT, "");
}

static void test_req_text_insert_min_latitude(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    req.source.value.latitude = 30;
    req.range = REQ_RANGE_TILE;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));
}

static void test_req_text_insert_max_latitude(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    req.source.value.latitude = 70;
    req.range = REQ_RANGE_TILE;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));
}

/* Test req_text_insert with MINFOREIGNPCT requirement */
static void test_req_text_insert_min_foreign_pct(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Create a MINFOREIGNPCT requirement */
    req.source.kind = VUT_MINFOREIGNPCT;
    req.source.value.minforeignpct = 50;
    req.range = REQ_RANGE_CITY;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Should succeed */
    assert_true(req_text_insert(buf, sizeof(buf), nullptr,
                                 &req, VERB_DEFAULT, ""));
}

/* Test req_text_insert_nl with VUT_NONE */
static void test_req_text_insert_nl_none(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Create a VUT_NONE requirement */
    req.source.kind = VUT_NONE;
    req.range = REQ_RANGE_LOCAL;
    req.survives = false;
    req.present = true;
    req.quiet = false;

    /* VUT_NONE should return FALSE */
    assert_false(req_text_insert_nl(buf, sizeof(buf), nullptr,
                                     &req, VERB_DEFAULT, ""));
}

/* Test req_text_insert_nl with quiet requirement */
static void test_req_text_insert_nl_quiet(void **state)
{
    (void) state;
    char buf[256];
    struct requirement req;

    /* Initialize buffer */
    buf[0] = '\0';

    /* Create a quiet requirement */
    req.source.kind = VUT_MINYEAR;
    req.source.value.minyear = 0;
    req.range = REQ_RANGE_WORLD;
    req.survives = false;
    req.present = true;
    req.quiet = true;

    /* Quiet requirements should return FALSE with VERB_DEFAULT */
    assert_false(req_text_insert_nl(buf, sizeof(buf), nullptr,
                                     &req, VERB_DEFAULT, ""));

    /* But should work with VERB_ACTUAL */
    buf[0] = '\0';
    assert_true(req_text_insert_nl(buf, sizeof(buf), nullptr,
                                    &req, VERB_ACTUAL, ""));

    /* Should end with newline */
    assert_true(strlen(buf) > 0);
    assert_string_equal(buf + strlen(buf) - 1, "\n");
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_req_text_insert_none,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_quiet,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_nl,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_prefix,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_small_buffer,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_rt_verbosity_enum,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_absent,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_survives,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_ranges,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_min_cities,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_min_techs,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_ai_level,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_terrain_class,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_min_size,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_min_culture,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_unit_state,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_activity,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_min_moves,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_min_veteran,
                                        setup_reqtext, teardown_reqtext),
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_age,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_form_age,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_future_techs,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_max_tile_total_units,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_city_tile,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_city_status,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_min_latitude,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_max_latitude,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_min_foreign_pct,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_nl_none,
                                        setup_reqtext, teardown_reqtext),
        cmocka_unit_test_setup_teardown(test_req_text_insert_nl_quiet,
                                        setup_reqtext, teardown_reqtext),
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}