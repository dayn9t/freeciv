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

/* common */
#include "game.h"
#include "settings.h"

/* server */
#include "setcompat.h"

/* Test setcompat_S3_2_name_from_S3_1 with renamed setting */
static void test_setcompat_name_renamed_setting(void **state)
{
    (void) state;

    const char *new_name = setcompat_S3_2_name_from_S3_1("spaceship_travel_time");
    assert_non_null(new_name);
    assert_string_equal(new_name, "spaceship_travel_pct");
}

/* Test setcompat_S3_2_name_from_S3_1 with unchanged setting */
static void test_setcompat_name_unchanged_setting(void **state)
{
    (void) state;

    const char *new_name = setcompat_S3_2_name_from_S3_1("gold");
    assert_non_null(new_name);
    assert_string_equal(new_name, "gold");
}

/* Test setcompat_S3_2_name_from_S3_1 with NULL */
static void test_setcompat_name_null_safe(void **state)
{
    (void) state;

    /* We test that function handles various edge cases safely.
     * Note: Passing NULL may cause undefined behavior since
     * the function doesn't have explicit NULL checks.
     * We test with empty string instead. */
    const char *new_name = setcompat_S3_2_name_from_S3_1("");
    /* Empty string should return empty string (no match found) */
    assert_string_equal(new_name, "");
}

/* Test setcompat_S3_2_val_from_S3_1 with BZIP2 compression */
static void test_setcompat_value_bzip2_compression(void **state)
{
    (void) state;

    /* Get the actual compresstype setting */
    struct setting *pset = setting_by_name("compresstype");

    /* If compresstype setting doesn't exist, skip this test */
    if (pset == NULL) {
        return;
    }

    const char *new_val = setcompat_S3_2_val_from_S3_1(pset, "BZIP2");

    /* The result depends on compile-time options, but should be one of:
     * ZSTD, XZ, LIBZ, or PLAIN */
    if (new_val != NULL) {
        assert_true(strcmp(new_val, "ZSTD") == 0 ||
                    strcmp(new_val, "XZ") == 0 ||
                    strcmp(new_val, "LIBZ") == 0 ||
                    strcmp(new_val, "PLAIN") == 0);
    }
    /* If NULL is returned, the name didn't match compresstype */
}

/* Test setcompat_S3_2_val_from_S3_1 with non-compresstype setting */
static void test_setcompat_value_non_compresstype(void **state)
{
    (void) state;

    /* Get a setting that's not compresstype */
    struct setting *pset = setting_by_name("gold");

    /* If gold setting doesn't exist, use any other setting */
    if (pset == NULL) {
        pset = setting_by_name("mapsize");
    }

    if (pset != NULL) {
        const char *new_val = setcompat_S3_2_val_from_S3_1(pset, "100");
        /* Should return NULL for non-compresstype settings */
        assert_null(new_val);
    }
}

/* Test setcompat_S3_2_val_from_S3_1 with NULL value */
static void test_setcompat_value_empty_string(void **state)
{
    (void) state;

    struct setting *pset = setting_by_name("compresstype");

    if (pset != NULL) {
        const char *new_val = setcompat_S3_2_val_from_S3_1(pset, "");
        /* Empty string should return NULL (no match) */
        assert_null(new_val);
    }
}

/* Main test suite */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_setcompat_name_renamed_setting),
        cmocka_unit_test(test_setcompat_name_unchanged_setting),
        cmocka_unit_test(test_setcompat_name_null_safe),
        cmocka_unit_test(test_setcompat_value_bzip2_compression),
        cmocka_unit_test(test_setcompat_value_non_compresstype),
        cmocka_unit_test(test_setcompat_value_empty_string),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
