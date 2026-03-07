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

/* utility */
#include "mem.h"

/* common */
#include "vision.h"

/************************************************************************//**
  Test vision_new creates a valid vision structure
****************************************************************************/
static void test_vision_new_basic(void **state)
{
  struct vision *vision;

  (void) state;

  vision = vision_new(NULL, NULL);

  assert_non_null(vision);
  assert_null(vision->player);
  assert_null(vision->tile);
  assert_true(vision->can_reveal_tiles);
  assert_int_equal(vision->radius_sq[V_MAIN], -1);
  assert_int_equal(vision->radius_sq[V_INVIS], -1);
  assert_int_equal(vision->radius_sq[V_SUBSURFACE], -1);

  /* Clean up - vision_free requires radius_sq to be -1 */
  free(vision);
}

/************************************************************************//**
  Test vision_free with properly cleared vision
****************************************************************************/
static void test_vision_free_basic(void **state)
{
  struct vision *vision;

  (void) state;

  vision = vision_new(NULL, NULL);
  assert_non_null(vision);

  /* vision_free requires radius_sq values to be -1, which is the default */
  vision_free(vision);
}

/************************************************************************//**
  Test vision_reveal_tiles default value
****************************************************************************/
static void test_vision_reveal_tiles_default(void **state)
{
  struct vision *vision;

  (void) state;

  vision = vision_new(NULL, NULL);
  assert_non_null(vision);

  /* Default should be TRUE */
  assert_true(vision->can_reveal_tiles);

  free(vision);
}

/************************************************************************//**
  Test vision_reveal_tiles setting and return value
****************************************************************************/
static void test_vision_reveal_tiles_set(void **state)
{
  struct vision *vision;
  bool previous;

  (void) state;

  vision = vision_new(NULL, NULL);
  assert_non_null(vision);

  /* Set to FALSE, should return previous value (TRUE) */
  previous = vision_reveal_tiles(vision, FALSE);
  assert_true(previous);
  assert_false(vision->can_reveal_tiles);

  /* Set back to TRUE, should return previous value (FALSE) */
  previous = vision_reveal_tiles(vision, TRUE);
  assert_false(previous);
  assert_true(vision->can_reveal_tiles);

  free(vision);
}

/************************************************************************//**
  Test vision_site_new creates a valid structure
****************************************************************************/
static void test_vision_site_new_basic(void **state)
{
  struct vision_site *psite;

  (void) state;

  psite = vision_site_new(1, NULL, NULL);

  assert_non_null(psite);
  assert_int_equal(psite->identity, 1);
  assert_null(psite->location);
  assert_null(psite->owner);
  assert_null(psite->original);
  assert_null(psite->name);

  vision_site_destroy(psite);
}

/************************************************************************//**
  Test vision_site_destroy with null name
****************************************************************************/
static void test_vision_site_destroy_null_name(void **state)
{
  struct vision_site *psite;

  (void) state;

  psite = vision_site_new(1, NULL, NULL);
  assert_non_null(psite);
  assert_null(psite->name);

  /* Should not crash when name is NULL */
  vision_site_destroy(psite);
}

/************************************************************************//**
  Test vision_site_size_get and vision_site_size_set
****************************************************************************/
static void test_vision_site_size(void **state)
{
  struct vision_site *psite;
  citizens size;

  (void) state;

  psite = vision_site_new(1, NULL, NULL);
  assert_non_null(psite);

  /* Default size should be 0 (from fc_calloc) */
  size = vision_site_size_get(psite);
  assert_int_equal(size, 0);

  /* Set a new size */
  vision_site_size_set(psite, 10);
  size = vision_site_size_get(psite);
  assert_int_equal(size, 10);

  /* Set another size */
  vision_site_size_set(psite, 50);
  size = vision_site_size_get(psite);
  assert_int_equal(size, 50);

  vision_site_destroy(psite);
}

/************************************************************************//**
  Test vision_site_size_get with null parameter
****************************************************************************/
static void test_vision_site_size_get_null(void **state)
{
  citizens size;

  (void) state;

  /* Should return 0 and not crash */
  size = vision_site_size_get(NULL);
  assert_int_equal(size, 0);
}

/************************************************************************//**
  Test vision_site_copy basic functionality
****************************************************************************/
static void test_vision_site_copy_basic(void **state)
{
  struct vision_site *original;
  struct vision_site *copy;

  (void) state;

  original = vision_site_new(100, NULL, NULL);
  assert_non_null(original);

  vision_site_size_set(original, 25);
  original->occupied = true;
  original->happy = true;
  original->walls = 3;

  copy = vision_site_copy(original);

  assert_non_null(copy);
  assert_ptr_not_equal(copy, original);
  assert_int_equal(copy->identity, original->identity);
  assert_ptr_equal(copy->location, original->location);
  assert_ptr_equal(copy->owner, original->owner);
  assert_int_equal(copy->size, original->size);
  assert_true(copy->occupied);
  assert_true(copy->happy);
  assert_int_equal(copy->walls, 3);

  vision_site_destroy(original);
  vision_site_destroy(copy);
}

/************************************************************************//**
  Test vision_site_copy with name
****************************************************************************/
static void test_vision_site_copy_with_name(void **state)
{
  struct vision_site *original;
  struct vision_site *copy;

  (void) state;

  original = vision_site_new(1, NULL, NULL);
  assert_non_null(original);

  /* Set name manually */
  original->name = fc_strdup("TestCity");

  copy = vision_site_copy(original);

  assert_non_null(copy);
  assert_non_null(copy->name);
  assert_string_equal(copy->name, "TestCity");
  /* Names should be different pointers (deep copy) */
  assert_ptr_not_equal(copy->name, original->name);

  vision_site_destroy(original);
  vision_site_destroy(copy);
}

/************************************************************************//**
  Test vision_site_copy with null name
****************************************************************************/
static void test_vision_site_copy_null_name(void **state)
{
  struct vision_site *original;
  struct vision_site *copy;

  (void) state;

  original = vision_site_new(1, NULL, NULL);
  assert_non_null(original);
  assert_null(original->name);

  copy = vision_site_copy(original);

  assert_non_null(copy);
  assert_null(copy->name);

  vision_site_destroy(original);
  vision_site_destroy(copy);
}

/************************************************************************//**
  Test V_RADIUS macro initialization
****************************************************************************/
static void test_v_radius_macro(void **state)
{
  v_radius_t radius = V_RADIUS(10, 5, 3);

  (void) state;

  assert_int_equal(radius[V_MAIN], 10);
  assert_int_equal(radius[V_INVIS], 5);
  assert_int_equal(radius[V_SUBSURFACE], 3);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* vision_new and vision_free tests */
    cmocka_unit_test(test_vision_new_basic),
    cmocka_unit_test(test_vision_free_basic),

    /* vision_reveal_tiles tests */
    cmocka_unit_test(test_vision_reveal_tiles_default),
    cmocka_unit_test(test_vision_reveal_tiles_set),

    /* vision_site_new and vision_site_destroy tests */
    cmocka_unit_test(test_vision_site_new_basic),
    cmocka_unit_test(test_vision_site_destroy_null_name),

    /* vision_site_size tests */
    cmocka_unit_test(test_vision_site_size),
    cmocka_unit_test(test_vision_site_size_get_null),

    /* vision_site_copy tests */
    cmocka_unit_test(test_vision_site_copy_basic),
    cmocka_unit_test(test_vision_site_copy_with_name),
    cmocka_unit_test(test_vision_site_copy_null_name),

    /* V_RADIUS macro test */
    cmocka_unit_test(test_v_radius_macro),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}